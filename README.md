# Live Histogram

A command line tool to show a real-time histogram of a data stream.

## Usage

The tool reads lines from the stdin. Every line is considered a symbol.

Example:

    python3 -c 'import random; [print(random.choice("ACABACAAABAABC")) for x in range(10000)]' | livehist

Sample output:

    5595 :: A
    2210 :: B
    2195 :: C

## Options


| Parameter | Description |
|--|--|
| --separator=SEPERATOR / -s SEPERATOR | Set the seperator between symbol and count (default: " :: ")      |
| --number=NUMBER / -n NUMBER  | Display only top NUMBER entries (default: all)                            |
| --no-clear / -c | Disable screen clear                                                                   |
| --reverse / -r  | Short ascending                                                                        |
| --percent / -p | Show frequency as percentage                                                            |
| --draw / -d | Draw bar for visualization                                                                 |
| --interval=SECONDS / -i SECONDS | Refresh interval (default: 0)                                          |
| --switch / -x | Show count first, then value                                                             |
| --help / -h | Show the help                                                                              |
| --outfile=FILE / -o FILE | Output latest histogram to file FILE.                                         |
| --atomic / -a | On Ctrl+C, application finishes printing the histogram.                                  |

## Install using PPA (recommended)

First, add the public key of the PPA and the PPA URL to the package manager, and update the package manager

    curl -s "https://misc0110.github.io/ppa/KEY.gpg" | sudo apt-key add -
    sudo curl -s -o /etc/apt/sources.list.d/misc0110.list "https://misc0110.github.io/ppa/file.list"
    sudo apt update

Then, simply install LiveTikZ

    sudo apt install livehist


## Compile

There are no dependencies on third-party libraries. To compile, simply run

    make

## Install

After building, simply run

    sudo make install

The binary (`livehist`) is installed to `/usr/bin/livehist`. Run `sudo make uninstall` to remove the application.
