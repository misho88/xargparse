# XArgParse
XArgParse (XAP) is header-only library that creates argument-parsing functions. It makes heavy use of the X-macro technique.

# Usage
There are four steps, aside from including the header file, the first of which is naturally the most involved:

0. include `xargparse.h` first rather than last since the X-macros are used as arguments, which makes them behave a bit more like normal functions
1. define the arguments
    1. define the names or positions by which they will be provided, their types, variable names and conversion functions
    2. define arguments which will stop further parsing (usually, `-h`/`--help` or `-v`/`--version` or for hierarchical parsing)
    3. define arguments which are required
    4. define display hints which help build more user-friendly usage and help function definitions
2. define the structure that will hold the values of the arguments 
3. define functions for parsing, printing usage and printing help
4. parse the arguments

Additionally, the parsing function will modify the `argc` pointer and `argv` array it is given, but it will not modify original strings `argv[i]`. Thus, if the original `argv` needs to be preserved, a shallow copy is sufficient. After parsing has completed, `argc` and `argv` will refer to the arguments which the parser did not use.

See `example.c` for details.

# Supported Argument Syntax
Arguments can be positional or keyword. `argv[0]` is not treated specially and should probably be listed explicitly as a positional argument. There is no requirement for the keywords to preceed or succeed the positionals; they can be intermixed.

Positional arguments are specified by their index. The indexes need not be sequential. The parser will skip over any unreferenced arguments.

Keyword arguments must have a short form consisting of a single `-` and one character that is not `\0` or `-` (e.g., `-i`). This creates a hard limit of about 95 such arguments, of which only the 62 alphanumeric ones are recommended. Going beyond that is probably not a good idea in the first place, but hierarhies of parsers are supported, parsing can be stopped early for certain arguments, and this limitation only applies any one parser.

Repeated keyword arguments are not supported; e.g., `-i 1 -i 2` or `-ii` is an error unless the first `-i` causes the parser to stop early. 

Short forms that take no arguments can be prepended to another arguments; e.g., `-x -y -i 1`, `-xy -i 1` and `-xyi 1` are all equivalent.

Arguments to short forms do not need a space; e.g., `-i1` and `-i 1` are equivalent. With the prepending rule, this means that, e.g., `-xi 1`, `-i1 -x` and `-xi1` are all equivalent, but `-i1x` would attempt to assign `1x` to `i`.

Keyword arguments can optionally have a long form consisting of `--` and a string (e.g., `--int`). Values can be passed with `=` or whitespace (but not both); e.g., `--int=1` and `--int 1` are equivalent. This means that `--arg=` is equivalent to `--arg ""`. Perhaps confusingly, `--arg=a b ...` is equivalent to `--arg a b ...` for multiple-value arguments.

Currently, unexpected keyword arguments stop the parser with an error. This may change.

# State of the Software

Largely untested. The basics seem to work.

# Build Requirements

None. `example.c` compiles with `gcc`, `clang` and `tcc` as of this writing.
