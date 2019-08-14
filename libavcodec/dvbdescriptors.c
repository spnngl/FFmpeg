#include "libavutil/mem.h"
#include "libavutil/dvb.h"
#include "dvbdescriptors.h"

static void* component_parse(DvbDescriptor *desc, const uint8_t **pp, const uint8_t *p_end)
{
    int val, text_len;
    const uint8_t *p = *pp;
    DvbComponentDescriptor *comp;

    comp = av_mallocz(sizeof(DvbComponentDescriptor));

    val = avpriv_dvb_get8(&p, p_end);
    if (val < 0)
        goto fail;
    comp->stream_content_ext = (val >> 4);
    comp->stream_content = val & 0xf;

    val = avpriv_dvb_get8(&p, p_end);
    if (val < 0)
        goto fail;
    comp->component_type = val;

    val = avpriv_dvb_get8(&p, p_end);
    if (val < 0)
        goto fail;
    comp->component_tag = val;

    memcpy(comp->iso_639_language_code, p, 3);
    p += 3;

    text_len = p_end - p;
    comp->text = av_malloc(text_len + 1);
    memcpy(comp->text, p, text_len);
    comp->text[text_len] = '\0';
    p += text_len;
    *pp = p;

    return comp;

fail:
    desc->free(comp);
    return NULL;
}

static int component_free(void *p)
{
    if (p) {
        DvbComponentDescriptor *comp = p;
        av_free(comp->text);
        av_free(p);
    }
    return 0;
}

static DvbDescriptor component_descriptor = {
    .parse = component_parse,
    .free = component_free,
};


static void* content_parse(DvbDescriptor *desc, const uint8_t **pp, const uint8_t *p_end)
{
    int val;
    const uint8_t *p = *pp;
    DvbContentDescriptor *content;

    content = av_mallocz(sizeof(DvbContentDescriptor));

    content->nb_descriptions = (p_end - p) / sizeof(DvbContentDescription);
    content->descriptions = av_mallocz(content->nb_descriptions * sizeof(DvbContentDescription*));

    for(int i = 0; i < content->nb_descriptions; i++) {
        DvbContentDescription *d;

        content->descriptions[i] = av_mallocz(sizeof(DvbContentDescription));
        d = content->descriptions[i];

        val = avpriv_dvb_get8(&p, p_end);
        if (val < 0)
            goto fail;
        d->content_nibble_lvl_1 = (val >> 4) & 0xf;
        d->content_nibble_lvl_2 = val & 0xf;

        val = avpriv_dvb_get8(&p, p_end);
        if (val < 0)
            goto fail;
        d->user_byte = val;
    }

    return content;

fail:
    desc->free(content);
    return NULL;
}

static int content_free(void *p)
{
    if (p) {
        DvbContentDescriptor *content = p;

        for(int i = 0; i < content->nb_descriptions; i++)
            av_free(content->descriptions[i]);
        av_free(content->descriptions);
        av_free(p);
    }
    return 0;
}

static DvbDescriptor content_descriptor = {
    .parse = content_parse,
    .free = content_free,
};


static void* data_broadcast_parse(DvbDescriptor *desc, const uint8_t **pp, const uint8_t *p_end)
{
    int val;
    const uint8_t *p = *pp;
    DvbDataBroadcastDescriptor *dbd;

    dbd = av_mallocz(sizeof(DvbDataBroadcastDescriptor));

    val = avpriv_dvb_get16(&p, p_end);
    if (val < 0)
        goto fail;
    dbd->data_broadcast_id = val;

    val = avpriv_dvb_get8(&p, p_end);
    if (val < 0)
        goto fail;
    dbd->component_tag = val;

    val = avpriv_dvb_get8(&p, p_end);
    if (val < 0)
        goto fail;
    dbd->selector_len = val;

    dbd->selector = av_malloc(dbd->selector_len + 1);
    memcpy(dbd->selector, p, dbd->selector_len);
    dbd->selector[dbd->selector_len] = '\0';
    p += dbd->selector_len;

    memcpy(dbd->iso_639_language_code, p, 3);
    p += 3;

    val = avpriv_dvb_get8(&p, p_end);
    if (val < 0)
        goto fail;
    dbd->text_len = val;


    dbd->text = av_malloc(dbd->text_len + 1);
    memcpy(dbd->selector, p, dbd->text_len);
    dbd->selector[dbd->text_len] = '\0';
    p += dbd->text_len;

    *pp = p;

    return dbd;

fail:
    desc->free(dbd);
    return NULL;
}

static int data_broadcast_free(void *p)
{
    if (p) {
        DvbDataBroadcastDescriptor *dbd = p;
        av_free(dbd->selector);
        av_free(dbd->text);
        av_free(dbd);
    }
    return 0;
}

static DvbDescriptor data_broadcast_descriptor = {
    .parse = data_broadcast_parse,
    .free = data_broadcast_free,
};


static void* extended_event_parse(DvbDescriptor *desc, const uint8_t **pp, const uint8_t *p_end)
{
    int val;
    const uint8_t *p = *pp, *items_end;
    DvbExtendedEventDescriptor *event;

    event = av_mallocz(sizeof(DvbExtendedEventDescriptor));

    val = avpriv_dvb_get8(&p, p_end);
    if (val < 0)
        goto fail;
    event->number = (val >> 4);
    event->last_number = val & 0xf;

    memcpy(event->iso_639_language_code, p, 3);
    p += 3;

    val = avpriv_dvb_get8(&p, p_end);
    if (val < 0)
        goto fail;
    event->length_of_items = val;

    items_end = p + event->length_of_items;

    event->descriptions = NULL;
    while(p < items_end) {
        SimpleLinkedList *cur = av_mallocz(sizeof(SimpleLinkedList));
        DvbExtendedEventDescription *extev_desc = av_mallocz(sizeof(DvbExtendedEventDescription));

        val = avpriv_dvb_get8(&p, items_end);
        if (val < 0)
            goto fail;
        extev_desc->item_description_length = val;

        extev_desc->descr = av_malloc(extev_desc->item_description_length + 1);
        memcpy(extev_desc->descr, p, extev_desc->item_description_length);
        extev_desc->descr[extev_desc->item_description_length] = '\0';
        p += extev_desc->item_description_length;

        val = avpriv_dvb_get8(&p, items_end);
        if (val < 0)
            goto fail;
        extev_desc->item_length = val;

        extev_desc->item = av_malloc(extev_desc->item_length + 1);
        memcpy(extev_desc->item, p, extev_desc->item_length);
        extev_desc->item[extev_desc->item_length] = '\0';
        p += extev_desc->item_length;

        cur->data = extev_desc;
        cur->next = event->descriptions;
        event->descriptions = cur;
    }

    val = avpriv_dvb_get8(&p, p_end);
    if (val < 0)
        goto fail;
    event->text_len = val;

    event->text = av_malloc(event->text_len + 1);
    memcpy(event->text, p, event->text_len);
    event->text[event->text_len] = '\0';
    p += event->text_len;

    *pp = p;
    return event;

fail:
    desc->free(event);
    return NULL;
}

static int extended_event_free(void *p)
{
    if (p) {
        DvbExtendedEventDescriptor *event = p;
        SimpleLinkedList *head, *next;

        for (head = event->descriptions; head;) {
            DvbExtendedEventDescription *d = head->data;
            next = head->next;
            av_free(d->descr);
            av_free(d->item);
            av_free(d);
            av_free(head);
            head = next;
        }
        av_free(event->text);
        av_free(p);
    }
    return 0;
}

static DvbDescriptor extended_event_descriptor = {
    .parse = extended_event_parse,
    .free = extended_event_free,
};


static int mobile_hand_over_linkage_free(DvbMobileHandOverLinkage *link)
{
    av_free(link);
    return 0;
}

static int event_linkage_free(DvbEventLinkage *link)
{
    av_free(link);
    return 0;
}

static int extended_event_linkage_free(DvbExtendedEventLinkage *link)
{
    SimpleLinkedList *head = link->descriptions, *next;

    while(head) {
        next = head->next;
        av_free(head->data);
        av_free(head);
        head = next;
    }

    av_free(link);
    return 0;
}

static int linkage_free(void *p)
{
    if (p) {
        DvbLinkageDescriptor *ld = p;


        switch(ld->linkage_type) {
        case DVB_LINKAGE_TYPE_MOBILE_HAND_OVER:
            mobile_hand_over_linkage_free(ld->link);
            break;
        case DVB_LINKAGE_TYPE_EVENT:
            event_linkage_free(ld->link);
            break;
        case DVB_LINKAGE_TYPE_EXTENDED_EVENT_FIRST ... DVB_LINKAGE_TYPE_EXTENDED_EVENT_LAST:
            extended_event_linkage_free(ld->link);
            break;
        // TODO implement
        case DVB_LINKAGE_TYPE_INFORMATION:
        case DVB_LINKAGE_TYPE_EPG:
        case DVB_LINKAGE_TYPE_CA_REPLACEMENT:
        case DVB_LINKAGE_TYPE_COMPLETE_BOUQUET:
        case DVB_LINKAGE_TYPE_SERVICE_REPLACEMENT:
        case DVB_LINKAGE_TYPE_DATA_BROADCAST:
        case DVB_LINKAGE_TYPE_RCS_MAP:
        case DVB_LINKAGE_TYPE_SYSTEM_SOFTWARE_UPDATE:
        case DVB_LINKAGE_TYPE_CONTAINS_SSU_BAT_NIT:
        case DVB_LINKAGE_TYPE_IP_MAC_NOTIF:
        case DVB_LINKAGE_TYPE_CONTAINS_INT_BAT_NIT:
        case DVB_LINKAGE_TYPE_DOWNLOADABLE_FONT_INFO:
        default:
            break;
        };

        av_free(ld->private_data);
        av_free(p);
    }
    return 0;
}

static void* mobile_hand_over_linkage_parse(const uint8_t **pp, const uint8_t *p_end)
{
    int val;
    const uint8_t *p = *pp;
    DvbMobileHandOverLinkage *mhol;

    mhol = av_mallocz(sizeof(DvbMobileHandOverLinkage));

    val = avpriv_dvb_get8(&p, p_end);
    if (val < 0)
        goto fail;
    mhol->hand_over_type = (val >> 4);
    mhol->reserved_future_use = (val >> 1) & 0x7;
    mhol->origin_type = val & 0x1;

    switch(mhol->hand_over_type) {
    case DVB_MOBILE_HAND_OVER_IDENTICAL_SERVICE:
    case DVB_MOBILE_HAND_OVER_LOCAL_VARIATION:
    case DVB_MOBILE_HAND_OVER_ASSOCIATED_SERVICE:
        val = avpriv_dvb_get16(&p, p_end);
        if (val < 0)
            goto fail;
        mhol->network_id = val;
        break;
    default:
        break;
    };

    if (!mhol->origin_type) {
        val = avpriv_dvb_get16(&p, p_end);
        if (val < 0)
            goto fail;
        mhol->initial_service_id = val;
    }

    *pp = p;

    return mhol;

fail:
    mobile_hand_over_linkage_free(mhol);
    return NULL;
}

static void* event_linkage_parse(const uint8_t **pp, const uint8_t *p_end)
{
    int val;
    const uint8_t *p = *pp;
    DvbEventLinkage *ev_link;

    ev_link = av_mallocz(sizeof(DvbEventLinkage));

    val = avpriv_dvb_get16(&p, p_end);
    if (val < 0)
        goto fail;
    ev_link->target_event_id = val;

    val = avpriv_dvb_get8(&p, p_end);
    if (val < 0)
        goto fail;
    ev_link->target_listed = (val >> 7);
    ev_link->event_simulcast = (val >> 6) & 0x1;
    ev_link->reserved = val & 0x3f;

    *pp = p;

    return ev_link;

fail:
    event_linkage_free(ev_link);
    return NULL;
}

static void* extended_event_linkage_parse(const uint8_t **pp, const uint8_t *p_end)
{
    int val;
    const uint8_t *p = *pp;
    uint8_t *descriptions_end;
    DvbExtendedEventLinkage *eel;

    eel = av_mallocz(sizeof(DvbExtendedEventLinkage));
    eel->descriptions = NULL;

    val = avpriv_dvb_get8(&p, p_end);
    if (val < 0)
        goto fail;
    eel->loop_length = val;

    descriptions_end = (uint8_t*)p + eel->loop_length;
    while(p < descriptions_end) {
        SimpleLinkedList *cur = av_mallocz(sizeof(SimpleLinkedList));
        DvbEELDescription *eeld = av_mallocz(sizeof(DvbEELDescription));

        val = avpriv_dvb_get16(&p, descriptions_end);
        if (val < 0)
            break;
        eeld->target_event_id = val;

        val = avpriv_dvb_get8(&p, descriptions_end);
        if (val < 0)
            break;
        eeld->target_listed = (val >> 7);
        eeld->event_simulcast = (val >> 6) & 0x1;
        eeld->link_type = (val >> 4) & 0x3;
        eeld->target_id_type = (val >> 2) & 0x3;
        eeld->original_network_id_flag = (val >> 1) & 0x1;
        eeld->service_id_flag = val & 0x1;

        val = avpriv_dvb_get16(&p, descriptions_end);
        if (val < 0)
            break;
        eeld->id = val;

        if (eeld->target_id_type != EXTEVENT_TARGET_TYPE_USER_DEFINED_ID) {
            if (eeld->original_network_id_flag) {
                val = avpriv_dvb_get16(&p, descriptions_end);
                if (val < 0)
                    break;
                eeld->target_original_network_id = val;
            }
            if (eeld->service_id_flag) {
                val = avpriv_dvb_get16(&p, descriptions_end);
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

    return eel;

fail:
    extended_event_linkage_free(eel);
    return NULL;
}

static void* linkage_parse(DvbDescriptor *desc, const uint8_t **pp, const uint8_t *p_end)
{
    int val;
    const uint8_t *p = *pp;
    DvbLinkageDescriptor *ld;

    ld = av_mallocz(sizeof(DvbLinkageDescriptor));

    val = avpriv_dvb_get16(&p, p_end);
    if (val < 0)
        goto fail;
    ld->transport_stream_id = val;

    val = avpriv_dvb_get16(&p, p_end);
    if (val < 0)
        goto fail;
    ld->original_network_id = val;

    val = avpriv_dvb_get16(&p, p_end);
    if (val < 0)
        goto fail;
    ld->service_id = val;

    val = avpriv_dvb_get8(&p, p_end);
    if (val < 0)
        goto fail;
    ld->linkage_type = val;

    switch(ld->linkage_type) {
    case DVB_LINKAGE_TYPE_MOBILE_HAND_OVER:
        ld->link = mobile_hand_over_linkage_parse(&p, p_end);
        break;
    case DVB_LINKAGE_TYPE_EVENT:
        ld->link = event_linkage_parse(&p, p_end);
        break;
    case DVB_LINKAGE_TYPE_EXTENDED_EVENT_FIRST ... DVB_LINKAGE_TYPE_EXTENDED_EVENT_LAST:
        ld->link = extended_event_linkage_parse(&p, p_end);
        break;
    case DVB_LINKAGE_TYPE_INFORMATION:
    case DVB_LINKAGE_TYPE_EPG:
    case DVB_LINKAGE_TYPE_CA_REPLACEMENT:
    case DVB_LINKAGE_TYPE_COMPLETE_BOUQUET:
    case DVB_LINKAGE_TYPE_SERVICE_REPLACEMENT:
    case DVB_LINKAGE_TYPE_DATA_BROADCAST:
    case DVB_LINKAGE_TYPE_RCS_MAP:
    case DVB_LINKAGE_TYPE_SYSTEM_SOFTWARE_UPDATE:
    case DVB_LINKAGE_TYPE_CONTAINS_SSU_BAT_NIT:
    case DVB_LINKAGE_TYPE_IP_MAC_NOTIF:
    case DVB_LINKAGE_TYPE_CONTAINS_INT_BAT_NIT:
    case DVB_LINKAGE_TYPE_DOWNLOADABLE_FONT_INFO:
    default:
        ld->link = NULL;
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

    return ld;

fail:
    desc->free(ld);
    return NULL;
}


static DvbDescriptor linkage_descriptor = {
    .parse = linkage_parse,
    .free = linkage_free,
};


static void* multilingual_component_parse(DvbDescriptor *desc, const uint8_t **pp, const uint8_t *p_end)
{
    int val;
    const uint8_t *p = *pp;
    DvbMultilingualComponentDescriptor *mcd;

    mcd = av_mallocz(sizeof(DvbMultilingualComponentDescriptor));

    val = avpriv_dvb_get8(&p, p_end);
    if (val < 0)
        goto fail;
    mcd->component_tag = val;

    while(p < p_end) {
        SimpleLinkedList *cur;
        DvbMultilingualComponentDescription *comp =
            av_mallocz(sizeof(DvbMultilingualComponentDescription));

        memcpy(comp->iso_639_language_code, p, 3);
        p += 3;

        val = avpriv_dvb_get8(&p, p_end);
        if (val < 0)
            goto fail;
        comp->text_description_len = val;

        comp->text = av_malloc(comp->text_description_len + 1);
        memcpy(comp->text, p, comp->text_description_len);
        comp->text[comp->text_description_len] = '\0';
        p += comp->text_description_len;

        cur = av_mallocz(sizeof(SimpleLinkedList));
        cur->data = comp;
        cur->next = mcd->descriptions;
        mcd->descriptions = cur;
    }

    return mcd;

fail:
    desc->free(mcd);
    return NULL;
}

static int multilingual_component_free(void *p)
{
    if (p) {
        DvbMultilingualComponentDescriptor *mcd = p;
        SimpleLinkedList *head, *next;

        for (head = mcd->descriptions; head;) {
            DvbMultilingualComponentDescription *d = head->data;

            next = head->next;
            av_free(d->text);
            av_free(d);
            av_free(head);
            head = next;
        }
        av_free(mcd);
    }
    return 0;
}

static DvbDescriptor multilingual_component_descriptor = {
    .parse = multilingual_component_parse,
    .free = multilingual_component_free,
};


static void* parental_rating_parse(DvbDescriptor *desc, const uint8_t **pp, const uint8_t *p_end)
{
    int val, i = 0;
    const uint8_t *p = *pp;
    DvbParentalRatingDescriptor *prd;

    prd = av_mallocz(sizeof(DvbParentalRatingDescriptor));
    prd->nb_descriptions = (p_end - p) / sizeof(DvbParentalRatingDescription);
    prd->descriptions = av_mallocz(prd->nb_descriptions * sizeof(DvbParentalRatingDescription*));

    while(p < p_end && i < prd->nb_descriptions) {
        DvbParentalRatingDescription *d;

        prd->descriptions[i] = av_mallocz(sizeof(DvbParentalRatingDescription));
        d = prd->descriptions[i];

        memcpy(d->country_code, p, 3);
        p += 3;

        val = avpriv_dvb_get8(&p, p_end);
        if (val < 0)
            goto fail;
        d->rating = val;
        i++;
    }

    *pp = p;

    return prd;

fail:
    desc->free(prd);
    return NULL;
}

static int parental_rating_free(void *p)
{
    if (p) {
        DvbParentalRatingDescriptor *prd = p;

        for (int i = 0; i < prd->nb_descriptions; i++)
            av_free(prd->descriptions[i]);
        av_free(prd->descriptions);
        av_free(p);
    }
    return 0;
}

static DvbDescriptor parental_rating_descriptor = {
    .parse = parental_rating_parse,
    .free = parental_rating_free,
};


static void* pdc_parse(DvbDescriptor *desc, const uint8_t **pp, const uint8_t *p_end)
{
    int val;
    const uint8_t *p = *pp;
    DvbPDCDescriptor *pdc;

    pdc = av_mallocz(sizeof(DvbPDCDescriptor));

    val = avpriv_dvb_get8(&p, p_end);
    if (val < 0)
        goto fail;
    pdc->reserved_future_use = (val >> 4) & 0xf;
    pdc->programme_identification_label |= (val << 16);

    val = avpriv_dvb_get16(&p, p_end);
    if (val < 0)
        goto fail;
    pdc->programme_identification_label |= val;

    return pdc;

fail:
    desc->free(pdc);
    return NULL;
}

static int pdc_free(void *p)
{
    if (p) {
        DvbPDCDescriptor *pdc = p;
        av_free(pdc);
    }
    return 0;
}

static DvbDescriptor pdc_descriptor = {
    .parse = pdc_parse,
    .free = pdc_free,
};


static void* private_data_specifier_parse(DvbDescriptor *desc, const uint8_t **pp, const uint8_t *p_end)
{
    int val;
    const uint8_t *p = *pp;
    DvbPrivateDataSpecifierDescriptor *pds;

    pds = av_mallocz(sizeof(DvbPrivateDataSpecifierDescriptor));

    val = avpriv_dvb_get32(&p, p_end);
    if (val < 0)
        goto fail;
    pds->specifier = val;

    return pds;

fail:
    desc->free(pds);
    return NULL;
}

static int private_data_specifier_free(void *p)
{
    if (p)
        av_free(p);
    return 0;
}

static DvbDescriptor private_data_specifier_descriptor = {
    .parse = private_data_specifier_parse,
    .free = private_data_specifier_free,
};


static void* short_event_parse(DvbDescriptor *desc, const uint8_t **pp, const uint8_t *p_end)
{
    int val;
    const uint8_t *p = *pp;
    DvbShortEventDescriptor *short_event;

    short_event = av_mallocz(sizeof(DvbShortEventDescriptor));

    memcpy(short_event->iso_639_language_code, p, 3);
    p += 3;

    val = avpriv_dvb_get8(&p, p_end);
    if (val < 0)
        goto fail;
    short_event->event_name_len = val;

    if (short_event->event_name_len) {
        short_event->event_name = av_malloc(short_event->event_name_len + 1);
        memcpy(short_event->event_name, p, short_event->event_name_len);
        short_event->event_name[short_event->event_name_len] = '\0';
        p += short_event->event_name_len;
    }

    val = avpriv_dvb_get8(&p, p_end);
    if (val < 0)
        goto fail;
    short_event->text_len = val;

    if (short_event->text_len) {
        short_event->text = av_malloc(short_event->text_len + 1);
        memcpy(short_event->text, p, short_event->text_len);
        short_event->text[short_event->text_len] = '\0';
        p += short_event->text_len;
    }
    *pp = p;

    return short_event;

fail:
    desc->free(short_event);
    return NULL;
}

static int short_event_free(void *p)
{
    if (p) {
        DvbShortEventDescriptor *short_event = p;
        av_free(short_event->event_name);
        av_free(short_event->text);
        av_free(p);
    }
    return 0;
}

static DvbDescriptor short_event_descriptor = {
    .parse = short_event_parse,
    .free = short_event_free,
};


static void* short_smoothing_buffer_parse(DvbDescriptor *desc, const uint8_t **pp, const uint8_t *p_end)
{
    int val, bytes_left;
    const uint8_t *p = *pp;
    DvbShortSmoothingBufferDescriptor *ssb;

    ssb = av_mallocz(sizeof(DvbShortSmoothingBufferDescriptor));

    val = avpriv_dvb_get8(&p, p_end);
    if (val < 0)
        goto fail;
    ssb->sb_size = (val >> 6);
    ssb->sb_leak_rate = val & 0x3f;

    bytes_left = p_end - p;
    ssb->DVB_reserved = av_malloc(bytes_left + 1);
    memcpy(ssb->DVB_reserved, p, bytes_left);
    ssb->DVB_reserved[bytes_left] = '\0';
    p += bytes_left;

    *pp = p;

    return ssb;

fail:
    desc->free(ssb);
    return NULL;
}

static int short_smoothing_buffer_free(void *p)
{
    if (p) {
        DvbShortSmoothingBufferDescriptor *ssb = p;
        av_free(ssb->DVB_reserved);
        av_free(ssb);
    }
    return 0;
}

static DvbDescriptor short_smoothing_buffer_descriptor = {
    .parse = short_smoothing_buffer_parse,
    .free = short_smoothing_buffer_free,
};


static void* stuffing_parse(DvbDescriptor *desc, const uint8_t **pp, const uint8_t *p_end)
{
    const uint8_t *p = *pp;
    const int stuff_size = p_end - p;
    DvbStuffingDescriptor *stuff;

    stuff = av_mallocz(sizeof(DvbStuffingDescriptor));
    stuff->stuffing_bytes = av_malloc(stuff_size + 1);
    memcpy(stuff->stuffing_bytes, p, stuff_size);
    stuff->stuffing_bytes[stuff_size] = '\0';
    p += stuff_size;

    *pp = p;

    return stuff;
}

static int stuffing_free(void *p)
{
    if (p) {
        DvbStuffingDescriptor *stuff = p;
        av_free(stuff->stuffing_bytes);
        av_free(p);
    }
    return 0;
}

static DvbDescriptor stuffing_descriptor = {
    .parse = stuffing_parse,
    .free = stuffing_free,
};


static void* telephone_parse(DvbDescriptor *desc, const uint8_t **pp, const uint8_t *p_end)
{
    int val;
    const uint8_t *p = *pp;
    DvbTelephoneDescriptor *tel;

    tel = av_mallocz(sizeof(DvbTelephoneDescriptor));

    val = avpriv_dvb_get8(&p, p_end);
    if (val < 0)
        goto fail;
    tel->reserved_future_use1 = (val >> 6) & 0x3;
    tel->foreign_availability = (val >> 5) & 0x1;
    tel->connection_type = val & 0x1f;

    val = avpriv_dvb_get8(&p, p_end);
    if (val < 0)
        goto fail;
    tel->reserved_future_use2 = (val >> 7) & 0x1;
    tel->country_prefix_len = (val >> 5) & 0x3;
    tel->international_area_code_len = (val >> 2) & 0x7;
    tel->operator_code_len = val & 0x3;

    val = avpriv_dvb_get8(&p, p_end);
    if (val < 0)
        goto fail;
    tel->reserved_future_use3 = (val >> 7) & 0x1;
    tel->national_area_code_len = (val >> 4) & 0x7;
    tel->core_number_len = val & 0xf;

    tel->country_prefix = av_malloc(tel->country_prefix_len + 1);
    memcpy(tel->country_prefix, p, tel->country_prefix_len);
    tel->country_prefix[tel->country_prefix_len] = '\0';
    p += tel->country_prefix_len;

    tel->international_area_code = av_malloc(tel->international_area_code_len + 1);
    memcpy(tel->international_area_code, p, tel->international_area_code_len);
    tel->international_area_code[tel->international_area_code_len] = '\0';
    p += tel->international_area_code_len;

    tel->operator_code = av_malloc(tel->operator_code_len + 1);
    memcpy(tel->operator_code, p, tel->operator_code_len);
    tel->operator_code[tel->operator_code_len] = '\0';
    p += tel->operator_code_len;

    tel->national_area_code = av_malloc(tel->national_area_code_len + 1);
    memcpy(tel->national_area_code, p, tel->national_area_code_len);
    tel->national_area_code[tel->national_area_code_len] = '\0';
    p += tel->national_area_code_len;

    tel->core_number = av_malloc(tel->core_number_len + 1);
    memcpy(tel->core_number, p, tel->core_number_len);
    tel->core_number[tel->core_number_len] = '\0';
    p += tel->core_number_len;

    *pp = p;

    return tel;

fail:
    desc->free(tel);
    return NULL;
}

static int telephone_free(void *p)
{
    if (p) {
        DvbTelephoneDescriptor *tel = p;
        av_free(tel->country_prefix);
        av_free(tel->international_area_code);
        av_free(tel->operator_code);
        av_free(tel->national_area_code);
        av_free(tel->core_number);
        av_free(p);
    }
    return 0;
}

static DvbDescriptor telephone_descriptor = {
    .parse = telephone_parse,
    .free = telephone_free,
};


static void* time_shifted_event_parse(DvbDescriptor *desc, const uint8_t **pp, const uint8_t *p_end)
{
    int val;
    const uint8_t *p = *pp;
    DvbTimeShiftedEventDescriptor *tsed;

    tsed = av_mallocz(sizeof(DvbTimeShiftedEventDescriptor));

    val = avpriv_dvb_get16(&p, p_end);
    if (val < 0)
        goto fail;
    tsed->reference_service_id = val;

    val = avpriv_dvb_get16(&p, p_end);
    if (val < 0)
        goto fail;
    tsed->reference_event_id = val;

    return tsed;

fail:
    desc->free(tsed);
    return NULL;
}

static int time_shifted_event_free(void *p)
{
    if (p)
        av_free(p);
    return 0;
}

static DvbDescriptor time_shifted_event_descriptor = {
    .parse = time_shifted_event_parse,
    .free = time_shifted_event_free,
};


static void* ca_identifier_parse(DvbDescriptor *desc, const uint8_t **pp, const uint8_t *p_end)
{
    const uint8_t *p = *pp;
    DvbCAIdentifierDescriptor *caid;

    caid = av_mallocz(sizeof(DvbCAIdentifierDescriptor));

    caid->nb_ids = (p_end - p) >> 1;
    caid->ca_system_id = av_mallocz(caid->nb_ids * sizeof(uint16_t));

    for(int i = 0; i < caid->nb_ids; i++) {
        int val = avpriv_dvb_get16(&p, p_end);
        if (val < 0)
            goto fail;
        caid->ca_system_id[i] = val;
    }

    return caid;

fail:
    desc->free(caid);
    return NULL;
}

static int ca_identifier_free(void *p)
{
    if (p) {
        DvbCAIdentifierDescriptor *caid = p;
        av_free(caid->ca_system_id);
        av_free(p);
    }
    return 0;
}

static DvbDescriptor ca_identifier_descriptor = {
    .parse = ca_identifier_parse,
    .free = ca_identifier_free,
};

static DvbDescriptor* all_descriptors[] = {
    [0x00 ... 0x01] = NULL,
    [DVB_DESCRIPTOR_TAG_VIDEO_STREAM_HEADER_PARAMS] = NULL,
    [DVB_DESCRIPTOR_TAG_AUDIO_STREAM_HEADER_PARAMS] = NULL,
    [DVB_DESCRIPTOR_TAG_HIERARCHY_STREAM] = NULL,
    [DVB_DESCRIPTOR_TAG_REGISTER_PRIVATE_FMT] = NULL,
    [DVB_DESCRIPTOR_TAG_STREAM_ALIGNMENT] = NULL,
    [DVB_DESCRIPTOR_TAG_TARGET_BACKGROUND] = NULL,
    [DVB_DESCRIPTOR_TAG_DISPLAY_POSITION] = NULL,
    [DVB_DESCRIPTOR_TAG_CAT_EMM_ECM_PID] = NULL,
    [DVB_DESCRIPTOR_TAG_ISO_639] = NULL,
    [DVB_DESCRIPTOR_TAG_EXTERNAL_CLOCK] = NULL,
    [DVB_DESCRIPTOR_TAG_MULTIPLEX_BOUNDS] = NULL,
    [DVB_DESCRIPTOR_TAG_COPYRIGHT_SYSTEM] = NULL,
    [DVB_DESCRIPTOR_TAG_MAX_BIT_RATE] = NULL,
    [DVB_DESCRIPTOR_TAG_PRIVATE_DATA] = NULL,
    [DVB_DESCRIPTOR_TAG_SMOOTHING_BUFFER] = NULL,
    [DVB_DESCRIPTOR_TAG_STD_VIDEO_LEAK_CONTROL] = NULL,
    [DVB_DESCRIPTOR_TAG_IBP_KEYFRAME] = NULL,
    [DVB_DESCRIPTOR_TAG_IEC13818_6_CAROUSEL] = NULL,
    [DVB_DESCRIPTOR_TAG_IEC13818_6_ASSOCIATION] = NULL,
    [DVB_DESCRIPTOR_TAG_IEC13818_6_DEFERRED_ASSOCIATION] = NULL,
    [DVB_DESCRIPTOR_TAG_IEC13818_6_RESERVED] = NULL,
    [DVB_DESCRIPTOR_TAG_DSM_NPT_REF] = NULL,
    [DVB_DESCRIPTOR_TAG_DSM_NPT_ENDPOINT] = NULL,
    [DVB_DESCRIPTOR_TAG_DSM_STREAM_MODE] = NULL,
    [DVB_DESCRIPTOR_TAG_DSM_STEAM_EVENT] = NULL,
    [DVB_DESCRIPTOR_TAG_IEC_14496_2_VIDEO_HEADER] = NULL,
    [DVB_DESCRIPTOR_TAG_IEC_14496_2_AUDIO_HEADER] = NULL,
    [DVB_DESCRIPTOR_TAG_IOD_PARAMS] = NULL,
    [DVB_DESCRIPTOR_TAG_SL_PARAMS] = NULL,
    [DVB_DESCRIPTOR_TAG_FMC_PARAMS] = NULL,
    [DVB_DESCRIPTOR_TAG_EXTERNAL_ES] = NULL,
    [DVB_DESCRIPTOR_TAG_MUXCODE] = NULL,
    [DVB_DESCRIPTOR_TAG_FMX_BUFFER_SIZE] = NULL,
    [DVB_DESCRIPTOR_TAG_MUX_BUFFER] = NULL,
    [DVB_DESCRIPTOR_TAG_CONTENT_LABELING] = NULL,
    [DVB_DESCRIPTOR_TAG_METADATA_POINTER] = NULL,
    [DVB_DESCRIPTOR_TAG_METADATA] = NULL,
    [DVB_DESCRIPTOR_TAG_METADATA_STD] = NULL,
    [DVB_DESCRIPTOR_TAG_IEC_14496_10_VIDEO_HEADER] = NULL,
    [DVB_DESCRIPTOR_TAG_IEC_13818_11_IPMP] = NULL,
    [DVB_DESCRIPTOR_TAG_TIMING_HRD] = NULL,
    [DVB_DESCRIPTOR_TAG_IEC_13818_7_AUDIO_HEADER] = NULL,
    [DVB_DESCRIPTOR_TAG_FLEXMUX_TIMING] = NULL,
    [DVB_DESCRIPTOR_TAG_TEXT_STREAM_HEADER] = NULL,
    [DVB_DESCRIPTOR_TAG_IEC_14496_3_AUDIO_HEADER] = NULL,
    [DVB_DESCRIPTOR_TAG_VIDEO_AUXILIARY_HEADER] = NULL,
    [DVB_DESCRIPTOR_TAG_VIDEO_SCALABLE_HEADER] = NULL,
    [DVB_DESCRIPTOR_TAG_VIDEO_MULTI_HEADER] = NULL,
    [DVB_DESCRIPTOR_TAG_IEC_15444_HEADER] = NULL,
    [DVB_DESCRIPTOR_TAG_VIDEO_MULTI_OP_POINT_HEADER] = NULL,
    [DVB_DESCRIPTOR_TAG_VIDEO_STEREOSCOPIC_HEADER] = NULL,
    [DVB_DESCRIPTOR_TAG_PROGRAM_STEREOSCOPIC_INFO] = NULL,
    [DVB_DESCRIPTOR_TAG_VIDEO_STEREOSCOPIC_INFO] = NULL,
    [0x37 ... 0x3F] = NULL,
    [DVB_DESCRIPTOR_TAG_NETWORK_NAME] = NULL,
    [DVB_DESCRIPTOR_TAG_SERVICE_LIST] = NULL,
    [DVB_DESCRIPTOR_TAG_STUFFING] = &stuffing_descriptor,
    [DVB_DESCRIPTOR_TAG_SATELLITE_DELIVERY_SYSTEM] = NULL,
    [DVB_DESCRIPTOR_TAG_CABLE_DELIVERY_SYSTEM] = NULL,
    [DVB_DESCRIPTOR_TAG_VBI_DATA] = NULL,
    [DVB_DESCRIPTOR_TAG_VBI_TELETEXT] = NULL,
    [DVB_DESCRIPTOR_TAG_BOUQUET_NAME] = NULL,
    [DVB_DESCRIPTOR_TAG_SERVICE] = NULL,
    [DVB_DESCRIPTOR_TAG_COUNTRY_AVAILABILITY] = NULL,
    [DVB_DESCRIPTOR_TAG_LINKAGE] = &linkage_descriptor,
    [DVB_DESCRIPTOR_TAG_NVOD_REFERENCE] = NULL,
    [DVB_DESCRIPTOR_TAG_TIME_SHIFTED_REFERENCE] = NULL,
    [DVB_DESCRIPTOR_TAG_SHORT_EVENT] = &short_event_descriptor,
    [DVB_DESCRIPTOR_TAG_EXTENDED_EVENT] = &extended_event_descriptor,
    [DVB_DESCRIPTOR_TAG_TIME_SHIFTED_EVENT] = &time_shifted_event_descriptor,
    [DVB_DESCRIPTOR_TAG_COMPONENT] = &component_descriptor,
    [DVB_DESCRIPTOR_TAG_MOSAIC] = NULL,
    [DVB_DESCRIPTOR_TAG_STREAM_IDENTIFIER] = NULL,
    [DVB_DESCRIPTOR_TAG_CA_IDENTIFIER] = &ca_identifier_descriptor,
    [DVB_DESCRIPTOR_TAG_CONTENT] = &content_descriptor,
    [DVB_DESCRIPTOR_TAG_PARENTAL_RATING] = &parental_rating_descriptor,
    [DVB_DESCRIPTOR_TAG_TELETEXT] = NULL,
    [DVB_DESCRIPTOR_TAG_TELEPHONE] = &telephone_descriptor,
    [DVB_DESCRIPTOR_TAG_LOCAL_TIME_OFFSET] = NULL,
    [DVB_DESCRIPTOR_TAG_SUBTITLING] = NULL,
    [DVB_DESCRIPTOR_TAG_TERRESTRIAL_DELIVERY] = NULL,
    [DVB_DESCRIPTOR_TAG_MULTILINGUAL_NETWORK_NAME] = NULL,
    [DVB_DESCRIPTOR_TAG_MULTILINGUAL_BOUQUET_NAME] = NULL,
    [DVB_DESCRIPTOR_TAG_MULTILINGUAL_SERVICE_NAME] = NULL,
    [DVB_DESCRIPTOR_TAG_MULTILINGUAL_COMPONENT] = &multilingual_component_descriptor,
    [DVB_DESCRIPTOR_TAG_PRIVATE_DATA_SPECIFIER] = &private_data_specifier_descriptor,
    [DVB_DESCRIPTOR_TAG_SERVICE_MOVE] = NULL,
    [DVB_DESCRIPTOR_TAG_SHORT_SMOOTHING_BUFFER] = &short_smoothing_buffer_descriptor,
    [DVB_DESCRIPTOR_TAG_FREQUENCY_LIST] = NULL,
    [DVB_DESCRIPTOR_TAG_PARTIAL_TRANSPORT_STREAM] = NULL,
    [DVB_DESCRIPTOR_TAG_DATA_BROADCAST] = &data_broadcast_descriptor,
    [DVB_DESCRIPTOR_TAG_SCRAMBLING] = NULL,
    [DVB_DESCRIPTOR_TAG_DATA_BROADCAST_ID] = NULL,
    [DVB_DESCRIPTOR_TAG_TRANSPORT_STREAM] = NULL,
    [DVB_DESCRIPTOR_TAG_DSNG] = NULL,
    [DVB_DESCRIPTOR_TAG_PDC] = &pdc_descriptor,
    [DVB_DESCRIPTOR_TAG_AC3] = NULL,
    [DVB_DESCRIPTOR_TAG_ANCILLARY_DATA] = NULL,
    [DVB_DESCRIPTOR_TAG_CELL_LIST] = NULL,
    [DVB_DESCRIPTOR_TAG_CELL_FREQUENCY_LINK] = NULL,
    [DVB_DESCRIPTOR_TAG_ANNOUNCEMENT_SUPPORT] = NULL,
    [DVB_DESCRIPTOR_TAG_APPLICATION_SIGNALING] = NULL,
    [DVB_DESCRIPTOR_TAG_ADAPTATION_FIELD_DATA] = NULL,
    [DVB_DESCRIPTOR_TAG_SERVICE_IDENTIFIER] = NULL,
    [DVB_DESCRIPTOR_TAG_SERVICE_AVAILABILITY] = NULL,
    [DVB_DESCRIPTOR_TAG_DEFAULT_AUTHORITY] = NULL,
    [DVB_DESCRIPTOR_TAG_RELATED_CONTENT] = NULL,
    [DVB_DESCRIPTOR_TAG_TVA_ID] = NULL,
    [DVB_DESCRIPTOR_TAG_CONTENT_IDENTIFIER] = NULL,
    [DVB_DESCRIPTOR_TAG_TIME_SLICE_FEC_IDENTIFIER] = NULL,
    [DVB_DESCRIPTOR_TAG_ECM_REPETITION_RATE] = NULL,
    [DVB_DESCRIPTOR_TAG_S2_SATELLITE_DELIVERY_SYSTEM] = NULL,
    [DVB_DESCRIPTOR_TAG_ENHANCED_AC3] = NULL,
    [DVB_DESCRIPTOR_TAG_DTS] = NULL,
    [DVB_DESCRIPTOR_TAG_AAC] = NULL,
    [DVB_DESCRIPTOR_TAG_XAIT_LOCATION] = NULL,
    [DVB_DESCRIPTOR_TAG_FTA_CONTENT_MANAGEMENT] = NULL,
    [DVB_DESCRIPTOR_TAG_EXTENSION] = NULL,
    [0x80 ... 0xFF] = NULL
};

int dvb_parse_descriptor_header(DvbDescriptorHeader *h, const uint8_t **pp,
                                const uint8_t *p_end)
{
    int val;
    const uint8_t *p = *pp;

    val = avpriv_dvb_get8(&p, p_end);
    if (val < 0)
        return val;
    h->tag = val;

    val = avpriv_dvb_get8(&p, p_end);
    if (val < 0)
        return val;
    h->len = val;

    *pp = p;

    return 0;
}

DvbDescriptor* ff_dvb_get_descriptor(DvbDescriptorHeader *h)
{
    return all_descriptors[h->tag];
}
