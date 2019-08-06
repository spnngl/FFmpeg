#ifndef AVFORMAT_MPEGTS_DESCRIPTORS_SHORT_SMOOTHING_BUFFER_H
#define AVFORMAT_MPEGTS_DESCRIPTORS_SHORT_SMOOTHING_BUFFER_H

#include <stdint.h>

struct MpegTSShortSmoothingBufferDescriptor {
    uint8_t sb_size :2;
    uint8_t sb_leak_rate :6;
    char* DVB_reserved;
} __attribute__((packed));
typedef struct MpegTSShortSmoothingBufferDescriptor MpegTSShortSmoothingBufferDescriptor;

const char* mpegts_sb_size_str(const uint8_t sb_size);
const char* mpegts_sb_leak_rate_str(const uint8_t sb_leak_rate);

#endif /* AVFORMAT_MPEGTS_DESCRIPTORS_SHORT_SMOOTHING_BUFFER_H */
