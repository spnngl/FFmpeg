#include "libavutil/mem.h"
#include "../mpegts.h"
#include "short_event.h"
#include "common.h"

static int short_event_parse(MpegTSDescriptor *desc, const uint8_t **pp, const uint8_t *p_end)
{
    int val;
    const uint8_t *p = *pp;
    MpegTSShortEventDescriptor *short_event;

    if (desc->priv)
        desc->free(desc);

    desc->priv = av_mallocz(sizeof(MpegTSShortEventDescriptor));
    short_event = desc->priv;

    memcpy(short_event->iso_639_language_code, p, 3);
    p += 3;

    val = mpegts_get8(&p, p_end);
    if (val < 0)
        return val;
    short_event->event_name_len = val;

    if (short_event->event_name_len) {
        short_event->event_name = av_malloc(short_event->event_name_len + 1);
        memcpy(short_event->event_name, p, short_event->event_name_len);
        short_event->event_name[short_event->event_name_len] = '\0';
        p += short_event->event_name_len;
    }

    val = mpegts_get8(&p, p_end);
    if (val < 0)
        return val;
    short_event->text_len = val;

    if (short_event->text_len) {
        short_event->text = av_malloc(short_event->text_len + 1);
        memcpy(short_event->text, p, short_event->text_len);
        short_event->text[short_event->text_len] = '\0';
        p += short_event->text_len;
    }
    *pp = p;

    return 0;
}

static int short_event_free(MpegTSDescriptor *desc)
{
    if (desc->priv) {
        MpegTSShortEventDescriptor *short_event = desc->priv;
        av_free(short_event->event_name);
        av_free(short_event->text);
        av_freep(&desc->priv);
    }
    return 0;
}

static void short_event_debug(MpegTSDescriptor *desc)
{
    if (desc->priv) {
        MpegTSShortEventDescriptor *short_event = desc->priv;
        av_log(NULL, AV_LOG_INFO, "Short event: iso_639_language_code(%.3s), "
               "event_name(%s), text(%s)\n", short_event->iso_639_language_code,
               short_event->event_name, short_event->text);
    }
}

MpegTSDescriptor short_event_descriptor = {
    .priv = NULL,
    .parse = short_event_parse,
    .free = short_event_free,
    .debug = short_event_debug
};
