#ifndef AVFORMAT_MPEGTS_DESCRIPTORS_LINKAGE_H
#define AVFORMAT_MPEGTS_DESCRIPTORS_LINKAGE_H

#include <stdint.h>

#include "common.h"

enum MpegTSLinkageDescriptorType {
    /* type 0x00 is reserved for future use */
    MPEGTS_LINKAGE_TYPE_INFORMATION =               0x01,
    MPEGTS_LINKAGE_TYPE_EPG =                       0x02,
    MPEGTS_LINKAGE_TYPE_CA_REPLACEMENT =            0x03,
    MPEGTS_LINKAGE_TYPE_COMPLETE_BOUQUET =          0x04,
    MPEGTS_LINKAGE_TYPE_SERVICE_REPLACEMENT =       0x05,
    MPEGTS_LINKAGE_TYPE_DATA_BROADCAST =            0x06,
    MPEGTS_LINKAGE_TYPE_RCS_MAP =                   0x07,
    MPEGTS_LINKAGE_TYPE_MOBILE_HAND_OVER =          0x08,
    MPEGTS_LINKAGE_TYPE_SYSTEM_SOFTWARE_UPDATE =    0x09,
    MPEGTS_LINKAGE_TYPE_CONTAINS_SSU_BAT_NIT =      0x0A,
    MPEGTS_LINKAGE_TYPE_IP_MAC_NOTIF =              0x0B,
    MPEGTS_LINKAGE_TYPE_CONTAINS_INT_BAT_NIT =      0x0C,
    MPEGTS_LINKAGE_TYPE_EVENT =                     0x0D,
    MPEGTS_LINKAGE_TYPE_EXTENDED_EVENT_FIRST =      0x0E,
    // EXTENDED_EVENT from 0x10 to 0x1F are reserved for future use
    MPEGTS_LINKAGE_TYPE_EXTENDED_EVENT_LAST =       0x1F,
    MPEGTS_LINKAGE_TYPE_DOWNLOADABLE_FONT_INFO =    0x20,
    /* types from 0x21 to 0x7F are reserved for future use */
    /* types from 0x80 to 0xFE are user defined */
    /* type 0xFF is reserved for future use */
};

struct MpegTSLinkageDescriptor {
    uint16_t transport_stream_id;
    uint16_t original_network_id;
    uint16_t service_id;
    uint8_t linkage_type;
    void *link;
    // N * private_data
    char *private_data;
};
typedef struct MpegTSLinkageDescriptor MpegTSLinkageDescriptor;

enum MpegTSMobileHandOverLinkageType {
    /* type 0x00 is reserved for future use */
    MPEGTS_MOBILE_HAND_OVER_IDENTICAL_SERVICE   = 0x01,
    MPEGTS_MOBILE_HAND_OVER_LOCAL_VARIATION     = 0x02,
    MPEGTS_MOBILE_HAND_OVER_ASSOCIATED_SERVICE  = 0x03,
    /* types from 0x04 to 0x0F are reserved for future use */
};

struct MpegTSMobileHandOverLinkage {
    uint8_t hand_over_type :4;
    uint8_t reserved_future_use :3;
    uint8_t origin_type :1;
    // network_id only if hand_over_type in (0x01, 0x02, 0x03)
    uint16_t network_id;
    // initial_service_id if origin_type == 0x00
    uint16_t initial_service_id;
} __attribute__((packed));
typedef struct MpegTSMobileHandOverLinkage MpegTSMobileHandOverLinkage;

struct MpegTSEventLinkage {
    uint16_t target_event_id;
    uint8_t target_listed :1;
    uint8_t event_simulcast :1;
    uint8_t reserved :6;
} __attribute__((packed));
typedef struct MpegTSEventLinkage MpegTSEventLinkage;

enum MpegTSExtendedEventTargetIdType {
    EXTEVENT_TARGET_TYPE_TRANSPORT_STREAM_ID        = 0,
    EXTEVENT_TARGET_TYPE_TARGET_TRANSPORT_STREAM_ID = 1,
    EXTEVENT_TARGET_TYPE_WILDCARD                   = 2,
    EXTEVENT_TARGET_TYPE_USER_DEFINED_ID            = 3,
};

struct MpegTSEELDescription {
    uint16_t target_event_id;
    char target_listed :1;
    char event_simulcast :1;
    uint8_t link_type :2;
    uint8_t target_id_type :2;
    char original_network_id_flag :1;
    char service_id_flag :1;
    // if target_id_type == 3 then user_defined_id else if target_id_type == 1
    // it is the target_transport_stream_id
    uint16_t id;
    // Below exists only if target_id_type != 3
    uint16_t target_original_network_id; // if original_network_id_flag
    uint16_t target_service_id; // if service_id_flag
} __attribute__((packed));
typedef struct MpegTSEELDescription MpegTSEELDescription;

struct MpegTSExtendedEventLinkage {
    uint8_t loop_length;
    SimpleLinkedList *descriptions;
};
typedef struct MpegTSExtendedEventLinkage MpegTSExtendedEventLinkage;

#endif /* AVFORMAT_MPEGTS_DESCRIPTORS_LINKAGE_H */
