#ifndef AVFORMAT_MPEGTS_DESCRIPTORS_EXTENDED_EVENT_H
#define AVFORMAT_MPEGTS_DESCRIPTORS_EXTENDED_EVENT_H

#include <stdint.h>

#include "common.h"

typedef struct MpegTSExtendedEventDescription MpegTSExtendedEventDescription;
typedef struct MpegTSExtendedEventDescriptor MpegTSExtendedEventDescriptor;

struct MpegTSExtendedEventDescription {
    uint8_t item_description_length;
    char *descr;
    uint8_t item_length;
    char *item;
} __attribute__((packed));

struct MpegTSExtendedEventDescriptor {
    uint8_t number :4;
    uint8_t last_number :4;
    char iso_639_language_code[3];
    uint8_t length_of_items;
    SimpleLinkedList *descriptions;
    uint8_t text_len;
    char *text;
} __attribute__((packed));

#endif /* AVFORMAT_MPEGTS_DESCRIPTORS_EXTENDED_EVENT_H */
