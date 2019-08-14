#include <string.h>

#include "mem.h"
#include "epg.h"

void av_epg_init_event(EPGEvent *event) {
    event->ids = 0ULL;
    event->shift_from.ids = 0ULL;

    memset(event->start_time, '\0', 5);
    memset(event->duration, '\0', 3);

    event->running_status = 0;
    event->free_ca_mode = 1;
    event->rating = 0;
    event->pil = 0;

    memset(event->country_code, '\0', 3);
    memset(event->iso_639_language_code, '\0', 3);

    memset(event->event_name, '\0', 256);
    memset(event->short_event_description, '\0', 256);
    memset(event->long_event_description, '\0', 512);
    memset(event->items, '\0', 512);

    event->component.stream_content_ext = 0;
    event->component.stream_content = 0;
    event->component.type = 0;
    event->component.tag = 0;
    memset(event->component.text, '\0', 32);

    event->content.nibble_lvl_1 = 0;
    event->content.nibble_lvl_2 = 0;
    event->content.user_byte = 0;

    event->sb.size = 0;
    event->sb.leak_rate = 0;
}

void av_epg_free_event(EPGEvent **event)
{
    av_free((*event)->event_name);
    av_free((*event)->short_event_description);
    av_free((*event)->long_event_description);
    av_free((*event)->items);
    // NB: Do not try to free component & content
    av_freep(event);
}

static void epg_show_event(EPGEvent *event, int loglevel)
{
    char* start_time = av_epg_start_time_to_str(event->start_time);
    char* duration = av_epg_duration_to_str(event->duration);
    char* short_content = av_epg_content_simple_str(event);
    char* long_content = av_epg_content_detailed_str(event);
    char pil[32];
    char comp[256];
    av_epg_pdc_date_str(event->pil, pil);
    av_epg_component_str(event, comp);

    av_log(NULL, loglevel, "EPGEvent: id(%i), shift_from_service_id(%i), "
           "shift_from_event_id(%i), start_time(%s), duration(%s), running_status(%s), "
           "free_ca_mode(%i), rating(%i), country_code(%.3s), PIL(%s), event_name(%s), "
           "short_event_descr(%s), "
           "long_event_descr(%s), items(%s), component_description(%s), "
           "component_text(%s), short_content_description(%s), "
           "long_content_description(%s), sb_size(%s), sb_leak_rate(%s)\n\n\n",
           event->event_id, event->shift_from.service_id, event->shift_from.event_id,
           start_time, duration, av_epg_running_status(event->running_status),
           event->free_ca_mode, event->rating, event->country_code, pil, event->event_name,
           event->short_event_description, event->long_event_description, event->items,
           comp, event->component.text,
           short_content, long_content,
           av_epg_sb_size_str(event->sb.size), av_epg_sb_leak_rate_str(event->sb.leak_rate));

    av_free(start_time);
    av_free(duration);
}

void av_epg_show_event(EPGEvent *event)
{
    epg_show_event(event, AV_LOG_DEBUG);
}

char* av_epg_bcd_to_str(const uint32_t bcd)
{
    char* ret;

    ret = av_malloc(16);
    snprintf(ret, 16, "%i%i:%i%i:%i%i", (bcd >> 20) & 0xf,
             (bcd >> 16) & 0xf, (bcd >> 12) & 0xf,
             (bcd >> 8) & 0xf, (bcd >> 4) & 0xf,
             bcd & 0xf);

    return ret;
}

char* av_epg_mjd_to_str(const uint16_t mjd)
{
    char* ret;
    uint16_t year, month, day, k;

    ret = av_malloc(16);

    year = (int)((mjd - 15078.2) / 365.25);
    month = (int)((mjd - 14956.1 - (int)(year * 365.25)) / 30.6001);
    day = mjd - 14956 - (int)(year * 365.25) - (int)(month * 30.6001);

    k = !!(month == 14 || month == 15);
    year += k;
    month -= 1 + 12 * k;

    snprintf(ret, 16, "%i/%i/%i", year + 1900, month, day);
    return ret;
}

char* av_epg_start_time_to_str(uint8_t start_time[5])
{
    char *ymd, *h, *ret;
    uint16_t mjd = (start_time[0] << 8) | start_time[1];
    uint32_t bcd = (start_time[2] << 16) | (start_time[3] << 8) | start_time[4];

    ymd = av_epg_mjd_to_str(mjd);
    h = av_epg_bcd_to_str(bcd);

    ret = av_malloc(32);
    snprintf(ret, 32, "%s %s", ymd, h);
    return ret;
}

char* av_epg_duration_to_str(uint8_t duration[3])
{
    uint32_t bcd = (duration[0] << 16) | (duration[1] << 8) | duration[2];
    return av_epg_bcd_to_str(bcd);
}

static const char* const all_running_status[8] = {
    [0] = "undefined\0",
    [1] = "not running\0",
    [2] = "starts in a few seconds (e.g. for video recording)\0",
    [3] = "pausing\0",
    [4] = "running\0",
    [5] = "service off-air\0",
    [6 ... 7] = "reserved for future use\0",
};

char* av_epg_running_status(const uint8_t running_status)
{
    return (running_status < 8) ? (char*)all_running_status[running_status] : (char*)"null";
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

void av_epg_component_str(EPGEvent *event, char* dst)
{
    if(!dst || !event)
        return;

    if (event->component.stream_content == 0x4)
        ac3_description(event->component.type, dst);
    else if (event->component.stream_content == 0x7 && event->component.type < 0x80)
        dvb_audio_description(event->component.type, dst);
    else if (event->component.stream_content == 0xB && event->component.stream_content_ext == 0xE)
        nga_feature_description(event->component.type, dst);
    else
        strcpy(dst, component_descriptions[event->component.stream_content]
                [event->component.stream_content_ext][event->component.type]);
}

#include "contenttables.h"

char* av_epg_content_simple_str(EPGEvent *event)
{
    return (char*)simple_nibbles[event->content.nibble_lvl_1];
}

char* av_epg_content_detailed_str(EPGEvent *event)
{
    return (char*)detailed_nibbles[event->content.nibble_lvl_1][event->content.nibble_lvl_2];
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
            ret = av_malloc(size);
            memcpy(ret, "undefined\0", size);
            break;
        case 0x01 ... 0x0F:
            ret = av_malloc(3);
            snprintf(ret, 3, "%d", rating + 3);
            break;
        default:
            ret = NULL;
            break;
    }
    return ret;
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
