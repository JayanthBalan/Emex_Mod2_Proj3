
#include "enc_dec.h"
#include "tag_reader.h"

static inline uint16_t uint8_uint16(uint8_t, uint8_t);

void utf16_iso(uint8_t *dat, size_t len) {
    size_t i = 0, j = 0;
    uint8_t *src = dat;
    uint8_t *dest = dat;

    for(; i < len - 1; i++, j += 2) {
        uint16_t value = uint8_uint16(src[j], src[j + 1]);

        if(value == 0x0000) {
            break;
        }

        if(value > 0x00FF) {
            dest[i] = '+';
        }
        else {
            dest[i] = (uint8_t)(value & 0xFF);
        }
    }
    dest[i] = 0x00;
}

int8_t iso_utf16(mp3_tag_t *mp3, frame_type_e index) {
    size_t i = 0, j = 0, len = mp3->frames[index].size;
    uint8_t *src = mp3->frames[index].frame_data;
    uint8_t enc_type = *src;

    if(enc_type == 0x01) {
        uint8_t *bom = mp3->frames[index].frame_data + 1;
        if(bom[0] == 0xFE && bom[1] == 0xFF) {
            fprintf(stdout, "%s: Unsupported Endianness\n", __FILE__);
            return -1;
        }
        if(bom[0] != 0xFF || bom[1] != 0xFE) {
            fprintf(stdout, "%s: Malformed UTF16 BOM\n", __FILE__);
            return -1;
        }
    }

    if(index == COMMENT_FRAME) {
        src += COMMENT_FRAME_DATA_LANGUAGE_LEN + COMMENT_FRAME_DATA_BOM_LEN + 1;
        len -= COMMENT_FRAME_DATA_LANGUAGE_LEN + COMMENT_FRAME_DATA_BOM_LEN + 1;

        for(; len >= 2; src += 2, len -= 2) {
            if(src[0] == 0x00 && src[1] == 0x00) {
                break;
            }
        }

        src += 2;
        len -= 2;
    }
    else {
        src += 1 + COMMENT_FRAME_DATA_BOM_LEN;
        len -= 1 + COMMENT_FRAME_DATA_BOM_LEN;

        for(; len >= 2; src += 2, len -= 2) {
            if(src[0] == 0x00 && src[1] == 0x00) {
                break;
            }
        }

        len -= 2;
    }

    uint8_t *dest = malloc((len * 2) + 2);
    if(dest == NULL) {
        fprintf(stderr, "%s: Heap Allocation Error\n", __FILE__);
        return -1;
    }

    for(; i < len; i++, j += 2) {
        dest[j] = src[i];
        dest[j + 1] = 0x00;
    }

    dest[j] = 0x00;
    dest[j + 1] = 0x00;

    memcpy(src, dest, j + 2);
    free(dest);
    dest = NULL;

    return 0;
}

static inline uint16_t uint8_uint16(uint8_t x, uint8_t y) {
    return (uint16_t)(x) | ((uint16_t)y << 8);
}
