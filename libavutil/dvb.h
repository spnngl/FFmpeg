/*
 * DVB sections
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

#include <stdint.h>

typedef struct DvbSectionHeader {
    /**
     * Section table_id, defined in libavformat/mpegts.h
     */
    uint8_t tid;
    /**
     * This field can be service_id, transport_stream_id, network_id,
     * bouquet_id ... depending on the current table. See ETSI EN 300 468.
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

#endif  /* AVUTIL_DVB_H */
