#ifndef AVFORMAT_MPEGTS_DESCRIPTORS_SERVICE_H
#define AVFORMAT_MPEGTS_DESCRIPTORS_SERVICE_H

#include <stdint.h>

struct MpegTSServiceDescriptor {
    uint8_t service_type;
    uint8_t provider_name_len;
    char *provider_name;
    uint8_t service_name_len;
    char *service_name;
};
typedef struct MpegTSServiceDescriptor MpegTSServiceDescriptor;

#endif /* AVFORMAT_MPEGTS_DESCRIPTORS_SERVICE_H */
