#include "libavutil/mem.h"
#include "../mpegts.h"
#include "stuffing.h"
#include "common.h"

static int stuffing_parse(MpegTSDescriptor *desc, const uint8_t **pp, const uint8_t *p_end)
{
    const uint8_t *p = *pp;
    const int stuff_size = p_end - p;
    MpegTSStuffingDescriptor *stuff;

    if (desc->priv)
        desc->free(desc);

    desc->priv = av_mallocz(sizeof(MpegTSStuffingDescriptor));
    stuff = desc->priv;
    stuff->stuffing_bytes = av_malloc(stuff_size + 1);
    memcpy(stuff->stuffing_bytes, p, stuff_size);
    stuff->stuffing_bytes[stuff_size] = '\0';
    p += stuff_size;
    *pp = p;

    return 0;
}

static int stuffing_free(MpegTSDescriptor *desc)
{
    if (desc->priv) {
        MpegTSStuffingDescriptor *stuff = desc->priv;
        av_free(stuff->stuffing_bytes);
        av_freep(&desc->priv);
    }
    return 0;
}

static void stuffing_debug(MpegTSDescriptor *desc)
{
    if (desc->priv) {
        MpegTSStuffingDescriptor *stuff = desc->priv;
        av_log(NULL, AV_LOG_INFO, "Stuffing descriptor: bytes(%s)\n",
               stuff->stuffing_bytes);
    }
}

MpegTSDescriptor stuffing_descriptor = {
    .priv = NULL,
    .parse = stuffing_parse,
    .free = stuffing_free,
    .debug = stuffing_debug
};
