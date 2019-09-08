#ifndef GETOPT_HELPER_H
#define GETOPT_HELPER_H

#include <string>
#include <getopt.h>

// ---------------------------------------------------------------------------
typedef struct {
    const char* name;
    int has_arg;
    int* flag;
    int val;
    const char* description;
    const char* arg_name;
} getopt_arg_t;


// ---------------------------------------------------------------------------
typedef struct {
    std::string seperator = " :: ";
    int clear = 1, reverse = 0, order = 0, percent = 0, draw = 0, atomic = 0;
    float interval = 0;
    size_t number = 0;
    char* outfile = NULL;
} Options;


// ---------------------------------------------------------------------------
struct option* getopt_get_long_options(getopt_arg_t* opt);
void show_usage(char *binary, getopt_arg_t* cli_options);
int parse_cli_options(Options& options, getopt_arg_t* cli_options, int argc, char* argv[]);

#endif
