
#ifndef TAG_READER_H
#define TAG_READER_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#define TAG_ID_LEN 3
#define TAG_HEADER_LEN 10
#define TAG_SIZE_LEN 4

#define FRAME_CNT 6
#define FRAME_ID_LEN 4
#define FRAME_FLAG_LEN 2
#define FRAME_SIZE_LEN 4
#define FRAME_HEADER_LEN 10
#define COMMENT_FRAME_DATA_LANGUAGE_LEN 3
#define COMMENT_FRAME_DATA_BOM_LEN 2

typedef enum _mp3_actions {
    view_details,
    modify_title_tag,
    modify_album_tag,
    modify_year_tag,
    modify_content_type_tag,
    modify_comment_tag,
    modify_artist_tag,
    help_option,
    invalid_option
} mp3_actions_e;

typedef enum _frame_type {
    TITLE_FRAME,
    ARTIST_FRAME,
    ALBUM_FRAME,
    YEAR_FRAME,
    CONTENT_FRAME,
    COMMENT_FRAME,
    NON_TARGET_FRAME
} frame_type_e;

typedef struct _tag_header {
    uint8_t tag_id[TAG_ID_LEN];
    uint8_t version;
    uint8_t revision;
    uint8_t flags;
    uint32_t tag_size;
} tag_header_t;

typedef struct _frame {
    uint8_t frame_id[FRAME_ID_LEN];
    uint32_t size;
    uint16_t flags;

    uint32_t frame_offset;
    uint32_t data_offset;

    uint8_t *frame_data; 
} frame_t;

typedef struct _mp3_tag {
    tag_header_t tag_header;
    frame_t frames[FRAME_CNT];
} mp3_tag_t;

int masterModifyOperation(const char*, const char*, mp3_actions_e);
int viewOperation(const char*);
int8_t parseMaster(FILE*, mp3_tag_t*);
int8_t encode_data(mp3_tag_t*, frame_type_e);

#endif
