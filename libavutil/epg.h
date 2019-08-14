#ifndef AVUTIL_EPG_H
#define AVUTIL_EPG_H

#include <stdint.h>
#include <stddef.h>

#define EPG_ID(__name) \
    union { \
        uint64_t ids; \
        struct { \
            uint16_t network_id; \
            uint16_t ts_id; \
            uint16_t service_id; \
            uint16_t event_id; \
        }; \
    } __name;

typedef struct EPGEvent {
    EPG_ID();
    EPG_ID(shift_from);
    uint8_t start_time[5];
    uint8_t duration[3];
    uint8_t running_status :3;
    uint8_t free_ca_mode :1;
    uint8_t rating;
    uint32_t pil :20;
    char country_code[3];
    char iso_639_language_code[3];
    char event_name[256];
    char short_event_description[256];
    char long_event_description[1024];
    char items[512];
    struct {
        uint8_t stream_content_ext :4;
        uint8_t stream_content :4;
        uint8_t type;
        uint8_t tag;
        char text[32];
    } component;
    struct {
        uint8_t nibble_lvl_1 :4;
        uint8_t nibble_lvl_2 :4;
        uint8_t user_byte;
    } content;
    struct {
        uint8_t size :2;
        uint8_t leak_rate :6;
    } sb;
} EPGEvent;

void av_epg_init_event(EPGEvent *event);
void av_epg_free_event(EPGEvent **event);
void av_epg_show_event(EPGEvent *event);
char* av_epg_bcd_to_str(const uint32_t bcd);
char* av_epg_mjd_to_str(const uint16_t mjd);
char* av_epg_start_time_to_str(uint8_t start_time[5]);
char* av_epg_duration_to_str(uint8_t duration[3]);
char* av_epg_running_status(const uint8_t running_status);
void av_epg_component_str(EPGEvent *event, char* dst);
char* av_epg_content_simple_str(EPGEvent *event);
char* av_epg_content_detailed_str(EPGEvent *event);
char* av_epg_parental_rating_min_age(const uint8_t rating);
int av_epg_pdc_date_str(const uint32_t pil, char* dst);
char* av_epg_sb_size_str(const uint8_t sb_size);
char* av_epg_sb_leak_rate_str(const uint8_t sb_leak_rate);

#endif /* AVUTIL_EPG_H */
