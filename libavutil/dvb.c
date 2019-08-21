#include "libavutil/internal.h"
#include "libavutil/intreadwrite.h"
#include "dvb.h"
#include "dvbdescriptors.h"

int avpriv_dvb_get8(const uint8_t **pp, const uint8_t *p_end)
{
    const uint8_t *p;
    int c;

    p = *pp;
    if (p >= p_end)
        return AVERROR_INVALIDDATA;
    c   = *p++;
    *pp = p;
    return c;
}

int avpriv_dvb_get16(const uint8_t **pp, const uint8_t *p_end)
{
    const uint8_t *p;
    int c;

    p = *pp;
    if (1 >= p_end - p)
        return AVERROR_INVALIDDATA;
    c   = AV_RB16(p);
    p  += 2;
    *pp = p;
    return c;
}

int avpriv_dvb_get32(const uint8_t **pp, const uint8_t *p_end)
{
    const uint8_t *p;
    int c;

    p = *pp;
    if (3 >= p_end - p)
        return AVERROR_INVALIDDATA;
    c   = AV_RB32(p);
    p  += 4;
    *pp = p;
    return c;
}

int avpriv_dvb_parse_section_header(DvbSectionHeader *h, const uint8_t **pp,
                                    const uint8_t *p_end)
{
    int val;

    val = avpriv_dvb_get8(pp, p_end);
    if (val < 0)
        return val;
    h->tid = val;
    *pp += 2;
    val  = avpriv_dvb_get16(pp, p_end);
    if (val < 0)
        return val;
    h->id = val;
    val = avpriv_dvb_get8(pp, p_end);
    if (val < 0)
        return val;
    h->version = (val >> 1) & 0x1f;
    h->current_next_indic = val & 0x1;
    val = avpriv_dvb_get8(pp, p_end);
    if (val < 0)
        return val;
    h->sec_num = val;
    val = avpriv_dvb_get8(pp, p_end);
    if (val < 0)
        return val;
    h->last_sec_num = val;
    return 0;
}

EPGSubTable* av_epg_subtable_alloc(void)
{
    EPGSubTable *subtable = av_mallocz(sizeof(*subtable));

    if (!subtable)
        return NULL;

    memset(subtable->start_time, '\0', 5);
    memset(subtable->duration, '\0', 3);

    subtable->free_ca_mode = 1;
    subtable->descriptors = NULL;

    return subtable;
}

EPGTable* av_epg_table_alloc(void)
{
    EPGTable *table = av_mallocz(sizeof(*table));

    if (!table)
        return NULL;

    table->subtables = NULL;
    return table;
}

void av_epg_subtable_free(EPGSubTable **psubtable)
{
    EPGSubTable *subtable = *psubtable;

    for (int j = 0; j < subtable->nb_descriptors; j++) {
        DvbDescriptorHeader *h = subtable->descriptors[j];
        const DvbDescriptor * const desc = av_dvb_get_descriptor(h);
        desc->free(h);
    }
    av_freep(psubtable);
}

void av_epg_table_free(EPGTable **ptable)
{
    EPGTable *table = *ptable;
    EPGSubTable *subtable;

    for (int i = 0; i < table->nb_subtables; i++) {
        subtable = table->subtables[i];
        av_epg_subtable_free(&subtable);
    }
    av_freep(ptable);
}

void av_epg_show_subtable(EPGSubTable *subtable, int av_log_level)
{
    char *start_time = av_epg_start_time_to_str(subtable->start_time);
    char *duration = av_epg_duration_to_str(subtable->duration);
    char *running_status = av_epg_running_status(subtable->running_status);

    uint32_t hex_duration = (subtable->duration[0] << 16) |
                            (subtable->duration[1] << 8)  |
                             subtable->duration[2];
    uint64_t hex_start_time = ((uint64_t)subtable->start_time[0] << 32) |
                              ((uint64_t)subtable->start_time[1] << 24) |
                              ((uint64_t)subtable->start_time[2] << 16) |
                              ((uint64_t)subtable->start_time[3] << 8)  |
                               (uint64_t)subtable->start_time[4];

    av_log(NULL, av_log_level, "\n\tEPG subtable:\n"
           "\t\tevent_id = %i\n"
           "\t\tstart_time = %s (raw %.10lx)\n"
           "\t\tduration = %s (raw %.06x)\n"
           "\t\trunning_status = %s (raw %i)\n"
           "\t\tfree_ca_mode = %i\n",
           subtable->event_id, start_time, hex_start_time,
           duration, hex_duration, running_status, subtable->running_status,
           subtable->free_ca_mode);

    for (int i = 0; i < subtable->nb_descriptors; i++) {
        DvbDescriptorHeader *h = subtable->descriptors[i];
        const DvbDescriptor * const desc = av_dvb_get_descriptor(h);
        desc->show(h, av_log_level);
    }

    av_free(start_time);
    av_free(duration);
}

void av_epg_show_table(EPGTable *table, int av_log_level)
{
    if (av_log_get_level() < av_log_level)
        return;

    av_log(NULL, av_log_level, "EPG Table:\n"
           "\ttid = 0x%.02x\n"
           "\tservice_id = %i\n"
           "\tversion = %i\n"
           "\tsection_num = %i\n"
           "\tlast_section_num = %i\n"
           "\tcurrent_next_index = %i\n"
           "\tts_id = %i\n"
           "\tnetwork_id = %i\n"
           "\tsegment_last_section_num = %i\n"
           "\tlast_tid = 0x%.02x\n"
           "\tcrc32 = %i\n",
           table->h.tid, table->h.id, table->h.version,table->h.sec_num,
           table->h.last_sec_num, table->h.current_next_indic,
           table->ts_id, table->network_id, table->segment_last_section_num,
           table->last_tid, table->crc);

    for (int i = 0; i < table->nb_subtables; i++)
        av_epg_show_subtable(table->subtables[i], av_log_level);
}

void av_dvb_bcd_to_arr(const uint32_t bcd, uint8_t arr[3])
{
    arr[0]  = ((bcd >> 20) & 0xf) * 10 + ((bcd >> 16) & 0xf);
    arr[1]  = ((bcd >> 12) & 0xf) * 10 + ((bcd >> 8 ) & 0xf);
    arr[2]  = ((bcd >> 4 ) & 0xf) * 10 +  (bcd & 0xf);
}

char* av_dvb_bcd_to_str(const uint32_t bcd)
{
    uint8_t arr[3];
    char* ret = av_malloc(16);

    if (!ret)
        return NULL;

    av_dvb_bcd_to_arr(bcd, arr);
    snprintf(ret, 16, "%.02i:%.02i:%.02i", arr[0], arr[1], arr[2]);

    return ret;
}

void av_dvb_mjd_to_arr(const uint16_t mjd, uint16_t arr[3])
{
    uint16_t year, month, day, k;

    year = (int)((mjd - 15078.2) / 365.25);
    month = (int)((mjd - 14956.1 - (int)(year * 365.25)) / 30.6001);
    day = mjd - 14956 - (int)(year * 365.25) - (int)(month * 30.6001);

    k = !!(month == 14 || month == 15);
    year += k;
    month -= 1 + 12 * k;

    arr[0] = year, arr[1] = month, arr[2] = day;
}

char* av_dvb_mjd_to_str(const uint16_t mjd)
{
    uint16_t arr[3];
    char* ret = av_malloc(16);

    if (!ret)
        return NULL;

    av_dvb_mjd_to_arr(mjd, arr);
    snprintf(ret, 16, "%.04i/%.02i/%.02i", arr[0] + 1900, arr[1], arr[2]);
    return ret;
}

char* av_epg_start_time_to_str(uint8_t start_time[5])
{
    char *ymd, *h, *ret;
    uint16_t mjd = ((uint16_t)start_time[0] << 8)  | (uint16_t)start_time[1];
    uint32_t bcd = ((uint32_t)start_time[2] << 16) |
                   ((uint32_t)start_time[3] << 8)  | (uint32_t)start_time[4];

    ymd = av_dvb_mjd_to_str(mjd);
    h = av_dvb_bcd_to_str(bcd);

    ret = av_malloc(32);

    if (!ret)
        return NULL;

    snprintf(ret, 32, "%s %s", ymd, h);

    av_free(ymd);
    av_free(h);
    return ret;
}

char* av_epg_duration_to_str(uint8_t duration[3])
{
    uint32_t bcd = ((uint32_t)duration[0] << 16)
                 | ((uint32_t)duration[1] << 8) | (uint32_t)duration[2];
    return av_dvb_bcd_to_str(bcd);
}

static const char* const all_running_status[8] = {
    "undefined\0",
    "not running\0",
    "starts in a few seconds (e.g. for video recording)\0",
    "pausing\0",
    "running\0",
    "service off-air\0",
    "reserved for future use\0",
    "reserved for future use\0",
};

char* av_epg_running_status(const uint8_t running_status)
{
    return (char*)((running_status < 8) ? all_running_status[running_status] : "null");
}
