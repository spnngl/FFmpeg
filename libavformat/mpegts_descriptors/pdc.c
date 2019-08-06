#include "libavutil/mem.h"
#include "../mpegts.h"
#include "pdc.h"
#include "common.h"

int mpegts_pdc_date_str(const uint32_t pil, char* dst)
{
    int head = 0, size;
    char cur[8];

    snprintf(cur, 8, "%i", (pil >> 15));
    size = strlen(cur);
    memcpy(dst, cur, size);
    head += size;
    memcpy(dst + head, "/\0", 1);
    head++;

    snprintf(cur, 8, "%i", (pil >> 11) & bit_mask(4));
    size = strlen(cur);
    memcpy(dst + head, cur, size);
    head += size;
    memcpy(dst + head, " \0", 1);
    head++;

    snprintf(cur, 8, "%i", (pil >> 6) & bit_mask(5));
    size = strlen(cur);
    memcpy(dst + head, cur, size);
    head += size;
    memcpy(dst + head, ":\0", 1);
    head++;

    snprintf(cur, 8, "%i", pil & bit_mask(6));
    size = strlen(cur);
    strcpy(dst + head, cur);
    head += size;

    return head;
}

static int pdc_parse(MpegTSDescriptor *desc, const uint8_t **pp, const uint8_t *p_end)
{
    int val;
    const uint8_t *p = *pp;
    MpegTSPDCDescriptor *pdc;

    if (desc->priv)
        desc->free(desc);

    desc->priv = av_mallocz(sizeof(MpegTSPDCDescriptor));
    pdc = desc->priv;

    val = mpegts_get8(&p, p_end);
    if (val < 0)
        return val;
    pdc->reserved_future_use = (val >> 4) & bit_mask(4);
    pdc->programme_identification_label |= (val << 16);

    val = mpegts_get16(&p, p_end);
    if (val < 0)
        return val;
    pdc->programme_identification_label |= val;

    return 0;
}

static int pdc_free(MpegTSDescriptor *desc)
{
    if (desc->priv) {
        MpegTSPDCDescriptor *pdc = desc->priv;
        av_free(pdc);
        desc->priv = NULL;
    }
    return 0;
}

static void pdc_debug(MpegTSDescriptor *desc)
{
    if (desc->priv) {
        char date[32];
        MpegTSPDCDescriptor *pdc = desc->priv;

        mpegts_pdc_date_str(pdc->programme_identification_label, date);
        av_log(NULL, MPEGTS_DESCRIPTORS_LOG, "PDC descriptor: date(%s)", date);
    }
}

MpegTSDescriptor pdc_descriptor = {
    .priv = NULL,
    .parse = pdc_parse,
    .free = pdc_free,
    .debug = pdc_debug
};
