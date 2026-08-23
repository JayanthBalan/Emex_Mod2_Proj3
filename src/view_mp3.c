
#include "tag_reader.h"
#include "enc_dec.h"

#include "tag_reader.h"

int viewOperation(const char *filename) {
    FILE *fp = fopen(filename, "rb");
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

    unsigned char ver = 48 + (unsigned char)tag.tag_header.version;

    fprintf(stdout, "%s: MP3 TAG Reader for ID3v2.%c\n", __FILE__, ver);

    fprintf(stdout, "%s: Title = %s\n", __FILE__, (const char*)(tag.frames[TITLE_FRAME].frame_data + 1));
    fprintf(stdout, "%s: Artist = %s\n", __FILE__, (const char*)(tag.frames[ARTIST_FRAME].frame_data + 1));
    fprintf(stdout, "%s: Album = %s\n", __FILE__, (const char*)(tag.frames[ALBUM_FRAME].frame_data + 1));
    fprintf(stdout, "%s: Year = %s\n", __FILE__, (const char*)(tag.frames[YEAR_FRAME].frame_data + 1));
    fprintf(stdout, "%s: Content = %s\n", __FILE__, (const char*)(tag.frames[CONTENT_FRAME].frame_data + 1));

    uint8_t *fr_dat = tag.frames[COMMENT_FRAME].frame_data;
    uint8_t enc_type = fr_dat[0];

    fr_dat += 1 + COMMENT_FRAME_DATA_LANGUAGE_LEN;
    if(enc_type == 0x01) {
        fr_dat += COMMENT_FRAME_DATA_BOM_LEN;
    }
    for(; *fr_dat != 0x00; fr_dat++);
    fr_dat++;
    fprintf(stdout, "%s: Comment = %s\n", __FILE__, (const char*)fr_dat);

    free(tag.frames[TITLE_FRAME].frame_data);
    free(tag.frames[ARTIST_FRAME].frame_data);
    free(tag.frames[ALBUM_FRAME].frame_data);
    free(tag.frames[YEAR_FRAME].frame_data);
    free(tag.frames[CONTENT_FRAME].frame_data);
    free(tag.frames[COMMENT_FRAME].frame_data);

    fclose(fp);

    return 0;
}