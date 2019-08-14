#ifndef AVUTIL_DVB_H
#define AVUTIL_DVB_H

#include <stdint.h>
#include <stddef.h>

typedef struct DvbSectionHeader {
    uint8_t tid;
    uint16_t id;
    uint8_t version;
    uint8_t sec_num;
    uint8_t last_sec_num;
} DvbSectionHeader;

int avpriv_dvb_get8 (const uint8_t **pp, const uint8_t *p_end);
int avpriv_dvb_get16(const uint8_t **pp, const uint8_t *p_end);
int avpriv_dvb_get32(const uint8_t **pp, const uint8_t *p_end);

int avpriv_dvb_parse_section_header(DvbSectionHeader *h, const uint8_t **pp,
                                    const uint8_t *p_end);

#endif  /* AVUTIL_DVB_H */
