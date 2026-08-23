
# Compiler
CC = gcc

# Compiler and Preprocessor flags
CFLAGS = -g -Wall -Wextra -Werror -std=c11 -O2
CPPFLAGS = -I$(HFILE)

# Addresses
CFILE = src
HFILE = include
TFILE = target_files

all: mp3_tag_reader

# Linker
mp3_tag_reader: main_mp3.o enc_dec.o modify_mp3.o view_mp3.o parse_mp3.o
	$(CC) main_mp3.o enc_dec.o modify_mp3.o view_mp3.o parse_mp3.o -o mp3_tag_reader

# Preprocessor -> Compiler -> Assembler
main_mp3.o: $(CFILE)/main_mp3.c $(HFILE)/enc_dec.h $(HFILE)/tag_reader.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $(CFILE)/main_mp3.c -o main_mp3.o

enc_dec.o: $(CFILE)/enc_dec.c $(HFILE)/enc_dec.h $(HFILE)/tag_reader.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $(CFILE)/enc_dec.c -o enc_dec.o

modify_mp3.o: $(CFILE)/modify_mp3.c $(HFILE)/tag_reader.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $(CFILE)/modify_mp3.c -o modify_mp3.o

view_mp3.o: $(CFILE)/view_mp3.c $(HFILE)/tag_reader.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $(CFILE)/view_mp3.c -o view_mp3.o

parse_mp3.o: $(CFILE)/parse_mp3.c $(HFILE)/enc_dec.h $(HFILE)/tag_reader.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $(CFILE)/parse_mp3.c -o parse_mp3.o

.PHONY: clean all

clean:
	rm -f *.o mp3_tag_reader
