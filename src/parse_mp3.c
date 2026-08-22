
#include "tag_reader.h"

static int8_t parseTagHeader(FILE*, mp3_tag_t*);
static int8_t parseFrameHeader(FILE*, mp3_tag_t*, frame_type_e);

int8_t parseMaster(FILE *filepointer, mp3_tag_t *mp3) {
    // Tag Header
    if(parseTagHeader(filepointer, mp3) == -1) {
        return -1;
    }

    // Title Frame
    if(parseFrameHeader(filepointer, mp3, TITLE_FRAME) == -1) {
        return -1;
    }

    // Artist Frame
    if(parseFrameHeader(filepointer, mp3, ARTIST_FRAME) == -1) {
        return -1;
    }

    // Album Frame
    if(parseFrameHeader(filepointer, mp3, ALBUM_FRAME) == -1) {
        return -1;
    }

    // Year Frame
    if(parseFrameHeader(filepointer, mp3, YEAR_FRAME) == -1) {
        return -1;
    }

    // Title Frame
    if(parseFrameHeader(filepointer, mp3, TITLE_FRAME) == -1) {
        return -1;
    }

    // Content Frame
    if(parseFrameHeader(filepointer, mp3, CONTENT_FRAME) == -1) {
        return -1;
    }

    // Comment Frame
    if(parseFrameHeader(filepointer, mp3, COMMENT_FRAME) == -1) {
        return -1;
    }

    return 0;
}

static int8_t parseFrameHeader(FILE *fp, mp3_tag_t *mp3, frame_type_e type) {
    

    return 0;
}

static int8_t parseTagHeader(FILE *filepointer, mp3_tag_t *mp3) {
    // TAG ID
    if(fread(&mp3->tag_header.tag_id, 1, TAG_ID_LEN, filepointer) < TAG_ID_LEN) {
        fprintf(stderr, "%s: Parse TAG_ID Error\n", __FILE__);
        return -1;
    }

    // Version
    if(fread(&mp3->tag_header.version, 1, 1, filepointer) < 1) {
        fprintf(stderr, "%s: Parse Version Error\n", __FILE__);
        return -1;
    }

    // Revision
    if(fread(&mp3->tag_header.revision, 1, 1, filepointer) < 1) {
        fprintf(stderr, "%s: Parse Revision Error\n", __FILE__);
        return -1;
    }

    // Flags
    if(fread(&mp3->tag_header.flags, 1, 1, filepointer) < 1) {
        fprintf(stderr, "%s: Parse Flags Error\n", __FILE__);
        return -1;
    }

    // Tag Size
    if(fread(&mp3->tag_header.tag_size, sizeof(uint32_t), 1, filepointer) < 1) {
        fprintf(stderr, "%s: Parse Tag_size Error\n", __FILE__);
        return -1;
    }

    return 0;
}
