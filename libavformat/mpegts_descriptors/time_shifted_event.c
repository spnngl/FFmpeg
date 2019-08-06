#include "libavutil/mem.h"
#include "../mpegts.h"
#include "time_shifted_event.h"
#include "common.h"

static int time_shifted_event_parse(MpegTSDescriptor *desc, const uint8_t **pp, const uint8_t *p_end)
{
    int val;
    const uint8_t *p = *pp;
    MpegTSTimeShiftedEventDescriptor *tsed;

    if (desc->priv)
        desc->free(desc);

    desc->priv = av_mallocz(sizeof(MpegTSTimeShiftedEventDescriptor));
    tsed = desc->priv;

    val = mpegts_get16(&p, p_end);
    if (val < 0)
        return val;
    tsed->reference_service_id = val;

    val = mpegts_get16(&p, p_end);
    if (val < 0)
        return val;
    tsed->reference_event_id = val;

    return 0;
}

static int time_shifted_event_free(MpegTSDescriptor *desc)
{
    if (desc->priv)
        av_freep(&desc->priv);
    return 0;
}

static void time_shifted_event_debug(MpegTSDescriptor *desc)
{
    if (desc->priv) {
        MpegTSTimeShiftedEventDescriptor *tsed = desc->priv;
        av_log(NULL, MPEGTS_DESCRIPTORS_LOG, "Time shifted event descriptor: "
                "reference_service_id(%i), reference_event_id(%i)\n",
                tsed->reference_service_id, tsed->reference_event_id);
    }
}

MpegTSDescriptor time_shifted_event_descriptor = {
    .priv = NULL,
    .parse = time_shifted_event_parse,
    .free = time_shifted_event_free,
    .debug = time_shifted_event_debug
};
