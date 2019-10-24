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
#include <signal.h>
#include "cli_parser.h"

// ---------------------------------------------------------------------------
static getopt_arg_t cli_options[] =
        {
                {"separator", required_argument, NULL, 's', "Set the seperator between symbol and count (default: \" :: \")", "SEPERATOR"},
                {"number",    required_argument, NULL, 'n', "Display only top NUMBER entries (default: all)", "NUMBER"},
                {"no-clear",  no_argument,       NULL, 'c', "Disable screen clear.", NULL},
                {"reverse",   no_argument,       NULL, 'r', "Short ascending.",      NULL},
                {"percent",   no_argument,       NULL, 'p', "Show frequency as percentage.", NULL},
                {"draw",      no_argument,       NULL, 'd', "Draw bar for visualization.", NULL},
                {"interval",  required_argument, NULL, 'i', "Refresh interval in seconds (default: 0).", "INTERVAL"},
                {"switch",    no_argument,       NULL, 'x', "Show count first, then value.", NULL},
                {"outfile",   required_argument, NULL, 'o', "Output latest histogram to file FILE.", "FILE"},
                {"atomic",    no_argument,       NULL, 'a', "On Ctrl+C, application finishes printing the histogram.", NULL},
                {"help",      no_argument,       NULL, 'h', "Show this help.",       NULL},
                {NULL, 0,                        NULL, 0, NULL,                      NULL}
        };


// ---------------------------------------------------------------------------
static volatile int terminated = 0;

// ---------------------------------------------------------------------------
static void interruption_handler(int signum) {
  if(signum == SIGINT) {
    terminated = 1;
  }
}

// ---------------------------------------------------------------------------
static void atomic_start() {
    terminated = 0;
    signal(SIGINT, interruption_handler);
}

// ---------------------------------------------------------------------------
static void atomic_end() {
    signal(SIGINT, SIG_DFL);
    fflush(stdout);
    if(terminated) {
        exit(130);
    }
}


// ---------------------------------------------------------------------------
static inline void trim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
        return !std::isspace(ch);
    }));
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// ---------------------------------------------------------------------------
static bool sorter(std::pair<std::string, size_t>& elem1, std::pair<std::string, size_t>& elem2) {
    return elem1.second > elem2.second;
}


// ---------------------------------------------------------------------------
static bool rev_sorter(std::pair<std::string, size_t>& elem1, std::pair<std::string, size_t>& elem2) {
    return elem1.second < elem2.second;
}

// ---------------------------------------------------------------------------
static size_t update(Options& options, std::string& line, std::map<std::string, size_t>& hist, std::vector<std::pair<std::string, size_t>>& pairs) {
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

    // sort bins
    sort(pairs.begin(), pairs.end(), options.reverse ? rev_sorter : sorter);
    return sum;
}

// ---------------------------------------------------------------------------
static void print_line(FILE* f, char* buffer, size_t maxlen, int draw, size_t val, size_t sum) {
    size_t len = fprintf(f, "%s", buffer);
    if(draw) {
        while(len++ < maxlen) {
            fprintf(f, " ");
        }
        fprintf(f, " |");
        for(unsigned int i = 0; i < ((120 - maxlen) * val) / sum; i++) fprintf(f, "#");
    }
    fprintf(f, "\n");
}

// ---------------------------------------------------------------------------
static void clear_screen(int vt100) {
    if(vt100) {
        std::cout << "\033[2J";
    } else {
        std::cout << std::endl;
    }
}

// ---------------------------------------------------------------------------
static void draw(Options& options, int output, FILE* out, std::vector<std::pair<std::string, size_t>>& pairs, size_t sum) {
    char buffer[256], tmp[256];
    size_t maxlen = 0;
    size_t limit = options.number;
    for (std::pair<std::string, size_t> element : pairs) {
        if(options.number && !limit) {
            break;
        }
        limit--;
        size_t len = 0;
        if(options.order) {
            len += snprintf(buffer, sizeof(buffer), "%s%s", element.first.c_str(), options.seperator.c_str());
            if(options.percent) len += snprintf(tmp, sizeof(tmp), "%f", element.second * 100.0 / sum);
            else len += snprintf(tmp, sizeof(tmp), "%zd", element.second);
            strncat(buffer, tmp, sizeof(buffer) - 1);
        } else {
            if(options.percent) len += snprintf(buffer, sizeof(buffer), "%f", element.second * 100.0 / sum);
            else len += snprintf(buffer, sizeof(buffer), "%zd", element.second);
            len += snprintf(tmp, sizeof(tmp), "%s%s", options.seperator.c_str(), element.first.c_str());
            strncat(buffer, tmp, sizeof(buffer) - 1);
        }
        if(len > maxlen) maxlen = len;
        if(output) {
            print_line(stdout, buffer, maxlen, options.draw, element.second, sum);

            if(options.outfile && out) {
                print_line(out, buffer, maxlen, options.draw, element.second, sum);
            }

        }
    }
}

// ---------------------------------------------------------------------------
static void draw_histogram(Options& options, std::vector<std::pair<std::string, size_t>>& pairs, size_t sum) {
    FILE* out = NULL;

    for(int output = 0; output < 2; output++) {
        if(output && (options.outfile || options.atomic))  {
            atomic_start();
            if(options.outfile) out = fopen(options.outfile, "w");
        }

        do {
            draw(options, output, out, pairs, sum);

            if(output && (options.outfile || options.atomic)) {
                if(options.outfile) {
                    fclose(out);
                }
                if(options.atomic) {
                    // on atomic update, redraw the histogram and exit afterwards
                    if(terminated == 1) {
                        terminated = 2;
                        clear_screen(options.clear);
                        continue;
                    }
                }
                atomic_end();
            }
            break;
        } while(options.atomic);
    }
}

// ---------------------------------------------------------------------------
int main(int argc, char* argv[]) {
    Options options;
    size_t last_update = 0;
    std::map<std::string, size_t> hist;
    std::vector<std::pair<std::string, size_t>> pairs;


    if(parse_cli_options(options, cli_options, argc, argv)) {
        return 1;
    }

    for (std::string line; getline(std::cin, line);) {
        size_t sum = update(options, line, hist, pairs);
        if(!sum) sum = 1;

        // output
        size_t now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

        if(!options.interval || now - last_update >= options.interval * 1000) {
            last_update = now;
            clear_screen(options.clear);
            draw_histogram(options, pairs, sum);
        }
    }
}
