#ifndef AVFORMAT_MPEGTS_DESCRIPTORS_PDC_H
#define AVFORMAT_MPEGTS_DESCRIPTORS_PDC_H

#include <stdint.h>

struct MpegTSPDCDescriptor {
    char reserved_future_use :4;
    uint32_t programme_identification_label :20;
} __attribute__((packed));
typedef struct MpegTSPDCDescriptor MpegTSPDCDescriptor;

int mpegts_pdc_date_str(const uint32_t pil, char* dst);

#endif /* AVFORMAT_MPEGTS_DESCRIPTORS_PDC_H */
