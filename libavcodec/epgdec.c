#include "avcodec.h"
#include "internal.h"
#include "decode.h"
#include "libavutil/common.h"
#include "libavutil/dvb.h"
#include "libavutil/dvbdescriptors.h"
#include "libavformat/mpegts.h"

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

static int epg_handle_descriptor(DvbDescriptorHeader *h, DvbDescriptor *desc,
                                 EPGSubTable *subtable, const uint8_t **pp,
                                 const uint8_t *p_end)
{
    void *data = desc->parse(desc, pp, p_end);
    if (!data)
        return AVERROR_INVALIDDATA;
    memcpy(data, h, sizeof(DvbDescriptorHeader));

    if (av_reallocp_array(&subtable->descriptors, (subtable->nb_descriptors + 1), sizeof(void*)) < 0) {
        desc->free(data);
        return AVERROR(ENOMEM);
    }
    subtable->descriptors[subtable->nb_descriptors++] = data;

    return 0;
}

static int epg_receive_frame(AVCodecContext *avctx, AVFrame *frame)
{
    EPGContext *epg_ctx = avctx->priv_data;
    EpgTidInfo *epg_info;
    DvbSectionHeader h1, *h = &h1;
    const uint8_t *p, *p_end;
    int val, ret, max_last_table_id;
    uint8_t next_version;
    AVFrameSideData *sd;
    AVBufferRef *buf_ref;
    EPGTable *table;

    if (!epg_ctx->pkt->data) {
        ret = ff_decode_get_packet(avctx, epg_ctx->pkt);
        if (ret < 0)
            return ret;
    }

    p       = epg_ctx->pkt->data;
    p_end   = p + epg_ctx->pkt->size - 4;

    if (avpriv_dvb_parse_section_header(h, &p, p_end) < 0)
        goto fail;

    table = av_epg_table_alloc();
    if (!table)
        goto fail;
    table->h = h1;

    val = avpriv_dvb_get16(&p, p_end);
    if (val < 0)
        goto fail;
    table->ts_id = val;

    val = avpriv_dvb_get16(&p, p_end);
    if (val < 0)
        goto fail;
    table->network_id = val;

    val = avpriv_dvb_get8(&p, p_end);
    if (val < 0)
        goto fail;
    table->segment_last_section_num = val;

    val = avpriv_dvb_get8(&p, p_end);
    if (val < 0)
        goto fail;
    table->last_tid = val;

    // Check eit data
    switch (h->tid) {
        case EIT_TID:
            epg_info = &epg_ctx->infos[0];
            if (table->last_tid != EIT_TID || h->last_sec_num != table->segment_last_section_num)
                goto fail;
            break;
        case OEIT_TID:
            epg_info = &epg_ctx->infos[1];
            if (table->last_tid != OEIT_TID || h->last_sec_num != table->segment_last_section_num)
                goto fail;
            break;
        case EITS_START_TID ... EITS_END_TID:
            epg_info = &epg_ctx->infos[2];
            if (table->segment_last_section_num != h->sec_num)
                goto fail;
            max_last_table_id = FFMAX(table->last_tid, epg_ctx->last_sched_table_id);
            if (table->last_tid != max_last_table_id)
                goto fail;
            epg_ctx->last_sched_table_id = max_last_table_id;
            break;
        case OEITS_START_TID ... OEITS_END_TID:
            epg_info = &epg_ctx->infos[3];
            if (table->segment_last_section_num != h->sec_num)
                goto fail;
            max_last_table_id = FFMAX(table->last_tid, epg_ctx->o_last_sched_table_id);
            if (table->last_tid != max_last_table_id)
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

    // Subtables
    while (p < p_end) {
        const uint8_t *desc_list_end;
        EPGSubTable *subtable;

        if (av_reallocp_array(&table->subtables, (table->nb_subtables + 1), sizeof(EPGSubTable*)) < 0)
            goto fail;

        table->subtables[table->nb_subtables] = av_epg_subtable_alloc();
        if (!table->subtables[table->nb_subtables])
            goto fail;
        subtable = table->subtables[table->nb_subtables++];

        // Get event_id
        val = avpriv_dvb_get16(&p, p_end);
        if (val < 0)
            break;
        subtable->event_id = val;

        memcpy(subtable->start_time, p, 5);
        p += 5;

        memcpy(subtable->duration, p, 3);
        p += 3;

        val = avpriv_dvb_get16(&p, p_end);
        if (val < 0)
            break;
        subtable->running_status = (val >> 13);
        subtable->free_ca_mode = (val >> 12) & 0x1;
        subtable->desc_loop_len = val & 0xfff;

        desc_list_end = p + subtable->desc_loop_len;

        // Descriptors
        for (;;) {
            DvbDescriptor *desc;
            DvbDescriptorHeader h;
            const uint8_t *desc_end;

            if (av_dvb_parse_descriptor_header(&h, &p, p_end) < 0)
                break;

            if (!(desc = (DvbDescriptor*)av_dvb_get_descriptor(&h)))
                continue;

            desc_end = p + h.len;
            if (desc_end > desc_list_end)
                break;

            if (epg_handle_descriptor(&h, desc, subtable, &p, desc_end) < 0)
                break;

            p = desc_end;
        }
        p = desc_list_end;
    }

    // CRC32
    val = avpriv_dvb_get32(&p_end, (p_end + 4));
    if (val < 0)
        goto fail;
    table->crc = val;

    buf_ref = av_buffer_allocz(sizeof(*table) + AV_INPUT_BUFFER_PADDING_SIZE);
    if (!buf_ref)
        goto fail;
    buf_ref->data = (uint8_t*)table;
    frame->buf[0] = buf_ref;

    sd = av_frame_new_side_data(frame, AV_FRAME_DATA_EPG_TABLE, sizeof(*table));
    sd->data = (uint8_t*)table;

    frame->pts = epg_ctx->pkt->pts;
    frame->pkt_dts = epg_ctx->pkt->dts;
    frame->pkt_size = epg_ctx->pkt->size;

    av_packet_unref(epg_ctx->pkt);
    return 0;

fail:
    av_packet_unref(epg_ctx->pkt);
    av_epg_table_free(&table);
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
