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
