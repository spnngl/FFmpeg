#ifndef AVFORMAT_MPEGTS_EPG_H
#define AVFORMAT_MPEGTS_EPG_H

#include "mpegts.h"
#include "mpegts_descriptors/common.h"

struct EPGEvent {
    uint16_t id;
    struct {
        uint16_t service_id;
        uint16_t event_id;
    } shift_from;
    uint8_t start_time[5];
    uint8_t duration[3];
    uint8_t running_status :3;
    uint8_t free_ca_mode :1;
    char* min_age;
    uint32_t pil :20;
    //char country_code[3];
    //char iso_639_language_code[3];
    char* event_name;
    char* short_event_description;
    char* long_event_description;
    char* items;
    uint8_t last_number :4;
    struct {
        char* short_desc;
        char* long_desc;
    } component;
    struct {
        char* short_desc;
        char* long_desc;
    } content;
    struct {
        uint8_t size :2;
        uint8_t leak_rate :6;
    } sb;
};
typedef struct EPGEvent EPGEvent;

struct EPGData {
    uint16_t id;
    SimpleLinkedList *head;
};
typedef struct EPGData EPGData;

EPGData* epg_get_data(SimpleLinkedList **epg, const uint16_t id);

EPGEvent* epg_get_event(SimpleLinkedList **epg,
                        const uint16_t transport_stream_id,
                        const uint16_t original_network_id,
                        const uint16_t event_id);

void epg_free_event(EPGEvent *event),
     epg_free_all(SimpleLinkedList **epg),
     epg_data_to_csv(SimpleLinkedList *epg, const char* path),
     epg_event_show(EPGEvent *event, int log_level);

char *epg_bcd_to_str(const uint32_t bcd),
     *epg_mjd_to_str(const uint16_t mjd),
     *epg_event_start_time_str(uint8_t start_time[5]),
     *epg_event_duration_str(uint8_t duration[3]);

int epg_handle_descriptor(MpegTSDescriptorHeader *h, MpegTSDescriptor *desc,
                         EPGEvent *event, const uint8_t **pp,
                         const uint8_t *p_end);

#endif /* AVFORMAT_MPEGTS_EPG_H */
