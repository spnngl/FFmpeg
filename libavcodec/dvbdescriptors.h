#ifndef AVCODEC_DVBDESCRIPTORS_H
#define AVCODEC_DVBDESCRIPTORS_H

#include <stddef.h>
#include <stdint.h>

typedef struct SimpleLinkedList SimpleLinkedList;
struct SimpleLinkedList {
    void *data;
    SimpleLinkedList *next;
};

typedef struct  DvbDescriptorHeader {
    uint8_t tag;
    uint8_t len;
} DvbDescriptorHeader;

typedef struct DvbDescriptor DvbDescriptor;
struct DvbDescriptor {
    void* (*parse)(DvbDescriptor *, const uint8_t **pp, const uint8_t *p_end);
    int (*free)(void*);
};

DvbDescriptor* ff_dvb_get_descriptor(DvbDescriptorHeader *h);
int dvb_parse_descriptor_header(DvbDescriptorHeader *h, const uint8_t **pp,
                                const uint8_t *p_end);

enum DvbDescriptorTag {
    /* Descriptors from 0x00 to 0x01 are reserved */
    DVB_DESCRIPTOR_TAG_VIDEO_STREAM_HEADER_PARAMS      = 0x02, // Video stream header parameters for ITU-T Rec. H.262, ISO/IEC 13818-2 and ISO/IEC 11172-2
 	DVB_DESCRIPTOR_TAG_AUDIO_STREAM_HEADER_PARAMS      = 0x03, // Audio stream header parameters for ISO/IEC 13818-3 and ISO/IEC 11172-3
 	DVB_DESCRIPTOR_TAG_HIERARCHY_STREAM                = 0x04, // Hierarchy for stream selection
 	DVB_DESCRIPTOR_TAG_REGISTER_PRIVATE_FMT            = 0x05, // Registration of private formats
 	DVB_DESCRIPTOR_TAG_STREAM_ALIGNMENT                = 0x06, // Data stream alignment for packetized video and audio sync point
 	DVB_DESCRIPTOR_TAG_TARGET_BACKGROUND               = 0x07, // Target background grid defines total display area size
 	DVB_DESCRIPTOR_TAG_DISPLAY_POSITION                = 0x08, // Video Window defines position in display area
 	DVB_DESCRIPTOR_TAG_CAT_EMM_ECM_PID                 = 0x09, // Conditional access system and EMM/ECM PID
 	DVB_DESCRIPTOR_TAG_ISO_639                         = 0x0A, // ISO 639 language and audio type
 	DVB_DESCRIPTOR_TAG_EXTERNAL_CLOCK                  = 0x0B, // System clock external reference
 	DVB_DESCRIPTOR_TAG_MULTIPLEX_BOUNDS                = 0x0C, // Multiplex buffer utilization bounds
 	DVB_DESCRIPTOR_TAG_COPYRIGHT_SYSTEM                = 0x0D, // Copyright identification system and reference
 	DVB_DESCRIPTOR_TAG_MAX_BIT_RATE                    = 0x0E, // Maximum bit rate
 	DVB_DESCRIPTOR_TAG_PRIVATE_DATA                    = 0x0F, // Private data indicator
 	DVB_DESCRIPTOR_TAG_SMOOTHING_BUFFER                = 0x10, // Smoothing buffer
 	DVB_DESCRIPTOR_TAG_STD_VIDEO_LEAK_CONTROL          = 0x11, // STD video buffer leak control
 	DVB_DESCRIPTOR_TAG_IBP_KEYFRAME                    = 0x12, // IBP video I-frame indicator
 	DVB_DESCRIPTOR_TAG_IEC13818_6_CAROUSEL             = 0x13, // ISO/IEC13818-6 DSM CC carousel identifier
 	DVB_DESCRIPTOR_TAG_IEC13818_6_ASSOCIATION          = 0x14, // ISO/IEC13818-6 DSM CC association tag
 	DVB_DESCRIPTOR_TAG_IEC13818_6_DEFERRED_ASSOCIATION = 0x15, // ISO/IEC13818-6 DSM CC deferred association tag
 	DVB_DESCRIPTOR_TAG_IEC13818_6_RESERVED             = 0x16, // ISO/IEC13818-6 DSM CC Reserved.
 	DVB_DESCRIPTOR_TAG_DSM_NPT_REF                     = 0x17, // DSM CC NPT reference
 	DVB_DESCRIPTOR_TAG_DSM_NPT_ENDPOINT                = 0x18, // DSM CC NPT endpoint
 	DVB_DESCRIPTOR_TAG_DSM_STREAM_MODE                 = 0x19, // DSM CC stream mode
 	DVB_DESCRIPTOR_TAG_DSM_STEAM_EVENT                 = 0x1A, // DSM CC stream event
 	DVB_DESCRIPTOR_TAG_IEC_14496_2_VIDEO_HEADER        = 0x1B, // Video stream header parameters for ISO/IEC 14496-2 (MPEG-4 H.263 based)
 	DVB_DESCRIPTOR_TAG_IEC_14496_2_AUDIO_HEADER        = 0x1C, // Audio stream header parameters for ISO/IEC 14496-3 (MPEG-4 LOAS multi-format framed)
 	DVB_DESCRIPTOR_TAG_IOD_PARAMS                      = 0x1D, // IOD parameters for ISO/IEC 14496-1
 	DVB_DESCRIPTOR_TAG_SL_PARAMS                       = 0x1E, // SL parameters for ISO/IEC 14496-1
 	DVB_DESCRIPTOR_TAG_FMC_PARAMS                      = 0x1F, // FMC parameters for ISO/IEC 14496-1
 	DVB_DESCRIPTOR_TAG_EXTERNAL_ES                     = 0x20, // External ES identifier for ISO/IEC 14496-1
 	DVB_DESCRIPTOR_TAG_MUXCODE                         = 0x21, // MuxCode for ISO/IEC 14496-1
 	DVB_DESCRIPTOR_TAG_FMX_BUFFER_SIZE                 = 0x22, // FMX Buffer Size for ISO/IEC 14496-1
 	DVB_DESCRIPTOR_TAG_MUX_BUFFER                      = 0x23, // Multiplex Buffer for ISO/IEC 14496-1
 	DVB_DESCRIPTOR_TAG_CONTENT_LABELING                = 0x24, // Content labeling for ISO/IEC 14496-1
 	DVB_DESCRIPTOR_TAG_METADATA_POINTER                = 0x25, // Metadata pointer
 	DVB_DESCRIPTOR_TAG_METADATA                        = 0x26, // Metadata
 	DVB_DESCRIPTOR_TAG_METADATA_STD                    = 0x27, // Metadata STD
 	DVB_DESCRIPTOR_TAG_IEC_14496_10_VIDEO_HEADER       = 0x28, // Video stream header parameters for ITU-T Rec. H.264 and ISO/IEC 14496-10
 	DVB_DESCRIPTOR_TAG_IEC_13818_11_IPMP               = 0x29, // ISO/IEC 13818-11 IPMP (DRM)
 	DVB_DESCRIPTOR_TAG_TIMING_HRD                      = 0x2A, // Timing and HRD for ITU-T Rec. H.264 and ISO/IEC 14496-10
 	DVB_DESCRIPTOR_TAG_IEC_13818_7_AUDIO_HEADER        = 0x2B, // Audio stream header parameters for ISO/IEC 13818-7 ADTS AAC
 	DVB_DESCRIPTOR_TAG_FLEXMUX_TIMING                  = 0x2C, // FlexMux Timing for ISO/IEC 14496-1
 	DVB_DESCRIPTOR_TAG_TEXT_STREAM_HEADER              = 0x2D, // Text stream header parameters for ISO/IEC 14496
 	DVB_DESCRIPTOR_TAG_IEC_14496_3_AUDIO_HEADER        = 0x2E, // Audio extension stream header parameters for ISO/IEC 14496-3 (MPEG-4 LOAS multi-format framed)
 	DVB_DESCRIPTOR_TAG_VIDEO_AUXILIARY_HEADER          = 0x2F, // Video auxiliary stream header parameters
 	DVB_DESCRIPTOR_TAG_VIDEO_SCALABLE_HEADER           = 0x30, // Video scalable stream header parameters
 	DVB_DESCRIPTOR_TAG_VIDEO_MULTI_HEADER              = 0x31, // Video multi stream header parameters
 	DVB_DESCRIPTOR_TAG_IEC_15444_HEADER                = 0x32, // Video stream header parameters for ITU-T Rec. T.800 and ISO/IEC 15444 (JPEG 2000)
 	DVB_DESCRIPTOR_TAG_VIDEO_MULTI_OP_POINT_HEADER     = 0x33, // Video multi operation point stream header parameters
 	DVB_DESCRIPTOR_TAG_VIDEO_STEREOSCOPIC_HEADER       = 0x34, // Video stereoscopic (3D) stream header parameters for ITU-T Rec. H.262, ISO/IEC 13818-2 and ISO/IEC 11172-2
 	DVB_DESCRIPTOR_TAG_PROGRAM_STEREOSCOPIC_INFO       = 0x35, // Program stereoscopic (3D) information
 	DVB_DESCRIPTOR_TAG_VIDEO_STEREOSCOPIC_INFO         = 0x36, // Video stereoscopic (3D) information
    /* Descriptors from 0x37 to 0x3F are reserved */
    /* Descriptors from 0x40 to 0x7F are used by DVB */
    DVB_DESCRIPTOR_TAG_NETWORK_NAME                    = 0x40,
    DVB_DESCRIPTOR_TAG_SERVICE_LIST                    = 0x41,
    DVB_DESCRIPTOR_TAG_STUFFING                        = 0x42,
    DVB_DESCRIPTOR_TAG_SATELLITE_DELIVERY_SYSTEM       = 0x43,
    DVB_DESCRIPTOR_TAG_CABLE_DELIVERY_SYSTEM           = 0x44,
    DVB_DESCRIPTOR_TAG_VBI_DATA                        = 0x45,
    DVB_DESCRIPTOR_TAG_VBI_TELETEXT                    = 0x46,
    DVB_DESCRIPTOR_TAG_BOUQUET_NAME                    = 0x47,
    DVB_DESCRIPTOR_TAG_SERVICE                         = 0x48,
    DVB_DESCRIPTOR_TAG_COUNTRY_AVAILABILITY            = 0x49,
    DVB_DESCRIPTOR_TAG_LINKAGE                         = 0x4A,
    DVB_DESCRIPTOR_TAG_NVOD_REFERENCE                  = 0x4B,
    DVB_DESCRIPTOR_TAG_TIME_SHIFTED_REFERENCE          = 0x4C,
    DVB_DESCRIPTOR_TAG_SHORT_EVENT                     = 0x4D,
    DVB_DESCRIPTOR_TAG_EXTENDED_EVENT                  = 0x4E,
    DVB_DESCRIPTOR_TAG_TIME_SHIFTED_EVENT              = 0x4F,
    DVB_DESCRIPTOR_TAG_COMPONENT                       = 0x50,
    DVB_DESCRIPTOR_TAG_MOSAIC                          = 0x51,
    DVB_DESCRIPTOR_TAG_STREAM_IDENTIFIER               = 0x52,
    DVB_DESCRIPTOR_TAG_CA_IDENTIFIER                   = 0x53,
    DVB_DESCRIPTOR_TAG_CONTENT                         = 0x54,
    DVB_DESCRIPTOR_TAG_PARENTAL_RATING                 = 0x55,
    DVB_DESCRIPTOR_TAG_TELETEXT                        = 0x56,
    DVB_DESCRIPTOR_TAG_TELEPHONE                       = 0x57,
    DVB_DESCRIPTOR_TAG_LOCAL_TIME_OFFSET               = 0x58,
    DVB_DESCRIPTOR_TAG_SUBTITLING                      = 0x59,
    DVB_DESCRIPTOR_TAG_TERRESTRIAL_DELIVERY            = 0x5A,
    DVB_DESCRIPTOR_TAG_MULTILINGUAL_NETWORK_NAME       = 0x5B,
    DVB_DESCRIPTOR_TAG_MULTILINGUAL_BOUQUET_NAME       = 0x5C,
    DVB_DESCRIPTOR_TAG_MULTILINGUAL_SERVICE_NAME       = 0x5D,
    DVB_DESCRIPTOR_TAG_MULTILINGUAL_COMPONENT          = 0x5E,
    DVB_DESCRIPTOR_TAG_PRIVATE_DATA_SPECIFIER          = 0x5F,
    DVB_DESCRIPTOR_TAG_SERVICE_MOVE                    = 0x60,
    DVB_DESCRIPTOR_TAG_SHORT_SMOOTHING_BUFFER          = 0x61,
    DVB_DESCRIPTOR_TAG_FREQUENCY_LIST                  = 0x62,
    DVB_DESCRIPTOR_TAG_PARTIAL_TRANSPORT_STREAM        = 0x63,
    DVB_DESCRIPTOR_TAG_DATA_BROADCAST                  = 0x64,
    DVB_DESCRIPTOR_TAG_SCRAMBLING                      = 0x65,
    DVB_DESCRIPTOR_TAG_DATA_BROADCAST_ID               = 0x66,
    DVB_DESCRIPTOR_TAG_TRANSPORT_STREAM                = 0x67,
    DVB_DESCRIPTOR_TAG_DSNG                            = 0x68,
    DVB_DESCRIPTOR_TAG_PDC                             = 0x69,
    DVB_DESCRIPTOR_TAG_AC3                             = 0x6A,
    DVB_DESCRIPTOR_TAG_ANCILLARY_DATA                  = 0x6B,
    DVB_DESCRIPTOR_TAG_CELL_LIST                       = 0x6C,
    DVB_DESCRIPTOR_TAG_CELL_FREQUENCY_LINK             = 0x6D,
    DVB_DESCRIPTOR_TAG_ANNOUNCEMENT_SUPPORT            = 0x6E,
    DVB_DESCRIPTOR_TAG_APPLICATION_SIGNALING           = 0x6F,
    DVB_DESCRIPTOR_TAG_ADAPTATION_FIELD_DATA           = 0x70,
    DVB_DESCRIPTOR_TAG_SERVICE_IDENTIFIER              = 0x71,
    DVB_DESCRIPTOR_TAG_SERVICE_AVAILABILITY            = 0x72,
    DVB_DESCRIPTOR_TAG_DEFAULT_AUTHORITY               = 0x73,
    DVB_DESCRIPTOR_TAG_RELATED_CONTENT                 = 0x74,
    DVB_DESCRIPTOR_TAG_TVA_ID                          = 0x75,
    DVB_DESCRIPTOR_TAG_CONTENT_IDENTIFIER              = 0x76,
    DVB_DESCRIPTOR_TAG_TIME_SLICE_FEC_IDENTIFIER       = 0x77,
    DVB_DESCRIPTOR_TAG_ECM_REPETITION_RATE             = 0x78,
    DVB_DESCRIPTOR_TAG_S2_SATELLITE_DELIVERY_SYSTEM    = 0x79,
    DVB_DESCRIPTOR_TAG_ENHANCED_AC3                    = 0x7A,
    DVB_DESCRIPTOR_TAG_DTS                             = 0x7B,
    DVB_DESCRIPTOR_TAG_AAC                             = 0x7C,
    DVB_DESCRIPTOR_TAG_XAIT_LOCATION                   = 0x7D,
    DVB_DESCRIPTOR_TAG_FTA_CONTENT_MANAGEMENT          = 0x7E,
    DVB_DESCRIPTOR_TAG_EXTENSION                       = 0x7F,
    /* Values from 0x80 to 0xfe are user defined */
    /* Value 0xff is reserved for future use */
};


typedef struct DvbComponentDescriptor {
    /** Stream_content{,ext} and component_type specify the type of stream */
    uint8_t stream_content_ext :4;
    uint8_t stream_content :4;
    uint8_t component_type;
    /** Has the same value as the component_tag field in the stream identifier
     * descriptor */
    uint8_t component_tag;
    /** Three character language code */
    char iso_639_language_code[3];
    /** Text description of the component stream */
    char *text;
} DvbComponentDescriptor;


typedef struct DvbContentDescription {
    uint8_t content_nibble_lvl_1 :4;
    uint8_t content_nibble_lvl_2 :4;
    uint8_t user_byte;
} DvbContentDescription;

typedef struct DvbContentDescriptor {
    int nb_descriptions;
    DvbContentDescription **descriptions;
} DvbContentDescriptor;


typedef struct DvbDataBroadcastDescriptor {
    uint16_t data_broadcast_id;
    uint8_t component_tag;
    uint8_t selector_len;
    char* selector;
    char iso_639_language_code[3];
    uint8_t text_len;
    char *text;
} DvbDataBroadcastDescriptor;


typedef struct DvbExtendedEventDescription {
    uint8_t item_description_length;
    /** Item description */
    char *descr;
    uint8_t item_length;
    /** Item text */
    char *item;
} DvbExtendedEventDescription;

typedef struct DvbExtendedEventDescriptor {
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
} DvbExtendedEventDescriptor;


enum DvbLinkageDescriptorType {
    /* type 0x00 is reserved for future use */
    DVB_LINKAGE_TYPE_INFORMATION =               0x01,
    DVB_LINKAGE_TYPE_EPG =                       0x02,
    DVB_LINKAGE_TYPE_CA_REPLACEMENT =            0x03,
    DVB_LINKAGE_TYPE_COMPLETE_BOUQUET =          0x04,
    DVB_LINKAGE_TYPE_SERVICE_REPLACEMENT =       0x05,
    DVB_LINKAGE_TYPE_DATA_BROADCAST =            0x06,
    DVB_LINKAGE_TYPE_RCS_MAP =                   0x07,
    DVB_LINKAGE_TYPE_MOBILE_HAND_OVER =          0x08,
    DVB_LINKAGE_TYPE_SYSTEM_SOFTWARE_UPDATE =    0x09,
    DVB_LINKAGE_TYPE_CONTAINS_SSU_BAT_NIT =      0x0A,
    DVB_LINKAGE_TYPE_IP_MAC_NOTIF =              0x0B,
    DVB_LINKAGE_TYPE_CONTAINS_INT_BAT_NIT =      0x0C,
    DVB_LINKAGE_TYPE_EVENT =                     0x0D,
    DVB_LINKAGE_TYPE_EXTENDED_EVENT_FIRST =      0x0E,
    // EXTENDED_EVENT from 0x10 to 0x1F are reserved for future use
    DVB_LINKAGE_TYPE_EXTENDED_EVENT_LAST =       0x1F,
    DVB_LINKAGE_TYPE_DOWNLOADABLE_FONT_INFO =    0x20,
    /* types from 0x21 to 0x7F are reserved for future use */
    /* types from 0x80 to 0xFE are user defined */
    /* type 0xFF is reserved for future use */
};

typedef struct DvbLinkageDescriptor {
    uint16_t transport_stream_id;
    uint16_t original_network_id;
    uint16_t service_id;
    uint8_t linkage_type;
    void *link;
    // N * private_data
    char *private_data;
} DvbLinkageDescriptor;

enum DvbMobileHandOverLinkageType {
    /* type 0x00 is reserved for future use */
    DVB_MOBILE_HAND_OVER_IDENTICAL_SERVICE   = 0x01,
    DVB_MOBILE_HAND_OVER_LOCAL_VARIATION     = 0x02,
    DVB_MOBILE_HAND_OVER_ASSOCIATED_SERVICE  = 0x03,
    /* types from 0x04 to 0x0F are reserved for future use */
};

typedef struct DvbMobileHandOverLinkage {
    uint8_t hand_over_type :4;
    uint8_t reserved_future_use :3;
    uint8_t origin_type :1;
    // network_id only if hand_over_type in (0x01, 0x02, 0x03)
    uint16_t network_id;
    // initial_service_id if origin_type == 0x00
    uint16_t initial_service_id;
} DvbMobileHandOverLinkage;

typedef struct DvbEventLinkage {
    uint16_t target_event_id;
    uint8_t target_listed :1;
    uint8_t event_simulcast :1;
    uint8_t reserved :6;
} DvbEventLinkage;

enum DvbExtendedEventTargetIdType {
    EXTEVENT_TARGET_TYPE_TRANSPORT_STREAM_ID        = 0,
    EXTEVENT_TARGET_TYPE_TARGET_TRANSPORT_STREAM_ID = 1,
    EXTEVENT_TARGET_TYPE_WILDCARD                   = 2,
    EXTEVENT_TARGET_TYPE_USER_DEFINED_ID            = 3,
};

typedef struct DvbEELDescription {
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
} DvbEELDescription;

typedef struct DvbExtendedEventLinkage {
    uint8_t loop_length;
    SimpleLinkedList *descriptions;
} DvbExtendedEventLinkage;


typedef struct DvbMultilingualComponentDescription {
    char iso_639_language_code[3];
    uint8_t text_description_len;
    char* text;
} DvbMultilingualComponentDescription;

typedef struct DvbMultilingualComponentDescriptor {
    uint8_t component_tag;
    // N * description
    SimpleLinkedList *descriptions;
} DvbMultilingualComponentDescriptor;


typedef struct DvbParentalRatingDescription {
    /** Identify a country or group of countries */
    char country_code[3];
    /** Coded rating, use @mpegts_parental_rating_min_age() to have the minimum
     * age */
    uint8_t rating;
} DvbParentalRatingDescription;

typedef struct DvbParentalRatingDescriptor {
    int nb_descriptions;
    DvbParentalRatingDescription **descriptions;
} DvbParentalRatingDescriptor;


typedef struct DvbPDCDescriptor {
    char reserved_future_use :4;
    uint32_t programme_identification_label :20;
} DvbPDCDescriptor;


typedef struct DvbPrivateDataSpecifierDescriptor {
    uint32_t specifier; // ETSI TS 101 162 [56]
} DvbPrivateDataSpecifierDescriptor;


/**
 * @brief Short event descriptor
 */
typedef struct DvbShortEventDescriptor {
    /** Three character language code of the following char* fields */
    char iso_639_language_code[3];
    uint8_t event_name_len;
    char *event_name;
    uint8_t text_len;
    /** Text description of the event */
    char *text;
} DvbShortEventDescriptor;


typedef struct DvbShortSmoothingBufferDescriptor {
    uint8_t sb_size :2;
    uint8_t sb_leak_rate :6;
    char* DVB_reserved;
} DvbShortSmoothingBufferDescriptor;


typedef struct DvbStuffingDescriptor {
    char *stuffing_bytes;
} DvbStuffingDescriptor;


typedef struct DvbTelephoneDescriptor {
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
} DvbTelephoneDescriptor;


typedef struct DvbTimeShiftedEventDescriptor {
    uint16_t reference_service_id;
    uint16_t reference_event_id;
} DvbTimeShiftedEventDescriptor;


typedef struct DvbCAIdentifierDescriptor {
    int nb_ids;
    uint16_t *ca_system_id;
} DvbCAIdentifierDescriptor;

#endif  /* AVCODEC_DVBDESCRIPTORS_H */
