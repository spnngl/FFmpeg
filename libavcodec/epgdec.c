#include "avcodec.h"
#include "internal.h"
#include "decode.h"
#include "libavutil/common.h"
#include "libavutil/dvb.h"
#include "libavutil/epg.h"
#include "libavformat/mpegts.h"
#include "dvbdescriptors.h"

typedef struct EpgTidInfo {
    int last_version;
    int last_section_num;
    int last_segment_section_num;
} EpgTidInfo;

typedef struct EPGContext {
    const AVClass *class;
    struct EpgTidInfo infos[4];
    int last_sched_table_id;
    int o_last_sched_table_id;
    AVPacket *pkt;
} EPGContext;

static int short_event_handle(void *data, EPGEvent *event)
{
    DvbShortEventDescriptor *short_event = data;
    if (short_event->event_name_len)
        strncpy(event->event_name, short_event->event_name, sizeof(event->event_name));
    if (short_event->text_len)
        strncpy(event->short_event_description, short_event->text,
                sizeof(event->short_event_description));
    return 0;
}

static int extended_event_handle(void *data, EPGEvent *event)
{
    DvbExtendedEventDescriptor *extended_event = data;
    SimpleLinkedList *head;

    if (extended_event->text_len)
    {
            int old_size = strlen(event->long_event_description),
                space_left = sizeof(event->long_event_description) - old_size,
                text_len = extended_event->text_len;

            if (text_len > space_left)
                text_len = space_left;

            memcpy(event->long_event_description + old_size,
                   extended_event->text, text_len);
            event->long_event_description[old_size + text_len] = '\0';
    }

    for (head = extended_event->descriptions; head; head = head->next) {
        DvbExtendedEventDescription *d = head->data;
        int old_size = strlen(event->items),
            space_left = sizeof(event->items) - old_size,
            size = d->item_description_length + d->item_length + 3;

        if (size > space_left)
            size = space_left;

        snprintf(event->items + old_size, size, "%s: %s,", d->descr, d->item);
    }

    return 0;
}

static int parental_rating_handle(void *data, EPGEvent *event)
{
    DvbParentalRatingDescriptor *prd = data;
    DvbParentalRatingDescription *prdd;

    // For now we only look at the first age indicated
    if (prd->nb_descriptions) {
        prdd = prd->descriptions[0];
        event->rating = prdd->rating;
        memcpy(event->country_code, prdd->country_code, 3);
    }

    return 0;
}

static int component_handle(void *data, EPGEvent *event)
{
    DvbComponentDescriptor *comp = data;

    event->component.stream_content_ext = comp->stream_content_ext;
    event->component.stream_content = comp->stream_content;
    event->component.type = comp->component_type;
    event->component.tag = comp->component_tag;
    strncpy(event->component.text, comp->text, 32);

    return 0;
}

static int content_handle(void *data, EPGEvent *event)
{
    DvbContentDescriptor *content = data;
    // For now we only use the first content description. Presence of multiple
    // descriptions seems rare.
    DvbContentDescription *content_desc =
        content->nb_descriptions > 0 ? content->descriptions[0] : NULL;

    event->content.nibble_lvl_1 = content_desc->content_nibble_lvl_1;
    event->content.nibble_lvl_2 = content_desc->content_nibble_lvl_2;
    event->content.user_byte = content_desc->user_byte;

    return 0;
}

static int time_shifted_event_handle(void *data, EPGEvent *event)
{
    DvbTimeShiftedEventDescriptor *tsed = data;
    event->shift_from.service_id = tsed->reference_service_id;
    event->shift_from.event_id = tsed->reference_event_id;
    return 0;
}

static int sb_buffer_handle(void *data, EPGEvent *event)
{
    DvbShortSmoothingBufferDescriptor *ssb = data;
    event->sb.size = ssb->sb_size;
    event->sb.leak_rate = ssb->sb_leak_rate;
    return 0;
}

static int pdc_handle(void *data, EPGEvent *event)
{
    DvbPDCDescriptor *pdc = data;
    event->pil = pdc->programme_identification_label;
    return 0;
}

static int(*epg_descriptors_arr[256])(void *, EPGEvent *) = {
    [0x00 ... 0xFF] = NULL,
    /* Below all supported descriptor according to dvb specs */
    [DVB_DESCRIPTOR_TAG_SHORT_EVENT] = &short_event_handle,
    [DVB_DESCRIPTOR_TAG_EXTENDED_EVENT] = &extended_event_handle,
    [DVB_DESCRIPTOR_TAG_PARENTAL_RATING] = &parental_rating_handle,
    [DVB_DESCRIPTOR_TAG_STUFFING] = NULL,
    [DVB_DESCRIPTOR_TAG_LINKAGE] = NULL,
    [DVB_DESCRIPTOR_TAG_TIME_SHIFTED_EVENT] = &time_shifted_event_handle,
    [DVB_DESCRIPTOR_TAG_COMPONENT] = &component_handle,
    [DVB_DESCRIPTOR_TAG_CA_IDENTIFIER] = NULL,
    [DVB_DESCRIPTOR_TAG_CONTENT] = &content_handle,
    [DVB_DESCRIPTOR_TAG_TELEPHONE] = NULL,
    [DVB_DESCRIPTOR_TAG_MULTILINGUAL_COMPONENT] = NULL,
    [DVB_DESCRIPTOR_TAG_PRIVATE_DATA_SPECIFIER] = NULL,
    [DVB_DESCRIPTOR_TAG_SHORT_SMOOTHING_BUFFER] = &sb_buffer_handle,
    [DVB_DESCRIPTOR_TAG_DATA_BROADCAST] = NULL,
    [DVB_DESCRIPTOR_TAG_PDC] = &pdc_handle,
    [DVB_DESCRIPTOR_TAG_CONTENT_IDENTIFIER] = NULL,
    [DVB_DESCRIPTOR_TAG_XAIT_LOCATION] = NULL,
    [DVB_DESCRIPTOR_TAG_FTA_CONTENT_MANAGEMENT] = NULL,
    [DVB_DESCRIPTOR_TAG_EXTENSION] = NULL,
    [DVB_DESCRIPTOR_TAG_TVA_ID] = NULL,
};

static int epg_handle_descriptor(DvbDescriptorHeader *h, DvbDescriptor *desc,
                                 EPGEvent *event, const uint8_t **pp,
                                 const uint8_t *p_end)
{
    int ret = AVERROR_PATCHWELCOME;
    int(*handle)(void*, EPGEvent*) = epg_descriptors_arr[h->tag];

    if (handle) {
        void *data = desc->parse(desc, pp, p_end);
        if (!data)
            return AVERROR_INVALIDDATA;

        ret = handle(data, event);
        desc->free(data);
    }
    return ret;
}

static int epg_receive_frame(AVCodecContext *avctx, AVFrame *frame)
{
    EPGContext *epg_ctx = avctx->priv_data;
    EpgTidInfo *epg_info;
    EPGEvent *event = NULL, **events = NULL;
    DvbSectionHeader h1, *h = &h1;
    const uint8_t *p, *p_end;
    int val, segment_last_section_number, last_table_id,
        max_last_table_id, ret, ts_id, network_id;
    int nb_events = 0;
    uint8_t next_version;
    AVFrameSideData *sd;
    AVBufferRef *buf_ref;

    if (!epg_ctx->pkt->data) {
        ret = ff_decode_get_packet(avctx, epg_ctx->pkt);
        if (ret < 0)
            return ret;
    }

    p       = epg_ctx->pkt->data;
    p_end   = p + epg_ctx->pkt->size - 4;

    //event = av_malloc(sizeof(EPGEvent));
    //av_epg_init_event(event);

    if (avpriv_dvb_parse_section_header(h, &p, p_end) < 0)
        goto fail;

    //event->service_id = h->id;

    val = avpriv_dvb_get16(&p, p_end);
    if (val < 0)
        goto fail;
    ts_id = val;

    val = avpriv_dvb_get16(&p, p_end);
    if (val < 0)
        goto fail;
    network_id = val;

    val = avpriv_dvb_get8(&p, p_end);
    if (val < 0)
        goto fail;
    segment_last_section_number = val;

    val = avpriv_dvb_get8(&p, p_end);
    if (val < 0)
        goto fail;
    last_table_id = val;

    // Check eit data
    switch (h->tid) {
        case EIT_TID:
            epg_info = &epg_ctx->infos[0];
            if (last_table_id != EIT_TID || h->last_sec_num != segment_last_section_number)
                goto fail;
            break;
        case OEIT_TID:
            epg_info = &epg_ctx->infos[1];
            if (last_table_id != OEIT_TID || h->last_sec_num != segment_last_section_number)
                goto fail;
            break;
        case EITS_START_TID ... EITS_END_TID:
            epg_info = &epg_ctx->infos[2];
            if (segment_last_section_number != h->sec_num)
                goto fail;
            max_last_table_id = FFMAX(last_table_id, epg_ctx->last_sched_table_id);
            if (last_table_id != max_last_table_id)
                goto fail;
            epg_ctx->last_sched_table_id = max_last_table_id;
            break;
        case OEITS_START_TID ... OEITS_END_TID:
            epg_info = &epg_ctx->infos[3];
            if (segment_last_section_number != h->sec_num)
                goto fail;
            max_last_table_id = FFMAX(last_table_id, epg_ctx->o_last_sched_table_id);
            if (last_table_id != max_last_table_id)
                goto fail;
            epg_ctx->o_last_sched_table_id = max_last_table_id;
            break;
        default:
            goto fail;
    }

    next_version = (epg_info->last_version == 31) ? 0 : epg_info->last_version + 1;
    if (epg_info->last_version != (-1) &&
            (h->version == epg_info->last_version || h->version == next_version))
        goto fail;

    while (p < p_end) {
        const uint8_t *desc_list_end;
        uint16_t descr_loop_len;

        event = NULL;

        // Get event_id
        val = avpriv_dvb_get16(&p, p_end);
        if (val < 0)
            break;

        for (int i = 0; i < nb_events; i++) {
            if (events[i]->event_id == val) {
                event = events[i];
                break;
            }
        }

        if (!event) {
            if (av_reallocp(&events, (nb_events + 1) * sizeof(EPGEvent*)) < 0)
                goto fail;
            events[nb_events] = av_malloc(sizeof(EPGEvent));
            if (!events[nb_events])
                goto fail;
            event = events[nb_events++];
            av_epg_init_event(event);

            event->network_id = network_id;
            event->ts_id = ts_id;
            event->service_id = h->id;
            event->event_id = val;
        }

        memcpy(event->start_time, p, 5);
        p += 5;

        memcpy(event->duration, p, 3);
        p += 3;

        val = avpriv_dvb_get16(&p, p_end);
        if (val < 0)
            break;
        event->running_status = (val >> 13);
        event->free_ca_mode = (val >> 12) & 0x1;
        descr_loop_len = val & 0xfff;

        desc_list_end = p + descr_loop_len;

        for (;;) {
            DvbDescriptor *desc;
            DvbDescriptorHeader h;
            const uint8_t *desc_end;

            if (dvb_parse_descriptor_header(&h, &p, p_end) < 0)
                break;

            if (!(desc = ff_dvb_get_descriptor(&h)))
                break;

            desc_end = p + h.len;
            if (desc_end > desc_list_end)
                break;

            if (epg_handle_descriptor(&h, desc, event, &p, desc_end) < 0)
                break;

            p = desc_end;
        }
        p = desc_list_end;
    }

    for (int i = 0; i < nb_events; i++) {
        event = events[i];

        buf_ref = av_buffer_allocz(sizeof(*event) + AV_INPUT_BUFFER_PADDING_SIZE);
        if (!buf_ref)
            goto fail;
        buf_ref->data = (uint8_t*)event;
        frame->buf[i] = buf_ref;

        sd = av_frame_new_side_data(frame, AV_FRAME_DATA_EPG_EVENT, sizeof(*event));
        sd->data = (uint8_t*)event;
    }

    frame->pts = epg_ctx->pkt->pts;
    frame->pkt_dts = epg_ctx->pkt->dts;
    frame->pkt_size = epg_ctx->pkt->size;

    av_packet_unref(epg_ctx->pkt);
    av_free(events);
    return 0;

fail:
    av_packet_unref(epg_ctx->pkt);
    for (int i = 0; i < nb_events; i++)
        av_free(events[i]);
    av_free(events);
    return AVERROR_INVALIDDATA;
}

static av_cold int decode_init(AVCodecContext *avctx)
{
    EPGContext *epg_ctx = avctx->priv_data;

    for (int i = 0; i < 4; i++)
        epg_ctx->infos[i].last_version = (-1);
    epg_ctx->last_sched_table_id = EITS_START_TID;
    epg_ctx->o_last_sched_table_id = OEITS_START_TID;

    epg_ctx->pkt = av_packet_alloc();
    return epg_ctx->pkt ? 0 : AVERROR(ENOMEM);
}

static av_cold int decode_end(AVCodecContext *avctx)
{
    EPGContext *epg_ctx = avctx->priv_data;
    av_packet_free(&epg_ctx->pkt);
    return 0;
}

static const AVClass epg_class = {
    .class_name = "EPG Decoder",
    .item_name  = av_default_item_name,
    .version    = LIBAVUTIL_VERSION_INT,
};

AVCodec ff_epg_decoder = {
    .name   = "epg",
    .long_name  = NULL_IF_CONFIG_SMALL("EPG (Electronic Program Guide)"),
    .type   = AVMEDIA_TYPE_DATA,
    .id = AV_CODEC_ID_EPG,
    .priv_data_size = sizeof(EPGContext),
    .init = decode_init,
    .close = decode_end,
    .receive_frame = epg_receive_frame,
    .priv_class = &epg_class,
};
