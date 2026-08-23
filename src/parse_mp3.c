
#include "tag_reader.h"
#include "enc_dec.h"

#define _FRAME_ID_TITLE "TIT2"
#define _FRAME_ID_ARTIST "TPE1"
#define _FRAME_ID_ALBUM "TALB"
#define _FRAME_ID_YEAR "TYER"
#define _FRAME_ID_CONTENT "TCON"
#define _FRAME_ID_COMMENT "COMM"

static int8_t parseTagHeader(FILE*, mp3_tag_t*);
static uint32_t decode_synchsafe(const uint8_t[]);
static frame_type_e identify_frame(uint8_t*);
static int8_t decode_data(mp3_tag_t*, frame_type_e);

int8_t parseMaster(FILE *filepointer, mp3_tag_t *mp3) {
    if(parseTagHeader(filepointer, mp3) == -1) {
        return -1;
    }

    if(fseek(filepointer, TAG_HEADER_LEN, SEEK_SET) != 0) {
        fprintf(stderr, "%s: File Seek Error\n", __FILE__);
        return -1;
    }

    size_t tot_bytes_consumed = 0, bytes_read;
    uint8_t frames_finished = 0;
    uint8_t buffer_frame[FRAME_HEADER_LEN];

    while(tot_bytes_consumed < mp3->tag_header.tag_size && frames_finished < FRAME_CNT) {
        if((bytes_read = fread(buffer_frame, 1, FRAME_HEADER_LEN, filepointer)) != FRAME_HEADER_LEN) {
            fprintf(stderr, "%s: Parse Frame Header Error\n", __FILE__);
            return -1;
        }

        if(buffer_frame[1] == 0 && buffer_frame[2] == 0 && buffer_frame[3] == 0 && buffer_frame[0] == 0) {
            fprintf(stdout, "%s: End Of Tag Reached\n", __FILE__);
            break;
        }

        tot_bytes_consumed += bytes_read;

        frame_type_e type_idx = identify_frame(buffer_frame);

        if(type_idx == NON_TARGET_FRAME) {
            uint32_t frame_size = ((uint32_t)buffer_frame[FRAME_ID_LEN] << 24) | ((uint32_t)buffer_frame[FRAME_ID_LEN + 1] << 16) |
                ((uint32_t)buffer_frame[FRAME_ID_LEN + 2] << 8) | (uint32_t)buffer_frame[FRAME_ID_LEN + 3];

            tot_bytes_consumed += frame_size;

            if(fseek(filepointer, frame_size, SEEK_CUR) != 0) {
                fprintf(stderr, "%s: File Seek Error\n", __FILE__);
                return -1;
            }
            continue;
        }

        memcpy(mp3->frames[type_idx].frame_id, buffer_frame, FRAME_ID_LEN);
        mp3->frames[type_idx].size = ((uint32_t)buffer_frame[FRAME_ID_LEN] << 24) | ((uint32_t)buffer_frame[FRAME_ID_LEN + 1] << 16) |
            ((uint32_t)buffer_frame[FRAME_ID_LEN + 2] << 8) | (uint32_t)buffer_frame[FRAME_ID_LEN + 3];
        mp3->frames[type_idx].flags = ((uint16_t)buffer_frame[FRAME_SIZE_LEN + FRAME_ID_LEN] << 8) |(uint16_t)buffer_frame[FRAME_SIZE_LEN + FRAME_ID_LEN + 1];
        mp3->frames[type_idx].frame_offset = ftell(filepointer) - FRAME_HEADER_LEN;
        mp3->frames[type_idx].data_offset = ftell(filepointer);
        mp3->frames[type_idx].frame_data = malloc(mp3->frames[type_idx].size);

        if(mp3->frames[type_idx].frame_data == NULL) {
            fprintf(stderr, "%s: Heap Allocation Error\n", __FILE__);
            return -1;
        }

        if((bytes_read = fread(mp3->frames[type_idx].frame_data, 1, mp3->frames[type_idx].size, filepointer)) != mp3->frames[type_idx].size) {
            fprintf(stderr, "%s: Parse Frame Data Error\n", __FILE__);
            return -1;
        }

        tot_bytes_consumed += bytes_read;
        frames_finished++;

        if(decode_data(mp3, type_idx) == -1) {
            fprintf(stderr, "%s: Decode Frame Data Error\n", __FILE__);
            return -1;
        }
    }

    return 0;
}

static int8_t decode_data(mp3_tag_t *tag, frame_type_e idx) {
    uint8_t *data = tag->frames[idx].frame_data;
    uint8_t enc_type = *data;
    uint32_t len = tag->frames[idx].size;

    if(enc_type == 0x00) {
        return 0;
    }

    if(enc_type == 0x01) {
        uint8_t *bom = tag->frames[idx].frame_data + 1;

        if(bom[0] == 0xFE && bom[1] == 0xFF) {
            fprintf(stdout, "%s: Unsupported Endianness\n", __FILE__);
            return -1;
        }

        if(bom[0] != 0xFF || bom[1] != 0xFE) {
            fprintf(stdout, "%s: Malformed UTF16 BOM\n", __FILE__);
            return -1;
        }

        if(idx == COMMENT_FRAME) {
            data += 1 + COMMENT_FRAME_DATA_LANGUAGE_LEN + COMMENT_FRAME_DATA_BOM_LEN;
            len -= 1 + COMMENT_FRAME_DATA_LANGUAGE_LEN + COMMENT_FRAME_DATA_BOM_LEN;

            for(; len >= 2; data += 2, len -= 2) {
                if(data[0] == 0x00 && data[1] == 0x00) {
                    break;
                }
            }
            if(len < 2) {
                return -1;
            }

            data += 2;
            len -= 2;
        }
        else {
            data += 1 + COMMENT_FRAME_DATA_BOM_LEN;
            len -= 1 + COMMENT_FRAME_DATA_BOM_LEN;
        }

        utf16_iso(data, len);
        return 0;
    }

    return -1;
}

int8_t encode_data(mp3_tag_t *tag, frame_type_e idx) {
    uint8_t *data = tag->frames[idx].frame_data;
    uint8_t enc_type = *data;

    if(enc_type == 0x00) {
        return 0;
    }

    if(enc_type == 0x01) {
        uint8_t *bom = tag->frames[idx].frame_data + 1;

        if(bom[0] == 0xFE && bom[1] == 0xFF) {
            fprintf(stdout, "%s: Unsupported Endianness\n", __FILE__);
            return -1;
        }
        if(bom[0] != 0xFF || bom[1] != 0xFE) {
            fprintf(stdout, "%s: Malformed UTF16 BOM\n", __FILE__);
            return -1;
        }

        return iso_utf16(tag, idx);
    }

    return -1;
}

static frame_type_e identify_frame(uint8_t *buffer) {
    if(memcmp(buffer, _FRAME_ID_TITLE, FRAME_ID_LEN) == 0) {
        return TITLE_FRAME;
    }
    else if(memcmp(buffer, _FRAME_ID_ARTIST, FRAME_ID_LEN) == 0) {
        return ARTIST_FRAME;
    }
    else if(memcmp(buffer, _FRAME_ID_ALBUM, FRAME_ID_LEN) == 0) {
        return ALBUM_FRAME;
    }
    else if(memcmp(buffer, _FRAME_ID_YEAR, FRAME_ID_LEN) == 0) {
        return YEAR_FRAME;
    }
    else if(memcmp(buffer, _FRAME_ID_CONTENT, FRAME_ID_LEN) == 0) {
        return CONTENT_FRAME;
    }
    else if(memcmp(buffer, _FRAME_ID_COMMENT, FRAME_ID_LEN) == 0) {
        return COMMENT_FRAME;
    }
    else {
        return NON_TARGET_FRAME;
    }

    return NON_TARGET_FRAME;
}

static uint32_t decode_synchsafe(const uint8_t buf[]) {
    uint32_t res = 0;

    res |= (uint32_t)(buf[0] & 0x7F) << 21;
    res |= (uint32_t)(buf[1] & 0x7F) << 14;
    res |= (uint32_t)(buf[2] & 0x7F) << 7;
    res |= (uint32_t)(buf[3] & 0x7F);

    return res;
}

static int8_t parseTagHeader(FILE *filepointer, mp3_tag_t *mp3) {
    if(fread(mp3->tag_header.tag_id, 1, TAG_ID_LEN, filepointer) != TAG_ID_LEN) {
        fprintf(stderr, "%s: Parse TAG_ID Error\n", __FILE__);
        return -1;
    }

    if(fread(&mp3->tag_header.version, 1, 1, filepointer) != 1) {
        fprintf(stderr, "%s: Parse Version Error\n", __FILE__);
        return -1;
    }

    if(memcmp(mp3->tag_header.tag_id, "ID3", 3) != 0) {
        fprintf(stderr, "%s: Unsupported tag\n", __FILE__);
        return -1;
    }

    if(mp3->tag_header.version != 3) {
        fprintf(stderr, "%s: Unsupported ID3 version\n", __FILE__);
        return -1;
    }

    if(fread(&mp3->tag_header.revision, 1, 1, filepointer) != 1) {
        fprintf(stderr, "%s: Parse Revision Error\n", __FILE__);
        return -1;
    }

    if(fread(&mp3->tag_header.flags, 1, 1, filepointer) != 1) {
        fprintf(stderr, "%s: Parse Flags Error\n", __FILE__);
        return -1;
    }

    if(mp3->tag_header.flags != 0) {
        fprintf(stderr, "%s: Unsupported ID3 Tag Flags\n", __FILE__);
        return -1;
    }

    uint8_t tag_size_buffer[TAG_SIZE_LEN];

    if(fread(tag_size_buffer, 1, TAG_SIZE_LEN, filepointer) != TAG_SIZE_LEN) {
        fprintf(stderr, "%s: Parse Tag_size Error\n", __FILE__);
        return -1;
    }

    mp3->tag_header.tag_size = decode_synchsafe(tag_size_buffer);

    return 0;
}
