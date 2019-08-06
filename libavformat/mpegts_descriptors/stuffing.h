#ifndef AVFORMAT_MPEGTS_DESCRIPTORS_STUFFING_H
#define AVFORMAT_MPEGTS_DESCRIPTORS_STUFFING_H

#include <stdint.h>

struct MpegTSStuffingDescriptor {
    char *stuffing_bytes;
};
typedef struct MpegTSStuffingDescriptor MpegTSStuffingDescriptor;

#endif /* AVFORMAT_MPEGTS_DESCRIPTORS_STUFFING_H */
