#include "mpegts_epg.h"
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

    if (short_event->event_name_len) {
        event->event_name = av_malloc(short_event->event_name_len + 1);
        strcpy(event->event_name, short_event->event_name);
    }

    if (short_event->text_len) {
        event->short_event_description = av_malloc(short_event->text_len + 1);
        strcpy(event->short_event_description, short_event->text);
    }

    return 0;
}

static int extended_event_handle(MpegTSDescriptor *desc, EPGEvent *event)
{
    MpegTSExtendedEventDescriptor *extended_event = desc->priv;
    SimpleLinkedList *head;

    if (extended_event->text_len) {
        if (event->long_event_description &&
                extended_event->number == (event->last_number + 1))
        {
            int new_size = strlen(event->long_event_description) +
                strlen(extended_event->text) + 1;
            if (av_reallocp(&event->long_event_description, new_size) < 0)
                return AVERROR(ENOMEM);
            strcpy(event->long_event_description + strlen(event->long_event_description),
                    extended_event->text);
        } else {
            if (av_reallocp(&event->long_event_description, extended_event->text_len + 1) < 0)
                return AVERROR(ENOMEM);
            strcpy(event->long_event_description, extended_event->text);
        }
    }

    for (head = extended_event->descriptions; head; head = head->next) {
        MpegTSExtendedEventDescription *d = head->data;
        int old_size = event->items ? strlen(event->items) : 0;
        int size = d->item_description_length + d->item_length + 3;

        if (av_reallocp(&event->items, size + old_size) < 0)
            break;
        snprintf(event->items + old_size, size, "%s: %s", d->descr, d->item);
    }
    event->last_number = extended_event->number;

    return 0;
}

static int parental_rating_handle(MpegTSDescriptor *desc, EPGEvent *event)
{
    MpegTSParentalRatingDescriptor *prd = desc->priv;

    // For now we only look at the first age indicated
    if (prd->nb_descriptions)
        event->min_age = mpegts_parental_rating_min_age(prd->descriptions[0]);

    return 0;
}

static int component_handle(MpegTSDescriptor *desc, EPGEvent *event)
{
    MpegTSComponentDescriptor *comp = desc->priv;

    if (!event->component.short_desc)
        event->component.short_desc = av_malloc(64);
    strncpy(event->component.short_desc, comp->text, 64);

    if (!event->component.long_desc)
        event->component.long_desc = av_malloc(256);
    mpegts_component_description(comp, event->component.long_desc);

    return 0;
}

static int content_handle(MpegTSDescriptor *desc, EPGEvent *event)
{
    MpegTSContentDescriptor *content = desc->priv;
    // For now we only use the first content description. Presence of multiple
    // descriptions seems rare.
    MpegTSContentDescription *content_desc =
        content->nb_descriptions > 0 ? content->descriptions[0] : NULL;

    if (content_desc) {
        event->content.short_desc = mpegts_content_simple_description(content_desc);
        event->content.long_desc = mpegts_content_detailed_description(content_desc);
    }
    return 0;
}

static int time_shifted_event_handle(MpegTSDescriptor *desc, EPGEvent *event)
{
    MpegTSTimeShiftedEventDescriptor *tsed = desc->priv;
    event->shift_from.service_id = tsed->reference_service_id;
    event->shift_from.event_id = tsed->reference_event_id;
    av_log(NULL, AV_LOG_ERROR, "Time shift for event %i !!\n", event->id);
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

void epg_event_show(EPGEvent *event, int log_level)
{
    char* start_time = epg_event_start_time_str(event->start_time);
    char* duration = epg_event_duration_str(event->duration);
    char pil[32];
    mpegts_pdc_date_str(event->pil, pil);

    av_log(NULL, log_level, "EPGEvent: id(%i), shift_from_service_id(%i), "
           "shift_from_event_id(%i), start_time(%s), duration(%s), running_status(%s), "
           "free_ca_mode(%i), min_age(%s), PIL(%s), event_name(%s), short_event_descr(%s), "
           "long_event_descr(%s), items(%s), short_component_description(%s), "
           "long_component_description(%s), short_content_description(%s), "
           "long_content_description(%s), sb_size(%s), sb_leak_rate(%s)\n\n\n",
           event->id, event->shift_from.service_id, event->shift_from.event_id,
           start_time, duration, mpegts_running_status_str(event->running_status),
           event->free_ca_mode, event->min_age, pil, event->event_name,
           event->short_event_description, event->long_event_description, event->items,
           event->component.short_desc, event->component.long_desc,
           event->content.short_desc, event->content.long_desc,
           mpegts_sb_size_str(event->sb.size), mpegts_sb_leak_rate_str(event->sb.leak_rate));

    av_free(start_time);
    av_free(duration);
}

EPGData* epg_get_data(SimpleLinkedList **epg, const uint16_t id)
{
    EPGData *cur;
    SimpleLinkedList *head;

    for (head = *epg; head; head = head->next) {
        cur = head->data;
        if (cur->id == id)
            return cur;
    }

    head = av_mallocz(sizeof(SimpleLinkedList));
    cur = av_mallocz(sizeof(EPGData));
    cur->id = id;
    cur->head = NULL;

    head->data = cur;
    head->next = *epg;
    *epg = head;

    return cur;
}

EPGEvent* epg_get_event(SimpleLinkedList **epg,
                        const uint16_t transport_stream_id,
                        const uint16_t original_network_id,
                        const uint16_t event_id)
{
    EPGEvent *event;
    EPGData *cur;
    SimpleLinkedList *head;

    cur = epg_get_data(epg, transport_stream_id);
    cur = epg_get_data(&cur->head, original_network_id);

    head = cur->head;
    while (head) {
        event = head->data;
        if (event->id == event_id)
            return event;
        head = head->next;
    }

    head = av_mallocz(sizeof(SimpleLinkedList));
    event = av_mallocz(sizeof(EPGEvent));
    event->id = event_id;
    event->last_number = 0;
    event->shift_from.service_id = event->shift_from.event_id = 0;

    head->data = event;
    head->next = cur->head;
    cur->head = head;

    return event;
}

void epg_free_event(EPGEvent *event)
{
    av_free(event->min_age);
    av_free(event->event_name);
    av_free(event->short_event_description);
    av_free(event->long_event_description);
    av_free(event->items);
    // NB: Do not try to free component & content
    av_free(event);
}

void epg_free_all(SimpleLinkedList **epg)
{
    EPGData *epg_tr, *epg_or;
    SimpleLinkedList *head_tr, *head_or, *head_ev, *next;

    for (head_tr = *epg; head_tr;) {
        epg_tr = head_tr->data;
        for (head_or = epg_tr->head; head_or;) {
            epg_or = head_or->data;
            for (head_ev = epg_or->head; head_ev;) {
                epg_free_event(head_ev->data);
                next = head_ev->next;
                av_freep(&head_ev);
                head_ev = next;
            }
            next = head_or->next;
            av_freep(&head_or);
            av_freep(&epg_or);
            head_or = next;
        }
        next = head_tr->next;
        av_freep(&head_tr);
        av_freep(&epg_tr);
        head_tr = next;
    }
}

#define EPG_LINE_SIZE 2048

static int epg_event_line_csv(EPGEvent *event, char* dst, const uint16_t transport_stream_id,
                              const uint16_t original_network_id)
{
    int size;
    char *duration = epg_event_duration_str(event->duration);
    char *start_time = epg_event_start_time_str(event->start_time);

    size = snprintf(
        dst, EPG_LINE_SIZE, "%i;%i;%i;%s;%s;%s;%i;%s;%s;%s;%s;%s;%s;%s;%s;%s\n",
        transport_stream_id, original_network_id,
        event->id, start_time, duration, mpegts_running_status_str(event->running_status),
        event->free_ca_mode, event->min_age, event->event_name, event->short_event_description,
        event->long_event_description, event->items, event->component.short_desc,
        event->component.long_desc, event->content.short_desc,
        event->content.long_desc
    );

    av_free(duration);
    av_free(start_time);

    return (size > EPG_LINE_SIZE) ? EPG_LINE_SIZE : size;
}

void epg_data_to_csv(SimpleLinkedList *epg, const char* path)
{
    FILE *csv;
    int size;
    char dst[EPG_LINE_SIZE];
    EPGData *epg_tr, *epg_or;
    EPGEvent *event;
    SimpleLinkedList *head_tr, *head_or, *head_ev;
    const char* columns = "transport_stream_id;original_network_id;event_id;start_time;"
                          "duration;running_status;free_ca_mode;min_age;event_name;"
                          "short_event_description;long_event_description;items;"
                          "short_component_description;long_component_description;"
                          "short_content_description;long_content_description\n\0";

    if (!(csv = fopen(path, "w"))) {
        av_log(NULL, AV_LOG_ERROR, "Could not write epg csv file %s.\n", path);
        return;
    }

    fwrite(columns, strlen(columns), 1, csv);

    for (head_tr = epg; head_tr; head_tr = head_tr->next) {
        epg_tr = head_tr->data;
        for (head_or = epg_tr->head; head_or; head_or = head_or->next) {
            epg_or = head_or->data;
            for (head_ev = epg_or->head; head_ev; head_ev = head_ev->next) {
                event = head_ev->data;
                size = epg_event_line_csv(event, dst, epg_tr->id, epg_or->id);
                fwrite(dst, size, 1, csv);
            }
        }
    }

    fclose(csv);
}

char* epg_bcd_to_str(const uint32_t bcd)
{
    char* ret;

    ret = av_malloc(16);
    snprintf(ret, 16, "%i%i:%i%i:%i%i", (bcd >> 20) & bit_mask(4),
             (bcd >> 16) & bit_mask(4), (bcd >> 12) & bit_mask(4),
             (bcd >> 8) & bit_mask(4), (bcd >> 4) & bit_mask(4),
             bcd & bit_mask(4));

    return ret;
}

char* epg_mjd_to_str(const uint16_t mjd)
{
    char* ret;
    uint16_t year, month, day, k;

    ret = av_malloc(16);

    year = (int)((mjd - 15078.2) / 365.25);
    month = (int)((mjd - 14956.1 - (int)(year * 365.25)) / 30.6001);
    day = mjd - 14956 - (int)(year * 365.25) - (int)(month * 30.6001);

    k = !!(month == 14 || month == 15);
    year += k;
    month -= 1 + 12 * k;

    snprintf(ret, 16, "%i/%i/%i", year + 1900, month, day);
    return ret;
}

char* epg_event_start_time_str(uint8_t start_time[5])
{
    char *ymd, *h, *ret;
    uint16_t mjd = (start_time[0] << 8) | start_time[1];
    uint32_t bcd = (start_time[2] << 16) | (start_time[3] << 8) | start_time[4];

    ymd = epg_mjd_to_str(mjd);
    h = epg_bcd_to_str(bcd);

    ret = av_malloc(32);
    snprintf(ret, 32, "%s %s", ymd, h);
    return ret;
}

char* epg_event_duration_str(uint8_t duration[3])
{
    uint32_t bcd = (duration[0] << 16) | (duration[1] << 8) | duration[2];
    return epg_bcd_to_str(bcd);
}
