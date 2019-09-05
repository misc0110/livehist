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
|--separator=SEPERATOR / -s SEPERATOR | Set the seperator between symbol and count (default: " :: ")      |
|--number=NUMBER / -n NUMBER  | Display only top NUMBER entries (default: all)                            |
|--no-clear / -c | Disable screen clear                                                                   |
|--reverse / -r  | Short ascending                                                                        |
|--percent / -p | Show frequency as percentage                                                            |
|--draw / -d | Draw bar for visualization                                                                 |
|--interval=SECONDS / -i SECONDS | Refresh interval (default: 0)                                          |
|--switch / -x | Show count first, then value                                                             |
|--help / -h | Show the help                                                                              |
