
#include "tag_reader.h"

#define HELP_CMD1 "-h"
#define HELP_CMD2 "--help"
#define VIEW_CMD "-v"
#define MODTIT_CMD "-t"
#define MODALB_CMD "-A"
#define MODIFY_CMD "-e"
#define MODYEAR_CMD "-y"
#define MODCON_CMD "-m"
#define MODCOMM_CMD "-c"
#define MODART_CMD "-a"

typedef enum _cla_arg_cnts {
    min_args = 2,
    help_args = 2,
    view_args = 3,
    modify_args = 5
} cla_args_cnts_e;

static mp3_actions_e CLA_Processor(int, const char**, const char**, const char**);
static void helpOperation(void);
static mp3_actions_e commandCMP2(const char*);

int main(int argc, char *argv[]) {

    const char *new_data, *file_name;
    mp3_actions_e ret_cla = CLA_Processor(argc, argv, &new_data, &file_name);
    switch(ret_cla) {
        case modify_title_tag:
        case modify_artist_tag:
        case modify_album_tag:
        case modify_content_type_tag:
        case modify_year_tag:
        case modify_comment_tag:
            fprintf(stdout, "%s: Modify Operation selected\n\n", __FILE__);
            return masterModifyOperation(new_data, file_name, ret_cla);
        case view_details:
            fprintf(stdout, "%s: View Operation selected\n\n", __FILE__);
            return viewOperation(file_name);
        case help_option:
            fprintf(stdout, "%s: Help operation selected\n\n", __FILE__);
            helpOperation();
            break;
        case invalid_option:
        default:
            fprintf(stdout, "%s: Invalid Command\n", __FILE__);
            fprintf(stdout, "%s: To view data, the command is \"./tag_reader -v filename.mp3\"\n\n", __FILE__);
            fprintf(stdout, "%s: To modify data, the command is \"./tag_reader -x \"new data here\" filename.mp3\" where x is any valid modify command option\n\n", __FILE__);
            fprintf(stdout, "%s: To receive help, the command is \"./tag_reader --help\" or \"./tag_reader -h\"\n\n", __FILE__);
    }

    return 0;
}

static mp3_actions_e CLA_Processor(int argc, const char** argv, const char** buffer, const char** file) {
    if(argc < min_args) {
        return invalid_option;
    }

    const char *cmd_option = argv[1];

    if(cmd_option[0] != '-') {
        return invalid_option;
    }

    if(strcmp(cmd_option, HELP_CMD1) == 0 || strcmp(cmd_option, HELP_CMD2) == 0) {
        if(argc != help_args) {
            return invalid_option;
        }

        return help_option;
    }
    if(strcmp(cmd_option, VIEW_CMD) == 0) {
        if(argv[2] == NULL || argc != view_args) {
            return invalid_option;
        }
        *file = argv[2];
        return view_details;
    }

    if(strcmp(cmd_option, MODIFY_CMD) == 0) {
        if(argc != modify_args || argv[2] == NULL || argv[3] == NULL || argv[4] == NULL) {
            return invalid_option;
        }

        *buffer = argv[3];
        *file = argv[4];
        return commandCMP2(argv[2]);
    }

    return invalid_option;
}

static mp3_actions_e commandCMP2(const char* cmd) {
    if(strcmp(cmd, MODALB_CMD) == 0) {
        return modify_album_tag;
    }
    if(strcmp(cmd, MODART_CMD) == 0) {
        return modify_artist_tag;
    }
    if(strcmp(cmd, MODCOMM_CMD) == 0) {
        return modify_comment_tag;
    }
    if(strcmp(cmd, MODCON_CMD) == 0) {
        return modify_content_type_tag;
    }
    if(strcmp(cmd, MODTIT_CMD) == 0) {
        return modify_title_tag;
    }
    if(strcmp(cmd, MODYEAR_CMD) == 0) {
        return modify_year_tag;
    }
    return invalid_option;
}

static void helpOperation(void) {
    fprintf(stdout, "%s: Help Menu\n", __FILE__);
    fprintf(stdout, "%s: 1. \"-v\" -> view mp3 file contents\n", __FILE__);
    fprintf(stdout, "%s: 2. \"-e\" -> modify mp3 file contents\n", __FILE__);
    fprintf(stdout, "%s: \t2.1. \"-t\" -> modify song title\n", __FILE__);
    fprintf(stdout, "%s: \t2.2. \"-A\" -> modify album name\n", __FILE__);
    fprintf(stdout, "%s: \t2.3. \"-y\" -> modify year\n", __FILE__);
    fprintf(stdout, "%s: \t2.4. \"-m\" -> modify content type\n", __FILE__);
    fprintf(stdout, "%s: \t2.5. \"-c\" -> modify comment\n", __FILE__);
    fprintf(stdout, "%s: \t2.6. \"-a\" -> modify artist name\n\n", __FILE__);
}
