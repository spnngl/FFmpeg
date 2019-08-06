#include "libavutil/mem.h"
#include "../mpegts.h"
#include "private_data_specifier.h"
#include "common.h"

static int private_data_specifier_parse(MpegTSDescriptor *desc, const uint8_t **pp, const uint8_t *p_end)
{
    int val;
    const uint8_t *p = *pp;
    MpegTSPrivateDataSpecifierDescriptor *pds;

    if (desc->priv)
        desc->free(desc);

    desc->priv = av_mallocz(sizeof(MpegTSPrivateDataSpecifierDescriptor));
    pds = desc->priv;

    val = mpegts_get32(&p, p_end);
    if (val < 0)
        return val;
    pds->specifier = val;

    return 0;
}

static int private_data_specifier_free(MpegTSDescriptor *desc)
{
    if (desc->priv) {
        MpegTSPrivateDataSpecifierDescriptor *pds = desc->priv;
        av_free(pds);
        desc->priv = NULL;
    }
    return 0;
}

static void private_data_specifier_debug(MpegTSDescriptor *desc)
{
    if (desc->priv) {
        MpegTSPrivateDataSpecifierDescriptor *pds = desc->priv;
        av_log(NULL, MPEGTS_DESCRIPTORS_LOG, "Private data specifier descriptor: "
               "specifier(%i)\n", pds->specifier);
    }
}

MpegTSDescriptor private_data_specifier_descriptor = {
    .priv = NULL,
    .parse = private_data_specifier_parse,
    .free = private_data_specifier_free,
    .debug = private_data_specifier_debug
};
