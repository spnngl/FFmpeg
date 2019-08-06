#include "libavutil/mem.h"
#include "../mpegts.h"
#include "short_smoothing_buffer.h"
#include "common.h"

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

const char* mpegts_sb_size_str(const uint8_t sb_size)
{
    return sb_size < 4 ? smoothing_buffer_size[sb_size] : "null\0";
}

const char* mpegts_sb_leak_rate_str(const uint8_t sb_leak_rate)
{
    return sb_leak_rate < 64 ? smoothing_buffer_leak_rate[sb_leak_rate] : "null\0";
}

static int short_smoothing_buffer_parse(MpegTSDescriptor *desc, const uint8_t **pp, const uint8_t *p_end)
{
    int val, bytes_left;
    const uint8_t *p = *pp;
    MpegTSShortSmoothingBufferDescriptor *ssb = desc->priv;

    if (desc->priv)
        desc->free(desc);

    desc->priv = av_mallocz(sizeof(MpegTSShortSmoothingBufferDescriptor));
    ssb = desc->priv;

    val = mpegts_get8(&p, p_end);
    if (val < 0)
        return val;
    ssb->sb_size = (val >> 6);
    ssb->sb_leak_rate = val & bit_mask(6);

    bytes_left = p_end - p;
    ssb->DVB_reserved = av_malloc(bytes_left + 1);
    memcpy(ssb->DVB_reserved, p, bytes_left);
    ssb->DVB_reserved[bytes_left] = '\0';
    p += bytes_left;

    *pp = p;

    return 0;
}

static int short_smoothing_buffer_free(MpegTSDescriptor *desc)
{
    if (desc->priv) {
        MpegTSShortSmoothingBufferDescriptor *ssb = desc->priv;
        av_free(ssb->DVB_reserved);
        av_free(ssb);
        desc->priv = NULL;
    }
    return 0;
}

static void short_smoothing_buffer_debug(MpegTSDescriptor *desc)
{
    if (desc->priv) {
        MpegTSShortSmoothingBufferDescriptor *ssb = desc->priv;
        av_log(NULL, AV_LOG_INFO, "Short smoothing buffer descriptor: "
               "sb_size(%s), sb_leak_rate(%s), DVB_reserved(%s)\n",
               mpegts_sb_size_str(ssb->sb_size),
               mpegts_sb_leak_rate_str(ssb->sb_leak_rate),
               ssb->DVB_reserved);
    }
}

MpegTSDescriptor short_smoothing_buffer_descriptor = {
    .priv = NULL,
    .parse = short_smoothing_buffer_parse,
    .free = short_smoothing_buffer_free,
    .debug = short_smoothing_buffer_debug
};
