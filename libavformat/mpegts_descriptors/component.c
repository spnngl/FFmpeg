#include "libavutil/mem.h"
#include "../mpegts.h"
#include "component.h"
#include "common.h"

static int component_parse(MpegTSDescriptor *desc, const uint8_t **pp, const uint8_t *p_end)
{
    int val, text_len;
    const uint8_t *p = *pp;
    MpegTSComponentDescriptor *comp;

    if (desc->priv)
        desc->free(desc);

    desc->priv = av_mallocz(sizeof(MpegTSComponentDescriptor));
    comp = desc->priv;

    val = mpegts_get8(&p, p_end);
    if (val < 0)
        return val;
    comp->stream_content_ext = (val >> 4);
    comp->stream_content = val & bit_mask(4);

    val = mpegts_get8(&p, p_end);
    if (val < 0)
        return val;
    comp->component_type = val;

    val = mpegts_get8(&p, p_end);
    if (val < 0)
        return val;
    comp->component_tag = val;

    memcpy(comp->iso_639_language_code, p, 3);
    p += 3;

    text_len = p_end - p;
    comp->text = av_malloc(text_len + 1);
    memcpy(comp->text, p, text_len);
    comp->text[text_len] = '\0';
    p += text_len;
    *pp = p;

    return 0;
}

static int component_free(MpegTSDescriptor *desc)
{
    if (desc->priv) {
        MpegTSComponentDescriptor *comp = desc->priv;
        av_free(comp->text);
        av_freep(&desc->priv);
    }
    return 0;
}

static void component_debug(MpegTSDescriptor *desc)
{
    if (desc->priv) {
        MpegTSComponentDescriptor *comp = desc->priv;

        av_log(NULL, MPEGTS_DESCRIPTORS_LOG, "Component descriptor: "
                "stream_content_ext(%i), stream_content(%i), "
                "component_type(%i), component_tag(%i), "
                "iso_639_language_code(%.3s), text(%s)\n",
                comp->stream_content_ext, comp->stream_content,
                comp->component_type, comp->component_tag,
                comp->iso_639_language_code, comp->text);
    }
}

MpegTSDescriptor component_descriptor = {
    .priv = NULL,
    .parse = component_parse,
    .free = component_free,
    .debug = component_debug
};
