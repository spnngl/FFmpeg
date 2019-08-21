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

#ifndef AVUTIL_SMOOTHING_BUFFER_TABLES_H
#define AVUTIL_SMOOTHING_BUFFER_TABLES_H

static const char* const smoothing_buffer_size[4] = {
    [0] = "DVB_reserved\0", [1] = "1536\0", [2] = "DVB_reserved\0", [3] = "DVB_reserved\0"
};

static const char* const smoothing_buffer_leak_rate[64] = {
    [0] = "DVB_reserved\0", [1] = "0.0009\0", [2] = "0.0018\0", [3] = "0.0036\0",
    [4] = "0.0072\0", [5] = "0.0108\0", [6] = "0.0144\0", [7] = "0.0216\0",
    [8] = "0.0288\0", [9] = "0.075\0", [10] = "0.5\0", [11] = "0.5625\0",
    [12] = "0.8437\0", [13] = "1.0\0", [14] = "1.1250\0", [15] = "1.5\0",
    [16] = "1.6875\0", [17] = "2.0\0", [18] = "2.25\0", [19] = "2.5\0",
    [20] = "3.0\0", [21] = "3.3750\0", [22] = "3.5\0", [23] = "4.0\0",
    [24] = "4.5\0", [25] = "5.0\0", [26] = "5.5\0", [27] = "6.0\0", [28] = "6.5\0",
    [29] = "6.75\0", [30] = "7.0\0", [31] = "7.5\0", [32] = "8.0\0", [33] = "9.0\0",
    [34] = "10.0\0", [35] = "11.0\0", [36] = "12.0\0", [37] = "13.0\0", [38] = "13.5\0",
    [39] = "14.0\0", [40] = "15.0\0", [41] = "16.0\0", [42] = "17.0\0", [43] = "18.0\0",
    [44] = "20.0\0", [45] = "22.0\0", [46] = "24.0\0", [47] = "26.0\0", [48] = "27.0\0",
    [49] = "28.0\0", [50] = "30.0\0", [51] = "32.0\0", [52] = "34.0\0", [53] = "36.0\0",
    [54] = "38.0\0", [55] = "40.0\0", [56] = "44.0\0", [57] = "48.0\0", [58] = "54.0\0",
    [59] = "72.0\0", [60] = "108.0\0", [61 ... 63] = "DVB_reserved\0",
};

#endif /* AVUTIL_SMOOTHING_BUFFER_TABLES_H */
