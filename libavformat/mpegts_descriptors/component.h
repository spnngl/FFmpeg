#ifndef AVFORMAT_MPEGTS_DESCRIPTORS_COMPONENT_H
#define AVFORMAT_MPEGTS_DESCRIPTORS_COMPONENT_H

#include <stdint.h>

struct MpegTSComponentDescriptor {
    /** Stream_content{,ext} and component_type specify the type of stream */
    uint8_t stream_content_ext :4;
    uint8_t stream_content :4;
    uint8_t component_type;
    /** Has the same value as the component_tag field in the stream identifier
     * descriptor */
    uint8_t component_tag;
    /** Three character language code */
    char iso_639_language_code[3];
    /** Text description of the component stream */
    char *text;
} __attribute__((packed));
typedef struct MpegTSComponentDescriptor MpegTSComponentDescriptor;

void mpegts_component_description(const MpegTSComponentDescriptor *comp, char* dst);

#endif /* AVFORMAT_MPEGTS_DESCRIPTORS_COMPONENT_H */
