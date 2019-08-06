#include "libavutil/mem.h"
#include "../mpegts.h"
#include "linkage.h"
#include "common.h"

static int mobile_hand_over_linkage_parse(MpegTSLinkageDescriptor *ld,
                                          const uint8_t **pp, const uint8_t *p_end)
{
    int val;
    const uint8_t *p = *pp;
    MpegTSMobileHandOverLinkage *mhol;

    ld->link = av_mallocz(sizeof(MpegTSMobileHandOverLinkage));
    mhol = ld->link;

    val = mpegts_get8(&p, p_end);
    if (val < 0)
        return val;
    mhol->hand_over_type = (val >> 4);
    mhol->reserved_future_use = (val >> 1) & bit_mask(3);
    mhol->origin_type = val & bit_mask(1);

    switch(mhol->hand_over_type) {
    case MPEGTS_MOBILE_HAND_OVER_IDENTICAL_SERVICE:
    case MPEGTS_MOBILE_HAND_OVER_LOCAL_VARIATION:
    case MPEGTS_MOBILE_HAND_OVER_ASSOCIATED_SERVICE:
        val = mpegts_get16(&p, p_end);
        if (val < 0)
            break;
        mhol->network_id = val;
        break;
    default:
        break;
    };

    if (!mhol->origin_type) {
        val = mpegts_get16(&p, p_end);
        if (val < 0)
            return val;
        mhol->initial_service_id = val;
    }

    *pp = p;

    return 0;
}

static int event_linkage_parse(MpegTSLinkageDescriptor *ld,
                               const uint8_t **pp, const uint8_t *p_end)
{
    int val;
    const uint8_t *p = *pp;
    MpegTSEventLinkage *ev_link;

    ld->link = av_mallocz(sizeof(MpegTSEventLinkage));
    ev_link = ld->link;

    val = mpegts_get16(&p, p_end);
    if (val < 0)
        return val;
    ev_link->target_event_id = val;

    val = mpegts_get8(&p, p_end);
    if (val < 0)
        return val;
    ev_link->target_listed = (val >> 7);
    ev_link->event_simulcast = (val >> 6) & bit_mask(1);
    ev_link->reserved = val & bit_mask(6);

    *pp = p;

    return 0;
}

static int extended_event_linkage_parse(MpegTSLinkageDescriptor *ld,
                                        const uint8_t **pp, const uint8_t *p_end)
{
    int val;
    const uint8_t *p = *pp;
    uint8_t *descriptions_end;
    MpegTSExtendedEventLinkage *eel;

    ld->link = av_mallocz(sizeof(MpegTSExtendedEventLinkage));
    eel = ld->link;
    eel->descriptions = NULL;

    val = mpegts_get8(&p, p_end);
    if (val < 0)
        return val;
    eel->loop_length = val;

    descriptions_end = (uint8_t*)p + eel->loop_length;
    while(p < descriptions_end) {
        SimpleLinkedList *cur = av_mallocz(sizeof(SimpleLinkedList));
        MpegTSEELDescription *eeld = av_mallocz(sizeof(MpegTSEELDescription));

        val = mpegts_get16(&p, descriptions_end);
        if (val < 0)
            break;
        eeld->target_event_id = val;

        val = mpegts_get8(&p, descriptions_end);
        if (val < 0)
            break;
        eeld->target_listed = (val >> 7);
        eeld->event_simulcast = (val >> 6) & bit_mask(1);
        eeld->link_type = (val >> 4) & bit_mask(2);
        eeld->target_id_type = (val >> 2) & bit_mask(2);
        eeld->original_network_id_flag = (val >> 1) & bit_mask(1);
        eeld->service_id_flag = val & bit_mask(1);

        val = mpegts_get16(&p, descriptions_end);
        if (val < 0)
            break;
        eeld->id = val;

        if (eeld->target_id_type != EXTEVENT_TARGET_TYPE_USER_DEFINED_ID) {
            if (eeld->original_network_id_flag) {
                val = mpegts_get16(&p, descriptions_end);
                if (val < 0)
                    break;
                eeld->target_original_network_id = val;
            }
            if (eeld->service_id_flag) {
                val = mpegts_get16(&p, descriptions_end);
                if (val < 0)
                    break;
                eeld->target_service_id = val;
            }
        }
        cur->data = eeld;
        cur->next = eel->descriptions;
        eel->descriptions = cur;
    }

    *pp = p;

    return 0;
}

static int linkage_parse(MpegTSDescriptor *desc, const uint8_t **pp, const uint8_t *p_end)
{
    int val;
    const uint8_t *p = *pp;
    MpegTSLinkageDescriptor *ld;

    if (desc->priv)
        desc->free(desc);

    desc->priv = av_mallocz(sizeof(MpegTSLinkageDescriptor));
    ld = desc->priv;

    val = mpegts_get16(&p, p_end);
    if (val < 0)
        return val;
    ld->transport_stream_id = val;

    val = mpegts_get16(&p, p_end);
    if (val < 0)
        return val;
    ld->original_network_id = val;

    val = mpegts_get16(&p, p_end);
    if (val < 0)
        return val;
    ld->service_id = val;

    val = mpegts_get8(&p, p_end);
    if (val < 0)
        return val;
    ld->linkage_type = val;

    switch(ld->linkage_type) {
    case MPEGTS_LINKAGE_TYPE_MOBILE_HAND_OVER:
        mobile_hand_over_linkage_parse(ld, &p, p_end);
        break;
    case MPEGTS_LINKAGE_TYPE_EVENT:
        event_linkage_parse(ld, &p, p_end);
        break;
    case MPEGTS_LINKAGE_TYPE_EXTENDED_EVENT_FIRST ... MPEGTS_LINKAGE_TYPE_EXTENDED_EVENT_LAST:
        extended_event_linkage_parse(ld, &p, p_end);
        break;
    // TODO implement
    case MPEGTS_LINKAGE_TYPE_INFORMATION:
    case MPEGTS_LINKAGE_TYPE_EPG:
    case MPEGTS_LINKAGE_TYPE_CA_REPLACEMENT:
    case MPEGTS_LINKAGE_TYPE_COMPLETE_BOUQUET:
    case MPEGTS_LINKAGE_TYPE_SERVICE_REPLACEMENT:
    case MPEGTS_LINKAGE_TYPE_DATA_BROADCAST:
    case MPEGTS_LINKAGE_TYPE_RCS_MAP:
    case MPEGTS_LINKAGE_TYPE_SYSTEM_SOFTWARE_UPDATE:
    case MPEGTS_LINKAGE_TYPE_CONTAINS_SSU_BAT_NIT:
    case MPEGTS_LINKAGE_TYPE_IP_MAC_NOTIF:
    case MPEGTS_LINKAGE_TYPE_CONTAINS_INT_BAT_NIT:
    case MPEGTS_LINKAGE_TYPE_DOWNLOADABLE_FONT_INFO:
    default:
        break;
    };

    {
        int left = p_end - p;
        ld->private_data = av_mallocz(left + 1);
        memcpy(ld->private_data, p, left);
        ld->private_data[left] = '\0';
        p += left;
    }

    *pp = p;

    return 0;
}

static int mobile_hand_over_linkage_free(MpegTSLinkageDescriptor *ld)
{
    av_freep(&ld->link);
    return 0;
}

static int event_linkage_free(MpegTSLinkageDescriptor *ld)
{
    av_freep(&ld->link);
    return 0;
}

static int extended_event_linkage_free(MpegTSLinkageDescriptor *ld)
{
    MpegTSExtendedEventLinkage *eel = ld->link;
    SimpleLinkedList *head = eel->descriptions, *next;

    while(head) {
        next = head->next;
        av_free(head->data);
        av_free(head);
        head = next;
    }

    av_freep(&ld->link);
    return 0;
}

static int linkage_free(MpegTSDescriptor *desc)
{
    if (desc->priv) {
        MpegTSLinkageDescriptor *ld = desc->priv;


        switch(ld->linkage_type) {
        case MPEGTS_LINKAGE_TYPE_MOBILE_HAND_OVER:
            mobile_hand_over_linkage_free(ld);
            break;
        case MPEGTS_LINKAGE_TYPE_EVENT:
            event_linkage_free(ld);
            break;
        case MPEGTS_LINKAGE_TYPE_EXTENDED_EVENT_FIRST ... MPEGTS_LINKAGE_TYPE_EXTENDED_EVENT_LAST:
            extended_event_linkage_free(ld);
            break;
        // TODO implement
        case MPEGTS_LINKAGE_TYPE_INFORMATION:
        case MPEGTS_LINKAGE_TYPE_EPG:
        case MPEGTS_LINKAGE_TYPE_CA_REPLACEMENT:
        case MPEGTS_LINKAGE_TYPE_COMPLETE_BOUQUET:
        case MPEGTS_LINKAGE_TYPE_SERVICE_REPLACEMENT:
        case MPEGTS_LINKAGE_TYPE_DATA_BROADCAST:
        case MPEGTS_LINKAGE_TYPE_RCS_MAP:
        case MPEGTS_LINKAGE_TYPE_SYSTEM_SOFTWARE_UPDATE:
        case MPEGTS_LINKAGE_TYPE_CONTAINS_SSU_BAT_NIT:
        case MPEGTS_LINKAGE_TYPE_IP_MAC_NOTIF:
        case MPEGTS_LINKAGE_TYPE_CONTAINS_INT_BAT_NIT:
        case MPEGTS_LINKAGE_TYPE_DOWNLOADABLE_FONT_INFO:
        default:
            break;
        };

        av_free(ld->private_data);
        av_freep(&desc->priv);
    }
    return 0;
}

static void mobile_hand_over_linkage_debug(MpegTSLinkageDescriptor *ld)
{
    MpegTSMobileHandOverLinkage *mhol = ld->link;
    av_log(NULL, MPEGTS_DESCRIPTORS_LOG,
            "Mobile hand-over: type (%i), reserved(%i), "
            "origin_type(%i), network_id(%i), initial_service_id(%i)\n",
            mhol->hand_over_type, mhol->reserved_future_use,
            mhol->origin_type, mhol->network_id, mhol->initial_service_id);
}

static void event_linkage_debug(MpegTSLinkageDescriptor *ld)
{
    MpegTSEventLinkage *ev_link = ld->link;
    av_log(NULL, MPEGTS_DESCRIPTORS_LOG,
            "Event linkage: target_event_id(%i), target_listed(%i), "
            "event_simulcast(%i), reserved(%i)\n",
            ev_link->target_event_id, ev_link->target_listed,
            ev_link->event_simulcast, ev_link->reserved);
}

static void extended_event_linkage_debug(MpegTSLinkageDescriptor *ld)
{
    MpegTSExtendedEventLinkage *eel = ld->link;
    SimpleLinkedList *head = eel->descriptions;

    while(head) {
        MpegTSEELDescription *eeld = head->data;
        av_log(NULL, MPEGTS_DESCRIPTORS_LOG, "Extended event linkage: "
                "target_event_id(%i), target_listed(%i), "
                "event_simulcast(%i), link_type(%i), "
                "target_id_type(%i), original_network_id_flag(%i), "
                "service_id_flag(%i), id(%i), target_original_network_id(%i), "
                "target_service_id(%i)\n", eeld->target_event_id, eeld->target_listed,
                eeld->event_simulcast, eeld->link_type, eeld->target_id_type,
                eeld->original_network_id_flag, eeld->service_id_flag,
                eeld->id, eeld->target_original_network_id, eeld->target_service_id);
        head = head->next;
    }
}

static void linkage_debug(MpegTSDescriptor *desc)
{
    if (desc->priv) {
        MpegTSLinkageDescriptor *ld = desc->priv;
        av_log(NULL, MPEGTS_DESCRIPTORS_LOG, "Linkage descriptor: transport_stream_id(%i), "
               "original_network_id(%i), service_id(%i), linkage_type(%i), "
               "private_data(%s)\n",
               ld->transport_stream_id, ld->original_network_id, ld->service_id,
               ld->linkage_type, ld->private_data);

        switch(ld->linkage_type) {
        case MPEGTS_LINKAGE_TYPE_MOBILE_HAND_OVER:
            mobile_hand_over_linkage_debug(ld);
            break;
        case MPEGTS_LINKAGE_TYPE_EVENT:
            event_linkage_debug(ld);
            break;
        case MPEGTS_LINKAGE_TYPE_EXTENDED_EVENT_FIRST ... MPEGTS_LINKAGE_TYPE_EXTENDED_EVENT_LAST:
            extended_event_linkage_debug(ld);
            break;
        // TODO implement
        case MPEGTS_LINKAGE_TYPE_INFORMATION:
        case MPEGTS_LINKAGE_TYPE_EPG:
        case MPEGTS_LINKAGE_TYPE_CA_REPLACEMENT:
        case MPEGTS_LINKAGE_TYPE_COMPLETE_BOUQUET:
        case MPEGTS_LINKAGE_TYPE_SERVICE_REPLACEMENT:
        case MPEGTS_LINKAGE_TYPE_DATA_BROADCAST:
        case MPEGTS_LINKAGE_TYPE_RCS_MAP:
        case MPEGTS_LINKAGE_TYPE_SYSTEM_SOFTWARE_UPDATE:
        case MPEGTS_LINKAGE_TYPE_CONTAINS_SSU_BAT_NIT:
        case MPEGTS_LINKAGE_TYPE_IP_MAC_NOTIF:
        case MPEGTS_LINKAGE_TYPE_CONTAINS_INT_BAT_NIT:
        case MPEGTS_LINKAGE_TYPE_DOWNLOADABLE_FONT_INFO:
        default:
            break;
        }
    }
}

MpegTSDescriptor linkage_descriptor = {
    .priv = NULL,
    .parse = linkage_parse,
    .free = linkage_free,
    .debug = linkage_debug
};
