#include "common.h"
#include "../mpegts.h"

static MpegTSDescriptor* all_descriptors[256] = {
    [0x00 ... 0xFF] = NULL,
};

int mpegts_parse_descriptor_header(MpegTSDescriptorHeader *h, const uint8_t **pp,
                                   const uint8_t *p_end)
{
    int val;
    const uint8_t *p = *pp;

    val = mpegts_get8(&p, p_end);
    if (val < 0)
        return val;
    h->tag = val;

    val = mpegts_get8(&p, p_end);
    if (val < 0)
        return val;
    h->len = val;

    *pp = p;

    return 0;
}

MpegTSDescriptor* mpegts_get_descriptor(MpegTSDescriptorHeader *h)
{
    return all_descriptors[h->tag];
}
