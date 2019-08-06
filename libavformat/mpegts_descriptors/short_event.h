#ifndef AVFORMAT_MPEGTS_DESCRIPTORS_SHORT_EVENT_H
#define AVFORMAT_MPEGTS_DESCRIPTORS_SHORT_EVENT_H

#include <stdint.h>

/**
 * @brief Short event descriptor
 */
struct MpegTSShortEventDescriptor {
    /** Three character language code of the following char* fields */
    char iso_639_language_code[3];
    uint8_t event_name_len;
    char *event_name;
    uint8_t text_len;
    /** Text description of the event */
    char *text;
} __attribute__((packed));
typedef struct MpegTSShortEventDescriptor MpegTSShortEventDescriptor;

#endif /* AVFORMAT_MPEGTS_DESCRIPTORS_SHORT_EVENT_H */
