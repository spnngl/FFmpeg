#ifndef AVFORMAT_MPEGTS_DESCRIPTORS_TIME_SHIFTED_EVENT_H
#define AVFORMAT_MPEGTS_DESCRIPTORS_TIME_SHIFTED_EVENT_H

#include <stdint.h>

struct MpegTSTimeShiftedEventDescriptor {
    uint16_t reference_service_id;
    uint16_t reference_event_id;
};
typedef struct MpegTSTimeShiftedEventDescriptor MpegTSTimeShiftedEventDescriptor;

#endif /* AVFORMAT_MPEGTS_DESCRIPTORS_TIME_SHIFTED_EVENT_H */
