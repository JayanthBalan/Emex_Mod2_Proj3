
#!/bin/bash

# Test Configuration
out_file="./mp3_tag_reader"
file_path="target_files/"

input_file="sample.mp3"

title_data="Persephones's Song"
album_data="Red Rising"
year_data="2526"
content_data="Melody"
comment_data="Rising Song"
artist_data="EO Au Lykos"

view_command="-v"
modify_command="-e"

title_command="-t"
album_command="-A"
year_command="-y"
content_command="-m"
comment_command="-c"
artist_command="-a"

help_command="--help"

view_userArguments() {
    "$out_file" "$@"
}

modify_userArguments() {
    "$out_file" "$@"
}

help_default() {
    "$out_file" "$help_command"
}

view_default() {
    "$out_file" "$view_command" "${file_path}${input_file}"
}

modify_title_default() {
    "$out_file" "$modify_command" "$title_command" "$title_data" "${file_path}${input_file}"
}

modify_album_default() {
    "$out_file" "$modify_command" "$album_command" "$album_data" "${file_path}${input_file}"
}

modify_year_default() {
    "$out_file" "$modify_command" "$year_command" "$year_data" "${file_path}${input_file}"
}

modify_content_default() {
    "$out_file" "$modify_command" "$content_command" "$content_data" "${file_path}${input_file}"
}

modify_comment_default() {
    "$out_file" "$modify_command" "$comment_command" "$comment_data" "${file_path}${input_file}"
}

modify_artist_default() {
    "$out_file" "$modify_command" "$artist_command" "$artist_data" "${file_path}${input_file}"
}

validate_default() {

    echo "View operation"
    view_default || return 1

    echo
    echo "Modify title operation"
    modify_title_default || return 1

    echo
    echo "Modify album operation"
    modify_album_default || return 1

    echo
    echo "Modify year operation"
    modify_year_default || return 1

    echo
    echo "Modify content operation"
    modify_content_default || return 1

    echo
    echo "Modify comment operation"
    modify_comment_default || return 1

    echo
    echo "Modify artist operation"
    modify_artist_default || return 1

    echo
    echo "Help operation"
    help_default || return 1

    echo "View operation"
    view_default || return 1

    return 0
}

if [ $# -eq 1 ] && [ "$1" = "clean" ]
then
    echo "Cleanse work files"
    exit 0
fi

make clean
make || exit 1

validate_default
if [ $? -ne 0 ]
then
    echo "run: Script Failed"
    exit 1
fi

echo
echo "run: Successfully tested MP3 Tag Reader project"
exit 0
