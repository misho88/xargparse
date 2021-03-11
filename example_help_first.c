#include "xargparse.h"

#define empty(_)

#define args0(_) \
	_('h', "help", bool        , help   , , xap_toggle) \

#define args1(_) \
	_( 0 , NULL  , char const *, program, , xap_string) \
	_( 1 , NULL  , int         , i     ,  , xap_int   ) \

#define args(_) \
	args1(_) \
	args0(_) \

#define reqs1(_) \
	_( 0 , NULL ) \
	_( 1 , NULL ) \

#define stop_after(_) \
	_('h', "help"   ) \

#define hints(_) \
	_( 0 ,  NULL , "example-program", XAP_NO_HELP      ) \
	_( 1 ,  NULL , "int"            , "a whole number" ) \
	_('h', "help", ""               , "print this help")

struct args xap_struct(args);

xap_define_parser(parse0, struct args, args0, empty, empty);
xap_define_parser(parse1, struct args, args1, empty, reqs1);

xap_define_fprint_usage(fprint_usage, args, reqs1, hints);
xap_define_fprint_help(fprint_help, args, hints);

int main(int argc, char ** argv)
{
	struct args args = { .help = false };

	xap_error_context_t ctx;
	ctx = parse0(&argc, argv, &args);
	if (ctx.error) {
		xap_fprint_error_context(&ctx, stderr);
		fprint_usage(stderr);
		return 1;
	}
	if (args.help) {
		fprint_usage(stdout);
		fprint_help(stdout);
		return 0;
	}

	ctx = parse1(&argc, argv, &args);
	if (ctx.error) {
		xap_fprint_error_context(&ctx, stderr);
		fprint_usage(stderr);
		return 1;
	}
	printf("integer is %d\n", args.i);
	return 0;
}
