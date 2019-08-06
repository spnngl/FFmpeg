#include "libavutil/mem.h"
#include "../mpegts.h"
#include "data_broadcast.h"
#include "common.h"

static int data_broadcast_parse(MpegTSDescriptor *desc, const uint8_t **pp, const uint8_t *p_end)
{
    int val;
    const uint8_t *p = *pp;
    MpegTSDataBroadcastDescriptor *dbd;

    if (desc->priv)
        desc->free(desc);

    desc->priv = av_mallocz(sizeof(MpegTSDataBroadcastDescriptor));
    dbd = desc->priv;

    val = mpegts_get16(&p, p_end);
    if (val < 0)
        return val;
    dbd->data_broadcast_id = val;

    val = mpegts_get8(&p, p_end);
    if (val < 0)
        return val;
    dbd->component_tag = val;

    val = mpegts_get8(&p, p_end);
    if (val < 0)
        return val;
    dbd->selector_len = val;

    dbd->selector = av_malloc(dbd->selector_len + 1);
    memcpy(dbd->selector, p, dbd->selector_len);
    dbd->selector[dbd->selector_len] = '\0';
    p += dbd->selector_len;

    memcpy(dbd->iso_639_language_code, p, 3);
    p += 3;

    val = mpegts_get8(&p, p_end);
    if (val < 0)
        return val;
    dbd->text_len = val;


    dbd->text = av_malloc(dbd->text_len + 1);
    memcpy(dbd->selector, p, dbd->text_len);
    dbd->selector[dbd->text_len] = '\0';
    p += dbd->text_len;

    *pp = p;

    return 0;
}

static int data_broadcast_free(MpegTSDescriptor *desc)
{
    if (desc->priv) {
        MpegTSDataBroadcastDescriptor *dbd = desc->priv;
        av_free(dbd->selector);
        av_free(dbd->text);
        av_free(dbd);
        desc->priv = NULL;
    }
    return 0;
}

static void data_broadcast_debug(MpegTSDescriptor *desc)
{
    if (desc->priv) {
        MpegTSDataBroadcastDescriptor *dbd = desc->priv;
        av_log(NULL, MPEGTS_DESCRIPTORS_LOG, "Data broadcast descriptor: id(%i), component_tag(%i), "
               "selector(%s), iso_639_language_code(%.3s), text(%s)\n",
               dbd->data_broadcast_id, dbd->component_tag, dbd->selector,
               dbd->iso_639_language_code, dbd->text);
    }
}

MpegTSDescriptor data_broadcast_descriptor = {
    .priv = NULL,
    .parse = data_broadcast_parse,
    .free = data_broadcast_free,
    .debug = data_broadcast_debug
};
