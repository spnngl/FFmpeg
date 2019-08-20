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
 * Public header for DVB Sections.
 */

#ifndef AVUTIL_DVB_H
#define AVUTIL_DVB_H

#include "dvbdescriptors.h"

typedef struct DvbSectionHeader {
    /**
     * Section table_id, defined in libavformat/mpegts.h
     */
    uint8_t tid;
    /**
     * This is a 16-bit field which serves as a label to identify this service
     * from any other service within a TS. The service_id is the same as the
     * program_number in the corresponding program_map_section.
     */
    uint16_t id;
    /**
     * This 5-bit field is the version number of the sub_table. The
     * version_number shall be incremented by 1 when a change in the
     * information carried within the sub_table occurs. When it reaches value
     * 31, it wraps around to 0. When the current_next_indicator is set to "1",
     * then the version_number shall be that of the currently applicable
     * sub_table. When the current_next_indicator is set to "0", then the
     * version_number shall be that of the next applicable sub_table.
     */
    uint8_t version;
    /**
     * This 8-bit field gives the number of the section. The section_number of
     * the first section in the sub_table shall be "0x00". The section_number
     * shall be incremented by 1 with each additional section with the same
     * table_id, service_id, transport_stream_id, and original_network_id. In
     * this case, the sub_table may be structured as a number of segments.
     * Within each segment the section_number shall increment by 1 with each
     * additional section, but a gap in numbering is permitted between the last
     * section of a segment and the first section of the adjacent segment.
     */
    uint8_t sec_num;
    /**
     * This 8-bit field specifies the number of the last section (that is, the
     * section with the highest section_number) of the sub_table of which this
     * section is part
     */
    uint8_t last_sec_num;
    /**
     * This 1-bit indicator, when set to "1" indicates that the sub_table is
     * the currently applicable sub_table. When the bit is set to "0", it
     * indicates that the sub_table sent is not yet applicable and shall be the
     * next sub_table to be valid
     */
    uint8_t current_next_indic;
} DvbSectionHeader;

int avpriv_dvb_get8 (const uint8_t **pp, const uint8_t *p_end);
int avpriv_dvb_get16(const uint8_t **pp, const uint8_t *p_end);
int avpriv_dvb_get32(const uint8_t **pp, const uint8_t *p_end);

int avpriv_dvb_parse_section_header(DvbSectionHeader *h, const uint8_t **pp,
                                    const uint8_t *p_end);

typedef struct EPGSubTable {
    /**
     * This 16-bit field contains the identification number of the described
     * event (uniquely allocated within a service definition).
     */
    uint16_t event_id;
    /**
     * This 40-bit field contains the start time of the event in Universal
     * Time, Co-ordinated (UTC) and Modified Julian Date (MJD).  This field is
     * coded as 16 bits giving the MJD followed by 24 bits coded as 6 digits in
     * 4-bit Binary Coded Decimal (BCD). If the start time is undefined (e.g.
     * for an event in a NVOD reference service) all bits of the field are set
     * to "1".
     */
    uint8_t start_time[5];
    /**
     * A 24-bit field containing the duration of the event in hours, minutes,
     * seconds in 4-bit Binary Coded Decimal (BCD) format.
     */
    uint8_t duration[3];
    /**
     * This is a 3-bit field indicating the status of the event as defined in
     * libavutil/dvb.c
     */
    uint8_t running_status :3;
    /**
     * This 1-bit field, when set to "0" indicates that all the component
     * streams of the event are not scrambled. When set to "1" it indicates
     * that access to one or more streams is controlled by a CA system.
     */
    uint8_t free_ca_mode :1;
    /**
     * This 12-bit field gives the total length in bytes of the following descriptors.
     */
    uint16_t desc_loop_len :12;
    struct {
        int nb_descriptors;
        void **descriptors;
    };
} EPGSubTable;

typedef struct EPGTable {
    DvbSectionHeader h;
    /**
     * This is a 16-bit field which serves as a label for identification of the
     * TS, about which the EIT informs, from any other multiplex within the
     * delivery system.
     */
    uint16_t ts_id;
    /**
     * This 16-bit field gives the label identifying the network_id of the
     * originating delivery system.
     */
    uint16_t network_id;
    /**
     * This 8-bit field specifies the number of the last section of this
     * segment of the sub_table. For sub_tables which are not segmented, this
     * field shall be set to the same value as the last_section_number field.
     */
    uint8_t segment_last_section_num;
    /**
     * This 8-bit field identifies the last table_id used. For EIT
     * present/following tables, this field shall be set to the same value as
     * the table_id field. For EIT schedule tables with table_id in the range
     * 0x50 to 0x5F, this field shall be set to the largest table_id
     * transmitted in this range for this service. For EIT schedule tables with
     * table_id inthe range 0x60 to 0x6F, this field shall be set to the
     * largest table_id transmitted in this range for this service.
     */
    uint8_t last_tid;
    struct {
        int nb_subtables;
        EPGSubTable **subtables;
    };
    /**
     * This is a 32-bit field that contains the CRC value that gives a zero
     * output of the registers in the decoder defined in annex A of ISO/IEC
     * 13818-1 after processing the entire private section.
     */
    int32_t crc;
} EPGTable;

/**
 * Allocate an EPGSubTable and set its fields to default values.
 * @return An EpgSubTable with default values or NULL on failure.
 */
EPGSubTable* av_epg_subtable_alloc(void);

/**
 * Allocate an EPGTable and set its fields to default values. The resulting
 * struct must be freed with av_epg_table_free().
 * @return An EPGTable with default values or NULL on failure.
 */
EPGTable* av_epg_table_alloc(void);

/**
 * Free the EPGSubtable and its dynamically allocated objects (DvbDescriptors).
 * @param[out]  psubtable   EPGSubTable to be freed. The pointer will be set to
 * NULL.
 */
void av_epg_subtable_free(EPGSubTable **psubtable);

/**
 * Free the EPGTable and its dynamically allocated objects (EPGSubTable).
 * @param[out]  ptable  EPGTable to be freed. The pointer will be set to NULL.
 */
void av_epg_table_free(EPGTable **ptable);

/**
 * Print an EPGSubTable and its descriptors informations at the log level
 * indicated.
 * @param   subtable        EPGSubTable to show
 * @param   av_log_level    AV_LOG_* constant
 */
void av_epg_show_subtable(EPGSubTable *subtable, int av_log_level);

/**
 * Print an EPGTable and its EPGSubTable informations at the log level
 * indicated.
 * @param   table           EPGTable to show
 * @param   av_log_level    AV_LOG_* constant
 */
void av_epg_show_table(EPGTable *table, int av_log_level);

/**
 * Separate hour/min/sec from a BCD (4-bit Binary Coded Decimal) and store them
 * in an array (indexes: 0 = hour, 1 = minutes, 2 = seconds).
 * @param       bcd     BCD time
 * @param[out]  arr     Array which store hour/min/sec separate values
 */
void av_dvb_bcd_to_arr(const uint32_t bcd, uint8_t arr[3]);

/**
 * Convert a BCD (4-bit Binary Coded Decimal) time into string.
 * @param   bcd     BCD time to convert.
 * @return  UTC time with "HH:mm:SS" format.
 *
 * @note    Returned char* has to be freed with av_free()
 *
 * @example "01:45:30" time is coded as 0x014530
 */
char* av_dvb_bcd_to_str(const uint32_t bcd);

/**
 * Separate year/month/day from an MJD (Modified Julian Date) date and store
 * them in an array (indexes: 0 = year, 1 = month, 2 = day).
 * @param       mjd     MJD date
 * @param[out]  arr     Array which store year/month/day separate values
 *
 * @note    Add 1900 to computed year to have the right one
 */
void av_dvb_mjd_to_arr(const uint16_t mjd, uint16_t arr[3]);

/**
 * Convert an MJD (Modified Julian Date) date into an understanble string.
 * @param   mjd     MJD date to convert.
 * @return  UTC date with "YYYY/MM/DD" format.
 *
 * @note    Returned char* has to be freed with av_free()
 *
 * @example "93/10/13" is coded as 0xC079
 */
char* av_dvb_mjd_to_str(const uint16_t mjd);

/**
 * Convert a concatenated MJD date and BCD time into an understandable string.
 * @param   start_time  Concatenated date and time.
 * @return  UTC datetime with "YYYY/MM/DD HH:mm:SS" format.
 *
 * @note    Returned char* has to be freed with av_free()
 *
 * @example "93/10/13 01:45:30" is coded as 0xC079014530
 */
char* av_epg_start_time_to_str(uint8_t start_time[5]);

/**
 * Convert an EPG duration to an understandable string
 * @param   duration    Duration to convert
 * @return  UTC time with "HH:mm:SS" format
 *
 * @note    Returned char* has to be freed with av_free()
 */
char* av_epg_duration_to_str(uint8_t duration[3]);

/**
 * Convert three bits coded running_status value to string
 * @param   running_status  Running status value (in [0;8[)
 * @return  Running status equivalent string
 *
 * @note    Do not use av_free() on returned string. Those are statically
 * allocated.
 */
char* av_epg_running_status(const uint8_t running_status);

#endif  /* AVUTIL_DVB_H */
