#ifndef AVFORMAT_MPEGTS_DESCRIPTORS_MULTILINGUAL_COMPONENT_H
#define AVFORMAT_MPEGTS_DESCRIPTORS_MULTILINGUAL_COMPONENT_H

#include <stdint.h>

#include "common.h"

struct MpegTSMultilingualComponentDescription {
    char iso_639_language_code[3];
    uint8_t text_description_len;
    char* text;
};
typedef struct MpegTSMultilingualComponentDescription MpegTSMultilingualComponentDescription;

struct MpegTSMultilingualComponentDescriptor {
    uint8_t component_tag;
    // N * description
    SimpleLinkedList *descriptions;
} __attribute__((packed));
typedef struct MpegTSMultilingualComponentDescriptor MpegTSMultilingualComponentDescriptor;

#endif /* AVFORMAT_MPEGTS_DESCRIPTORS_MULTILINGUAL_COMPONENT_H */
