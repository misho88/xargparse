/* Copyright 2021 Mihail Georgiev
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR
 * IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef XARGPARSE_H
#define XARGPARSE_H

/*
 * Arguments are defined as follows:
 * #define ARGUMENTS \
 *     X(sopt, lopt, type, name, arry, conv)
 *     X('i', "int",  int ,   i,     , xap_int  ) \
 *     X('i', "int",  int ,   i,  [2], xap_int_2) \
 *     ...
 * where -s or --lopt corresponds to a variable of type type named name that
 * will hold count values each of which will be converted from their string
 * representations with conv. The second line gives a concrete example that
 * corresponds to integer argument.
 *
 * The help descriptions are defined as follows:
 * #define DESCRIPTIONS \
 * 	X('s', "help message") \
 * 	X('i', "an integer")
 * where the short option is used to relate a message to its variable.
 *
 * The required arguments are defined as follows:
 * #define REQUIRED_ARGUMENTS "si"
 * wherein 's' and 'i' are the short option names.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>

/* feedback */
#ifndef xap_fprint
#define xap_fprint(stream, fmt, ...) \
	fprintf(stream, fmt "\n" __VA_OPT__(,) __VA_ARGS__)
#endif
#ifndef xap_print
#define xap_print(...) \
	xap_fprint(stdout, __VA_ARGS__);
#endif
#ifndef xap_eprint
#define xap_eprint(...) \
	xap_fprint(stderr, __VA_ARGS__);
#endif
#ifndef xap_debug
#define xap_debug(fmt, ...) \
	xap_eprint("%s:%d:%s(): " fmt, __FILE__, __LINE__, __func__ __VA_OPT__(,) __VA_ARGS__)
#endif


#define xap_identity(...) \
	(__VA_ARGS__)
#define xap_failure_fatal(err, ...) \
	do if (err) { exit(err); } while(0)
#define xap_failure_return(err, ...) \
	do if (err) { return err; } while(0)
#define xap_failure_eprint_fatal(err, ...) \
	do if (err) { xap_eprint(__VA_ARGS__); exit(err); } while(0)
#define xap_failure_eprint_return(err, ...) \
	do if (err) { xap_eprint(__VA_ARGS__); return err; } while(0)
#define xap_failure_debug_fatal(err, ...) \
	do if (err) { xap_debug(__VA_ARGS__); exit(err); } while(0)
#define xap_failure_debug_return(err, ...) \
	do if (err) { xap_debug(__VA_ARGS__); return err; } while(0)

#ifndef xap_failure
#define xap_failure xap_failure_debug_fatal
#endif


typedef int(*xap_assign)(int, char **, void *, int *);


static inline
int xap_toggle(int argc, char ** argv, bool * target, int * consumed)
{
	*target = !*target;
	*consumed = 0;
	return 0;
}

static inline
int xap_string(int argc, char ** argv, char const ** target, int * consumed)
{
	*consumed = 0;
	xap_failure(argc < 1, "need 1 argument");
	*target = argv[0];
	*consumed = 1;
	return 0;
}

static inline
int xap_double(int argc, char ** argv, double * target, int * consumed)
{
	char * endptr;
	*consumed = 0;
	xap_failure(argc < 1, "need 1 argument");
	xap_failure(argv[0][0] == '\0', "empty argument");
	double value = strtod(argv[0], &endptr);
	xap_failure(endptr[0] != '\0', "in \"%s\", strtod() failed to convert \"%s\"\n", argv[0], endptr);
	*target = value;
	*consumed = 1;
	return 0;
}

static inline
int xap_long(int argc, char ** argv, long * target, int * consumed)
{
	char * endptr;
	*consumed = 0;
	xap_failure(argc < 1, "need 1 argument");
	xap_failure(argv[0][0] == '\0', "empty argument");
	double value = strtol(argv[0], &endptr, 10);
	xap_failure(endptr[0] != '\0', "in \"%s\", strtol() failed to convert \"%s\"\n", argv[0], endptr);
	*target = value;
	*consumed = 1;
	return 0;
}

static inline
int xap_int(int argc, char ** argv, int * target, int * consumed)
{
	long tmp;
	int rv; if ((rv = xap_long(argc, argv, &tmp, consumed)) != 0) return rv;
	*target = tmp;
	return 0;
}

static inline
int xap_float(int argc, char ** argv, float * target, int * consumed)
{
	double tmp;
	int rv; if ((rv = xap_double(argc, argv, &tmp, consumed)) != 0) return rv;
	*target = tmp;
	return 0;
}


static inline
int xap_fprint_double(double d, FILE * stream) { return fprintf(stream, "%lg", d); }


static inline
int xap_fprint_long(long l, FILE * stream) { return fprintf(stream, "%ld", l); }


static inline
void xap_fprint_args(int argc, char ** argv, FILE * stream)
{
	for (int i = 0; i < argc; i++)
		fprintf(stream, "'%s'%s", argv[i], i < argc - 1 ? " " : "\n");
}



#define xap_define_repeat(name, type, func, count) \
	static inline \
	int name(int argc, char ** argv, type (*target)[count], int * consumed) \
	{ \
		*consumed = 0; \
		for (int i = 0; i < count; i++) { \
			int loc_consumed; \
			xap_failure(func(argc, argv, *target + i, &loc_consumed), #func " failed"); \
			argc -= loc_consumed; \
			argv += loc_consumed; \
			*consumed += loc_consumed; \
		} \
		return 0; \
	}


#define xap_derive_state_name(sopt, lopt, type, name, arry, conv) SET_ ## name,


#define xap_next_arg_logic(arguments) \
	if (index >= argc - 1) { \
		state = CHECK_REQUIRED; \
		break; \
	} \
	arg = argv[++index]; \
	\
	if (positional_only || arg[0] != '-') { \
		state = POSITIONAL_ARG; \
	} \
	else { \
		++arg; \
		if (arg[0] != '-') { \
			state = SOPT; \
		} \
		else { \
			++arg; \
			if (arg[0] != '\0') { \
				state = LOPT; \
			} \
			else { \
				positional_only = true; \
				state = NEXT_ARG; \
			} \
		} \
	}


#define xap_check_parsed_logic(arguments) \



#define xap_positional_arg_logic(arguments) \
	posv[*posc] = arg; \
	*posc += 1; \
	state = NEXT_ARG;



#define xap_compare_sopt(sopt, lopt, type, name, arry, conv) \
	else if (arg[0] == sopt) { \
		++arg; \
		state = SET_ ## name; \
	}

#define xap_sopt_logic(arguments) \
	if (0) { } \
	arguments(xap_compare_sopt) \
	else { \
		xap_failure(-1, "unknown argument: -%c\n", arg[0]); \
	}



#define xap_compare_lopt(sopt, lopt, type, name, arry, conv) \
	else if (strcmp(arg, lopt) == 0) { \
		arg += strlen(lopt); \
		state = SET_ ## name; \
	}

#define xap_lopt_logic(arguments) \
	if (0) { } \
	arguments(xap_compare_lopt) \
	else { \
		xap_failure(-1, "unknown argument: --%s\n", arg); \
	}


#define xap_generate_set_logic(sopt, lopt, type, name, arry, conv) \
	case SET_ ## name: \
		xap_failure(parsed[strchr(sopts, sopt) - sopts], "-%c can only be passed once", sopt); \
		rv = conv(argc - 1 - index, argv + 1 + index, &args->name, &consumed); \
		xap_failure(rv, "conversion failed"); \
		xap_failure(consumed && arg[0] != '\0', "%s cannot appear in %s", arg, argv[index]); \
		index += consumed; \
		parsed[strchr(sopts, sopt) - sopts] = true; \
		state = arg[0] == '\0' ? NEXT_ARG : SOPT; \
		break;



#define xap_check_required_logic(arguments) \
	for (int i = 0; sopts[i] != '\0'; i++) { \
		char sopt = sopts[i]; \
		xap_failure(strchr(required, sopt) && !parsed[i], "-%c is required", sopt); \
	} \
	return 0;


#define xap_extract_sopt(sopt, lopt, type, name, arry, conv) sopt,


#define xap_declare_argument_field(sopt, lopt, type, name, arry, conv) type name arry;
#define xap_declare_positional_field(type, name, arry, conv) type name arry;

#define xap_declare_argument_struct(name, arguments, positionals) \
	struct name { \
		arguments(xap_declare_argument_field) \
		positionals(xap_declare_positional_field) \
	}

#define xap_declare_argument_parser(name, struct_type) \
	int name(int argc, char ** argv, struct_type * args, char * required, int * posc, char ** posv)

#define xap_define_argument_parser(name, struct_type, arguments) \
	xap_declare_argument_parser(name, struct_type) \
	{ \
		*posc = 0; \
		enum state { \
			NEXT_ARG, \
			POSITIONAL_ARG, \
			SOPT, \
			LOPT, \
			\
			arguments(xap_derive_state_name) \
			\
			CHECK_REQUIRED, \
		} state = NEXT_ARG; \
		char sopts[] = { \
			arguments(xap_extract_sopt) \
			'\0' \
		};\
		bool parsed[sizeof(sopts) - 1] = { false }; \
		\
		int index = 0, consumed = -1; \
		bool positional_only = false; \
		char * arg = NULL; \
		int rv; \
		\
		for (;;) switch (state) { \
		case NEXT_ARG: \
			xap_next_arg_logic(arguments) \
			break; \
		case POSITIONAL_ARG: \
			xap_positional_arg_logic(arguments) \
			break; \
		case SOPT: \
			xap_sopt_logic(arguments) \
			break; \
		case LOPT: \
			xap_lopt_logic(arguments) \
			break; \
		\
		arguments(xap_generate_set_logic); \
		\
		case CHECK_REQUIRED: \
			xap_check_required_logic(arguments) \
			break; \
		default: \
			xap_failure(-2, "state machine logic is wrong"); \
			break; \
		} \
		return 0; \
	}


#define xap_set_positional(type, name, arry, conv) \
	rv = conv(posc - index, posv + index, &args->name, &consumed); \
	xap_failure(rv, "conversion failed"); \
	index += consumed; \


#define xap_declare_positional_parser(name, struct_type) \
	int name(int posc, char ** posv, struct_type * args, int * n_parsed)

#define xap_define_positional_parser(name, struct_type, positionals) \
	xap_declare_positional_parser(name, struct_type) \
	{ \
		*n_parsed = 0; \
		int index = 0, consumed = -1; \
		int rv; \
		\
		positionals(xap_set_positional) \
		\
		*n_parsed = index; \
		return 0; \
	}

#define xap_return_description_logic(d_sopt, d_desc) \
	if (sopt == d_sopt) return d_desc;

#define xap_argument_help_printf_format(sopt, lopt, type, name, arry, conv) \
	"-%c, --%-14s %s\n"

#define xap_argument_help_printf_args(sopt, lopt, type, name, arry, conv) \
	sopt, lopt, xap_get_description(sopt),

#define xap_declare_fprint_help(name) \
	int name(char * program_name, FILE * stream)

#define xap_define_fprint_help(name, arguments, positionals, descriptions) \
	\
	char * xap_get_description(char sopt) \
	{ \
		descriptions(xap_return_description_logic) \
		return "---"; \
	} \
	\
	xap_declare_fprint_help(name) \
	{ \
		int rv = 0; \
		rv += fprintf( \
			stream, \
			"%s\n" \
			arguments(xap_argument_help_printf_format) \
			"%s", \
			program_name, \
			arguments(xap_argument_help_printf_args) \
			"" \
		); \
		\
		\
		return rv; \
	}


#endif/*XARGPARSE_H*/
