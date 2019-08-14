#include "libavutil/internal.h"
#include "libavutil/error.h"
#include "libavutil/intreadwrite.h"
#include "dvb.h"

int avpriv_dvb_get8(const uint8_t **pp, const uint8_t *p_end)
{
    const uint8_t *p;
    int c;

    p = *pp;
    if (p >= p_end)
        return AVERROR_INVALIDDATA;
    c   = *p++;
    *pp = p;
    return c;
}

int avpriv_dvb_get16(const uint8_t **pp, const uint8_t *p_end)
{
    const uint8_t *p;
    int c;

    p = *pp;
    if (1 >= p_end - p)
        return AVERROR_INVALIDDATA;
    c   = AV_RB16(p);
    p  += 2;
    *pp = p;
    return c;
}

int avpriv_dvb_get32(const uint8_t **pp, const uint8_t *p_end)
{
    const uint8_t *p;
    int c;

    p = *pp;
    if (1 >= p_end - p)
        return AVERROR_INVALIDDATA;
    c   = AV_RB32(p);
    p  += 4;
    *pp = p;
    return c;
}

int avpriv_dvb_parse_section_header(DvbSectionHeader *h, const uint8_t **pp,
                                    const uint8_t *p_end)
{
    int val;

    val = avpriv_dvb_get8(pp, p_end);
    if (val < 0)
        return val;
    h->tid = val;
    *pp += 2;
    val  = avpriv_dvb_get16(pp, p_end);
    if (val < 0)
        return val;
    h->id = val;
    val = avpriv_dvb_get8(pp, p_end);
    if (val < 0)
        return val;
    h->version = (val >> 1) & 0x1f;
    val = avpriv_dvb_get8(pp, p_end);
    if (val < 0)
        return val;
    h->sec_num = val;
    val = avpriv_dvb_get8(pp, p_end);
    if (val < 0)
        return val;
    h->last_sec_num = val;
    return 0;
}
