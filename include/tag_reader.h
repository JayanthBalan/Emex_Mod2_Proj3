
#ifndef TAG_READER_H
#define TAG_READER_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>

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

typedef struct _tag_header {
    uint8_t identifier;
} tag_header_t;

int masterModifyOperation(char*, const char*, mp3_actions_e);
int viewOperation(const char*);

#endif
