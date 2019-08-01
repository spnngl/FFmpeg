#include "libavutil/mem.h"
#include "../mpegts.h"
#include "content.h"
#include "common.h"


#if CONFIG_SMALL
char* mpegts_content_simple_description(MpegTSContentDescription * const content)
{
    return "null";
}

char* mpegts_content_detailed_description(MpegTSContentDescription * const content)
{
    return "null";
}
#else
static const char* const simple_nibbles[0x10] = {
    [0x0]           = "undefined\0",
    [0x1]           = "Movie/Drama\0",
    [0x2]           = "News/Current affairs\0",
    [0x3]           = "Show/Game show\0",
    [0x4]           = "Sports\0",
    [0x5]           = "Children's/Youth programmes\0",
    [0x6]           = "Music/Ballet/Dance\0",
    [0x7]           = "Arts/Culture (without music)\0",
    [0x8]           = "Social/Political issues/Economics\0",
    [0x9]           = "Education/Science/Factual topics\0",
    [0xA]           = "Leisure hobbies\0",
    [0xB]           = "Special characteristics\0",
    [0xC]           = "Adult\0",
    [0xD ... 0xE]   = "reserved for future use\0",
    [0xF]           = "user defined\0",
};

// First index is nibble1 value
// Second index is nibble2 value
static const char* const detailed_nibbles[0x10][0x10] = {
    [0x0] = {
        [0x0 ... 0xF] = "undefined"
    },
    [0x1] = {
        [0x0]           = "movie/drama (general)\0",
        [0x1]           = "detective/thriller\0",
        [0x2]           = "adventure/western/war\0",
        [0x3]           = "science fiction/fantasy/horror\0",
        [0x4]           = "comedy\0",
        [0x5]           = "soap/melodrama/folkloric\0",
        [0x6]           = "romance\0",
        [0x7]           = "serious/classical/religious/historical movie/drama\0",
        [0x8]           = "adult movie/drama\0",
        [0x9 ... 0xE]   = "reserved for future use\0",
        [0xF]           = "user defined\0",
    },
    [0x2] = {
        [0x0]           = "news/current affairs (general)\0",
        [0x1]           = "news/weather report\0",
        [0x2]           = "news magazine\0",
        [0x3]           = "documentary\0",
        [0x4]           = "discussion/interview/debate\0",
        [0x5 ... 0xE]   = "reserved for future use\0",
        [0xF]           = "user defined\0",
    },
    [0x3] = {
        [0x0]           = "show/game show (general)\0",
        [0x1]           = "game show/quiz/contest\0",
        [0x2]           = "variety show\0",
        [0x3]           = "talk show\0",
        [0x4 ... 0xE]   = "reserved for future use\0",
        [0xF]           = "user defined\0",
    },
    [0x4] = {
        [0x0]           = "sports (general)\0",
        [0x1]           = "sports special events\0",
        [0x2]           = "sports magazines\0",
        [0x3]           = "football/soccer\0",
        [0x4]           = "tennis/squash\0",
        [0x5]           = "team sports (excluding football)\0",
        [0x6]           = "athletics\0",
        [0x7]           = "motor sport\0",
        [0x8]           = "water sport\0",
        [0x9]           = "winter sports\0",
        [0xA]           = "equestrian\0",
        [0xB]           = "martial sports\0",
        [0xC ... 0xE]   = "reserved for future use\0",
        [0xF]           = "user defined\0",
    },
    [0x5] = {
        [0x0]           = "children's/youth programmes (general)\0",
        [0x1]           = "pre-school children's programmes\0",
        [0x2]           = "entertainment programmes for 6 to 14\0",
        [0x3]           = "entertainment programmes for 10 to 16\0",
        [0x4]           = "informational/educational/school programmes\0",
        [0x5]           = "cartoons/puppets\0",
        [0x6 ... 0xE]   = "reserved for future use\0",
        [0xF]           = "user defined\0",
    },
    [0x6] = {
        [0x0]           = "music/ballet/dance (general)\0",
        [0x1]           = "rock/pop\0",
        [0x2]           = "serious/classical music\0",
        [0x3]           = "folk/traditional music\0",
        [0x4]           = "jazz\0",
        [0x5]           = "musical/opera\0",
        [0x6]           = "ballet\0",
        [0x7 ... 0xE]   = "reserved for future use\0",
        [0xF]           = "user defined\0",
    },
    [0x7] = {
        [0x0]           = "arts/culture (without music, general)\0",
        [0x1]           = "performing arts\0",
        [0x2]           = "fine arts\0",
        [0x3]           = "religion\0",
        [0x4]           = "popular culture/traditionals arts\0",
        [0x5]           = "literature\0",
        [0x6]           = "film/cinema\0",
        [0x7]           = "experimental film/video\0",
        [0x8]           = "broadcasting/press\0",
        [0x9]           = "new media\0",
        [0xA]           = "arts/culture magazines\0",
        [0xB]           = "fashion\0",
        [0xC ... 0xE]   = "reserved for future use\0",
        [0xF]           = "user defined\0",
    },
    [0x8] = {
        [0x0]           = "social/political issues/economics (general)\0",
        [0x1]           = "magazines/reports/documentary\0",
        [0x2]           = "economics/social advisory\0",
        [0x3]           = "remarkable people\0",
        [0x4 ... 0xE]   = "reserved for future use\0",
        [0xF]           = "user defined\0",
    },
    [0x9] = {
        [0x0]           = "education/science/factual topics (general)\0",
        [0x1]           = "nature/animals/environment\0",
        [0x2]           = "technology/natural sciences\0",
        [0x3]           = "medicine/physiology/psychology\0",
        [0x4]           = "foreign countries/expeditions\0",
        [0x5]           = "social/spiritual sciences\0",
        [0x6]           = "further education\0",
        [0x7]           = "languages\0",
        [0x8 ... 0xE]   = "reserved for future use\0",
        [0xF]           = "user defined\0",
    },
    [0xA] = {
        [0x0]           = "leisure hobbies (general)\0",
        [0x1]           = "tourism/travel\0",
        [0x2]           = "handicraft\0",
        [0x3]           = "motoring\0",
        [0x4]           = "fitness and health\0",
        [0x5]           = "cooking\0",
        [0x6]           = "advertisement/shopping\0",
        [0x7]           = "gardening\0",
        [0x8 ... 0xE]   = "reserved for future use\0",
        [0xF]           = "user defined\0",
    },
    [0xB] = {
        [0x0]           = "original language\0",
        [0x1]           = "black and white\0",
        [0x2]           = "unpublished\0",
        [0x3]           = "live broadcast\0",
        [0x4]           = "plano-stereoscopic\0",
        [0x5]           = "local or regional\0",
        [0x6 ... 0xE]   = "reserved for future use\0",
        [0xF]           = "user defined\0",
    },
    [0xC] = {
        [0x0]           = "adult (general)\0",
        [0x1 ... 0xE]   = "reserved for future use\0",
        [0xF]           = "user defined\0",
    },
    [0xD ... 0xE] = {
        [0x0 ... 0xF]   = "reserved for future use\0",
    },
    [0xF] = {
        [0x0 ... 0xF]   = "user defined\0",
    }
};

char* mpegts_content_simple_description(MpegTSContentDescription * const content)
{
    return (char*)simple_nibbles[content->content_nibble_lvl_1];
}

char* mpegts_content_detailed_description(MpegTSContentDescription * const content)
{
    return (char*)detailed_nibbles[content->content_nibble_lvl_1][content->content_nibble_lvl_2];
}
#endif

static int content_parse(MpegTSDescriptor *desc, const uint8_t **pp, const uint8_t *p_end)
{
    int val;
    const uint8_t *p = *pp;
    MpegTSContentDescriptor *content;

    if (desc->priv)
        desc->free(desc);

    desc->priv = av_mallocz(sizeof(MpegTSContentDescriptor));
    content = desc->priv;

    content->nb_descriptions = (p_end - p) / sizeof(MpegTSContentDescription);
    content->descriptions = av_mallocz(content->nb_descriptions * sizeof(MpegTSContentDescription*));

    for(int i = 0; i < content->nb_descriptions; i++) {
        MpegTSContentDescription *d;

        content->descriptions[i] = av_mallocz(sizeof(MpegTSContentDescription));
        d = content->descriptions[i];

        val = mpegts_get8(&p, p_end);
        if (val < 0)
            break;
        d->content_nibble_lvl_1 = (val >> 4) & bit_mask(4);
        d->content_nibble_lvl_2 = val & bit_mask(4);

        val = mpegts_get8(&p, p_end);
        if (val < 0)
            break;
        d->user_byte = val;
    }

    return (val < 0);
}

static int content_free(MpegTSDescriptor *desc)
{
    if (desc->priv) {
        MpegTSContentDescriptor *content = desc->priv;

        for(int i = 0; i < content->nb_descriptions; i++)
            av_free(content->descriptions[i]);
        av_free(content->descriptions);
        av_freep(&desc->priv);
    }
    return 0;
}

static void content_debug(MpegTSDescriptor *desc)
{
    if (desc->priv) {
        MpegTSContentDescriptor *content = desc->priv;

        av_log(NULL, AV_LOG_INFO, "Content descriptor: \n");

        for(int i = 0; i < content->nb_descriptions; i++) {
            MpegTSContentDescription *d = content->descriptions[i];
            av_log(NULL, AV_LOG_INFO, "Content #%i: "
                    "nibble1(%i), nibble2(%i), user_byte(%i), title(%s), "
                    "description(%s)\n", i, d->content_nibble_lvl_1,
                    d->content_nibble_lvl_2, d->user_byte,
                    mpegts_content_simple_description(d),
                    mpegts_content_detailed_description(d));
        }

    }
}

MpegTSDescriptor content_descriptor = {
    .priv = NULL,
    .parse = content_parse,
    .free = content_free,
    .debug = content_debug
};
