#ifndef AVFORMAT_MPEGTS_DESCRIPTORS_DATA_BROADCAST_H
#define AVFORMAT_MPEGTS_DESCRIPTORS_DATA_BROADCAST_H

#include <stdint.h>

struct MpegTSDataBroadcastDescriptor {
    uint16_t data_broadcast_id;
    uint8_t component_tag;
    uint8_t selector_len;
    char* selector;
    char iso_639_language_code[3];
    uint8_t text_len;
    char *text;
};
typedef struct MpegTSDataBroadcastDescriptor MpegTSDataBroadcastDescriptor;

#endif /* AVFORMAT_MPEGTS_DESCRIPTORS_DATA_BROADCAST_H */
