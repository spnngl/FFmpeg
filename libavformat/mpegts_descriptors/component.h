#ifndef AVFORMAT_MPEGTS_DESCRIPTORS_COMPONENT_H
#define AVFORMAT_MPEGTS_DESCRIPTORS_COMPONENT_H

#include <stdint.h>

struct MpegTSComponentDescriptor {
    uint8_t stream_content_ext :4;
    uint8_t stream_content :4;
    uint8_t component_type;
    uint8_t component_tag;
    char iso_639_language_code[3];
    char *text;
} __attribute__((packed));
typedef struct MpegTSComponentDescriptor MpegTSComponentDescriptor;

void mpegts_component_description(const MpegTSComponentDescriptor *comp, char* dst);

#endif /* AVFORMAT_MPEGTS_DESCRIPTORS_COMPONENT_H */
