#include "libavutil/mem.h"
#include "../mpegts.h"
#include "telephone.h"
#include "common.h"

static int telephone_parse(MpegTSDescriptor *desc, const uint8_t **pp, const uint8_t *p_end)
{
    int val;
    const uint8_t *p = *pp;
    MpegTSTelephoneDescriptor *tel;

    if (desc->priv)
        desc->free(desc);

    desc->priv = av_mallocz(sizeof(MpegTSTelephoneDescriptor));
    tel = desc->priv;

    val = mpegts_get8(&p, p_end);
    if (val < 0)
        return val;
    tel->reserved_future_use1 = (val >> 6) & bit_mask(2);
    tel->foreign_availability = (val >> 5) & bit_mask(1);
    tel->connection_type = val & bit_mask(5);

    val = mpegts_get8(&p, p_end);
    if (val < 0)
        return val;
    tel->reserved_future_use2 = (val >> 7) & bit_mask(1);
    tel->country_prefix_len = (val >> 5) & bit_mask(2);
    tel->international_area_code_len = (val >> 2) & bit_mask(3);
    tel->operator_code_len = val & bit_mask(2);

    val = mpegts_get8(&p, p_end);
    if (val < 0)
        return val;
    tel->reserved_future_use3 = (val >> 7) & bit_mask(1);
    tel->national_area_code_len = (val >> 4) & bit_mask(3);
    tel->core_number_len = val & bit_mask(4);

    tel->country_prefix = av_malloc(tel->country_prefix_len + 1);
    memcpy(tel->country_prefix, p, tel->country_prefix_len);
    tel->country_prefix[tel->country_prefix_len] = '\0';
    p += tel->country_prefix_len;

    tel->international_area_code = av_malloc(tel->international_area_code_len + 1);
    memcpy(tel->international_area_code, p, tel->international_area_code_len);
    tel->international_area_code[tel->international_area_code_len] = '\0';
    p += tel->international_area_code_len;

    tel->operator_code = av_malloc(tel->operator_code_len + 1);
    memcpy(tel->operator_code, p, tel->operator_code_len);
    tel->operator_code[tel->operator_code_len] = '\0';
    p += tel->operator_code_len;

    tel->national_area_code = av_malloc(tel->national_area_code_len + 1);
    memcpy(tel->national_area_code, p, tel->national_area_code_len);
    tel->national_area_code[tel->national_area_code_len] = '\0';
    p += tel->national_area_code_len;

    tel->core_number = av_malloc(tel->core_number_len + 1);
    memcpy(tel->core_number, p, tel->core_number_len);
    tel->core_number[tel->core_number_len] = '\0';
    p += tel->core_number_len;

    *pp = p;

    return 0;
}

static int telephone_free(MpegTSDescriptor *desc)
{
    if (desc->priv) {
        MpegTSTelephoneDescriptor *tel = desc->priv;
        av_free(tel->country_prefix);
        av_free(tel->international_area_code);
        av_free(tel->operator_code);
        av_free(tel->national_area_code);
        av_free(tel->core_number);
        av_freep(&desc->priv);
    }
    return 0;
}

static void telephone_debug(MpegTSDescriptor *desc)
{
    if (desc->priv) {
        MpegTSTelephoneDescriptor *tel = desc->priv;
        av_log(NULL, AV_LOG_INFO, "Telephone descriptor: foreign_availability(%i), "
               "connection_type(%i), country_prefix(%s), international_area_code(%s), "
               "operator_code(%s), national_area_code(%s), core_number(%s)",
               tel->foreign_availability, tel->connection_type, tel->country_prefix,
               tel->international_area_code, tel->operator_code,
               tel->national_area_code, tel->core_number);
    }
}

MpegTSDescriptor telephone_descriptor = {
    .priv = NULL,
    .parse = telephone_parse,
    .free = telephone_free,
    .debug = telephone_debug
};
