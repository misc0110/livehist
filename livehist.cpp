#include <iostream>
#include <string>
#include <map>
#include <set>
#include <functional>
#include <algorithm> 
#include <cctype>
#include <locale>
#include <vector>
#include <chrono>
#include <memory.h>
#include <unistd.h>
#include <getopt.h>
#include "getopt_helper.h"

static getopt_arg_t options[] =
        {
                {"separator", required_argument, NULL, 's', "Set the seperator between symbol and count (default: \" :: \")", "SEPERATOR"},
                {"number",    required_argument, NULL, 'n', "Display only top NUMBER entries (default: all)", "NUMBER"},
                {"no-clear",  no_argument,       NULL, 'c', "Disable screen clear.", NULL},
                {"reverse",   no_argument,       NULL, 'r', "Short ascending.",      NULL},
                {"percent",   no_argument,       NULL, 'p', "Show frequency as percentage.", NULL},
                {"draw",      no_argument,       NULL, 'd', "Draw bar for visualization.", NULL},
                {"interval",  required_argument, NULL, 'i', "Refresh interval (default: 0).", NULL},
                {"switch",    no_argument,       NULL, 'x', "Show count first, then value.", NULL},
                {"help",      no_argument,       NULL, 'h', "Show this help.",       NULL},
                {NULL, 0,                        NULL, 0, NULL,                      NULL}
        };

        
void show_usage(char *binary) {
    printf("USAGE\n  %s [options] \n\n", binary);
    getopt_arg_t null_option = {0};
    size_t count = 0;
    printf("\nOPTIONS\n");
    do {
        if (!memcmp((void *) &options[count], (void *) &null_option, sizeof(getopt_arg_t))) {
            break;
        } else if (options[count].description) {
            printf("  -%c%s%s%s %s%s%s%s\n     ",
                         options[count].val,
                         options[count].has_arg != no_argument ? " " : "",
                         options[count].has_arg != no_argument ? options[count].arg_name : "",
                         options[count].name ? "," : "", options[count].name ? "--" : "", options[count].name,
                         options[count].has_arg != no_argument ? "=" : "",
                         options[count].has_arg != no_argument ? options[count].arg_name : "");
            printf("%s", options[count].description);
            printf("\n\n");
        }
        count++;
    } while (1);
    printf("\n");
    printf("EXAMPLE\n  cat file | %s.\n\n\n", binary);
    printf("AUTHORS\n  Written by Michael Schwarz.\n\n");

}

static inline void trim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
        return !std::isspace(ch);
    }));
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

std::map<std::string, size_t> hist;
std::vector<std::pair<std::string, size_t>> pairs;

bool sorter(std::pair<std::string, size_t>& elem1, std::pair<std::string, size_t>& elem2) {
    return elem1.second > elem2.second;
}
bool rev_sorter(std::pair<std::string, size_t>& elem1, std::pair<std::string, size_t>& elem2) {
    return elem1.second < elem2.second;
}

size_t update(std::string& line) {
    // update histogram
    trim(line);
    if(hist.find(line) != hist.end()) {
        hist[line]++;
    } else {
        hist[line] = 1;
    }

    size_t sum = 0;
    pairs.clear();
    for (auto itr = hist.begin(); itr != hist.end(); ++itr) {
        pairs.push_back(*itr);
        sum += itr->second;
    }
    return sum;
}

int main(int argc, char* argv[]) {
    std::string seperator = " :: ";
    int clear = 1, reverse = 0, order = 0, percent = 0, draw = 0;
    float interval = 0;
    size_t number = 0, last_update = 0;
    
    struct option *long_options = getopt_get_long_options((getopt_arg_t *) options);
    int c;
    while ((c = getopt_long(argc, argv, ":s:chrn:xpdi:", long_options, NULL)) != EOF) {
        switch (c) {
            case 's':
                seperator = std::string(optarg);
                break;
            case 'c':
                clear = 0;
                break;
            case 'h':
                show_usage(argv[0]);
                return 0;
            case 'r':
                reverse = 1;
                break;
            case 'x':
                order = 1;
                break;
            case 'p':
                percent = 1;
                break;
            case 'd':
                draw = 1;
                break;
            case 'i':
                interval = strtof(optarg, NULL);
                break;
            case 'n':
                number = strtoul(optarg, NULL, 0);
                break;
            case ':':
                printf("Option -%c requires an argument.\n",
                             optopt);
                printf("\n");
                show_usage(argv[0]);
                return 1;
            case '?':
                if (isprint(optopt)) {
                    printf("Unknown option -%c.\n", optopt);
                } else {
                    printf("Unknown option character \\x%x.\n",
                                 optopt);
                }
                printf("\n");
                show_usage(argv[0]);
                return 1;
            default:
                show_usage(argv[0]);
                return 0;
        }
    }

    free(long_options);    
    
    for (std::string line; getline(std::cin, line);) {
        size_t sum = update(line);
        if(!sum) sum = 1;

        // sort bins
        sort(pairs.begin(), pairs.end(), reverse ? rev_sorter : sorter);
    
        // output
        size_t now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

        if(!interval || now - last_update >= interval * 1000) {
            last_update = now;

            if(clear) {
                std::cout << "\033[2J";
            } else {
                std::cout << std::endl;
            }

            size_t maxlen = 0;
            for(int output = 0; output < 2; output++) {
                char buffer[256], tmp[256];
                size_t limit = number;
                for (std::pair<std::string, size_t> element : pairs) {
                    if(number && !limit) {
                        break;
                    }
                    limit--;
                    size_t len = 0;
                    if(order) {
                        len += snprintf(buffer, sizeof(buffer), "%s%s", element.first.c_str(), seperator.c_str());
                        if(percent) len += snprintf(tmp, sizeof(tmp), "%f", element.second * 100.0 / sum);
                        else len += snprintf(tmp, sizeof(tmp), "%zd", element.second);
                        strncat(buffer, tmp, sizeof(buffer));
                    } else {
                        if(percent) len += snprintf(buffer, sizeof(buffer), "%f", element.second * 100.0 / sum);
                        else len += snprintf(buffer, sizeof(buffer), "%zd", element.second);
                        len += snprintf(tmp, sizeof(tmp), "%s%s", seperator.c_str(), element.first.c_str());
                        strncat(buffer, tmp, sizeof(buffer));
                    }
                    if(len > maxlen) maxlen = len;
    //                 snprintf(tmp, sizeof(tmp), "%s\n", buffer);
                    if(output) {
                        len = printf("%s", buffer);
                        if(draw) {
                            while(len++ < maxlen) {
                                printf(" ");
                            }
                            printf(" |");
                            for(unsigned int i = 0; i < ((120 - maxlen) * element.second) / sum; i++) printf("#");
                        }
                        printf("\n");
                    }
                }
            }
        }
    }
}
