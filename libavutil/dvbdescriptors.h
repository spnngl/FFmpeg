/*
 * copyright (c) 2019 Anthony Delannoy
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/**
 * @file
 * Public header for DVB Descriptors.
 */

#ifndef AVCODEC_DVBDESCRIPTORS_H
#define AVCODEC_DVBDESCRIPTORS_H

#include <stddef.h>
#include <stdint.h>

typedef struct SimpleLinkedList SimpleLinkedList;
struct SimpleLinkedList {
    void *data;
    SimpleLinkedList *next;
};

/**
 * Every descriptor contains this header identifying its type and length.
 */
typedef struct  DvbDescriptorHeader {
    /**
     * The descriptor tag is an 8-bit field which identifies each descriptor.
     * Those values are listed below as `enum DvbDescriptorTag`.  Those values
     * with MPEG-2 normative meaning are described in ISO/IEC 13818-1.
     */
    uint8_t tag;
    /**
     * The descriptor length is an 8-bit field specifying the total number of
     * bytes of the data portion of the descriptor following the byte defining
     * the value of this field.
     */
    uint8_t len;
} DvbDescriptorHeader;

/**
 * This function parse a descriptor header pointed at by pp.
 * @param[out]  h       DvbDescriptorHeader getting data
 * @param       pp      Header data pointer
 * @param       p_end   Header data end pointer
 * @return      Error codes, 0 on success
 */
int av_dvb_parse_descriptor_header(DvbDescriptorHeader *h, const uint8_t **pp,
                                   const uint8_t *p_end);

/**
 * This structure helps the manipulation of descriptors.  One structure
 * declared in libavutil/dvbdescriptors.c for each descriptor.
 */
typedef struct DvbDescriptor DvbDescriptor;
struct DvbDescriptor {
    /**
     * Function to parse data from a descriptor.
     * @param   desc    Specific DvbDescriptor for current data
     * @param   pp      Pointer to descriptor data
     * @param   p_end   Pointer to the end of descriptor data
     * @return  Descriptor pointer or NULL on failure.
     */
    void* (*parse)(DvbDescriptor *, const uint8_t **pp, const uint8_t *p_end);
    /**
     * Function to print descriptor data on stdout.
     * @param   p               Descriptor pointer
     * @param   av_log_level    One of AV_LOG_*
     */
    void (*show)(void *p, int av_log_level);
    /**
     * Function to free descriptor data.
     * @param   p   Descriptor pointer
     * @return  Error codes, 0 on success
     */
    int (*free)(void *p);
};

/**
 * This function returns the right DvbDescriptor struct for the specified
 * DvbDescriptorHeader.
 * @param   h   Descriptor header
 * @return  Specific DvbDescriptor or NULL on failure.
 */
const DvbDescriptor* const av_dvb_get_descriptor(DvbDescriptorHeader *h);

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
    /* Values from 0x80 to 0xFE are user defined */
    /* Value 0xFF is reserved for future use */
};


/**
 * The component descriptor identifies the type of component stream and
 * may be used to provide a text description of the elementary stream
 * (see libavutil/componenttables.h).
 */
typedef struct DvbComponentDescriptor {
    DvbDescriptorHeader h;
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

/**
 * This function converts component stream informations into text
 * @param           comp    Component descriptor to translate
 * @param[out]      dst     Destination char array
 */
void av_epg_component_str(DvbComponentDescriptor *comp, char* dst);

/**
 * The intention of the content descriptor (see libavutil/contenttables.h) is
 * to provide classification information for an event.
 */
typedef struct DvbContentDescription {
    uint8_t nibble_lvl_1 :4; //< first level of a content identifier
    uint8_t nibble_lvl_2 :4; //< second level of a content identifier
    uint8_t user_byte; //< defined by the broadcaster
} DvbContentDescription;

/**
 * The intention of the content descriptor is to provide classification
 * information for an event.
 */
typedef struct DvbContentDescriptor {
    DvbDescriptorHeader h;
    // N * DvbContentDescription struct
    struct {
        int nb_descriptions;
        DvbContentDescription **descriptions;
    };
} DvbContentDescriptor;

/**
 * This function convert one content classification to a generic text
 * description
 * @param   description     Classification information to translate
 * @return  Generic text content description
 *
 * @note    Do not free resulting text, those are statically allocated.
 */
char* av_epg_content_simple_str(DvbContentDescription *description);

/**
 * This function convert one content classification to a detailed text
 * description
 * @param   description     Classification information to translate
 * @return  Detailed text content description
 *
 * @note    Do not free resulting text, those are statically allocated.
 */
char* av_epg_content_detailed_str(DvbContentDescription *description);


/**
 * The data broadcast descriptor identifies the type of the data component and
 * may be used to provide a text description of the data component.
 */
typedef struct DvbDataBroadcastDescriptor {
    DvbDescriptorHeader h;
    /**
    * This 16-bit field identifies the data broadcast specification that is used
    * to broadcast the data in the broadcast network. It shall be coded according
    * to ETSI TS 101 162.
    */
    uint16_t data_broadcast_id;
    /**
     * This optional 8-bit field has the same value as the component_tag field
     * in the stream identifier descriptor that may be present in the PSI
     * program map section for the stream on which the data is broadcast. If
     * this field is not used it shall be set to the value 0x00.
     */
    uint8_t component_tag;
    /**
     *  This 8-bit field specifies the length in bytes of the following selector field.
     */
    uint8_t selector_len;
    /**
     * The syntax and semantics of the selector field shall be defined by the
     * data broadcast specification that is identified in the data_broadcast_id
     * field. The selector field may contain service specific information that
     * is necessary to identify an entry-point of the broadcast data.
     */
    char* selector;
    /**
     * This 24-bit field contains the ISO 639-2 three character language code
     * of the following text fields.
     */
    char iso_639_language_code[3];
    /**
     * This 8-bit field specifies the length in bytes of the following text
     * describing the data component.
     */
    uint8_t text_len;
    /**
     * This field specifies the text description of the data component.
     */
    char *text;
} DvbDataBroadcastDescriptor;


typedef struct DvbExtendedEventDescription {
    /**
     * This 8-bit field specifies the length in bytes of the item description.
     */
    uint8_t item_description_length;
    /**
     * Item description
     */
    char *descr;
    /**
     * This 8-bit field specifies the length in bytes of the item text.
     */
    uint8_t item_length;
    /**
     * Item text
     */
    char *item;
} DvbExtendedEventDescription;

/**
 * The extended event descriptor provides a detailed text description of an
 * event, which may be used in addition to the short event descriptor. More
 * than one extended event descriptor can be associated to allow information
 * about one event greater in length than 256 bytes to be conveyed. Text
 * information can be structured into two columns, one giving an item
 * description field and the other the item text. A typical application for
 * this structure is to give a cast list, where for example the item
 * description field might be "Producer" and the item field would give the name
 * of the producer.
 */
typedef struct DvbExtendedEventDescriptor {
    DvbDescriptorHeader h;
    /**
     * This 4-bit field gives the number of the descriptor. It is used to
     * associate information which cannot be fitted into a single descriptor.
     * The descriptor_number of the first extended_event_descriptor of an
     * associated set of extended_event_descriptors shall be "0x00". The
     * descriptor_number shall be incremented by 1 with each additional
     * extended_event_descriptor in this sectio
     */
    uint8_t number :4;
    /**
     * This 4-bit field specifies the number of the last
     * extended_event_descriptor (that is, the descriptor with the highest
     * value of descriptor_number) of the associated set of descriptors of
     * which this descriptor is part.
     */
    uint8_t last_number :4;
    /**
     * This 24-bit field identifies the language of the following text field.
     */
    char iso_639_language_code[3];
    /**
     * This is an 8-bit field specifying the length in bytes of the following items.
     */
    uint8_t length_of_items;
    /** N * DvbExtendedEventDescription struct */
    SimpleLinkedList *descriptions;
    /**
     * This 8-bit field specifies the length in bytes of the non itemized extended text.
     */
    uint8_t text_len;
    /**
     * This field specify the non itemized extended text.
     */
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
    // Note: EXTENDED_EVENT from 0x10 to 0x1F are reserved for future use
    DVB_LINKAGE_TYPE_EXTENDED_EVENT_LAST =       0x1F,
    DVB_LINKAGE_TYPE_DOWNLOADABLE_FONT_INFO =    0x20,
    /* types from 0x21 to 0x7F are reserved for future use */
    /* types from 0x80 to 0xFE are user defined */
    /* type 0xFF is reserved for future use */
};

/**
 * The linkage descriptor identifies a service that can be presented if the
 * consumer requests for additional information related to a specific entity
 * described by the SI system. The location of the linkage descriptor in the
 * syntax indicates the entity for which additional information is available.
 * For example a linkage descriptor located within the NIT shall point to
 * a service providing additional information on the network, a linkage
 * descriptor in the BAT shall provide a link to a service informing about the
 * bouquet, etc.
 *
 * A CA replacement service can be identified using the linkage descriptor.
 * This service may be selected automatically by the IRD if the CA denies
 * access to the specific entity described by the SI system.
 *
 * A service replacement service can also be identified using the
 * linkage_descriptor.  This replacement service may be selected automatically
 * by the IRD when the running status of the current service is set to
 * "not_running".
 */
typedef struct DvbLinkageDescriptor {
    DvbDescriptorHeader h;
    /**
     * This is a 16-bit field which identifies the TS containing the information service indicated.
     */
    uint16_t ts_id;
    /**
     * This 16-bit field gives the label identifying the network_id of the
     * originating delivery system of the information service indicated.
     */
    uint16_t original_network_id;
    /**
     * This is a 16-bit field which uniquely identifies an information service
     * within a TS. The service_id is the same as the program_number in the
     * corresponding program_map_section. If the linkage_type field has the
     * value 0x04, then the service_id field is not relevant, and shall be set
     * to 0x0000.
     */
    uint16_t service_id;
    /**
     * This is an 8-bit field specifying the type of linkage e.g. to
     * information (see `enum DvbLinkageDescriptorType` above).
     */
    uint8_t linkage_type;
    /**
     * Pointer to a Dvb*Linkage struct
     * If linkage_type == 0x08 then DvbMobileHandOverLinkage
     * If linkage_type == 0x0D then DvbEventLinkage
     * If linkage_type in [0x0E ; 0x1F] then DvbExtendedEventLinkage
     */
    void *link;
    /**
     * This is an 8-bit field, the value of which is privately defined.
     */
    char *private_data;
} DvbLinkageDescriptor;

enum DvbMobileHandOverLinkageType {
    /* type 0x00 is reserved for future use */
    DVB_MOBILE_HAND_OVER_IDENTICAL_SERVICE   = 0x01,
    DVB_MOBILE_HAND_OVER_LOCAL_VARIATION     = 0x02,
    DVB_MOBILE_HAND_OVER_ASSOCIATED_SERVICE  = 0x03,
    /* types from 0x04 to 0x0F are reserved for future use */
};

/**
 * A service to which a mobile receiver might hand-over to can also be
 * identified using the DvbMobileHandOverLinkage structure in the
 * linkage_descriptor. This service may be selected automatically by the IRD
 * when the actual service is no longer receivable under its service_id. The
 * hand-over_type identifies whether the linkage_descriptor links to the same
 * service in a different country, to a local variation of the service or an
 * associated service.
 */
typedef struct DvbMobileHandOverLinkage {
    /**
     * This is a 4-bit field specifying the type of hand-over (see `enum
     * DvbMobileHandOverLinkage`).
     */
    uint8_t hand_over_type :4;
    uint8_t reserved_future_use :3;
    /**
     * This is a flag specifying in which table the link is originated.
     * If origin_type == 0 then NIT else SDT
     */
    uint8_t origin_type :1;
    /**
     * This is a 16-bit field which identifies the terrestrial network that
     * supports the service indicated.
     *
     * @note network_id present only and only if hand_over_type in (0x01, 0x02, 0x03)
     */
    uint16_t network_id;
    /**
     * This is a 16-bit field which identifies the service for which the hand-over linkage is valid.
     *
     * @note initial_service_id present only and only if origin_type == 0x00
     */
    uint16_t initial_service_id;
} DvbMobileHandOverLinkage;

/**
 * Two events can be signalled as equivalent using the using the
 * DvbEventLinkage structure in the linkage_descriptor. The event being
 * linked to may be a simulcast or may be time offset. The event_simulcast flag
 * shall only be set if the target event is higher quality.
 */
typedef struct DvbEventLinkage {
    /**
     * This 16-bit field identifies the event_id of the event (the target
     * event), carried on the service defined by the original_network_id,
     * transport_stream_id and service_id, which is equivalent to the event
     * identified by the location of this descriptor (the source event).
     */
    uint16_t target_event_id;
    /**
     * This 1-bit field signals whether the service defined by the
     * original_network_id, transport_stream_id and service_id is included in
     * the SDT carried in that Transport Stream. When target_listed is set to
     * 1 (one), the service shall be included in the SDT, otherwise it may not
     * be. In the latter case, the following conditions shall be met:
     *   - the service_type for the service shall be 0x19 (H.264/AVC HD digital
     *   television service) if the events are simulcast, otherwise the
     *   service_type shall be the same as for the service where the source event
     *   is carried.
     *   - EIT p/f information shall be available for the service in
     *   that Transport Stream.
     *   - the service shall be running.
     */
    uint8_t target_listed :1;
    /**
     * This 1-bit field shall be set to 1 (one) when the target event and the
     * source event are being simulcast. It shall be set to 0 (zero) when the
     * events are offset in time.
     */
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
    /**
     * Several events can be signalled as equivalent using the using the
     * extended event_linkage_info() structure in the linkage_descriptor. The
     * event being linked to may be a simulcast or may be time offset.
     */
    uint16_t target_event_id;
    /**
     * This 1-bit field signals whether the service defined by the
     * original_network_id, transport_stream_id and service_id is included in
     * the SDT carried in that Transport Stream. When target_listed is set to
     * 1 (one), the service shall be included in the SDT, otherwise it may not
     * be. In the latter case, the following conditions shall be met:
     *   - the service_type for the service shall be 0x19 (H.264/AVC HD digital
     *   television service) if the events are simulcast, otherwise the
     *   service_type shall be the same as for the service where the source
     *   event is carried
     *   - EIT p/f information shall be available for the service in that
     *   Transport Stream.
     *   - the service shall be running.
     */
    char target_listed :1;
    /**
     * This 1-bit field shall be set to 1 (one) when the target event and the
     * source event are being simulcast. It shall be set to 0 (zero) when the
     * events are offset in time.
     */
    char event_simulcast :1;
    /**
     * This 2-bit field indicates the type of the target service. It shall be
     * coded according libavutil/linkagetables.h
     */
    uint8_t link_type :2;
    /**
     * This 2-bit field, together with the original_network_id_flag, and the
     * service_id_flag, identifies the target service or services. A value of
     * zero indicates that the transport_stream_id shall be used to identify
     * a single target service. A value of one indicates that the
     * target_transport_stream_id shall be used instead of the
     * transport_stream_id to identify a single target service. A value of two
     * indicates that the target services can be in one or more transport
     * streams (wildcarded TSid). A value of 3 indicates the target services
     * are matched using a user defined identifier. The target_id_type field
     * shall be coded according to `enum DvbExtendedEventTargetIdType`.
     */
    uint8_t target_id_type :2;
    /**
     * This 1-bit flag, which when set to 1 (one) indicates that the
     * target_original_network_id shall be used instead of the
     * original_network_id to determine the target service.
     */
    char original_network_id_flag :1;
    /**
     * This 1-bit flag, which when set to 1 (one) indicates that the
     * target_service_id shall be used instead of the service_id to determine
     * the target service.
     */
    char service_id_flag :1;
    /**
     * user_defined_id:
     *      If this 16-bit field is used, the linkage descriptor should be in
     *      the scope of a private data specifier descriptor, so the receiver
     *      can determine the meaning of the user_defined_id field.
     * target_transport_stream_id:
     *      This is a 16-bit field which identifies an alternate TS containing
     *      the information service indicated, under the control of the
     *      target_id_type, the original_network_id_flag, and the
     *      service_id_flag fields.
     *
     * @note if target_id_type == 3 then user_defined_id else if target_id_type == 1
     *       it is the target_transport_stream_id
     */
    uint16_t id;

    // Below exists only if target_id_type != 3

    /**
     * This 16-bit field gives the label identifying the network_id of an
     * alternate originating delivery system of the information service
     * indicated, under the control of the target_id_type, the
     * original_network_id_flag, and the service_id_flag fields.
     *
     * @note exist only and only if original_network_id_flag is set
     */
    uint16_t target_original_network_id;
    /**
     * This is a 16-bit field which identifies an alternate information service
     * indicated, under the control of the target_id_type, the
     * original_network_id_flag, and the service_id_flag fields.
     *
     * @note exist only and only if service_id_flag is set
     */
    uint16_t target_service_id;
} DvbEELDescription;

/**
 * Several events can be signalled as equivalent using the using the
 * DvbExtendedEventLinkage structure in the linkage_descriptor. The event being
 * linked to may be a simulcast or may be time offset.
 */
typedef struct DvbExtendedEventLinkage {
    /**
     * Size in bytes of following descriptions (N * DvbEELDescription).
     */
    uint8_t loop_length;
    SimpleLinkedList *descriptions;
} DvbExtendedEventLinkage;


typedef struct DvbMultilingualComponentDescription {
    /**
     * This 24-bit field identifies the language of the following text
     * description of the component.
     */
    char iso_639_language_code[3];
    /**
     * Length in bytes of following text.
     */
    uint8_t text_description_len;
    /**
     * This field specifies a text description of the component stream.
     */
    char* text;
} DvbMultilingualComponentDescription;

/**
 * The multilingual component descriptor provides a text description of
 * a component in one or more languages. The component is identified by its
 * component tag value.
 */
typedef struct DvbMultilingualComponentDescriptor {
    DvbDescriptorHeader h;
    /**
     * This 8-bit field has the same value as the component_tag field in the
     * stream identifier descriptor (if present in the PSI program map section)
     * for the component stream.
     */
    uint8_t component_tag;
    // N * description
    SimpleLinkedList *descriptions;
} DvbMultilingualComponentDescriptor;


typedef struct DvbParentalRatingDescription {
    /**
     * Identify a country or group of countries.
     */
    char country_code[3];
    /**
     * This 8-bit field gives the recommended minimum age in years of the end
     * user. See av_epg_parental_rating_min_age().
     */
    uint8_t rating;
} DvbParentalRatingDescription;

/**
 * This descriptor gives a rating based on age and allows for extensions based
 * on other rating criteria.
 */
typedef struct DvbParentalRatingDescriptor {
    DvbDescriptorHeader h;
    struct {
        int nb_descriptions;
        DvbParentalRatingDescription **descriptions;
    };
} DvbParentalRatingDescriptor;

char* av_epg_parental_rating_min_age(const uint8_t rating);


/**
 * The PDC-descriptor extends the DVB system with the functionalities of PDC
 * (ETSI EN 300 231). The descriptor carries the Programme Identification Label
 * (PIL) as defined in ETSI EN 300 231. The PIL contains date and time of the
 * first published start time of a certain event.
 */
typedef struct DvbPDCDescriptor {
    DvbDescriptorHeader h;
    char reserved_future_use :4;
    /**
     * This 20-bit field gives the Programme Identification Label. Its
     * representation is (with bit 1 as the leftmost bit):
     *   - Bit 1 to 5 is the day
     *   - Bit 6 to 9 is the month
     *   - Bit 10 to 14 is the hour
     *   - Bit 15 to 20 is the minute
     */
    uint32_t programme_identification_label :20;
} DvbPDCDescriptor;

int av_epg_pdc_date_str(const uint32_t pil, char* dst);


/**
 * This descriptor is used to identify the specifier of any private descriptors
 * or private fields within descriptors.
 */
typedef struct DvbPrivateDataSpecifierDescriptor {
    DvbDescriptorHeader h;
    /**
     * This field shall be coded according to ETSI TS 101 162.
     */
    uint32_t specifier;
} DvbPrivateDataSpecifierDescriptor;


/**
 * The short event descriptor provides the name of the event and a short
 * description of the event in text form.
 */
typedef struct DvbShortEventDescriptor {
    DvbDescriptorHeader h;
    /**
     * This 24-bit field contains three character language code of the language
     * of the following text fields.
     */
    char iso_639_language_code[3];
    uint8_t event_name_len;
    char *event_name;
    uint8_t text_len;
    /**
     * Text description of the event
     */
    char *text;
} DvbShortEventDescriptor;


/**
 * A smoothing_buffer_descriptor is specified in ISO/IEC 13818-1 which enables
 * the bit-rate of a service to be signalled in the PSI.
 *
 * For use in DVB SI Tables, a more compact and efficient descriptor, the
 * short_smoothing_buffer_descriptor, is defined.
 *
 * This descriptor may be included in the EIT Present/Following and EIT
 * Schedule Tables to signal the bit-rate for each event.
 *
 * The bit-rate is expressed in terms of a smoothing buffer size and output
 * leak rate.
 *
 * The presence of the descriptor in the EIT Present/Following and EIT Schedule
 * Tables is optional.
 *
 * The data flows into and from the smoothing buffer are defined as follows:
 *   - bytes of TS packets belonging to the associated service are input to the
 *   smoothing buffer at the time defined by equation 2-4 of ISO/IEC 13818-1
 *   (definition of the mathematical byte delivery schedule). The following
 *   packets belong to the service:
 *       - all TS packets of all elementary streams of the service, i.e. all
 *       PIDs which are listed as elementary_PIDs in the extended program
 *       information part of the PMT section for the service during the time
 *       that the event is transmitted;
 *       - all TS packets of the PID which is identified as the program_map_PID
 *       for the service in the PAT at the time that the event is transmitted;
 *       - all TS packets of the PID which is identified as the PCR_PID in the
 *       PMT section for the service at the time that the event is transmitted.
 *   - all bytes that enter the buffer also exit it.
 */
typedef struct DvbShortSmoothingBufferDescriptor {
    DvbDescriptorHeader h;
    /**
     * This 2-bit field indicates the size of the smoothing buffer, and is
     * coded according to libavutil/smoothing_buffer_tables.h
     */
    uint8_t sb_size :2;
    /**
     * This 6-bit field indicates the value of the leak rate from the buffer,
     * and is coded according to libavutil/smoothing_buffer_tables.h
     */
    uint8_t sb_leak_rate :6;
    char* DVB_reserved;
} DvbShortSmoothingBufferDescriptor;

/**
 * This function convert DvbSHortSmoothingBufferDescriptor::sb_size to an
 * understandable string.
 * @param   sb_size     Smooth_buffer_size value
 * @return  Corresponding sb_size string.
 *
 * @note    Do not free resulting string, those are statically allocated.
 */
char* av_epg_sb_size_str(const uint8_t sb_size);

/**
 * This function convert DvbSHortSmoothingBufferDescriptor::sb_leak_rate to an
 * understandable string.
 * @param   sb_leak_rate     Smooth_buffer_leak_rate value
 * @return  Corresponding sb_leak_rate string.
 *
 * @note    Do not free resulting string, those are statically allocated.
 */
char* av_epg_sb_leak_rate_str(const uint8_t sb_leak_rate);


/**
 * The stuffing descriptor provides a means of invalidating previously coded
 * descriptors or inserting dummy descriptors for table stuffing.
 */
typedef struct DvbStuffingDescriptor {
    DvbDescriptorHeader h;
    /**
     * This is an 8-bit field. Each occurrence of the field may be set to any
     * value. The IRDs may discard the stuffing bytes.
     */
    char *stuffing_bytes;
} DvbStuffingDescriptor;


/**
 * The telephone descriptor may be used to indicate a telephone number which
 * may be used in conjunction with a modem (PSTN or cable) to exploit
 * narrowband interactive channels.
 * Further information is given in ETSI TS 102 201.
 */
typedef struct DvbTelephoneDescriptor {
    DvbDescriptorHeader h;
    char reserved_future_use1 :2;
    /**
     * This is a 1-bit flag. When set to "1" it indicates that the number
     * described can be called from outside of the country specified by the
     * country_prefix. When set to "0" it indicates that the number can only be
     * called from inside the country specified by the country_prefix.
     */
    char foreign_availability :1;
    /**
     * This is a 5-bit field which indicates connection types. One example of
     * the use of the connection type is to inform the IRD that when, if an
     * interaction is initiated, if the connection is not made within 1 minute,
     * then the connection attempt should be aborted.
     */
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


/**
 * The time shifted event descriptor is used in place of the
 * short_event_descriptor to indicate an event which is a time shifted copy of
 * another event.
 */
typedef struct DvbTimeShiftedEventDescriptor {
    DvbDescriptorHeader h;
    /**
     * This 16-bit field identifies the reference service of a NVOD collection
     * of services. The reference service can always be found in this Transport
     * Stream. The service_id here does not have a corresponding program_number
     * in the program_map_section.
     */
    uint16_t reference_service_id;
    /**
     * This 16-bit field identifies the reference event of which the event
     * described by this descriptor is a time shifted-copy.
     */
    uint16_t reference_event_id;
} DvbTimeShiftedEventDescriptor;


/**
 * The CA identifier descriptor indicates whether a particular bouquet, service
 * or event is associated with a conditional access system and identifies the
 * CA system type by means of the CA_system_id.
 */
typedef struct DvbCAIdentifierDescriptor {
    DvbDescriptorHeader h;
    struct {
        int nb_ids;
        /**
         * This 16-bit field identifies the CA system.
         */
        uint16_t *ca_system_id;
    };
} DvbCAIdentifierDescriptor;

#endif  /* AVCODEC_DVBDESCRIPTORS_H */
