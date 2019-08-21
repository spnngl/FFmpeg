#include "libavutil/mem.h"
#include "libavutil/dvb.h"
#include "dvbdescriptors.h"

#define descriptors_malloc(__size) ({ \
        void * __p = av_malloc(__size); \
        if (!__p) \
            goto fail; \
        __p; \
    })

#define descriptors_mallocz(__size) ({ \
        void * __p = av_mallocz(__size); \
        if (!__p) \
            goto fail; \
        __p; \
    })

static void* component_parse(DvbDescriptor *desc, const uint8_t **pp, const uint8_t *p_end)
{
    int val, text_len;
    const uint8_t *p = *pp;
    DvbComponentDescriptor *comp;

    comp = descriptors_mallocz(sizeof(DvbComponentDescriptor));

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
    comp->text = descriptors_malloc(text_len + 1);
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
    DvbComponentDescriptor *comp = p;
    av_free(comp->text);
    av_free(p);
    return 0;
}

#include "componenttables.h"

static void nga_feature_description(const uint8_t component_type, char* dst)
{
    int head = 0, size;
    const char* header = "NGA features: \0";

    size = strlen(header);
    memcpy(dst, header, size);
    head += size;

    if (component_type & 0x40) {
        size = strlen(NGA_B6);
        memcpy(dst + head, NGA_B6, size);
        head += size;
        memcpy(dst + head, ", \0", 2);
        head += 2;
    }

    if (component_type & 0x20) {
        size = strlen(NGA_B5);
        memcpy(dst + head, NGA_B5, size);
        head += size;
        memcpy(dst + head, ", \0", 2);
        head += 2;
    }

    if (component_type & 0x10) {
        size = strlen(NGA_B4);
        memcpy(dst + head, NGA_B4, size);
        head += size;
        memcpy(dst + head, ", \0", 2);
        head += 2;
    }

    if (component_type & 0x08) {
        size = strlen(NGA_B3);
        memcpy(dst + head, NGA_B3, size);
        head += size;
        memcpy(dst + head, ", \0", 2);
        head += 2;
    }

    if (component_type & 0x04) {
        size = strlen(NGA_B2);
        memcpy(dst + head, NGA_B2, size);
        head += size;
        memcpy(dst + head, ", \0", 2);
        head += 2;
    }

    strcpy(dst + head, nga_b1b0[component_type & 0x3]);
}

static int ac3_description(const uint8_t component_type, char* dst)
{
    int head = 0, size;
    const char* cur;

    cur = ac3_b7[!!(component_type & 0x80)];
    size = strlen(cur);
    memcpy(dst + head, cur, size);
    head += size;
    memcpy(dst + head, ", \0", 2);
    head += 2;

    cur = ac3_b6[!!(component_type & 0x40)];
    size = strlen(cur);
    memcpy(dst + head, cur, size);
    head += size;
    memcpy(dst + head, ", \0", 2);
    head += 2;

    cur = ac3_b543[(component_type >> 3) & 0x7];
    size = strlen(cur);
    memcpy(dst + head, cur, size);
    head += size;
    memcpy(dst + head, ", \0", 2);
    head += 2;

    cur = ac3_b210[component_type & 0x7];
    size = strlen(cur);
    strcpy(dst + head, cur);
    head += size;

    return head;
}

static int dvb_audio_description(const uint8_t component_type, char* dst)
{
    int head = 0, size;
    const char* cur;
    const char* header = "NGA features: \0";

    size = strlen(header);
    memcpy(dst, header, size);
    head += size;

    cur = dvb_audio_b6[!!(component_type & 0x40)];
    size = strlen(cur);
    memcpy(dst + head, cur, size);
    head += size;
    memcpy(dst + head, ", \0", 2);
    head += 2;

    cur = dvb_audio_b543[(component_type >> 3) & 0x7];
    size = strlen(cur);
    memcpy(dst + head, cur, size);
    head += size;
    memcpy(dst + head, ", \0", 2);
    head += 2;

    cur = dvb_audio_b210[component_type & 0x7];
    size = strlen(cur);
    memcpy(dst + head, cur, size);
    head += size;

    return head;
}

void av_epg_component_str(DvbComponentDescriptor *comp, char* dst)
{
    if(!dst || !comp)
        return;

    if (comp->stream_content == 0x4)
        ac3_description(comp->component_type, dst);
    else if (comp->stream_content == 0x7 && comp->component_type < 0x80)
        dvb_audio_description(comp->component_type, dst);
    else if (comp->stream_content == 0xB && comp->stream_content_ext == 0xE)
        nga_feature_description(comp->component_type, dst);
    else
        strcpy(dst, component_descriptions[comp->stream_content]
                [comp->stream_content_ext][comp->component_type]);
}

static void component_show(void *p, int av_log_level)
{
    char dst[512];
    DvbComponentDescriptor *comp = p;

    av_epg_component_str(comp, dst);

    av_log(NULL, av_log_level, "\t\tComponent descriptor (tag 0x%.02x):\n"
            "\t\t\tstream_content_ext = 0x%.01x\n"
            "\t\t\tstream_content     = 0x%.01x\n"
            "\t\t\tcomponent_type     = 0x%.02x\n"
            "\t\t\tcomponent_tag      = 0x%.02x\n"
            "\t\t\tcomp_descr         = %s\n"
            "\t\t\tiso_639_lang_code  = %.03s\n"
            "\t\t\ttext               = %s\n",
            comp->h.tag, comp->stream_content_ext,
            comp->stream_content, comp->component_type,
            comp->component_tag, dst, comp->iso_639_language_code,
            comp->text);
}

static const DvbDescriptor component_descriptor = {
    .parse = component_parse,
    .show = component_show,
    .free = component_free,
};


static void* content_parse(DvbDescriptor *desc, const uint8_t **pp, const uint8_t *p_end)
{
    int val;
    const uint8_t *p = *pp;
    DvbContentDescriptor *content;

    content = descriptors_mallocz(sizeof(DvbContentDescriptor));

    content->nb_descriptions = (p_end - p) / sizeof(DvbContentDescription);
    content->descriptions = descriptors_mallocz(content->nb_descriptions * sizeof(DvbContentDescription*));

    for(int i = 0; i < content->nb_descriptions; i++) {
        DvbContentDescription *d;

        content->descriptions[i] = descriptors_mallocz(sizeof(DvbContentDescription));
        d = content->descriptions[i];

        val = avpriv_dvb_get8(&p, p_end);
        if (val < 0)
            goto fail;
        d->nibble_lvl_1 = (val >> 4) & 0xf;
        d->nibble_lvl_2 = val & 0xf;

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
    DvbContentDescriptor *content = p;

    for(int i = 0; i < content->nb_descriptions; i++)
        av_free(content->descriptions[i]);
    av_free(content->descriptions);
    av_free(p);
    return 0;
}

#include "contenttables.h"

char* av_epg_content_simple_str(DvbContentDescription *description)
{
    return (char*)simple_nibbles[description->nibble_lvl_1];
}

char* av_epg_content_detailed_str(DvbContentDescription *description)
{
    return (char*)detailed_nibbles[description->nibble_lvl_1][description->nibble_lvl_2];
}

static void content_show(void *p, int av_log_level)
{
    DvbContentDescriptor *content = p;

    av_log(NULL, av_log_level, "\t\tContent descriptor (tag 0x%.02x):\n", content->h.tag);

    for (int i = 0; i < content->nb_descriptions; i++) {
        DvbContentDescription *d = content->descriptions[i];

        av_log(NULL, av_log_level, "\t\tContent description #%i:\n"
                "\t\t\tsimple_descr = %s\n"
                "\t\t\tdetailed_descr = %s\n"
                "\t\t\tnibble_lvl_1 = 0x%.01x\n"
                "\t\t\tnibble_lvl_2 = 0x%.01x\n"
                "\t\t\tuser_byte    = 0x%.02x\n",
                i, av_epg_content_simple_str(d),
                av_epg_content_detailed_str(d),
                d->nibble_lvl_1, d->nibble_lvl_2,
                d->user_byte);
    }
}

static const DvbDescriptor content_descriptor = {
    .parse = content_parse,
    .show = content_show,
    .free = content_free,
};


static void* data_broadcast_parse(DvbDescriptor *desc, const uint8_t **pp, const uint8_t *p_end)
{
    int val;
    const uint8_t *p = *pp;
    DvbDataBroadcastDescriptor *dbd;

    dbd = descriptors_mallocz(sizeof(DvbDataBroadcastDescriptor));

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

    dbd->selector = descriptors_malloc(dbd->selector_len + 1);
    memcpy(dbd->selector, p, dbd->selector_len);
    dbd->selector[dbd->selector_len] = '\0';
    p += dbd->selector_len;

    memcpy(dbd->iso_639_language_code, p, 3);
    p += 3;

    val = avpriv_dvb_get8(&p, p_end);
    if (val < 0)
        goto fail;
    dbd->text_len = val;


    dbd->text = descriptors_malloc(dbd->text_len + 1);
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
    DvbDataBroadcastDescriptor *dbd = p;
    av_free(dbd->selector);
    av_free(dbd->text);
    av_free(dbd);
    return 0;
}

static void data_broadcast_show(void *p, int av_log_level)
{
    DvbDataBroadcastDescriptor *dbd = p;

    av_log(NULL, av_log_level, "\t\tData broadcast descriptor (tag 0x%.02x):\n"
            "\t\t\tdata_broadcast_id = %i\n"
            "\t\t\tcomponent_tag = %i\n"
            "\t\t\tselector = %s\n"
            "\t\t\tiso_639_lang_code = %.03s\n"
            "\t\t\ttext = %s\n",
            dbd->h.tag, dbd->data_broadcast_id, dbd->component_tag,
            dbd->selector, dbd->iso_639_language_code, dbd->text);
}

static const DvbDescriptor data_broadcast_descriptor = {
    .parse = data_broadcast_parse,
    .show = data_broadcast_show,
    .free = data_broadcast_free,
};


static void* extended_event_parse(DvbDescriptor *desc, const uint8_t **pp, const uint8_t *p_end)
{
    int val;
    const uint8_t *p = *pp, *items_end;
    DvbExtendedEventDescriptor *event;

    event = descriptors_mallocz(sizeof(DvbExtendedEventDescriptor));

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
        SimpleLinkedList *cur = descriptors_mallocz(sizeof(SimpleLinkedList));
        DvbExtendedEventDescription *extev_desc = descriptors_mallocz(sizeof(DvbExtendedEventDescription));

        val = avpriv_dvb_get8(&p, items_end);
        if (val < 0)
            goto fail;
        extev_desc->item_description_length = val;

        extev_desc->descr = descriptors_malloc(extev_desc->item_description_length + 1);
        memcpy(extev_desc->descr, p, extev_desc->item_description_length);
        extev_desc->descr[extev_desc->item_description_length] = '\0';
        p += extev_desc->item_description_length;

        val = avpriv_dvb_get8(&p, items_end);
        if (val < 0)
            goto fail;
        extev_desc->item_length = val;

        extev_desc->item = descriptors_malloc(extev_desc->item_length + 1);
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

    event->text = descriptors_malloc(event->text_len + 1);
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
    return 0;
}

static void extended_event_show(void *p, int av_log_level)
{
    int count = 0;
    DvbExtendedEventDescriptor *event = p;
    SimpleLinkedList *head;

    av_log(NULL, av_log_level, "\t\tExtended Event descriptor (tag 0x%.02x):\n"
            "\t\t\tnumber = %i\n"
            "\t\t\tlast_number = %i\n"
            "\t\t\tiso_639_lang_code = %.03s\n"
            "\t\t\ttext = %s\n",
            event->h.tag, event->number, event->last_number,
            event->iso_639_language_code, event->text);

    for (head = event->descriptions; head; head = head->next) {
        DvbExtendedEventDescription *d = head->data;

        av_log(NULL, av_log_level, "\t\t\tDescription #%i:\n"
                "\t\t\t\tdescr = %s\n"
                "\t\t\t\titem  = %s\n",
                count++, d->descr, d->item);
    }
}

static const DvbDescriptor extended_event_descriptor = {
    .parse = extended_event_parse,
    .show = extended_event_show,
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
    return 0;
}

static void* mobile_hand_over_linkage_parse(const uint8_t **pp, const uint8_t *p_end)
{
    int val;
    const uint8_t *p = *pp;
    DvbMobileHandOverLinkage *mhol;

    mhol = descriptors_mallocz(sizeof(DvbMobileHandOverLinkage));

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

    ev_link = descriptors_mallocz(sizeof(DvbEventLinkage));

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

#include "linkagetables.h"

static void* extended_event_linkage_parse(const uint8_t **pp, const uint8_t *p_end)
{
    int val;
    const uint8_t *p = *pp;
    uint8_t *descriptions_end;
    DvbExtendedEventLinkage *eel;

    eel = descriptors_mallocz(sizeof(DvbExtendedEventLinkage));
    eel->descriptions = NULL;

    val = avpriv_dvb_get8(&p, p_end);
    if (val < 0)
        goto fail;
    eel->loop_length = val;

    descriptions_end = (uint8_t*)p + eel->loop_length;
    while(p < descriptions_end) {
        SimpleLinkedList *cur = descriptors_mallocz(sizeof(SimpleLinkedList));
        DvbEELDescription *eeld = descriptors_mallocz(sizeof(DvbEELDescription));

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

    ld = descriptors_mallocz(sizeof(DvbLinkageDescriptor));

    val = avpriv_dvb_get16(&p, p_end);
    if (val < 0)
        goto fail;
    ld->ts_id = val;

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
        ld->private_data = descriptors_mallocz(left + 1);
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

static void linkage_show(void *p, int av_log_level)
{
    av_log(NULL, AV_LOG_WARNING, "\t\tLinkage desciptor show function not implemented\n");
}

static const DvbDescriptor linkage_descriptor = {
    .parse = linkage_parse,
    .show = linkage_show,
    .free = linkage_free,
};


static void* multilingual_component_parse(DvbDescriptor *desc, const uint8_t **pp, const uint8_t *p_end)
{
    int val;
    const uint8_t *p = *pp;
    DvbMultilingualComponentDescriptor *mcd;

    mcd = descriptors_mallocz(sizeof(DvbMultilingualComponentDescriptor));

    val = avpriv_dvb_get8(&p, p_end);
    if (val < 0)
        goto fail;
    mcd->component_tag = val;

    while(p < p_end) {
        SimpleLinkedList *cur;
        DvbMultilingualComponentDescription *comp =
            descriptors_mallocz(sizeof(DvbMultilingualComponentDescription));

        memcpy(comp->iso_639_language_code, p, 3);
        p += 3;

        val = avpriv_dvb_get8(&p, p_end);
        if (val < 0)
            goto fail;
        comp->text_description_len = val;

        comp->text = descriptors_malloc(comp->text_description_len + 1);
        memcpy(comp->text, p, comp->text_description_len);
        comp->text[comp->text_description_len] = '\0';
        p += comp->text_description_len;

        cur = descriptors_mallocz(sizeof(SimpleLinkedList));
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
    return 0;
}

static void multilingual_component_show(void *p, int av_log_level)
{
    int count = 0;
    SimpleLinkedList *head;
    DvbMultilingualComponentDescriptor *mcd = p;

    av_log(NULL, av_log_level, "\t\tMultilingual component descriptor (tag 0x%.02x):\n"
           "\t\t\tcomponent tag = %i\n", mcd->h.tag, mcd->component_tag);

    for (head = mcd->descriptions; head; head = head->next) {
        DvbMultilingualComponentDescription *d = head->data;

        av_log(NULL, av_log_level, "\t\t\tMultilingual component description #%i"
               "\t\t\t\tiso_639_language_code = %.03s\n"
               "\t\t\t\ttext = %s\n",
               count++,  d->iso_639_language_code, d->text);
    }

}

static const DvbDescriptor multilingual_component_descriptor = {
    .parse = multilingual_component_parse,
    .show = multilingual_component_show,
    .free = multilingual_component_free,
};


static void* parental_rating_parse(DvbDescriptor *desc, const uint8_t **pp, const uint8_t *p_end)
{
    int val, i = 0;
    const uint8_t *p = *pp;
    DvbParentalRatingDescriptor *prd;

    prd = descriptors_mallocz(sizeof(DvbParentalRatingDescriptor));
    prd->nb_descriptions = (p_end - p) / sizeof(DvbParentalRatingDescription);
    prd->descriptions = descriptors_mallocz(prd->nb_descriptions * sizeof(DvbParentalRatingDescription*));

    while(p < p_end && i < prd->nb_descriptions) {
        DvbParentalRatingDescription *d;

        prd->descriptions[i] = descriptors_mallocz(sizeof(DvbParentalRatingDescription));
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
    DvbParentalRatingDescriptor *prd = p;

    for (int i = 0; i < prd->nb_descriptions; i++)
        av_free(prd->descriptions[i]);
    av_free(prd->descriptions);
    av_free(p);
    return 0;
}

char* av_epg_parental_rating_min_age(const uint8_t rating)
{
    int size;
    char *ret;

    switch(rating) {
        // Defined by broadcaster
        case 0x10 ... 0xFF:
        // Undefined value
        case 0x00:
            size = sizeof("undefined") + 1;
            ret = descriptors_malloc(size);
            memcpy(ret, "undefined\0", size);
            break;
        case 0x01 ... 0x0F:
            ret = descriptors_malloc(3);
            snprintf(ret, 3, "%d", rating + 3);
            break;
        default:
            ret = NULL;
            break;
    }
    return ret;

fail:
    return NULL;
}

static void parental_rating_show(void *p, int av_log_level)
{
    DvbParentalRatingDescriptor *par = p;

    av_log(NULL, av_log_level, "\t\tParental Rating descriptor (tag 0x%.02x):\n",
            par->h.tag);

    for (int i = 0; i < par->nb_descriptions; i++) {
        DvbParentalRatingDescription *d = par->descriptions[i];
        char *rating = av_epg_parental_rating_min_age(d->rating);

        av_log(NULL, av_log_level, "\t\t\tDescription #%i:\n"
                "\t\t\t\tcountry_code = %.03s\n"
                "\t\t\t\trating = %s (raw %i)\n",
                i, d->country_code, rating, d->rating);
        av_free(rating);
    }
}

static const DvbDescriptor parental_rating_descriptor = {
    .parse = parental_rating_parse,
    .show = parental_rating_show,
    .free = parental_rating_free,
};


static void* pdc_parse(DvbDescriptor *desc, const uint8_t **pp, const uint8_t *p_end)
{
    int val;
    const uint8_t *p = *pp;
    DvbPDCDescriptor *pdc;

    pdc = descriptors_mallocz(sizeof(DvbPDCDescriptor));

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
    DvbPDCDescriptor *pdc = p;
    av_free(pdc);
    return 0;
}

int av_epg_pdc_date_str(const uint32_t pil, char* dst)
{
    int head = 0, size;
    char cur[8];

    snprintf(cur, 8, "%i", (pil >> 15));
    size = strlen(cur);
    memcpy(dst, cur, size);
    head += size;
    memcpy(dst + head, "/\0", 1);
    head++;

    snprintf(cur, 8, "%i", (pil >> 11) & 0xf);
    size = strlen(cur);
    memcpy(dst + head, cur, size);
    head += size;
    memcpy(dst + head, " \0", 1);
    head++;

    snprintf(cur, 8, "%i", (pil >> 6) & 0x1f);
    size = strlen(cur);
    memcpy(dst + head, cur, size);
    head += size;
    memcpy(dst + head, ":\0", 1);
    head++;

    snprintf(cur, 8, "%i", pil & 0x3f);
    size = strlen(cur);
    strcpy(dst + head, cur);
    head += size;

    return head;
}

static void pdc_show(void *p, int av_log_level)
{
    char date[32];
    DvbPDCDescriptor *pdc = p;

    av_epg_pdc_date_str(pdc->programme_identification_label, date);
    av_log(NULL, av_log_level, "\t\tPDC descriptor (tag 0x%.02x):\n"
            "\t\t\tdate = %s (raw %i)\n", pdc->h.tag, date,
            pdc->programme_identification_label);
}

static const DvbDescriptor pdc_descriptor = {
    .parse = pdc_parse,
    .show = pdc_show,
    .free = pdc_free,
};


static void* private_data_specifier_parse(DvbDescriptor *desc, const uint8_t **pp, const uint8_t *p_end)
{
    int val;
    const uint8_t *p = *pp;
    DvbPrivateDataSpecifierDescriptor *pds;

    pds = descriptors_mallocz(sizeof(DvbPrivateDataSpecifierDescriptor));

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
    av_free(p);
    return 0;
}

static void private_data_specifier_show(void *p, int av_log_level)
{
    DvbPrivateDataSpecifierDescriptor *pr = p;

    av_log(NULL, av_log_level, "\t\tPrivate data specifier (tag 0x%.02x):\n"
            "\t\t\tspecifier = %i\n", pr->h.tag, pr->specifier);
}

static const DvbDescriptor private_data_specifier_descriptor = {
    .parse = private_data_specifier_parse,
    .show = private_data_specifier_show,
    .free = private_data_specifier_free,
};


static void* short_event_parse(DvbDescriptor *desc, const uint8_t **pp, const uint8_t *p_end)
{
    int val;
    const uint8_t *p = *pp;
    DvbShortEventDescriptor *short_event;

    short_event = descriptors_mallocz(sizeof(DvbShortEventDescriptor));

    memcpy(short_event->iso_639_language_code, p, 3);
    p += 3;

    val = avpriv_dvb_get8(&p, p_end);
    if (val < 0)
        goto fail;
    short_event->event_name_len = val;

    if (short_event->event_name_len) {
        short_event->event_name = descriptors_malloc(short_event->event_name_len + 1);
        memcpy(short_event->event_name, p, short_event->event_name_len);
        short_event->event_name[short_event->event_name_len] = '\0';
        p += short_event->event_name_len;
    }

    val = avpriv_dvb_get8(&p, p_end);
    if (val < 0)
        goto fail;
    short_event->text_len = val;

    if (short_event->text_len) {
        short_event->text = descriptors_malloc(short_event->text_len + 1);
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
    DvbShortEventDescriptor *short_event = p;
    av_free(short_event->event_name);
    av_free(short_event->text);
    av_free(p);
    return 0;
}

static void short_event_show(void *p, int av_log_level)
{
    DvbShortEventDescriptor *event = p;

    av_log(NULL, av_log_level, "\t\tShort event descriptor (tag 0x%.02x):\n"
            "\t\t\tiso_639_language_code = %.03s\n"
            "\t\t\tevent_name = %s\n"
            "\t\t\ttext = %s\n",
            event->h.tag, event->iso_639_language_code, event->event_name,
            event->text);
}

static const DvbDescriptor short_event_descriptor = {
    .parse = short_event_parse,
    .show = short_event_show,
    .free = short_event_free,
};


static void* short_smoothing_buffer_parse(DvbDescriptor *desc, const uint8_t **pp, const uint8_t *p_end)
{
    int val, bytes_left;
    const uint8_t *p = *pp;
    DvbShortSmoothingBufferDescriptor *ssb;

    ssb = descriptors_mallocz(sizeof(DvbShortSmoothingBufferDescriptor));

    val = avpriv_dvb_get8(&p, p_end);
    if (val < 0)
        goto fail;
    ssb->sb_size = (val >> 6);
    ssb->sb_leak_rate = val & 0x3f;

    bytes_left = p_end - p;
    ssb->DVB_reserved = descriptors_malloc(bytes_left + 1);
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
    DvbShortSmoothingBufferDescriptor *ssb = p;
    av_free(ssb->DVB_reserved);
    av_free(ssb);
    return 0;
}

#include "smoothing_buffer_tables.h"

char* av_epg_sb_size_str(const uint8_t sb_size)
{
    return sb_size < 4 ? (char*)smoothing_buffer_size[sb_size] : (char*)"null\0";
}

char* av_epg_sb_leak_rate_str(const uint8_t sb_leak_rate)
{
    return sb_leak_rate < 64 ? (char*)smoothing_buffer_leak_rate[sb_leak_rate] : (char*)"null\0";
}

static void short_smoothing_buffer_show(void *p, int av_log_level)
{
    DvbShortSmoothingBufferDescriptor *ssb = p;

    av_log(NULL, av_log_level, "\t\tShort smoothing buffer (tag 0x%.02x):\n"
            "\t\t\tsb_size = %s (raw %i)\n"
            "\t\t\tsb_leak_rate = %s (raw %i)\n",
            ssb->h.tag, av_epg_sb_size_str(ssb->sb_size), ssb->sb_size,
            av_epg_sb_leak_rate_str(ssb->sb_leak_rate), ssb->sb_leak_rate);
}

static const DvbDescriptor short_smoothing_buffer_descriptor = {
    .parse = short_smoothing_buffer_parse,
    .show = short_smoothing_buffer_show,
    .free = short_smoothing_buffer_free,
};


static void* stuffing_parse(DvbDescriptor *desc, const uint8_t **pp, const uint8_t *p_end)
{
    const uint8_t *p = *pp;
    const int stuff_size = p_end - p;
    DvbStuffingDescriptor *stuff;

    stuff = descriptors_mallocz(sizeof(DvbStuffingDescriptor));
    stuff->stuffing_bytes = descriptors_malloc(stuff_size + 1);
    memcpy(stuff->stuffing_bytes, p, stuff_size);
    stuff->stuffing_bytes[stuff_size] = '\0';
    p += stuff_size;

    *pp = p;

    return stuff;

fail:
    desc->free(stuff);
    return NULL;
}

static int stuffing_free(void *p)
{
    DvbStuffingDescriptor *stuff = p;
    av_free(stuff->stuffing_bytes);
    av_free(p);
    return 0;
}

static void stuffing_show(void *p, int av_log_level)
{
    DvbStuffingDescriptor *stuff = p;

    av_log(NULL, av_log_level, "\t\tStuffing descriptor (tag 0x%.02x):\n"
           "\t\t\tstuffing_bytes = %s\n",
           stuff->h.tag, stuff->stuffing_bytes);
}

static const DvbDescriptor stuffing_descriptor = {
    .parse = stuffing_parse,
    .show = stuffing_show,
    .free = stuffing_free,
};


static void* telephone_parse(DvbDescriptor *desc, const uint8_t **pp, const uint8_t *p_end)
{
    int val;
    const uint8_t *p = *pp;
    DvbTelephoneDescriptor *tel;

    tel = descriptors_mallocz(sizeof(DvbTelephoneDescriptor));

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

    tel->country_prefix = descriptors_malloc(tel->country_prefix_len + 1);
    memcpy(tel->country_prefix, p, tel->country_prefix_len);
    tel->country_prefix[tel->country_prefix_len] = '\0';
    p += tel->country_prefix_len;

    tel->international_area_code = descriptors_malloc(tel->international_area_code_len + 1);
    memcpy(tel->international_area_code, p, tel->international_area_code_len);
    tel->international_area_code[tel->international_area_code_len] = '\0';
    p += tel->international_area_code_len;

    tel->operator_code = descriptors_malloc(tel->operator_code_len + 1);
    memcpy(tel->operator_code, p, tel->operator_code_len);
    tel->operator_code[tel->operator_code_len] = '\0';
    p += tel->operator_code_len;

    tel->national_area_code = descriptors_malloc(tel->national_area_code_len + 1);
    memcpy(tel->national_area_code, p, tel->national_area_code_len);
    tel->national_area_code[tel->national_area_code_len] = '\0';
    p += tel->national_area_code_len;

    tel->core_number = descriptors_malloc(tel->core_number_len + 1);
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
    DvbTelephoneDescriptor *tel = p;
    av_free(tel->country_prefix);
    av_free(tel->international_area_code);
    av_free(tel->operator_code);
    av_free(tel->national_area_code);
    av_free(tel->core_number);
    av_free(p);
    return 0;
}

static void telephone_show(void *p, int av_log_level)
{
    DvbTelephoneDescriptor *tel = p;

    av_log(NULL, av_log_level, "\t\tTelephone descriptor (tag 0x%.02x):\n"
            "\t\t\tforeign_availability = %i\n"
            "\t\t\tconnection_type = %i\n"
            "\t\t\tcountry_prefix = %s\n"
            "\t\t\tinternational_area_code = %s\n"
            "\t\t\toperator_code = %s\n"
            "\t\t\tnational_area_code = %s\n"
            "\t\t\tcore_number = %s\n",
            tel->h.tag, tel->foreign_availability, tel->connection_type,
            tel->country_prefix, tel->international_area_code,
            tel->operator_code, tel->national_area_code, tel->core_number);
}

static const DvbDescriptor telephone_descriptor = {
    .parse = telephone_parse,
    .show = telephone_show,
    .free = telephone_free,
};


static void* time_shifted_event_parse(DvbDescriptor *desc, const uint8_t **pp, const uint8_t *p_end)
{
    int val;
    const uint8_t *p = *pp;
    DvbTimeShiftedEventDescriptor *tsed;

    tsed = descriptors_mallocz(sizeof(DvbTimeShiftedEventDescriptor));

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
    av_free(p);
    return 0;
}

static void time_shifted_event_show(void *p, int av_log_level)
{
    DvbTimeShiftedEventDescriptor *tsed = p;

    av_log(NULL, av_log_level, "\t\tTime Shifted Event descriptor (tag 0x%.02x):\n"
            "\t\t\treference_service_id = %i\n"
            "\t\t\treference_event_id = %i\n",
            tsed->h.tag, tsed->reference_service_id, tsed->reference_event_id);
}

static const DvbDescriptor time_shifted_event_descriptor = {
    .parse = time_shifted_event_parse,
    .show = time_shifted_event_show,
    .free = time_shifted_event_free,
};


static void* ca_identifier_parse(DvbDescriptor *desc, const uint8_t **pp, const uint8_t *p_end)
{
    const uint8_t *p = *pp;
    DvbCAIdentifierDescriptor *caid;

    caid = descriptors_mallocz(sizeof(DvbCAIdentifierDescriptor));

    caid->nb_ids = (p_end - p) >> 1;
    caid->ca_system_id = descriptors_mallocz(caid->nb_ids * sizeof(uint16_t));

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
    DvbCAIdentifierDescriptor *caid = p;
    av_free(caid->ca_system_id);
    av_free(p);
    return 0;
}

static void ca_identifier_show(void *p, int av_log_level)
{
    DvbCAIdentifierDescriptor *caid = p;

    av_log(NULL, av_log_level, "\t\tCA Identifier descriptor (tag 0x%.02x):\n", caid->h.tag);
    for (int i = 0; i < caid->nb_ids; i++)
        av_log(NULL, av_log_level, "\t\t\tca_system_id #%i = %i\n",
                i, caid->ca_system_id[i]);
}

static const DvbDescriptor ca_identifier_descriptor = {
    .parse = ca_identifier_parse,
    .show = ca_identifier_show,
    .free = ca_identifier_free,
};

static const DvbDescriptor * const all_descriptors[] = {
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

int av_dvb_parse_descriptor_header(DvbDescriptorHeader *h, const uint8_t **pp,
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

const DvbDescriptor* const av_dvb_get_descriptor(DvbDescriptorHeader *h)
{
    return all_descriptors[h->tag];
}
