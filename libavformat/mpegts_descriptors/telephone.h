#ifndef AVFORMAT_MPEGTS_DESCRIPTORS_TELEPHONE_H
#define AVFORMAT_MPEGTS_DESCRIPTORS_TELEPHONE_H

#include <stdint.h>

struct MpegTSTelephoneDescriptor {
    char reserved_future_use1 :2;
    char foreign_availability :1;
    uint8_t connection_type :5;
    char reserved_future_use2 :1;
    uint8_t country_prefix_len :2;
    uint8_t international_area_code_len :3;
    uint8_t operator_code_len :2;
    char reserved_future_use3 :1;
    uint8_t national_area_code_len :3;
    uint8_t core_number_len :4;
    char* country_prefix;
    char* international_area_code;
    char* operator_code;
    char* national_area_code;
    char* core_number;
} __attribute__((packed));
typedef struct MpegTSTelephoneDescriptor MpegTSTelephoneDescriptor;

#endif /* AVFORMAT_MPEGTS_DESCRIPTORS_TELEPHONE_H */
