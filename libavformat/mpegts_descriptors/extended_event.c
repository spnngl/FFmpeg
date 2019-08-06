#include <string.h>

#include "libavutil/mem.h"
#include "../mpegts.h"
#include "extended_event.h"
#include "common.h"

static int extended_event_parse(MpegTSDescriptor *desc, const uint8_t **pp, const uint8_t *p_end)
{
    int val;
    const uint8_t *p = *pp, *items_end;
    MpegTSExtendedEventDescriptor *event;

    if (desc->priv)
        desc->free(desc);

    desc->priv = av_mallocz(sizeof(MpegTSExtendedEventDescriptor));
    event = desc->priv;

    val = mpegts_get8(&p, p_end);
    if (val < 0)
        return val;
    event->number = (val >> 4);
    event->last_number = val & bit_mask(4);

    memcpy(event->iso_639_language_code, p, 3);
    p += 3;

    val = mpegts_get8(&p, p_end);
    if (val < 0)
        return val;
    event->length_of_items = val;

    items_end = p + event->length_of_items;

    event->descriptions = NULL;
    while(p < items_end) {
        SimpleLinkedList *cur = av_mallocz(sizeof(SimpleLinkedList));
        MpegTSExtendedEventDescription *extev_desc =
            av_mallocz(sizeof(MpegTSExtendedEventDescription));

        val = mpegts_get8(&p, items_end);
        if (val < 0)
            return val;
        extev_desc->item_description_length = val;

        extev_desc->descr = av_malloc(extev_desc->item_description_length + 1);
        memcpy(extev_desc->descr, p, extev_desc->item_description_length);
        extev_desc->descr[extev_desc->item_description_length] = '\0';
        p += extev_desc->item_description_length;

        val = mpegts_get8(&p, items_end);
        if (val < 0)
            return val;
        extev_desc->item_length = val;

        extev_desc->item = av_malloc(extev_desc->item_length + 1);
        memcpy(extev_desc->item, p, extev_desc->item_length);
        extev_desc->item[extev_desc->item_length] = '\0';
        p += extev_desc->item_length;

        cur->data = extev_desc;
        cur->next = event->descriptions;
        event->descriptions = cur;
    }

    val = mpegts_get8(&p, p_end);
    if (val < 0)
        return val;
    event->text_len = val;

    event->text = av_malloc(event->text_len + 1);
    memcpy(event->text, p, event->text_len);
    event->text[event->text_len] = '\0';
    p += event->text_len;
    *pp = p;

    return 0;
}

static int extended_event_free(MpegTSDescriptor *desc)
{
    if (desc->priv) {
        MpegTSExtendedEventDescriptor *event = desc->priv;
        SimpleLinkedList *head, *next;

        for (head = event->descriptions; head;) {
            MpegTSExtendedEventDescription *d = head->data;

            next = head->next;
            av_free(d->descr);
            av_free(d->item);
            av_free(d);
            av_free(head);
            head = next;
        }
        av_free(event->text);
        av_freep(&desc->priv);
    }
    return 0;
}

static void extended_event_debug(MpegTSDescriptor *desc)
{
    if (desc->priv) {
        int count = 0;
        MpegTSExtendedEventDescriptor *event = desc->priv;
        SimpleLinkedList *head;

        av_log(NULL, AV_LOG_INFO, "Extended event: number(%i), last_number(%i), "
               "iso_639_language_code(%.3s), length_of_items(%i), text_len(%i), "
               "text(%s)\n",event->number, event->last_number, event->iso_639_language_code,
               event->length_of_items, event->text_len, event->text);

        for (head = event->descriptions; head; head = head->next) {
            MpegTSExtendedEventDescription *d = head->data;
            av_log(NULL, AV_LOG_INFO, "Description #%i: item_description_length(%i), "
                   "descr(%s), item_length(%i), item(%s)", count++,
                   d->item_description_length, d->descr, d->item_length, d->item);
        }
    }
}

MpegTSDescriptor extended_event_descriptor = {
    .priv = NULL,
    .parse = extended_event_parse,
    .free = extended_event_free,
    .debug = extended_event_debug
};
