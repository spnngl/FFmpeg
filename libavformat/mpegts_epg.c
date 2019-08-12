#include "libavutil/epg.h"
#include "mpegts.h"
#include "mpegts_descriptors/common.h"
#include "mpegts_descriptors/service.h"
#include "mpegts_descriptors/short_event.h"
#include "mpegts_descriptors/extended_event.h"
#include "mpegts_descriptors/parental_rating.h"
#include "mpegts_descriptors/stuffing.h"
#include "mpegts_descriptors/time_shifted_event.h"
#include "mpegts_descriptors/component.h"
#include "mpegts_descriptors/ca_identifier.h"
#include "mpegts_descriptors/content.h"
#include "mpegts_descriptors/short_smoothing_buffer.h"
#include "mpegts_descriptors/pdc.h"

static int short_event_handle(MpegTSDescriptor *desc, EPGEvent *event)
{
    MpegTSShortEventDescriptor *short_event = desc->priv;
    if (short_event->event_name_len)
        strncpy(event->event_name, short_event->event_name, sizeof(event->event_name));
    if (short_event->text_len)
        strncpy(event->short_event_description, short_event->text,
                sizeof(event->short_event_description));
    return 0;
}

static int extended_event_handle(MpegTSDescriptor *desc, EPGEvent *event)
{
    MpegTSExtendedEventDescriptor *extended_event = desc->priv;
    SimpleLinkedList *head;

    if (extended_event->text_len && extended_event->number == (event->last_number + 1))
    {
            int old_size = strlen(event->long_event_description),
                space_left = sizeof(event->long_event_description) - old_size,
                text_len = strlen(event->long_event_description);

            if (text_len > space_left)
                text_len = space_left;

            strncpy(event->long_event_description + old_size,
                    extended_event->text, text_len);
    }

    for (head = extended_event->descriptions; head; head = head->next) {
        MpegTSExtendedEventDescription *d = head->data;
        int old_size = strlen(event->items),
            space_left = sizeof(event->items) - old_size,
            size = d->item_description_length + d->item_length + 3;

        if (size > space_left)
            size = space_left;

        snprintf(event->items + old_size, size, "%s: %s,", d->descr, d->item);
    }
    event->last_number = extended_event->number;

    return 0;
}

static int parental_rating_handle(MpegTSDescriptor *desc, EPGEvent *event)
{
    MpegTSParentalRatingDescriptor *prd = desc->priv;
    MpegTSParentalRatingDescription *prdd;

    // For now we only look at the first age indicated
    if (prd->nb_descriptions) {
        prdd = prd->descriptions[0];
        event->rating = prdd->rating;
        memcpy(event->country_code, prdd->country_code, 3);
    }

    return 0;
}

static int component_handle(MpegTSDescriptor *desc, EPGEvent *event)
{
    MpegTSComponentDescriptor *comp = desc->priv;

    event->component.stream_content_ext = comp->stream_content_ext;
    event->component.stream_content = comp->stream_content;
    event->component.type = comp->component_type;
    event->component.tag = comp->component_tag;
    strncpy(event->component.text, comp->text, 32);

    return 0;
}

static int content_handle(MpegTSDescriptor *desc, EPGEvent *event)
{
    MpegTSContentDescriptor *content = desc->priv;
    // For now we only use the first content description. Presence of multiple
    // descriptions seems rare.
    MpegTSContentDescription *content_desc =
        content->nb_descriptions > 0 ? content->descriptions[0] : NULL;

    event->content.nibble_lvl_1 = content_desc->content_nibble_lvl_1;
    event->content.nibble_lvl_2 = content_desc->content_nibble_lvl_2;
    event->content.user_byte = content_desc->user_byte;

    return 0;
}

static int time_shifted_event_handle(MpegTSDescriptor *desc, EPGEvent *event)
{
    MpegTSTimeShiftedEventDescriptor *tsed = desc->priv;
    event->shift_from.service_id = tsed->reference_service_id;
    event->shift_from.event_id = tsed->reference_event_id;
    av_log(NULL, AV_LOG_ERROR, "Time shift for event %i !!\n", event->event_id);
    return 0;
}

static int sb_buffer_handle(MpegTSDescriptor *desc, EPGEvent *event)
{
    MpegTSShortSmoothingBufferDescriptor *ssb = desc->priv;
    event->sb.size = ssb->sb_size;
    event->sb.leak_rate = ssb->sb_leak_rate;
    return 0;
}

static int pdc_handle(MpegTSDescriptor *desc, EPGEvent *event)
{
    MpegTSPDCDescriptor *pdc = desc->priv;
    event->pil = pdc->programme_identification_label;
    return 0;
}

static int(*epg_descriptors_arr[256])(MpegTSDescriptor *desc, EPGEvent *event) = {
    [0x00 ... 0xFF] = NULL,
    /* Below all supported descriptor according to dvb specs */
    [MPEGTS_DESCRIPTOR_TAG_SHORT_EVENT] = &short_event_handle,
    [MPEGTS_DESCRIPTOR_TAG_EXTENDED_EVENT] = &extended_event_handle,
    [MPEGTS_DESCRIPTOR_TAG_PARENTAL_RATING] = &parental_rating_handle,
    [MPEGTS_DESCRIPTOR_TAG_STUFFING] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_LINKAGE] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_TIME_SHIFTED_EVENT] = &time_shifted_event_handle,
    [MPEGTS_DESCRIPTOR_TAG_COMPONENT] = &component_handle,
    [MPEGTS_DESCRIPTOR_TAG_CA_IDENTIFIER] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_CONTENT] = &content_handle,
    [MPEGTS_DESCRIPTOR_TAG_TELEPHONE] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_MULTILINGUAL_COMPONENT] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_PRIVATE_DATA_SPECIFIER] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_SHORT_SMOOTHING_BUFFER] = &sb_buffer_handle,
    [MPEGTS_DESCRIPTOR_TAG_DATA_BROADCAST] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_PDC] = &pdc_handle,
    [MPEGTS_DESCRIPTOR_TAG_CONTENT_IDENTIFIER] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_XAIT_LOCATION] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_FTA_CONTENT_MANAGEMENT] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_EXTENSION] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_TVA_ID] = NULL,
};

int epg_handle_descriptor(MpegTSDescriptorHeader *h, MpegTSDescriptor *desc,
                          EPGEvent *event, const uint8_t **pp,
                          const uint8_t *p_end)
{
    int ret = AVERROR_PATCHWELCOME;
    int(*handle)(MpegTSDescriptor*, EPGEvent*) = epg_descriptors_arr[h->tag];

    if (handle) {
        ret = desc->parse(desc, pp, p_end);
        if (ret < 0)
            return ret;

        ret = handle(desc, event);
        if (ret < 0)
            return ret;
    }
    return ret;
}
