#ifndef AVFORMAT_MPEGTS_DESCRIPTORS_CA_IDENTIFIER_H
#define AVFORMAT_MPEGTS_DESCRIPTORS_CA_IDENTIFIER_H

#include <stdint.h>

struct MpegTSCAIdentifierDescriptor {
    int nb_ids;
    uint16_t *ca_system_id;
} __attribute__((packed));
typedef struct MpegTSCAIdentifierDescriptor MpegTSCAIdentifierDescriptor;

#endif /* AVFORMAT_MPEGTS_DESCRIPTORS_CA_IDENTIFIER_H */
