
#include "tag_reader.h"

static int8_t get_data_offset(mp3_tag_t*, frame_type_e, size_t*);
static int8_t write_frame_data(FILE*, mp3_tag_t*, frame_type_e);

int masterModifyOperation(char *mod_dat, const char *filename, mp3_actions_e opera_type) {
    FILE *fp = fopen(filename, "r+b");
    if(fp == NULL) {
        fprintf(stderr, "%s: File Pointer Error\n", __FILE__);
        return -1;
    }
    mp3_tag_t tag;
    int ret = (int)parseMaster(fp, &tag);
    if(ret == -1) {
        fclose(fp);
        return -1;
    }

    frame_type_e frame_idx;

    switch(opera_type) {
        case modify_title_tag:
            frame_idx = TITLE_FRAME;
            break;
        case modify_artist_tag:
            frame_idx = ARTIST_FRAME;
            break;
        case modify_album_tag:
            frame_idx = ALBUM_FRAME;
            break;
        case modify_year_tag:
            frame_idx = YEAR_FRAME;
            break;
        case modify_content_type_tag:
            frame_idx = CONTENT_FRAME;
            break;
        case modify_comment_tag:
            frame_idx = COMMENT_FRAME;
            break;
        default:
            fprintf(stderr, "%s: Invalid Modify Operation\n", __FILE__);
            fclose(fp);
            return -1;
    }

    size_t data_offset;
    if(get_data_offset(&tag, frame_idx, &data_offset) == -1) {
        fprintf(stderr, "%s: Frame Data Offset Error\n", __FILE__);
        fclose(fp);
        return -1;
    }

    size_t new_data_len = strlen(mod_dat);
    size_t available_len = tag.frames[frame_idx].size - data_offset;

    if(new_data_len + 1 > available_len) {
        fprintf(stderr, "%s: New Data Size Exceeds Limit\n", __FILE__);
        fclose(fp);
        return -1;
    }

    memset(tag.frames[frame_idx].frame_data + data_offset, 0, available_len);
    memcpy(tag.frames[frame_idx].frame_data + data_offset, mod_dat, new_data_len);

    if(encode_data(&tag, frame_idx) == -1) {
        fprintf(stderr, "%s: Encode Frame Data Error\n", __FILE__);
        fclose(fp);
        return -1;
    }

    if(write_frame_data(fp, &tag, frame_idx) == -1) {
        fprintf(stderr, "%s: Write Frame Data Error\n", __FILE__);
        fclose(fp);
        return -1;
    }

    free(tag.frames[TITLE_FRAME].frame_data);
    free(tag.frames[ARTIST_FRAME].frame_data);
    free(tag.frames[ALBUM_FRAME].frame_data);
    free(tag.frames[YEAR_FRAME].frame_data);
    free(tag.frames[CONTENT_FRAME].frame_data);
    free(tag.frames[COMMENT_FRAME].frame_data);
    fclose(fp);

    return 0;
}

static int8_t get_data_offset(mp3_tag_t *tag, frame_type_e idx, size_t *offset) {
    uint8_t *data = tag->frames[idx].frame_data;
    uint8_t enc_type = data[0];

    if(enc_type == 0x00) {
        if(idx == COMMENT_FRAME) {
            data += 1 + COMMENT_FRAME_DATA_LANGUAGE_LEN;

            while(*data != 0x00) {
                data++;
            }

            data++;
            *offset = (size_t)(data - tag->frames[idx].frame_data);
        }
        else {
            *offset = 1;
        }

        return 0;
    }

    if(enc_type == 0x01) {
        if(data[1] == 0xFE && data[2] == 0xFF) {
            fprintf(stderr, "%s: Unsupported Endianness\n", __FILE__);
            return -1;
        }

        if(data[1] != 0xFF || data[2] != 0xFE) {
            fprintf(stderr, "%s: Malformed UTF16 BOM\n", __FILE__);
            return -1;
        }

        if(idx == COMMENT_FRAME) {
            data += 1 + COMMENT_FRAME_DATA_LANGUAGE_LEN + COMMENT_FRAME_DATA_BOM_LEN;

            while(data[0] != 0x00 || data[1] != 0x00) {
                data += 2;
            }

            data += 2;
            *offset = (size_t)(data - tag->frames[idx].frame_data);
        }
        else {
            *offset = 1 + COMMENT_FRAME_DATA_BOM_LEN;
        }

        return 0;
    }

    return -1;
}

static int8_t write_frame_data(FILE *fp, mp3_tag_t *tag, frame_type_e idx) {
    if(fseek(fp, tag->frames[idx].data_offset, SEEK_SET) != 0) {
        return -1;
    }

    if(fwrite(tag->frames[idx].frame_data, 1, tag->frames[idx].size, fp) != tag->frames[idx].size) {
        return -1;
    }

    return 0;
}
