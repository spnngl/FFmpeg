#ifndef AVFORMAT_MPEGTS_DESCRIPTORS_EXTENDED_EVENT_H
#define AVFORMAT_MPEGTS_DESCRIPTORS_EXTENDED_EVENT_H

#include <stdint.h>

#include "common.h"

typedef struct MpegTSExtendedEventDescription MpegTSExtendedEventDescription;
typedef struct MpegTSExtendedEventDescriptor MpegTSExtendedEventDescriptor;

struct MpegTSExtendedEventDescription {
    uint8_t item_description_length;
    /** Item description */
    char *descr;
    uint8_t item_length;
    /** Item text */
    char *item;
} __attribute__((packed));

struct MpegTSExtendedEventDescriptor {
    /** This 4-bit field gives the number of the descriptor. It is used to
     * associate information which cannot be fitted into a single descriptor.
     * The descriptor_number of the first extended_event_descriptor of an
     * associated set of extended_event_descriptors shall be "0x00". The
     * descriptor_number shall be incremented by 1 with each additional
     * extended_event_descriptor in this sectio */
    uint8_t number :4;
    /** Number of the last extended_event_descriptor of the associated
     * set of descriptors */
    uint8_t last_number :4;
    /** Three character language code of the following texts */
    char iso_639_language_code[3];
    uint8_t length_of_items;
    /** MpegTSExtendedEventDescription structure(s) */
    SimpleLinkedList *descriptions;
    uint8_t text_len;
    /** Text description of the event */
    char *text;
} __attribute__((packed));

#endif /* AVFORMAT_MPEGTS_DESCRIPTORS_EXTENDED_EVENT_H */
