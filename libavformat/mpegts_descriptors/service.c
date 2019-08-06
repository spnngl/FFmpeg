#include "libavutil/mem.h"
#include "../mpegts.h"
#include "common.h"
#include "service.h"

static int service_parse(MpegTSDescriptor *desc, const uint8_t **pp, const uint8_t *p_end)
{
    int val;
    const uint8_t *p = *pp;
    MpegTSServiceDescriptor *serv;

    if (desc->priv)
        desc->free(desc);

    desc->priv = av_mallocz(sizeof(MpegTSServiceDescriptor));
    serv = desc->priv;

    val = mpegts_get8(&p, p_end);
    if (val < 0)
        return val;
    serv->service_type = val;

    val = mpegts_get8(&p, p_end);
    if (val < 0)
        return val;
    serv->provider_name_len = val;

    if(serv->provider_name_len > p_end - p)
        return val;

    serv->provider_name = av_malloc(serv->provider_name_len + 1);
    memcpy(serv->provider_name, p, serv->provider_name_len);
    serv->provider_name[serv->provider_name_len] = '\0';
    p += serv->provider_name_len;

    val = mpegts_get8(&p, p_end);
    if (val < 0)
        return val;
    serv->service_name_len = val;

    if(serv->service_name_len > p_end - p)
        return val;

    serv->service_name = av_malloc(serv->service_name_len + 1);
    memcpy(serv->service_name, p, serv->service_name_len);
    serv->service_name[serv->service_name_len] = '\0';
    p += serv->service_name_len;

    *pp = p;

    return 0;
}

static int service_free(MpegTSDescriptor *desc)
{
    if (desc->priv) {
        MpegTSServiceDescriptor *serv = desc->priv;
        av_free(serv->provider_name);
        av_free(serv->service_name);
        av_freep(&desc->priv);
    }
    return 0;
}

static void service_debug(MpegTSDescriptor *desc)
{
}

MpegTSDescriptor service_descriptor = {
    .priv = NULL,
    .parse = service_parse,
    .free = service_free,
    .debug = service_debug
};
