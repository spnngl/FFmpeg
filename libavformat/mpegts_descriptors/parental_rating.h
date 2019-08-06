#ifndef AVFORMAT_MPEGTS_DESCRIPTORS_PARENTAL_RATING_H
#define AVFORMAT_MPEGTS_DESCRIPTORS_PARENTAL_RATING_H

#include <stdint.h>

typedef struct MpegTSParentalRatingDescriptor MpegTSParentalRatingDescriptor;
typedef struct MpegTSParentalRatingDescription MpegTSParentalRatingDescription;

struct MpegTSParentalRatingDescription {
    /** Identify a country or group of countries */
    char country_code[3];
    /** Coded rating, use @mpegts_parental_rating_min_age() to have the minimum
     * age */
    uint8_t rating;
} __attribute__((packed));

struct MpegTSParentalRatingDescriptor {
    int nb_descriptions;
    MpegTSParentalRatingDescription **descriptions;
} __attribute__((packed));

char* mpegts_parental_rating_min_age(MpegTSParentalRatingDescription * const);

#endif /* AVFORMAT_MPEGTS_DESCRIPTORS_PARENTAL_RATING_H */
