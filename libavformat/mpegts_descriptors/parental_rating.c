#include "libavutil/mem.h"
#include "../mpegts.h"
#include "parental_rating.h"
#include "common.h"

static int parental_rating_parse(MpegTSDescriptor *desc, const uint8_t **pp, const uint8_t *p_end)
{
    int val, i = 0;
    const uint8_t *p = *pp;
    MpegTSParentalRatingDescriptor *prd;

    if (desc->priv)
        desc->free(desc);

    desc->priv = av_mallocz(sizeof(MpegTSParentalRatingDescriptor));
    prd = desc->priv;
    prd->nb_descriptions = (p_end - p) / sizeof(MpegTSParentalRatingDescription);
    prd->descriptions = av_mallocz(prd->nb_descriptions * sizeof(MpegTSParentalRatingDescription*));

    while(p < p_end && i < prd->nb_descriptions) {
        MpegTSParentalRatingDescription *d;

        prd->descriptions[i] = av_mallocz(sizeof(MpegTSParentalRatingDescription));
        d = prd->descriptions[i];

        memcpy(d->country_code, p, 3);
        p += 3;

        val = mpegts_get8(&p, p_end);
        if (val < 0)
            break;
        d->rating = val;
        i++;
    }

    *pp = p;

    return 0;
}

static int parental_rating_free(MpegTSDescriptor *desc)
{
    if (desc->priv) {
        MpegTSParentalRatingDescriptor *prd = desc->priv;

        for (int i = 0; i < prd->nb_descriptions; i++)
            av_free(prd->descriptions[i]);
        av_free(prd->descriptions);
        av_freep(&desc->priv);
    }
    return 0;
}

static void parental_rating_debug(MpegTSDescriptor *desc)
{
    if (desc->priv) {
        MpegTSParentalRatingDescriptor *prd = desc->priv;

        for (int i = 0; i < prd->nb_descriptions; i++) {
            MpegTSParentalRatingDescription *d = prd->descriptions[i];
            char *min_age = mpegts_parental_rating_min_age(d);
            av_log(NULL, AV_LOG_INFO, "Parental rating #%i: country_code(%.3s), "
                   "rating(%i) -> min_age = %s\n", i, d->country_code,
                   d->rating, min_age);
            av_free(min_age);
        }
    }
}

char* mpegts_parental_rating_min_age(MpegTSParentalRatingDescription * const description)
{
    int size;
    char *ret;

    switch(description->rating) {
        // Defined by broadcaster
        case 0x10 ... 0xFF:
        // Undefined value
        case 0x00:
            size = sizeof("undefined") + 1;
            ret = av_malloc(size);
            memcpy(ret, "undefined\0", size);
            break;
        case 0x01 ... 0x0F:
            ret = av_malloc(3);
            snprintf(ret, 3, "%d", description->rating + 3);
            break;
        default:
            ret = NULL;
            break;
    }
    return ret;
}

MpegTSDescriptor parental_rating_descriptor = {
    .priv = NULL,
    .parse = parental_rating_parse,
    .free = parental_rating_free,
    .debug = parental_rating_debug
};
