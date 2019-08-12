#include "libavutil/mem.h"
#include "../mpegts.h"
#include "content.h"
#include "common.h"

static int content_parse(MpegTSDescriptor *desc, const uint8_t **pp, const uint8_t *p_end)
{
    int val;
    const uint8_t *p = *pp;
    MpegTSContentDescriptor *content;

    if (desc->priv)
        desc->free(desc);

    desc->priv = av_mallocz(sizeof(MpegTSContentDescriptor));
    content = desc->priv;

    content->nb_descriptions = (p_end - p) / sizeof(MpegTSContentDescription);
    content->descriptions = av_mallocz(content->nb_descriptions * sizeof(MpegTSContentDescription*));

    for(int i = 0; i < content->nb_descriptions; i++) {
        MpegTSContentDescription *d;

        content->descriptions[i] = av_mallocz(sizeof(MpegTSContentDescription));
        d = content->descriptions[i];

        val = mpegts_get8(&p, p_end);
        if (val < 0)
            break;
        d->content_nibble_lvl_1 = (val >> 4) & bit_mask(4);
        d->content_nibble_lvl_2 = val & bit_mask(4);

        val = mpegts_get8(&p, p_end);
        if (val < 0)
            break;
        d->user_byte = val;
    }

    return (val < 0);
}

static int content_free(MpegTSDescriptor *desc)
{
    if (desc->priv) {
        MpegTSContentDescriptor *content = desc->priv;

        for(int i = 0; i < content->nb_descriptions; i++)
            av_free(content->descriptions[i]);
        av_free(content->descriptions);
        av_freep(&desc->priv);
    }
    return 0;
}

static void content_debug(MpegTSDescriptor *desc)
{
    if (desc->priv) {
        MpegTSContentDescriptor *content = desc->priv;

        av_log(NULL, MPEGTS_DESCRIPTORS_LOG, "Content descriptor: \n");

        for(int i = 0; i < content->nb_descriptions; i++) {
            MpegTSContentDescription *d = content->descriptions[i];
            av_log(NULL, MPEGTS_DESCRIPTORS_LOG, "Content #%i: "
                    "nibble1(%i), nibble2(%i), user_byte(%i)\n",
                    i, d->content_nibble_lvl_1,
                    d->content_nibble_lvl_2, d->user_byte);
        }

    }
}

MpegTSDescriptor content_descriptor = {
    .priv = NULL,
    .parse = content_parse,
    .free = content_free,
    .debug = content_debug
};
