#ifndef AVFORMAT_MPEGTS_DESCRIPTORS_PRIVATE_DATA_SPECIFIER_H
#define AVFORMAT_MPEGTS_DESCRIPTORS_PRIVATE_DATA_SPECIFIER_H

#include <stdint.h>

struct MpegTSPrivateDataSpecifierDescriptor {
    uint32_t specifier; // ETSI TS 101 162 [56]
};
typedef struct MpegTSPrivateDataSpecifierDescriptor MpegTSPrivateDataSpecifierDescriptor;

#endif /* AVFORMAT_MPEGTS_DESCRIPTORS_PRIVATE_DATA_SPECIFIER_H */
