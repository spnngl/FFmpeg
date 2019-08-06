#include "libavutil/mem.h"
#include "../mpegts.h"
#include "ca_identifier.h"
#include "common.h"

static int ca_identifier_parse(MpegTSDescriptor *desc, const uint8_t **pp, const uint8_t *p_end)
{
    const uint8_t *p = *pp;
    MpegTSCAIdentifierDescriptor *caid;

    if (desc->priv)
        desc->free(desc);

    desc->priv = av_mallocz(sizeof(MpegTSCAIdentifierDescriptor));
    caid = desc->priv;

    caid->nb_ids = (p_end - p) >> 1;
    caid->ca_system_id = av_mallocz(caid->nb_ids * sizeof(uint16_t));

    for(int i = 0; i < caid->nb_ids; i++) {
        int val = mpegts_get16(&p, p_end);
        if (val < 0)
            break;
        caid->ca_system_id[i] = val;
    }

    return 0;
}

static int ca_identifier_free(MpegTSDescriptor *desc)
{
    if (desc->priv) {
        MpegTSCAIdentifierDescriptor *caid = desc->priv;
        av_free(caid->ca_system_id);
        av_freep(&desc->priv);
    }
    return 0;
}

static void ca_identifier_debug(MpegTSDescriptor *desc)
{
    if (desc->priv) {
        MpegTSCAIdentifierDescriptor *caid = desc->priv;

        av_log(NULL, AV_LOG_INFO, "CA identifier descriptor: ");
        for(int i = 0; i < caid->nb_ids; i++)
            av_log(NULL, AV_LOG_INFO, "id #%i: %i\n", i, caid->ca_system_id[i]);
    }
}

MpegTSDescriptor ca_identifier_descriptor = {
    .priv = NULL,
    .parse = ca_identifier_parse,
    .free = ca_identifier_free,
    .debug = ca_identifier_debug
};
