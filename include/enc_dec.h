
#ifndef ENC_DEC_H
#define ENC_DEC_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include "tag_reader.h"

void utf16_iso(uint8_t*, size_t);
int8_t iso_utf16(mp3_tag_t*, frame_type_e);

#endif
