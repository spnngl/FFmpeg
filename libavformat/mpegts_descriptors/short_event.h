#ifndef AVFORMAT_MPEGTS_DESCRIPTORS_SHORT_EVENT_H
#define AVFORMAT_MPEGTS_DESCRIPTORS_SHORT_EVENT_H

#include <stdint.h>

struct MpegTSShortEventDescriptor {
    char iso_639_language_code[3];
    uint8_t event_name_len;
    char *event_name;
    uint8_t text_len;
    char *text;
} __attribute__((packed));
typedef struct MpegTSShortEventDescriptor MpegTSShortEventDescriptor;

#endif /* AVFORMAT_MPEGTS_DESCRIPTORS_SHORT_EVENT_H */
