#ifndef AVFORMAT_MPEGTS_DESCRIPTORS_CONTENT_H
#define AVFORMAT_MPEGTS_DESCRIPTORS_CONTENT_H

#include <stdint.h>

struct MpegTSContentDescription {
    uint8_t content_nibble_lvl_1 :4;
    uint8_t content_nibble_lvl_2 :4;
    uint8_t user_byte;
} __attribute__((packed));
typedef struct MpegTSContentDescription MpegTSContentDescription;

struct MpegTSContentDescriptor {
    int nb_descriptions;
    MpegTSContentDescription **descriptions;
};
typedef struct MpegTSContentDescriptor MpegTSContentDescriptor;

#endif /* AVFORMAT_MPEGTS_DESCRIPTORS_CONTENT_H */
