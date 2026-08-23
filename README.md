
# MP3 Tag Reader

> MP3 Tag Reader is a C project that reads and modifies ID3 metadata stored in MP3 files.

## Features

- View MP3 tag information
- Modify MP3 tag information
- Supports ID3v2.3
- Supports the following frames:
  - `TIT2` - Title
  - `TPE1` - Artist
  - `TALB` - Album
  - `TYER` - Year
  - `TCON` - Content Type
  - `COMM` - Comment
- Supports ISO-8859-1 and UTF-16 encoded tag data
- Handles UTF-16 little-endian BOM
- Command-line interface
- Makefile based compilation

## Commands

./mp3_tag_reader -e -t "New Song" sample.mp3
./mp3_tag_reader -e -a "New Artist" sample.mp3
./mp3_tag_reader -e -A "New Album" sample.mp3
./mp3_tag_reader -e -y "2026" sample.mp3
./mp3_tag_reader -e -m "Rock" sample.mp3
./mp3_tag_reader -e -c "New Comment" sample.mp3
./mp3_tag_reader -h
./mp3_tag_reader --help
./mp3_tag_reader -v filename.mp3

### Build Options

make
make clean

### Run Options

bash run.bash
./run.bash
