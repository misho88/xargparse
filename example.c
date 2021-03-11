#include "xargparse.h"

/* define a function that applies xap_int to each element of a 4-element array */
xap_define_repeat(xap_int_4, int, xap_int, 4);


/* argument definitions
 *
 * put any showstoppers like -h and -v first
 *
 * positionals are enumerated sequentially even if they consume multiple
 * arguments (e.g., the two below consume 5 arguments in total). If the
 * specification was instead for position 0 then 4, the first argument
 * would still consume four elements of the argv, three more would be
 * skipped, and only then would another be consumed.
 */

#define arguments(_) \
	_( 0 , NULL     , char const *, program,    , xap_string) \
	_('h', "help"   , bool        , help   ,    , xap_toggle) \
	_('v', "version", bool        , version,    , xap_toggle) \
	_('i', "int"    , int         , i      ,    , xap_int   ) \
	_('s', "string" , char const *, s      ,    , xap_string) \
	_('t', "toggle" , bool        , t      ,    , xap_toggle) \
	_('I', "4ints"  , int         , i4     , [4], xap_int_4 ) \
	_( 1 , NULL     , int         , ip     ,    , xap_int   ) \
	_( 3 , NULL     , char const *, sp     ,    , xap_string)

/* arguments that stops the parsing early
 *
 * anything nonzero would work for the second argument, e.g.,
 *     #define stop_after _('h',1) _{'v',1}
 * since it is only used to differentiate positional and keyword arguments
 */
#define stop_after(_) \
	_('h', "help"   ) \
	_('v', "version")

/* required arguments
 *
 * only the last necessary positional argument has to be listed since it is
 * impossible to get to it without going through the preceding ones
 */
#define required(_) \
	_( 0 , NULL ) \
	_('i', "int") \
	_( 1 , NULL )

/* hints for displaying help and usage messages
 *
 * start with the short option/index as always, then:
 *  - the display name for the argument (a sensible value is inferred if NULL)
 *     - argv[0], i.e., (0, NULL), is the program name and should probably be
 *       included
 *     - "" (not NULL) should be used for arguments that take no values
 *     - note that arguments() above has no concept of how many values a given
 *       argument consumes; put something sensible for array inputs
 *  - help message:
 *     - NULL to display default message "---"
 *     - "" omit the line altogether (e.g., for argv[0])
 *     - " " or something similar for a blank message
 */
#define display_hints(_) \
	_( 0 , NULL     , "example-program" , "" /* or XAP_NO_HELP */    ) \
	_( 1 , NULL     , "int"             , "a whole number"          ) \
	_('h', "help"   , ""                , "show this help and exit"  ) \
	_('v', "version", ""                , "show the version and exit") \
	_('I', "4ints"  , "int int int int" , "four integers"            )

/* define a structure that will hold the arguments above */
struct args xap_struct(arguments);

/* create the function that will parse the arguments */
xap_define_parser(parse, struct args, arguments, stop_after, required);

/* create the function that will print usage */
xap_define_fprint_usage(fprint_usage, arguments, required, display_hints);

xap_define_fprint_help(fprint_help, arguments, display_hints);

int main(int argc, char ** argv)
{
	struct args args = { .help = false };

	xap_error_context_t ctx = parse(&argc, argv, &args);
	if (ctx.error) {
		xap_fprint_error_context(&ctx, stderr);
		fprint_usage(stderr);
	}
	else if (args.help) {
		fprint_usage(stdout);
		fprint_help(stdout);
	}
	else if (args.version) {
		fprintf(stderr, "Version 0.0.0 (Example Program)\n");
	}
	else {
		printf(
			"SUCCESSFULLY PARSED:\n"
			"0:%s -h:%d -v:%d -i:%d -s:%s -t:%d -I:%d,%d,%d,%d, 1:%d, 3:%s\n",
			args.program, args.help, args.version, args.i, args.s, args.t, args.i4[0], args.i4[1], args.i4[2], args.i4[3], args.ip, args.sp
		);
		if (argc) {
			printf("AND DID NOT PARSE:\n");
			xap_fprint_args(argc, argv, stdout);
		};
	}
}
