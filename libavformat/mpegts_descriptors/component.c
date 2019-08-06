#include "libavutil/mem.h"
#include "../mpegts.h"
#include "component.h"
#include "common.h"


#if CONFIG_SMALL
void mpegts_component_description(const MpegTSComponentDescriptor *comp, char* dst)
{
    strcpy(dst, "null\0");
}
#else
// First index is stream_content
// Second index is stream_content_ext
// Third index is component_type
static const char* const component_descriptions[0x10][0x10][0x100] = {
    [0x0] = {
        [0x0 ... 0xF] = {
            [0x00 ... 0xFF] = "reserved for future use\0",
        }
    },
    [0x1] = {
        [0x0 ... 0xF] = {
            [0x00] = "reserved for future use\0",
            [0x01] = "MPEG-2 video, 4:3 aspect ratio, 25 Hz\0",
            [0x02] = "MPEG-2 video, 16:9 aspect ratio with pan vectors, 25 Hz\0",
            [0x03] = "MPEG-2 video, 16:9 aspect ratio without pan vectors, 25 Hz\0",
            [0x04] = "MPEG-2 video, > 16:9 aspect ratio, 25 Hz\0",
            [0x05] = "MPEG-2 video, 4:3 aspect ratio, 30 Hz\0",
            [0x06] = "MPEG-2 video, 16:9 aspect ratio with pan vectors, 30 Hz\0",
            [0x07] = "MPEG-2 video, 16:9 aspect ratio without pan vectors, 30 Hz\0",
            [0x08] = "MPEG-2 video, > 16:9 aspect ratio, 30 Hz\0",
            [0x09] = "MPEG-2 high definition video, 4:3 aspect ratio, 25 Hz\0",
            [0x0A] = "MPEG-2 high definition video, 16:9 aspect ratio with pan vectors, 25 Hz\0",
            [0x0B] = "MPEG-2 high definition video, 16:9 aspect ratio without pan vectors, 25 Hz\0",
            [0x0C] = "MPEG-2 high definition video, > 16:9 aspect ratio, 25 Hz\0",
            [0x0D] = "MPEG-2 high definition video, 4:3 aspect ratio, 30 Hz\0",
            [0x0E] = "MPEG-2 high definition video, 16:9 aspect ratio with pan vectors, 30 Hz\0",
            [0x0F] = "MPEG-2 high definition video, 16:9 aspect ratio without pan vectors, 30 Hz\0",
            [0x10] = "MPEG-2 high definition video, > 16:9 aspect ratio, 30 Hz\0",
            [0x11 ... 0xAF] = "reserved for future use\0",
            [0xB0 ... 0xFE] = "user defined\0",
            [0xFF] = "reserved for future use\0",
        },
    },
    [0x2] = {
        [0x0 ... 0xF] = {
            [0x00] = "reserved for future use\0",
            [0x01] = "MPEG-1 Layer 2 audio, single mono channel\0",
            [0x02] = "MPEG-1 Layer 2 audio, dual mono channel\0",
            [0x03] = "MPEG-1 Layer 2 audio, stereo (2 channel)\0",
            [0x04] = "MPEG-1 Layer 2 audio, multi-lingual, multi-channel\0",
            [0x05] = "MPEG-1 Layer 2 audio, surround sound\0",
            [0x06 ... 0x3F] = "reserved for future use\0",
            [0x40] = "MPEG-1 Layer 2 audio description for the visually impaired\0",
            [0x41] = "MPEG-1 Layer 2 audio for the hard of hearing\0",
            [0x42] = "receiver-mix supplementary audio\0",
            [0x43 ... 0x46] = "reserved for future use\0",
            [0x47] = "MPEG-1 Layer 2 audio, receiver-mix audio description\0",
            [0x48] = "MPEG-1 Layer 2 audio, broadcast-mix audio description\0",
            [0x49 ... 0xAF] = "reserved for future use\0",
            [0xB0 ... 0xFE] = "user defined\0",
            [0xFF] = "reserved for future use\0",
        },
    },
    [0x3] = {
        [0x0 ... 0xF] = {
            [0x00] = "reserved for future use\0",
            [0x01] = "EBU Teletext subtitles\0",
            [0x02] = "associated EBU Teletext \0",
            [0x03] = "VBI data\0",
            [0x04 ... 0x0F] = "reserved for future use\0",
            [0x10] = "VB subtitles (normal) with no monitor aspect ratio criticality\0",
            [0x11] = "DVB subtitles (normal) for display on 4:3 aspect ratio monitor\0",
            [0x12] = "DVB subtitles (normal) for display on 16:9 aspect ratio monitor\0",
            [0x13] = "DVB subtitles (normal) for display on 2.21:1 aspect ratio monitor\0",
            [0x14] = "DVB subtitles (normal) for display on a high definition monitor\0",
            [0x15] = "DVB subtitles (normal) with plano-stereoscopic disparity for display "
                     "on a high definition monitor\0",
            [0x16 ... 0x1F] = "reserved for future use\0",
            [0x20] = "DVB subtitles (for the hard of hearing) with no monitor aspect ratio criticality\0",
            [0x21] = "DVB subtitles (for the hard of hearing) for display on 4:3 aspect ratio monitor\0",
            [0x22] = "DVB subtitles (for the hard of hearing) for display on 16:9 aspect ratio monitor\0",
            [0x23] = "DVB subtitles (for the hard of hearing) for display on 2.21:1 aspect ratio monitor\0",
            [0x24] = "DVB subtitles (for the hard of hearing) for display on a high definition monitor\0",
            [0x25] = "DVB subtitles (for the hard of hearing) with plano-stereoscopic disparity for display on a high definition monitor\0",
            [0x26 ... 0x2F] = "reserved for future use\0",
            [0x30] = "open (in-vision) sign language interpretation for the deaf\0",
            [0x31] = "closed sign language interpretation for the deaf\0",
            [0x32 ... 0x3F] = "reserved for future use\0",
            [0x40] = "video spatial resolution has been upscaled from lower resolution source material\0",
            [0x41] = "Video is standard dynamic range (SDR)\0",
            [0x42] = "Video is high dynamic range (HDR) remapped from standard dynamic range (SDR) source material\0",
            [0x43] = "Video is high dynamic range (HDR) up-converted from standard dynamic range (SDR) source material\0",
            [0x44] = "Video is standard frame rate, less than or equal to 60 Hz\0",
            [0x45] = "High frame rate video generated from lower frame rate source material\0",
            [0x46 ... 0x7F] = "reserved for future use\0",
            [0x80] = "dependent SAOC-DE data stream\0",
            [0x81 ... 0xAF] = "reserved for future use\0",
            [0xB0 ... 0xFE] = "user defined\0",
            [0xFF] = "reserved for future use\0",
        },
    },
    [0x4] = {
        [0x0 ... 0xF] = {
            [0x00 ... 0x7F] = "reserved for AC-3 audio modes\0",
            [0x80 ... 0xFF] = "reserved for enhanced AC-3 audio modes\0",
        },
    },
    [0x5] = {
        [0x0 ... 0xF] = {
            [0x00] = "reserved for future use\0",
            [0x01] = "H.264/AVC standard definition video, 4:3 aspect ratio, 25 Hz\0",
            [0x02] = "reserved for future use\0",
            [0x03] = "H.264/AVC standard definition video, 16:9 aspect ratio, 25 Hz\0",
            [0x04] = "H.264/AVC standard definition video, > 16:9 aspect ratio, 25 Hz\0",
            [0x05] = "H.264/AVC standard definition video, 4:3 aspect ratio, 30 Hz\0",
            [0x06] = "reserved for future use\0",
            [0x07] = "H.264/AVC standard definition video, 16:9 aspect ratio, 30 Hz\0",
            [0x08] = "H.264/AVC standard definition video, > 16:9 aspect ratio, 30 Hz\0",
            [0x09 ... 0x0A] = "reserved for future use\0",
            [0x0B] = "H.264/AVC high definition video, 16:9 aspect ratio, 25 Hz\0",
            [0x0C] = "H.264/AVC high definition video, > 16:9 aspect ratio, 25 Hz\0",
            [0x0D ... 0x0E] = "reserved for future use\0",
            [0x0F] = "H.264/AVC high definition video, 16:9 aspect ratio, 30 Hz\0",
            [0x10] = "H.264/AVC high definition video, > 16:9 aspect ratio, 30 Hz\0",
            [0x11 ... 0x7F] = "reserved for future use\0",
            [0x80] = "H.264/AVC plano-stereoscopic frame compatible high definition video, "
                     "16:9 aspect ratio, 25 Hz, Side-by-Side\0",
            [0x81] = "H.264/AVC plano-stereoscopic frame compatible high definition video, "
                     "16:9 aspect ratio, 25 Hz, Top-and-Bottom\0",
            [0x82] = "H.264/AVC plano-stereoscopic frame compatible high definition video, "
                     "16:9 aspect ratio, 30 Hz, Side-by-Side\0",
            [0x83] = "H.264/AVC stereoscopic frame compatible high definition video, "
                     "16:9 aspect ratio, 30 Hz, Top-and-Bottom\0",
            [0x84] = "H.264/MVC dependent view, plano-stereoscopic service compatible video\0",
            [0x85 ... 0xAF] = "reserved for future use\0",
            [0xB0 ... 0xFE] = "user defined\0",
            [0xFF] = "reserved for future use\0",
        },
    },
    [0x6] = {
        [0x0 ... 0xF] = {
            [0x00] = "reserved for future use\0",
            [0x01] = "HE AAC audio, single mono channel\0",
            [0x02] = "reserved for future use\0",
            [0x03] = "HE AAC audio, stereo\0",
            [0x04] = "reserved for future use\0",
            [0x05] = "HE AAC audio, surround sound\0",
            [0x06 ... 0x3F] = "reserved for future use\0",
            [0x40] = "HE AAC audio description for the visually impaired\0",
            [0x41] = "HE AAC audio for the hard of hearing\0",
            [0x42] = "HE AAC receiver-mix supplementary audio\0",
            [0x43] = "HE AAC v2 audio, stereo\0",
            [0x44] = "HE AAC v2 audio description for the visually impaired\0",
            [0x45] = "HE AAC v2 audio for the hard of hearing\0",
            [0x46] = "HE AAC v2 receiver-mix supplementary audio\0",
            [0x47] = "HE AAC receiver-mix audio description for the visually impaired\0",
            [0x48] = "HE AAC broadcast-mix audio description for the visually impaired\0",
            [0x49] = "HE AAC v2 receiver-mix audio description for the visually impaired\0",
            [0x4a] = "HE AAC v2 broadcast-mix audio description for the visually impaired\0",
            [0x4b ... 0x9F] = "reserved for future use\0",
            [0xA0] = "HE AAC, or HE AAC v2 with SAOC-DE ancillary data\0",
            [0xA1 ... 0xAF] = "reserved for future use\0",
            [0xB0 ... 0xFE] = "user defined\0",
            [0xFF] = "reserved for future use\0",
        },
    },
    [0x7] = {
        [0x0 ... 0xF] = {
            [0x00 ... 0x7F] = "reserved for DTS and DTS-HD audio modes",
            [0x80 ... 0xFF] = "reserved for future use\0",
        },
    },
    [0x8] = {
        [0x0 ... 0xF] = {
            [0x00] = "reserved for future use\0",
            [0x01] = "DVB SRM data\0",
            [0x02 ... 0xFF] = "reserved for DVB CPCM modes\0",
        },
    },
    [0x9] = {
        [0x0] = {
            [0x00] = "HEVC Main Profile high definition video, 50 Hz\0",
            [0x01] = "HEVC Main 10 Profile high definition video, 50 Hz\0",
            [0x02] = "HEVC Main Profile high definition video, 60 Hz\0",
            [0x03] = "HEVC Main Profile high definition video, 60 Hz\0",
            [0x04] = "HEVC ultra high definition video\0",
            [0x05] = "HEVC ultra high definition video with PQ10 HDR with a frame "
                     "rate lower than or equal to 60 Hz\0",
            [0x06] = "HEVC ultra high definition video, frame rate of 100 Hz, "
                     "120 000/1 001 Hz, or 120 Hz without a half frame rate "
                     "HEVC temporal video sub-bit-stream\0",
            [0x07] = "HEVC ultra high definition video with PQ10 HDR, frame rate of "
                     "100 Hz, 120 000/1 001 Hz, or 120 Hz without a half frame rate "
                     "HEVC temporal video sub-bit-stream\0",
            [0x08 ... 0xFF] = "reserved for future use\0",
        },
        [0x1] = {
            [0x00] = "AC-4 main audio, mono\0",
            [0x01] = "AC-4 main audio, mono, dialogue enhancement enabled\0",
            [0x02] = "AC-4 main audio, stereo\0",
            [0x03] = "AC-4 main audio, stereo, dialogue enhancement enabled\0",
            [0x04] = "AC-4 main audio, multichannel\0",
            [0x05] = "AC-4 main audio, multichannel, dialogue enhancement enabled\0",
            [0x06] = "AC-4 broadcast-mix audio description, mono, for the visually impaired\0",
            [0x07] = "AC-4 broadcast-mix audio description, mono, for the visually impaired,"
                     " dialogue enhancement enabled\0",
            [0x08] = "AC-4 broadcast-mix audio description, stereo, for the visually impaired\0",
            [0x09] = "AC-4 broadcast-mix audio description, stereo, for the visually impaired,"
                     " dialogue enhancement enabled\0",
            [0x0A] = "AC-4 broadcast-mix audio description, multichannel, for the visually impaired\0",
            [0x0B] = "AC-4 broadcast-mix audio description, multichannel, for the visually impaired,"
                     " dialogue enhancement enabled\0",
            [0x0C] = "AC-4 receiver-mix audio description, mono, for the visually impaired\0",
            [0x0D] = "AC-4 receiver-mix audio description, stereo, for the visually impaired\0",
            [0x0E] = "AC-4 Part-2\0",
            [0x0F] = "MPEG-H Audio LC Profile\0",
            [0x10 ... 0xFF] = "reserved for future use\0",
        },
        [0x2] = {
            [0x00 ... 0xFF] = "TTML subtitles\0",
        },
        [0x3 ... 0xF] = {
            [0x00 ... 0xFF] = "reserved for future use\0",
        },
    },
    [0xA] = {
        [0x0 ... 0xF] = {
            [0x00 ... 0xFF] = "reserved for future use\0",
        },
    },
    [0xB] = {
        [0x0 ... 0xD] = {
            [0x00 ... 0xFF] = "reserved for future use\0",
        },
        [0xE] = {
            [0x00 ... 0xFF] = "NGA component type feature flags\0",
        },
        [0xF] = {
            [0x00] = "less than 16:9 aspect ratio\0",
            [0x01] = "16:9 aspect ratio\0",
            [0x02] = "greater than 16:9 aspect ratio\0",
            [0x03] = "plano-stereoscopic top and bottom (TaB) frame-packing\0",
            [0x04] = "HLG10 HDR\0",
            [0x05] = "HEVC temporal video subset for a frame rate of 100 Hz, 120 000/1 001 Hz, or 120 Hz\0",
            [0x06 ... 0xFF] = "reserved for future use\0",
        },
    },
    [0xC ... 0xF] = {
        [0x0 ... 0xF] = {
            [0x00 ... 0xFF] = "user defined\0",
        },
    },
};

#define NGA_B6 "content is pre-rendered for consumption with headphones\0"
#define NGA_B5 "content enables interactivity\0"
#define NGA_B4 "content enables dialogue enhancement\0"
#define NGA_B3 "content contains spoken subtitles\0"
#define NGA_B2 "content contains audio description\0"

static const char* const nga_b1b0[4] = {
    [0] = "no preference\0", [1] = "stereo\0",
    [2] = "two-dimensional\0", [3] = "three-dimensional\0"
};

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

    strcpy(dst + head, nga_b1b0[component_type & bit_mask(2)]);
}

static const char* const ac3_b7[2] = {
    [0] = "AC-3\0",
    [1] = "Enhanced AC-3\0"
};

static const char* const ac3_b6[2] = {
    [0] = "Decoded audio stream is an associated service intended to be "
          "combined with another decoded audio stream before presentation to the listener\0",
    [1] = "Decoded audio stream is a full service\0"
};

static const char* const ac3_b543[8] = {
    [0] = "Complete Main\0",
    [1] = "Music and Effects\0",
    [2] = "Visually Impaired\0",
    [3] = "Hearing Impaired\0",
    [4] = "Dialogue\0",
    [5] = "Commentary\0",
    [6] = "Emergency\0",
    [7] = "Voiceover or Karaoke\0"
};

static const char* const ac3_b210[8] = {
    [0] = "Mono\0", [1] = "1+1 mode\0", [2] = "2 channels (stereo)\0",
    [3] = "2 channels surround encoded (stereo)\0",
    [4] = "Multichannel audio (> 2 channels)\0",
    [5] = "Multichannel audio (> 5.1 channels)\0",
    [6] = "Elementary stream contains multiple programmes carried in independent substreams\0",
    [7] = "reserved for future use\0"
};

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

    cur = ac3_b543[(component_type >> 3) & bit_mask(3)];
    size = strlen(cur);
    memcpy(dst + head, cur, size);
    head += size;
    memcpy(dst + head, ", \0", 2);
    head += 2;

    cur = ac3_b210[component_type & bit_mask(3)];
    size = strlen(cur);
    strcpy(dst + head, cur);
    head += size;

    return head;
}

static const char* const dvb_audio_b6[2] = {
    "Decoded audio stream is intended to be combined with another decoded audio stream before presentation\0",
    "Decoded audio stream is a full service (suitable for decoding and presentation to the listener\0"
};

static const char* const dvb_audio_b543[8] = {
    [0] = "Complete Main\0",
    [1] = "Music and Effects\0",
    [2] = "Visually Impaired\0",
    [3] = "Hearing Impaired\0",
    [4] = "Dialogue\0",
    [5] = "Commentary\0",
    [6] = "Emergency\0",
    [7] = "Voiceover\0"
};

static const char* const dvb_audio_b210[8] = {
    [0] = "Mono\0", [1] = "reserved\0", [2] = "2 channels (stereo, LoRo)\0",
    [3] = "2 channels matrix encoded (stereo, LtRt)\0",
    [4] = "Multichannel audio (> 2 channels)\0",
    [5 ... 7] = "reserved\0",
};

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

    cur = dvb_audio_b543[(component_type >> 3) & bit_mask(3)];
    size = strlen(cur);
    memcpy(dst + head, cur, size);
    head += size;
    memcpy(dst + head, ", \0", 2);
    head += 2;

    cur = dvb_audio_b210[component_type & bit_mask(3)];
    size = strlen(cur);
    memcpy(dst + head, cur, size);
    head += size;

    return head;
}

void mpegts_component_description(const MpegTSComponentDescriptor *comp, char* dst)
{
    if(!dst)
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
#endif

static int component_parse(MpegTSDescriptor *desc, const uint8_t **pp, const uint8_t *p_end)
{
    int val, text_len;
    const uint8_t *p = *pp;
    MpegTSComponentDescriptor *comp;

    if (desc->priv)
        desc->free(desc);

    desc->priv = av_mallocz(sizeof(MpegTSComponentDescriptor));
    comp = desc->priv;

    val = mpegts_get8(&p, p_end);
    if (val < 0)
        return val;
    comp->stream_content_ext = (val >> 4);
    comp->stream_content = val & bit_mask(4);

    val = mpegts_get8(&p, p_end);
    if (val < 0)
        return val;
    comp->component_type = val;

    val = mpegts_get8(&p, p_end);
    if (val < 0)
        return val;
    comp->component_tag = val;

    memcpy(comp->iso_639_language_code, p, 3);
    p += 3;

    text_len = p_end - p;
    comp->text = av_malloc(text_len + 1);
    memcpy(comp->text, p, text_len);
    comp->text[text_len] = '\0';
    p += text_len;
    *pp = p;

    return 0;
}

static int component_free(MpegTSDescriptor *desc)
{
    if (desc->priv) {
        MpegTSComponentDescriptor *comp = desc->priv;
        av_free(comp->text);
        av_freep(&desc->priv);
    }
    return 0;
}

static void component_debug(MpegTSDescriptor *desc)
{
    if (desc->priv) {
        char description[512];
        MpegTSComponentDescriptor *comp = desc->priv;

        mpegts_component_description(comp, description);
        av_log(NULL, MPEGTS_DESCRIPTORS_LOG, "Component descriptor: "
                "stream_content_ext(%i), stream_content(%i), "
                "component_type(%i), description(%s), component_tag(%i), "
                "iso_639_language_code(%.3s), text(%s)\n",
                comp->stream_content_ext, comp->stream_content,
                comp->component_type, description, comp->component_tag,
                comp->iso_639_language_code, comp->text);
    }
}

MpegTSDescriptor component_descriptor = {
    .priv = NULL,
    .parse = component_parse,
    .free = component_free,
    .debug = component_debug
};
