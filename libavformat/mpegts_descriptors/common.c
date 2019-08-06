#include "common.h"
#include "../mpegts.h"

extern MpegTSDescriptor service_descriptor;
extern MpegTSDescriptor short_event_descriptor;
extern MpegTSDescriptor extended_event_descriptor;
extern MpegTSDescriptor parental_rating_descriptor;
extern MpegTSDescriptor stuffing_descriptor;
extern MpegTSDescriptor time_shifted_event_descriptor;
extern MpegTSDescriptor component_descriptor;
extern MpegTSDescriptor ca_identifier_descriptor;
extern MpegTSDescriptor content_descriptor;
extern MpegTSDescriptor telephone_descriptor;

static MpegTSDescriptor* all_descriptors[] = {
    [0x00 ... 0x01] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_VIDEO_STREAM_HEADER_PARAMS] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_AUDIO_STREAM_HEADER_PARAMS] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_HIERARCHY_STREAM] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_REGISTER_PRIVATE_FMT] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_STREAM_ALIGNMENT] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_TARGET_BACKGROUND] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_DISPLAY_POSITION] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_CAT_EMM_ECM_PID] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_ISO_639] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_EXTERNAL_CLOCK] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_MULTIPLEX_BOUNDS] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_COPYRIGHT_SYSTEM] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_MAX_BIT_RATE] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_PRIVATE_DATA] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_SMOOTHING_BUFFER] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_STD_VIDEO_LEAK_CONTROL] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_IBP_KEYFRAME] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_IEC13818_6_CAROUSEL] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_IEC13818_6_ASSOCIATION] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_IEC13818_6_DEFERRED_ASSOCIATION] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_IEC13818_6_RESERVED] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_DSM_NPT_REF] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_DSM_NPT_ENDPOINT] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_DSM_STREAM_MODE] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_DSM_STEAM_EVENT] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_IEC_14496_2_VIDEO_HEADER] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_IEC_14496_2_AUDIO_HEADER] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_IOD_PARAMS] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_SL_PARAMS] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_FMC_PARAMS] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_EXTERNAL_ES] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_MUXCODE] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_FMX_BUFFER_SIZE] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_MUX_BUFFER] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_CONTENT_LABELING] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_METADATA_POINTER] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_METADATA] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_METADATA_STD] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_IEC_14496_10_VIDEO_HEADER] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_IEC_13818_11_IPMP] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_TIMING_HRD] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_IEC_13818_7_AUDIO_HEADER] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_FLEXMUX_TIMING] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_TEXT_STREAM_HEADER] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_IEC_14496_3_AUDIO_HEADER] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_VIDEO_AUXILIARY_HEADER] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_VIDEO_SCALABLE_HEADER] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_VIDEO_MULTI_HEADER] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_IEC_15444_HEADER] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_VIDEO_MULTI_OP_POINT_HEADER] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_VIDEO_STEREOSCOPIC_HEADER] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_PROGRAM_STEREOSCOPIC_INFO] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_VIDEO_STEREOSCOPIC_INFO] = NULL,
    [0x37 ... 0x3F] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_NETWORK_NAME] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_SERVICE_LIST] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_STUFFING] = &stuffing_descriptor,
    [MPEGTS_DESCRIPTOR_TAG_SATELLITE_DELIVERY_SYSTEM] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_CABLE_DELIVERY_SYSTEM] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_VBI_DATA] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_VBI_TELETEXT] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_BOUQUET_NAME] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_SERVICE] = &service_descriptor,
    [MPEGTS_DESCRIPTOR_TAG_COUNTRY_AVAILABILITY] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_LINKAGE] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_NVOD_REFERENCE] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_TIME_SHIFTED_REFERENCE] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_SHORT_EVENT] = &short_event_descriptor,
    [MPEGTS_DESCRIPTOR_TAG_EXTENDED_EVENT] = &extended_event_descriptor,
    [MPEGTS_DESCRIPTOR_TAG_TIME_SHIFTED_EVENT] = &time_shifted_event_descriptor,
    [MPEGTS_DESCRIPTOR_TAG_COMPONENT] = &component_descriptor,
    [MPEGTS_DESCRIPTOR_TAG_MOSAIC] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_STREAM_IDENTIFIER] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_CA_IDENTIFIER] = &ca_identifier_descriptor,
    [MPEGTS_DESCRIPTOR_TAG_CONTENT] = &content_descriptor,
    [MPEGTS_DESCRIPTOR_TAG_PARENTAL_RATING] = &parental_rating_descriptor,
    [MPEGTS_DESCRIPTOR_TAG_TELETEXT] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_TELEPHONE] = &telephone_descriptor,
    [MPEGTS_DESCRIPTOR_TAG_LOCAL_TIME_OFFSET] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_SUBTITLING] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_TERRESTRIAL_DELIVERY] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_MULTILINGUAL_NETWORK_NAME] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_MULTILINGUAL_BOUQUET_NAME] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_MULTILINGUAL_SERVICE_NAME] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_MULTILINGUAL_COMPONENT] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_PRIVATE_DATA_SPECIFIER] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_SERVICE_MOVE] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_SHORT_SMOOTHING_BUFFER] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_FREQUENCY_LIST] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_PARTIAL_TRANSPORT_STREAM] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_DATA_BROADCAST] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_SCRAMBLING] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_DATA_BROADCAST_ID] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_TRANSPORT_STREAM] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_DSNG] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_PDC] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_AC3] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_ANCILLARY_DATA] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_CELL_LIST] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_CELL_FREQUENCY_LINK] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_ANNOUNCEMENT_SUPPORT] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_APPLICATION_SIGNALING] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_ADAPTATION_FIELD_DATA] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_SERVICE_IDENTIFIER] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_SERVICE_AVAILABILITY] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_DEFAULT_AUTHORITY] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_RELATED_CONTENT] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_TVA_ID] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_CONTENT_IDENTIFIER] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_TIME_SLICE_FEC_IDENTIFIER] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_ECM_REPETITION_RATE] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_S2_SATELLITE_DELIVERY_SYSTEM] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_ENHANCED_AC3] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_DTS] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_AAC] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_XAIT_LOCATION] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_FTA_CONTENT_MANAGEMENT] = NULL,
    [MPEGTS_DESCRIPTOR_TAG_EXTENSION] = NULL,
    [0x80 ... 0xFF] = NULL
};

int mpegts_parse_descriptor_header(MpegTSDescriptorHeader *h, const uint8_t **pp,
                                   const uint8_t *p_end)
{
    int val;
    const uint8_t *p = *pp;

    val = mpegts_get8(&p, p_end);
    if (val < 0)
        return val;
    h->tag = val;

    val = mpegts_get8(&p, p_end);
    if (val < 0)
        return val;
    h->len = val;

    *pp = p;

    return 0;
}

MpegTSDescriptor* mpegts_get_descriptor(MpegTSDescriptorHeader *h)
{
    return all_descriptors[h->tag];
}
