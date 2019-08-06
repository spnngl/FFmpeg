#ifndef AVFORMAT_MPEGTS_DESCRIPTORS_COMMON_H
#define AVFORMAT_MPEGTS_DESCRIPTORS_COMMON_H

#include <stddef.h>
#include <stdint.h>

#define MPEGTS_DESCRIPTORS_LOG AV_LOG_TRACE

#define bit_mask(x) ((unsigned)(1 << x) - 1)

struct SimpleLinkedList {
    void *data;
    struct SimpleLinkedList *next;
};
typedef struct SimpleLinkedList SimpleLinkedList;

typedef struct MpegTSDescriptorHeader MpegTSDescriptorHeader;
typedef struct MpegTSDescriptor MpegTSDescriptor;

struct MpegTSDescriptorHeader {
    uint8_t tag;
    uint8_t len;
};

struct MpegTSDescriptor {
    void *priv;
    int (*parse)(MpegTSDescriptor *, const uint8_t **pp, const uint8_t *p_end);
    int (*free)(MpegTSDescriptor *);
    void (*debug)(MpegTSDescriptor *);
};

MpegTSDescriptor* mpegts_get_descriptor(MpegTSDescriptorHeader *h);
int mpegts_parse_descriptor_header(MpegTSDescriptorHeader *h, const uint8_t **pp,
                                   const uint8_t *p_end);

enum MpegTSDescriptorTag {
    /* Descriptors from 0x00 to 0x01 are reserved */
    MPEGTS_DESCRIPTOR_TAG_VIDEO_STREAM_HEADER_PARAMS      = 0x02, // Video stream header parameters for ITU-T Rec. H.262, ISO/IEC 13818-2 and ISO/IEC 11172-2
 	MPEGTS_DESCRIPTOR_TAG_AUDIO_STREAM_HEADER_PARAMS      = 0x03, // Audio stream header parameters for ISO/IEC 13818-3 and ISO/IEC 11172-3
 	MPEGTS_DESCRIPTOR_TAG_HIERARCHY_STREAM                = 0x04, // Hierarchy for stream selection
 	MPEGTS_DESCRIPTOR_TAG_REGISTER_PRIVATE_FMT            = 0x05, // Registration of private formats
 	MPEGTS_DESCRIPTOR_TAG_STREAM_ALIGNMENT                = 0x06, // Data stream alignment for packetized video and audio sync point
 	MPEGTS_DESCRIPTOR_TAG_TARGET_BACKGROUND               = 0x07, // Target background grid defines total display area size
 	MPEGTS_DESCRIPTOR_TAG_DISPLAY_POSITION                = 0x08, // Video Window defines position in display area
 	MPEGTS_DESCRIPTOR_TAG_CAT_EMM_ECM_PID                 = 0x09, // Conditional access system and EMM/ECM PID
 	MPEGTS_DESCRIPTOR_TAG_ISO_639                         = 0x0A, // ISO 639 language and audio type
 	MPEGTS_DESCRIPTOR_TAG_EXTERNAL_CLOCK                  = 0x0B, // System clock external reference
 	MPEGTS_DESCRIPTOR_TAG_MULTIPLEX_BOUNDS                = 0x0C, // Multiplex buffer utilization bounds
 	MPEGTS_DESCRIPTOR_TAG_COPYRIGHT_SYSTEM                = 0x0D, // Copyright identification system and reference
 	MPEGTS_DESCRIPTOR_TAG_MAX_BIT_RATE                    = 0x0E, // Maximum bit rate
 	MPEGTS_DESCRIPTOR_TAG_PRIVATE_DATA                    = 0x0F, // Private data indicator
 	MPEGTS_DESCRIPTOR_TAG_SMOOTHING_BUFFER                = 0x10, // Smoothing buffer
 	MPEGTS_DESCRIPTOR_TAG_STD_VIDEO_LEAK_CONTROL          = 0x11, // STD video buffer leak control
 	MPEGTS_DESCRIPTOR_TAG_IBP_KEYFRAME                    = 0x12, // IBP video I-frame indicator
 	MPEGTS_DESCRIPTOR_TAG_IEC13818_6_CAROUSEL             = 0x13, // ISO/IEC13818-6 DSM CC carousel identifier
 	MPEGTS_DESCRIPTOR_TAG_IEC13818_6_ASSOCIATION          = 0x14, // ISO/IEC13818-6 DSM CC association tag
 	MPEGTS_DESCRIPTOR_TAG_IEC13818_6_DEFERRED_ASSOCIATION = 0x15, // ISO/IEC13818-6 DSM CC deferred association tag
 	MPEGTS_DESCRIPTOR_TAG_IEC13818_6_RESERVED             = 0x16, // ISO/IEC13818-6 DSM CC Reserved.
 	MPEGTS_DESCRIPTOR_TAG_DSM_NPT_REF                     = 0x17, // DSM CC NPT reference
 	MPEGTS_DESCRIPTOR_TAG_DSM_NPT_ENDPOINT                = 0x18, // DSM CC NPT endpoint
 	MPEGTS_DESCRIPTOR_TAG_DSM_STREAM_MODE                 = 0x19, // DSM CC stream mode
 	MPEGTS_DESCRIPTOR_TAG_DSM_STEAM_EVENT                 = 0x1A, // DSM CC stream event
 	MPEGTS_DESCRIPTOR_TAG_IEC_14496_2_VIDEO_HEADER        = 0x1B, // Video stream header parameters for ISO/IEC 14496-2 (MPEG-4 H.263 based)
 	MPEGTS_DESCRIPTOR_TAG_IEC_14496_2_AUDIO_HEADER        = 0x1C, // Audio stream header parameters for ISO/IEC 14496-3 (MPEG-4 LOAS multi-format framed)
 	MPEGTS_DESCRIPTOR_TAG_IOD_PARAMS                      = 0x1D, // IOD parameters for ISO/IEC 14496-1
 	MPEGTS_DESCRIPTOR_TAG_SL_PARAMS                       = 0x1E, // SL parameters for ISO/IEC 14496-1
 	MPEGTS_DESCRIPTOR_TAG_FMC_PARAMS                      = 0x1F, // FMC parameters for ISO/IEC 14496-1
 	MPEGTS_DESCRIPTOR_TAG_EXTERNAL_ES                     = 0x20, // External ES identifier for ISO/IEC 14496-1
 	MPEGTS_DESCRIPTOR_TAG_MUXCODE                         = 0x21, // MuxCode for ISO/IEC 14496-1
 	MPEGTS_DESCRIPTOR_TAG_FMX_BUFFER_SIZE                 = 0x22, // FMX Buffer Size for ISO/IEC 14496-1
 	MPEGTS_DESCRIPTOR_TAG_MUX_BUFFER                      = 0x23, // Multiplex Buffer for ISO/IEC 14496-1
 	MPEGTS_DESCRIPTOR_TAG_CONTENT_LABELING                = 0x24, // Content labeling for ISO/IEC 14496-1
 	MPEGTS_DESCRIPTOR_TAG_METADATA_POINTER                = 0x25, // Metadata pointer
 	MPEGTS_DESCRIPTOR_TAG_METADATA                        = 0x26, // Metadata
 	MPEGTS_DESCRIPTOR_TAG_METADATA_STD                    = 0x27, // Metadata STD
 	MPEGTS_DESCRIPTOR_TAG_IEC_14496_10_VIDEO_HEADER       = 0x28, // Video stream header parameters for ITU-T Rec. H.264 and ISO/IEC 14496-10
 	MPEGTS_DESCRIPTOR_TAG_IEC_13818_11_IPMP               = 0x29, // ISO/IEC 13818-11 IPMP (DRM)
 	MPEGTS_DESCRIPTOR_TAG_TIMING_HRD                      = 0x2A, // Timing and HRD for ITU-T Rec. H.264 and ISO/IEC 14496-10
 	MPEGTS_DESCRIPTOR_TAG_IEC_13818_7_AUDIO_HEADER        = 0x2B, // Audio stream header parameters for ISO/IEC 13818-7 ADTS AAC
 	MPEGTS_DESCRIPTOR_TAG_FLEXMUX_TIMING                  = 0x2C, // FlexMux Timing for ISO/IEC 14496-1
 	MPEGTS_DESCRIPTOR_TAG_TEXT_STREAM_HEADER              = 0x2D, // Text stream header parameters for ISO/IEC 14496
 	MPEGTS_DESCRIPTOR_TAG_IEC_14496_3_AUDIO_HEADER        = 0x2E, // Audio extension stream header parameters for ISO/IEC 14496-3 (MPEG-4 LOAS multi-format framed)
 	MPEGTS_DESCRIPTOR_TAG_VIDEO_AUXILIARY_HEADER          = 0x2F, // Video auxiliary stream header parameters
 	MPEGTS_DESCRIPTOR_TAG_VIDEO_SCALABLE_HEADER           = 0x30, // Video scalable stream header parameters
 	MPEGTS_DESCRIPTOR_TAG_VIDEO_MULTI_HEADER              = 0x31, // Video multi stream header parameters
 	MPEGTS_DESCRIPTOR_TAG_IEC_15444_HEADER                = 0x32, // Video stream header parameters for ITU-T Rec. T.800 and ISO/IEC 15444 (JPEG 2000)
 	MPEGTS_DESCRIPTOR_TAG_VIDEO_MULTI_OP_POINT_HEADER     = 0x33, // Video multi operation point stream header parameters
 	MPEGTS_DESCRIPTOR_TAG_VIDEO_STEREOSCOPIC_HEADER       = 0x34, // Video stereoscopic (3D) stream header parameters for ITU-T Rec. H.262, ISO/IEC 13818-2 and ISO/IEC 11172-2
 	MPEGTS_DESCRIPTOR_TAG_PROGRAM_STEREOSCOPIC_INFO       = 0x35, // Program stereoscopic (3D) information
 	MPEGTS_DESCRIPTOR_TAG_VIDEO_STEREOSCOPIC_INFO         = 0x36, // Video stereoscopic (3D) information
    /* Descriptors from 0x37 to 0x3F are reserved */
    /* Descriptors from 0x40 to 0x7F are used by DVB */
    MPEGTS_DESCRIPTOR_TAG_NETWORK_NAME                    = 0x40,
    MPEGTS_DESCRIPTOR_TAG_SERVICE_LIST                    = 0x41,
    MPEGTS_DESCRIPTOR_TAG_STUFFING                        = 0x42,
    MPEGTS_DESCRIPTOR_TAG_SATELLITE_DELIVERY_SYSTEM       = 0x43,
    MPEGTS_DESCRIPTOR_TAG_CABLE_DELIVERY_SYSTEM           = 0x44,
    MPEGTS_DESCRIPTOR_TAG_VBI_DATA                        = 0x45,
    MPEGTS_DESCRIPTOR_TAG_VBI_TELETEXT                    = 0x46,
    MPEGTS_DESCRIPTOR_TAG_BOUQUET_NAME                    = 0x47,
    MPEGTS_DESCRIPTOR_TAG_SERVICE                         = 0x48,
    MPEGTS_DESCRIPTOR_TAG_COUNTRY_AVAILABILITY            = 0x49,
    MPEGTS_DESCRIPTOR_TAG_LINKAGE                         = 0x4A,
    MPEGTS_DESCRIPTOR_TAG_NVOD_REFERENCE                  = 0x4B,
    MPEGTS_DESCRIPTOR_TAG_TIME_SHIFTED_REFERENCE          = 0x4C,
    MPEGTS_DESCRIPTOR_TAG_SHORT_EVENT                     = 0x4D,
    MPEGTS_DESCRIPTOR_TAG_EXTENDED_EVENT                  = 0x4E,
    MPEGTS_DESCRIPTOR_TAG_TIME_SHIFTED_EVENT              = 0x4F,
    MPEGTS_DESCRIPTOR_TAG_COMPONENT                       = 0x50,
    MPEGTS_DESCRIPTOR_TAG_MOSAIC                          = 0x51,
    MPEGTS_DESCRIPTOR_TAG_STREAM_IDENTIFIER               = 0x52,
    MPEGTS_DESCRIPTOR_TAG_CA_IDENTIFIER                   = 0x53,
    MPEGTS_DESCRIPTOR_TAG_CONTENT                         = 0x54,
    MPEGTS_DESCRIPTOR_TAG_PARENTAL_RATING                 = 0x55,
    MPEGTS_DESCRIPTOR_TAG_TELETEXT                        = 0x56,
    MPEGTS_DESCRIPTOR_TAG_TELEPHONE                       = 0x57,
    MPEGTS_DESCRIPTOR_TAG_LOCAL_TIME_OFFSET               = 0x58,
    MPEGTS_DESCRIPTOR_TAG_SUBTITLING                      = 0x59,
    MPEGTS_DESCRIPTOR_TAG_TERRESTRIAL_DELIVERY            = 0x5A,
    MPEGTS_DESCRIPTOR_TAG_MULTILINGUAL_NETWORK_NAME       = 0x5B,
    MPEGTS_DESCRIPTOR_TAG_MULTILINGUAL_BOUQUET_NAME       = 0x5C,
    MPEGTS_DESCRIPTOR_TAG_MULTILINGUAL_SERVICE_NAME       = 0x5D,
    MPEGTS_DESCRIPTOR_TAG_MULTILINGUAL_COMPONENT          = 0x5E,
    MPEGTS_DESCRIPTOR_TAG_PRIVATE_DATA_SPECIFIER          = 0x5F,
    MPEGTS_DESCRIPTOR_TAG_SERVICE_MOVE                    = 0x60,
    MPEGTS_DESCRIPTOR_TAG_SHORT_SMOOTHING_BUFFER          = 0x61,
    MPEGTS_DESCRIPTOR_TAG_FREQUENCY_LIST                  = 0x62,
    MPEGTS_DESCRIPTOR_TAG_PARTIAL_TRANSPORT_STREAM        = 0x63,
    MPEGTS_DESCRIPTOR_TAG_DATA_BROADCAST                  = 0x64,
    MPEGTS_DESCRIPTOR_TAG_SCRAMBLING                      = 0x65,
    MPEGTS_DESCRIPTOR_TAG_DATA_BROADCAST_ID               = 0x66,
    MPEGTS_DESCRIPTOR_TAG_TRANSPORT_STREAM                = 0x67,
    MPEGTS_DESCRIPTOR_TAG_DSNG                            = 0x68,
    MPEGTS_DESCRIPTOR_TAG_PDC                             = 0x69,
    MPEGTS_DESCRIPTOR_TAG_AC3                             = 0x6A,
    MPEGTS_DESCRIPTOR_TAG_ANCILLARY_DATA                  = 0x6B,
    MPEGTS_DESCRIPTOR_TAG_CELL_LIST                       = 0x6C,
    MPEGTS_DESCRIPTOR_TAG_CELL_FREQUENCY_LINK             = 0x6D,
    MPEGTS_DESCRIPTOR_TAG_ANNOUNCEMENT_SUPPORT            = 0x6E,
    MPEGTS_DESCRIPTOR_TAG_APPLICATION_SIGNALING           = 0x6F,
    MPEGTS_DESCRIPTOR_TAG_ADAPTATION_FIELD_DATA           = 0x70,
    MPEGTS_DESCRIPTOR_TAG_SERVICE_IDENTIFIER              = 0x71,
    MPEGTS_DESCRIPTOR_TAG_SERVICE_AVAILABILITY            = 0x72,
    MPEGTS_DESCRIPTOR_TAG_DEFAULT_AUTHORITY               = 0x73,
    MPEGTS_DESCRIPTOR_TAG_RELATED_CONTENT                 = 0x74,
    MPEGTS_DESCRIPTOR_TAG_TVA_ID                          = 0x75,
    MPEGTS_DESCRIPTOR_TAG_CONTENT_IDENTIFIER              = 0x76,
    MPEGTS_DESCRIPTOR_TAG_TIME_SLICE_FEC_IDENTIFIER       = 0x77,
    MPEGTS_DESCRIPTOR_TAG_ECM_REPETITION_RATE             = 0x78,
    MPEGTS_DESCRIPTOR_TAG_S2_SATELLITE_DELIVERY_SYSTEM    = 0x79,
    MPEGTS_DESCRIPTOR_TAG_ENHANCED_AC3                    = 0x7A,
    MPEGTS_DESCRIPTOR_TAG_DTS                             = 0x7B,
    MPEGTS_DESCRIPTOR_TAG_AAC                             = 0x7C,
    MPEGTS_DESCRIPTOR_TAG_XAIT_LOCATION                   = 0x7D,
    MPEGTS_DESCRIPTOR_TAG_FTA_CONTENT_MANAGEMENT          = 0x7E,
    MPEGTS_DESCRIPTOR_TAG_EXTENSION                       = 0x7F,
    /* Values from 0x80 to 0xfe are user defined */
    /* Value 0xff is reserved for future use */
};

#endif /* AVFORMAT_MPEGTS_DESCRIPTORS_COMMON_H */
