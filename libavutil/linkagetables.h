/*
 * epg data decoder - tables used by epg decoder
 * Copyright (c) 2019 Anthony Delannoy
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
 * epg data decoder - tables used by epg decoder
 */

#ifndef AVUTIL_LINKAGE_TABLES_H
#define AVUTIL_LINKAGE_TABLES_H

static const char* const link_type_coding[4][0x20] = {
    [0] = {
        [0x00 ... 0x0D] = "null\0",
        [0x0E] = "SD\0",
        [0x0F] = "UHD\0",
        [0x10 ... 0x1F] = "reserved for future use\0",
    },
    [1] = {
        [0x00 ... 0x0D] = "null\0",
        [0x0E] = "HD\0",
        [0x0F] = "service frame compatible plano-stereoscopic\0",
        [0x10 ... 0x1F] = "reserved for future use\0",
    },
    [2] = {
        [0x00 ... 0x0D] = "null\0",
        [0x0E] = "frame compatible plano-stereoscopic H.264/AVC\0",
        [0x0F] = "reserved for future use\0",
        [0x10 ... 0x1F] = "reserved for future use\0",
    },
    [3] = {
        [0x00 ... 0x0D] = "null\0",
        [0x0E] = "service compatible plano-stereoscopic MVC\0",
        [0x0F] = "reserved for future use\0",
        [0x10 ... 0x1F] = "reserved for future use\0",
    },
};

#endif /* AVUTIL_LINKAGE_TABLES_H */
