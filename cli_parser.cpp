#include "cli_parser.h"
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

// ---------------------------------------------------------------------------
struct option* getopt_get_long_options(getopt_arg_t* opt) {
    size_t count = 0;
    getopt_arg_t null_option = {0};
    do {
        if(!memcmp((void*)&opt[count], (void*)&null_option, sizeof(getopt_arg_t))) {
            break;
        } else {
            count++;
        }
    } while(1);

    struct option* gopt = (struct option*)malloc(sizeof(struct option) * count);
    if(!gopt) {
        return NULL;
    }
    for(size_t i = 0; i < count; i++) {
        gopt[i].name = opt[i].name;
        gopt[i].has_arg = opt[i].has_arg;
        gopt[i].flag = opt[i].flag;
        gopt[i].val = opt[i].val;
    }
    return gopt;
}


// ---------------------------------------------------------------------------
void show_usage(char *binary, getopt_arg_t* cli_options) {
    printf("USAGE\n  %s [options] \n\n", binary);
    getopt_arg_t null_option = {0};
    size_t count = 0;
    printf("\nOPTIONS\n");
    do {
        if (!memcmp((void *) &cli_options[count], (void *) &null_option, sizeof(getopt_arg_t))) {
            break;
        } else if (cli_options[count].description) {
            printf("  -%c%s%s%s %s%s%s%s\n     ",
                         cli_options[count].val,
                         cli_options[count].has_arg != no_argument ? " " : "",
                         cli_options[count].has_arg != no_argument ? cli_options[count].arg_name : "",
                         cli_options[count].name ? "," : "", cli_options[count].name ? "--" : "", cli_options[count].name,
                         cli_options[count].has_arg != no_argument ? "=" : "",
                         cli_options[count].has_arg != no_argument ? cli_options[count].arg_name : "");
            printf("%s", cli_options[count].description);
            printf("\n\n");
        }
        count++;
    } while (1);
    printf("\n");
    printf("EXAMPLE\n  cat file | %s.\n\n\n", binary);
    printf("AUTHORS\n  Written by Michael Schwarz.\n\n");
    printf("VERSION\n  %s\n\n", APPVERSION);
}


// ---------------------------------------------------------------------------
int parse_cli_options(Options& options, getopt_arg_t* cli_options, int argc, char* argv[]) {
    struct option *long_options = getopt_get_long_options((getopt_arg_t *) cli_options);
    int c;
    while ((c = getopt_long(argc, argv, ":s:o:chrn:xpdi:av", long_options, NULL)) != EOF) {
        switch (c) {
            case 's':
                options.seperator = std::string(optarg);
                break;
            case 'c':
                options.clear = 0;
                break;
            case 'h':
                show_usage(argv[0], cli_options);
                return 1;
            case 'r':
                options.reverse = 1;
                break;
            case 'x':
                options.order = 1;
                break;
            case 'p':
                options.percent = 1;
                break;
            case 'd':
                options.draw = 1;
                break;
            case 'i':
                options.interval = strtof(optarg, NULL);
                break;
            case 'n':
                options.number = strtoul(optarg, NULL, 0);
                break;
            case 'o':
                options.outfile = optarg;
                break;
            case 'a':
                options.atomic = 1;
                break;
            case 'v':
                options.version = 1;
                break;
            case ':':
                printf("Option -%c requires an argument.\n",
                             optopt);
                printf("\n");
                show_usage(argv[0], cli_options);
                return 1;
            case '?':
                if (isprint(optopt)) {
                    printf("Unknown option -%c.\n", optopt);
                } else {
                    printf("Unknown option character \\x%x.\n",
                                 optopt);
                }
                printf("\n");
                show_usage(argv[0], cli_options);
                return 1;
            default:
                show_usage(argv[0], cli_options);
                return 1;
        }
    }

    free(long_options);
    return 0;
}
