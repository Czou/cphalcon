/* Driver template for the LEMON parser generator.
** The author disclaims copyright to this source code.
*/
/* First off, code is include which follows the "include" declaration
** in the input file. */
#include <stdio.h>
/* #line 56 "parser.y" */


#include "php_phalcon.h"
#include "mvc/view/engine/volt/parser.h"
#include "mvc/view/engine/volt/scanner.h"
#include "mvc/view/engine/volt/volt.h"
#include "mvc/view/exception.h"

#include <ext/standard/php_smart_str.h>

#include "kernel/main.h"
#include "kernel/memory.h"
#include "kernel/fcall.h"
#include "kernel/exception.h"

static zval *phvolt_ret_literal_zval(int type, phvolt_parser_token *T, phvolt_scanner_state *state)
{
	zval *ret;

	MAKE_STD_ZVAL(ret);
	array_init(ret);
	add_assoc_long(ret, "type", type);
	if (T) {
		add_assoc_stringl(ret, "value", T->token, T->token_len, 0);
		efree(T);
	}

	Z_ADDREF_P(state->active_file);
	add_assoc_zval(ret, "file", state->active_file);
	add_assoc_long(ret, "line", state->active_line);

	return ret;
}

static zval *phvolt_ret_if_statement(zval *expr, zval *true_statements, zval *false_statements, phvolt_scanner_state *state)
{
	zval *ret;

	MAKE_STD_ZVAL(ret);
	array_init(ret);
	add_assoc_long(ret, "type", PHVOLT_T_IF);
	add_assoc_zval(ret, "expr", expr);

	if (true_statements) {
		add_assoc_zval(ret, "true_statements", true_statements);
	}
	if (false_statements) {
		add_assoc_zval(ret, "false_statements", false_statements);
	}

	Z_ADDREF_P(state->active_file);
	add_assoc_zval(ret, "file", state->active_file);
	add_assoc_long(ret, "line", state->active_line);

	return ret;
}

static zval *phvolt_ret_elseif_statement(zval *expr, phvolt_scanner_state *state)
{
	zval *ret;

	MAKE_STD_ZVAL(ret);
	array_init(ret);
	add_assoc_long(ret, "type", PHVOLT_T_ELSEIF);
	add_assoc_zval(ret, "expr", expr);

	Z_ADDREF_P(state->active_file);
	add_assoc_zval(ret, "file", state->active_file);
	add_assoc_long(ret, "line", state->active_line);

	return ret;
}

static zval *phvolt_ret_elsefor_statement(phvolt_scanner_state *state)
{
	zval *ret;

	MAKE_STD_ZVAL(ret);
	array_init(ret);
	add_assoc_long(ret, "type", PHVOLT_T_ELSEFOR);

	Z_ADDREF_P(state->active_file);
	add_assoc_zval(ret, "file", state->active_file);
	add_assoc_long(ret, "line", state->active_line);

	return ret;
}

static zval *phvolt_ret_for_statement(phvolt_parser_token *variable, phvolt_parser_token *key, zval *expr, zval *if_expr, zval *block_statements, phvolt_scanner_state *state)
{
	zval *ret;

	MAKE_STD_ZVAL(ret);
	array_init(ret);
	add_assoc_long(ret, "type", PHVOLT_T_FOR);

	add_assoc_stringl(ret, "variable", variable->token, variable->token_len, 0);
	efree(variable);

	if (key) {
		add_assoc_stringl(ret, "key", key->token, key->token_len, 0);
		efree(key);
	}

	add_assoc_zval(ret, "expr", expr);

	if (if_expr) {
		add_assoc_zval(ret, "if_expr", if_expr);
	}

	add_assoc_zval(ret, "block_statements", block_statements);

	Z_ADDREF_P(state->active_file);
	add_assoc_zval(ret, "file", state->active_file);
	add_assoc_long(ret, "line", state->active_line);

	return ret;
}

static zval *phvolt_ret_cache_statement(zval *expr, zval *lifetime, zval *block_statements, phvolt_scanner_state *state)
{
	zval *ret;

	MAKE_STD_ZVAL(ret);
	array_init(ret);

	add_assoc_long(ret, "type", PHVOLT_T_CACHE);
	add_assoc_zval(ret, "expr", expr);

	if (lifetime) {
		add_assoc_zval(ret, "lifetime", lifetime);
	}
	add_assoc_zval(ret, "block_statements", block_statements);

	Z_ADDREF_P(state->active_file);
	add_assoc_zval(ret, "file", state->active_file);
	add_assoc_long(ret, "line", state->active_line);

	return ret;
}

static zval *phvolt_ret_set_statement(zval *assignments)
{
	zval *ret;

	MAKE_STD_ZVAL(ret);
	array_init_size(ret, 2);
	add_assoc_long(ret, "type", PHVOLT_T_SET);

	add_assoc_zval(ret, "assignments", assignments);

	return ret;
}

static zval *phvolt_ret_set_assignment(phvolt_parser_token *variable, int operator, zval *expr, phvolt_scanner_state *state)
{

	zval *ret;

	MAKE_STD_ZVAL(ret);
	array_init_size(ret, 5);

	add_assoc_stringl(ret, "variable", variable->token, variable->token_len, 0);
	efree(variable);

	add_assoc_long(ret, "op", operator);

	add_assoc_zval(ret, "expr", expr);

	Z_ADDREF_P(state->active_file);
	add_assoc_zval(ret, "file", state->active_file);
	add_assoc_long(ret, "line", state->active_line);

	return ret;
}

static zval *phvolt_ret_echo_statement(zval *expr, phvolt_scanner_state *state)
{
	zval *ret;

	MAKE_STD_ZVAL(ret);
	array_init_size(ret, 4);
	add_assoc_long(ret, "type", PHVOLT_T_ECHO);
	add_assoc_zval(ret, "expr", expr);

	Z_ADDREF_P(state->active_file);
	add_assoc_zval(ret, "file", state->active_file);
	add_assoc_long(ret, "line", state->active_line);

	return ret;
}

static zval *phvolt_ret_block_statement(phvolt_parser_token *name, zval *block_statements, phvolt_scanner_state *state)
{
	zval *ret;

	MAKE_STD_ZVAL(ret);
	array_init(ret);

	add_assoc_long(ret, "type", PHVOLT_T_BLOCK);

	add_assoc_stringl(ret, "name", name->token, name->token_len, 0);
	efree(name);

	if (block_statements) {
		add_assoc_zval(ret, "block_statements", block_statements);
	}

	Z_ADDREF_P(state->active_file);
	add_assoc_zval(ret, "file", state->active_file);
	add_assoc_long(ret, "line", state->active_line);

	return ret;
}

static zval *phvolt_ret_macro_statement(phvolt_parser_token *macro_name, zval *parameters, zval *block_statements, phvolt_scanner_state *state)
{
	zval *ret;

	MAKE_STD_ZVAL(ret);
	array_init(ret);
	add_assoc_long(ret, "type", PHVOLT_T_MACRO);

	add_assoc_stringl(ret, "name", macro_name->token, macro_name->token_len, 0);
	efree(macro_name);

	if (parameters) {
		add_assoc_zval(ret, "parameters", parameters);
	}

	if (block_statements) {
		add_assoc_zval(ret, "block_statements", block_statements);
	}

	Z_ADDREF_P(state->active_file);
	add_assoc_zval(ret, "file", state->active_file);
	add_assoc_long(ret, "line", state->active_line);

	return ret;
}

static zval *phvolt_ret_macro_parameter(phvolt_parser_token *variable, zval *default_value, phvolt_scanner_state *state)
{
	zval *ret;

	MAKE_STD_ZVAL(ret);
	array_init_size(ret, 5);

	add_assoc_stringl(ret, "variable", variable->token, variable->token_len, 0);
	efree(variable);

	if (default_value) {
		add_assoc_zval(ret, "default", default_value);
	}

	Z_ADDREF_P(state->active_file);
	add_assoc_zval(ret, "file", state->active_file);
	add_assoc_long(ret, "line", state->active_line);

	return ret;
}

static zval *phvolt_ret_extends_statement(phvolt_parser_token *P, phvolt_scanner_state *state)
{
	zval *ret;

	MAKE_STD_ZVAL(ret);
	array_init_size(ret, 4);

	add_assoc_long(ret, "type", PHVOLT_T_EXTENDS);
	add_assoc_stringl(ret, "path", P->token, P->token_len, 0);
	efree(P);

	Z_ADDREF_P(state->active_file);
	add_assoc_zval(ret, "file", state->active_file);
	add_assoc_long(ret, "line", state->active_line);

	return ret;
}

static zval *phvolt_ret_include_statement(zval *path, zval *params, phvolt_scanner_state *state)
{
	zval *ret;

	MAKE_STD_ZVAL(ret);
	array_init_size(ret, 4);

	add_assoc_long(ret, "type", PHVOLT_T_INCLUDE);

	add_assoc_zval(ret, "path", path);
	if (params) {
		add_assoc_zval(ret, "params", params);
	}

	Z_ADDREF_P(state->active_file);
	add_assoc_zval(ret, "file", state->active_file);
	add_assoc_long(ret, "line", state->active_line);

	return ret;
}

static zval *phvolt_ret_do_statement(zval *expr, phvolt_scanner_state *state)
{
	zval *ret;

	MAKE_STD_ZVAL(ret);
	array_init_size(ret, 4);

	add_assoc_long(ret, "type", PHVOLT_T_DO);

	add_assoc_zval(ret, "expr", expr);

	Z_ADDREF_P(state->active_file);
	add_assoc_zval(ret, "file", state->active_file);
	add_assoc_long(ret, "line", state->active_line);

	return ret;
}

static zval *phvolt_ret_return_statement(zval *expr, phvolt_scanner_state *state)
{
	zval *ret;

	MAKE_STD_ZVAL(ret);
	array_init_size(ret, 4);

	add_assoc_long(ret, "type", PHVOLT_T_RETURN);

	add_assoc_zval(ret, "expr", expr);

	Z_ADDREF_P(state->active_file);
	add_assoc_zval(ret, "file", state->active_file);
	add_assoc_long(ret, "line", state->active_line);

	return ret;
}

static zval *phvolt_ret_autoescape_statement(int enable, zval *block_statements, phvolt_scanner_state *state)
{
	zval *ret;

	MAKE_STD_ZVAL(ret);
	array_init_size(ret, 5);

	add_assoc_long(ret, "type", PHVOLT_T_AUTOESCAPE);
	add_assoc_long(ret, "enable", enable);
	add_assoc_zval(ret, "block_statements", block_statements);

	Z_ADDREF_P(state->active_file);
	add_assoc_zval(ret, "file", state->active_file);
	add_assoc_long(ret, "line", state->active_line);

	return ret;
}

static zval *phvolt_ret_empty_statement(phvolt_scanner_state *state)
{
	zval *ret;

	MAKE_STD_ZVAL(ret);
	array_init_size(ret, 3);
	add_assoc_long(ret, "type", PHVOLT_T_EMPTY_STATEMENT);

	Z_ADDREF_P(state->active_file);
	add_assoc_zval(ret, "file", state->active_file);
	add_assoc_long(ret, "line", state->active_line);

	return ret;
}

static zval *phvolt_ret_break_statement(phvolt_scanner_state *state)
{
	zval *ret;

	MAKE_STD_ZVAL(ret);
	array_init_size(ret, 3);
	add_assoc_long(ret, "type", PHVOLT_T_BREAK);

	Z_ADDREF_P(state->active_file);
	add_assoc_zval(ret, "file", state->active_file);
	add_assoc_long(ret, "line", state->active_line);

	return ret;
}

static zval *phvolt_ret_continue_statement(phvolt_scanner_state *state)
{
	zval *ret;

	MAKE_STD_ZVAL(ret);
	array_init_size(ret, 3);
	add_assoc_long(ret, "type", PHVOLT_T_CONTINUE);

	Z_ADDREF_P(state->active_file);
	add_assoc_zval(ret, "file", state->active_file);
	add_assoc_long(ret, "line", state->active_line);

	return ret;
}

static zval *phvolt_ret_zval_list(zval *list_left, zval *right_list)
{

	zval *ret;
	HashPosition pos;
	HashTable *list;

	MAKE_STD_ZVAL(ret);
	array_init(ret);

	if (list_left) {

		list = Z_ARRVAL_P(list_left);
		if (zend_hash_index_exists(list, 0)) {
			zend_hash_internal_pointer_reset_ex(list, &pos);
			for (;; zend_hash_move_forward_ex(list, &pos)) {

				zval ** item;

				if (zend_hash_get_current_data_ex(list, (void**) &item, &pos) == FAILURE) {
					break;
				}

				Z_ADDREF_PP(item);
				add_next_index_zval(ret, *item);

			}
			zval_ptr_dtor(&list_left);
		} else {
			add_next_index_zval(ret, list_left);
		}
	}

	add_next_index_zval(ret, right_list);

	return ret;
}

static zval *phvolt_ret_named_item(phvolt_parser_token *name, zval *expr, phvolt_scanner_state *state)
{
	zval *ret;

	MAKE_STD_ZVAL(ret);
	array_init(ret);
	add_assoc_zval(ret, "expr", expr);
	if (name != NULL) {
		add_assoc_stringl(ret, "name", name->token, name->token_len, 0);
		efree(name);
	}

	Z_ADDREF_P(state->active_file);
	add_assoc_zval(ret, "file", state->active_file);
	add_assoc_long(ret, "line", state->active_line);

	return ret;
}

static zval *phvolt_ret_expr(int type, zval *left, zval *right, zval *ternary, phvolt_scanner_state *state)
{
	zval *ret;

	MAKE_STD_ZVAL(ret);
	array_init(ret);
	add_assoc_long(ret, "type", type);

	if (ternary) {
		add_assoc_zval(ret, "ternary", ternary);
	}

	if (left) {
		add_assoc_zval(ret, "left", left);
	}

	if (right) {
		add_assoc_zval(ret, "right", right);
	}

	Z_ADDREF_P(state->active_file);
	add_assoc_zval(ret, "file", state->active_file);
	add_assoc_long(ret, "line", state->active_line);

	return ret;
}

static zval *phvolt_ret_slice(zval *left, zval *start, zval *end, phvolt_scanner_state *state)
{
	zval *ret;

	MAKE_STD_ZVAL(ret);
	array_init(ret);
	add_assoc_long(ret, "type", PHVOLT_T_SLICE);
	add_assoc_zval(ret, "left", left);

	if (start != NULL) {
		add_assoc_zval(ret, "start", start);
	}

	if (end != NULL) {
		add_assoc_zval(ret, "end", end);
	}

	Z_ADDREF_P(state->active_file);
	add_assoc_zval(ret, "file", state->active_file);
	add_assoc_long(ret, "line", state->active_line);

	return ret;
}

static zval *phvolt_ret_func_call(zval *expr, zval *arguments, phvolt_scanner_state *state)
{

	zval *ret;

	MAKE_STD_ZVAL(ret);
	array_init(ret);
	add_assoc_long(ret, "type", PHVOLT_T_FCALL);
	add_assoc_zval(ret, "name", expr);

	if (arguments) {
		add_assoc_zval(ret, "arguments", arguments);
	}

	Z_ADDREF_P(state->active_file);
	add_assoc_zval(ret, "file", state->active_file);
	add_assoc_long(ret, "line", state->active_line);

	return ret;
}

static zval *phvolt_ret_macro_call_statement(zval *expr, zval *arguments, zval *caller, phvolt_scanner_state *state)
{

	zval *ret;

	MAKE_STD_ZVAL(ret);
	array_init(ret);
	add_assoc_long(ret, "type", PHVOLT_T_CALL);
	add_assoc_zval(ret, "name", expr);

	if (arguments) {
		add_assoc_zval(ret, "arguments", arguments);
	}

	if (caller) {
		add_assoc_zval(ret, "caller", caller);
	}

	Z_ADDREF_P(state->active_file);
	add_assoc_zval(ret, "file", state->active_file);
	add_assoc_long(ret, "line", state->active_line);

	return ret;
}


/* #line 564 "parser.c" */
/* Next is all token values, in a form suitable for use by makeheaders.
** This section will be null unless lemon is run with the -m switch.
*/
/* 
** These constants (all generated automatically by the parser generator)
** specify the various kinds of tokens (terminals) that the parser
** understands. 
**
** Each symbol here is a terminal symbol in the grammar.
*/
/* Make sure the INTERFACE macro is defined.
*/
#ifndef INTERFACE
# define INTERFACE 1
#endif
/* The next thing included is series of defines which control
** various aspects of the generated parser.
**    YYCODETYPE         is the data type used for storing terminal
**                       and nonterminal numbers.  "unsigned char" is
**                       used if there are fewer than 250 terminals
**                       and nonterminals.  "int" is used otherwise.
**    YYNOCODE           is a number of type YYCODETYPE which corresponds
**                       to no legal terminal or nonterminal number.  This
**                       number is used to fill in empty slots of the hash 
**                       table.
**    YYFALLBACK         If defined, this indicates that one or more tokens
**                       have fall-back values which should be used if the
**                       original value of the token will not parse.
**    YYACTIONTYPE       is the data type used for storing terminal
**                       and nonterminal numbers.  "unsigned char" is
**                       used if there are fewer than 250 rules and
**                       states combined.  "int" is used otherwise.
**    phvolt_TOKENTYPE     is the data type used for minor tokens given 
**                       directly to the parser from the tokenizer.
**    YYMINORTYPE        is the data type used for all minor tokens.
**                       This is typically a union of many types, one of
**                       which is phvolt_TOKENTYPE.  The entry in the union
**                       for base tokens is called "yy0".
**    YYSTACKDEPTH       is the maximum depth of the parser's stack.
**    phvolt_ARG_SDECL     A static variable declaration for the %extra_argument
**    phvolt_ARG_PDECL     A parameter declaration for the %extra_argument
**    phvolt_ARG_STORE     Code to store %extra_argument into yypParser
**    phvolt_ARG_FETCH     Code to extract %extra_argument from yypParser
**    YYNSTATE           the combined number of states.
**    YYNRULE            the number of rules in the grammar
**    YYERRORSYMBOL      is the code number of the error symbol.  If not
**                       defined, then do no error processing.
*/
#define YYCODETYPE unsigned char
#define YYNOCODE 121
#define YYACTIONTYPE unsigned short int
#define phvolt_TOKENTYPE phvolt_parser_token*
typedef union {
  phvolt_TOKENTYPE yy0;
  zval* yy168;
  int yy241;
} YYMINORTYPE;
#define YYSTACKDEPTH 100
#define phvolt_ARG_SDECL phvolt_parser_status *status;
#define phvolt_ARG_PDECL ,phvolt_parser_status *status
#define phvolt_ARG_FETCH phvolt_parser_status *status = yypParser->status
#define phvolt_ARG_STORE yypParser->status = status
#define YYNSTATE 335
#define YYNRULE 147
#define YYERRORSYMBOL 83
#define YYERRSYMDT yy241
#define YY_NO_ACTION      (YYNSTATE+YYNRULE+2)
#define YY_ACCEPT_ACTION  (YYNSTATE+YYNRULE+1)
#define YY_ERROR_ACTION   (YYNSTATE+YYNRULE)

/* Next are that tables used to determine what action to take based on the
** current state and lookahead token.  These tables are used to implement
** functions that take a state number and lookahead value and return an
** action integer.  
**
** Suppose the action integer is N.  Then the action is determined as
** follows
**
**   0 <= N < YYNSTATE                  Shift N.  That is, push the lookahead
**                                      token onto the stack and goto state N.
**
**   YYNSTATE <= N < YYNSTATE+YYNRULE   Reduce by rule N-YYNSTATE.
**
**   N == YYNSTATE+YYNRULE              A syntax error has occurred.
**
**   N == YYNSTATE+YYNRULE+1            The parser accepts its input.
**
**   N == YYNSTATE+YYNRULE+2            No such action.  Denotes unused
**                                      slots in the yy_action[] table.
**
** The action table is constructed as a single large table named yy_action[].
** Given state S and lookahead X, the action is computed as
**
**      yy_action[ yy_shift_ofst[S] + X ]
**
** If the index value yy_shift_ofst[S]+X is out of range or if the value
** yy_lookahead[yy_shift_ofst[S]+X] is not equal to X or if yy_shift_ofst[S]
** is equal to YY_SHIFT_USE_DFLT, it means that the action is not in the table
** and that yy_default[S] should be used instead.  
**
** The formula above is for computing the action when the lookahead is
** a terminal symbol.  If the lookahead is a non-terminal (as occurs after
** a reduce action) then the yy_reduce_ofst[] array is used in place of
** the yy_shift_ofst[] array and YY_REDUCE_USE_DFLT is used in place of
** YY_SHIFT_USE_DFLT.
**
** The following are the tables generated in this section:
**
**  yy_action[]        A single table containing all actions.
**  yy_lookahead[]     A table containing the lookahead for each entry in
**                     yy_action.  Used to detect hash collisions.
**  yy_shift_ofst[]    For each state, the offset into yy_action for
**                     shifting terminals.
**  yy_reduce_ofst[]   For each state, the offset into yy_action for
**                     shifting non-terminals after a reduce.
**  yy_default[]       Default action for each state.
*/
static YYACTIONTYPE yy_action[] = {
 /*     0 */    82,   92,  108,   60,   52,   54,   66,   62,   64,   72,
 /*    10 */    74,   76,   78,   68,   70,   48,   46,   50,   43,   40,
 /*    20 */    56,   89,   58,   84,   87,   88,   96,   80,  335,  224,
 /*    30 */    84,   87,   88,   96,   80,  273,   43,   40,   56,   89,
 /*    40 */    58,   84,   87,   88,   96,   80,  272,  308,  171,   30,
 /*    50 */    82,   92,   32,   60,   52,   54,   66,   62,   64,   72,
 /*    60 */    74,   76,   78,   68,   70,   48,   46,   50,   43,   40,
 /*    70 */    56,   89,   58,   84,   87,   88,   96,   80,  301,  173,
 /*    80 */    82,   92,  112,   60,   52,   54,   66,   62,   64,   72,
 /*    90 */    74,   76,   78,   68,   70,   48,   46,   50,   43,   40,
 /*   100 */    56,   89,   58,   84,   87,   88,   96,   80,   36,  234,
 /*   110 */    82,   92,   85,   60,   52,   54,   66,   62,   64,   72,
 /*   120 */    74,   76,   78,   68,   70,   48,   46,   50,   43,   40,
 /*   130 */    56,   89,   58,   84,   87,   88,   96,   80,  317,  312,
 /*   140 */    82,   92,  235,   60,   52,   54,   66,   62,   64,   72,
 /*   150 */    74,   76,   78,   68,   70,   48,   46,   50,   43,   40,
 /*   160 */    56,   89,   58,   84,   87,   88,   96,   80,  181,   26,
 /*   170 */    82,   92,  136,   60,   52,   54,   66,   62,   64,   72,
 /*   180 */    74,   76,   78,   68,   70,   48,   46,   50,   43,   40,
 /*   190 */    56,   89,   58,   84,   87,   88,   96,   80,  180,   39,
 /*   200 */    82,   92,  108,   60,   52,   54,   66,   62,   64,   72,
 /*   210 */    74,   76,   78,   68,   70,   48,   46,   50,   43,   40,
 /*   220 */    56,   89,   58,   84,   87,   88,   96,   80,   82,   92,
 /*   230 */    94,   60,   52,   54,   66,   62,   64,   72,   74,   76,
 /*   240 */    78,   68,   70,   48,   46,   50,   43,   40,   56,   89,
 /*   250 */    58,   84,   87,   88,   96,   80,   27,  259,    4,    5,
 /*   260 */     6,    7,    8,    9,   10,   11,   12,   13,   14,   15,
 /*   270 */    16,   17,   18,   19,   20,   21,   22,   91,  128,  128,
 /*   280 */   107,   82,   92,  289,   60,   52,   54,   66,   62,   64,
 /*   290 */    72,   74,   76,   78,   68,   70,   48,   46,   50,   43,
 /*   300 */    40,   56,   89,   58,   84,   87,   88,   96,   80,   48,
 /*   310 */    46,   50,   43,   40,   56,   89,   58,   84,   87,   88,
 /*   320 */    96,   80,  115,  127,  208,  102,  179,  194,  283,   82,
 /*   330 */    92,  137,   60,   52,   54,   66,   62,   64,   72,   74,
 /*   340 */    76,   78,   68,   70,   48,   46,   50,   43,   40,   56,
 /*   350 */    89,   58,   84,   87,   88,   96,   80,  131,  237,   82,
 /*   360 */    92,  143,   60,   52,   54,   66,   62,   64,   72,   74,
 /*   370 */    76,   78,   68,   70,   48,   46,   50,   43,   40,   56,
 /*   380 */    89,   58,   84,   87,   88,   96,   80,  462,  240,   82,
 /*   390 */    92,  463,   60,   52,   54,   66,   62,   64,   72,   74,
 /*   400 */    76,   78,   68,   70,   48,   46,   50,   43,   40,   56,
 /*   410 */    89,   58,   84,   87,   88,   96,   80,  141,  243,   82,
 /*   420 */    92,  144,   60,   52,   54,   66,   62,   64,   72,   74,
 /*   430 */    76,   78,   68,   70,   48,   46,   50,   43,   40,   56,
 /*   440 */    89,   58,   84,   87,   88,   96,   80,   92,  170,   60,
 /*   450 */    52,   54,   66,   62,   64,   72,   74,   76,   78,   68,
 /*   460 */    70,   48,   46,   50,   43,   40,   56,   89,   58,   84,
 /*   470 */    87,   88,   96,   80,  257,  282,  300,  462,   82,   92,
 /*   480 */   168,   60,   52,   54,   66,   62,   64,   72,   74,   76,
 /*   490 */    78,   68,   70,   48,   46,   50,   43,   40,   56,   89,
 /*   500 */    58,   84,   87,   88,   96,   80,  463,  303,   82,   92,
 /*   510 */   177,   60,   52,   54,   66,   62,   64,   72,   74,   76,
 /*   520 */    78,   68,   70,   48,   46,   50,   43,   40,   56,   89,
 /*   530 */    58,   84,   87,   88,   96,   80,  183,  319,   82,   92,
 /*   540 */   182,   60,   52,   54,   66,   62,   64,   72,   74,   76,
 /*   550 */    78,   68,   70,   48,   46,   50,   43,   40,   56,   89,
 /*   560 */    58,   84,   87,   88,   96,   80,   82,   92,  196,   60,
 /*   570 */    52,   54,   66,   62,   64,   72,   74,   76,   78,   68,
 /*   580 */    70,   48,   46,   50,   43,   40,   56,   89,   58,   84,
 /*   590 */    87,   88,  206,   80,   60,   52,   54,   66,   62,   64,
 /*   600 */    72,   74,   76,   78,   68,   70,   48,   46,   50,   43,
 /*   610 */    40,   56,   89,   58,   84,   87,   88,   96,   80,   52,
 /*   620 */    54,   66,   62,   64,   72,   74,   76,   78,   68,   70,
 /*   630 */    48,   46,   50,   43,   40,   56,   89,   58,   84,   87,
 /*   640 */    88,   96,   80,  483,    1,    2,  259,    4,    5,    6,
 /*   650 */     7,    8,    9,   10,   11,   12,   13,   14,   15,   16,
 /*   660 */    17,   18,   19,   20,   21,   22,   66,   62,   64,   72,
 /*   670 */    74,   76,   78,   68,   70,   48,   46,   50,   43,   40,
 /*   680 */    56,   89,   58,   84,   87,   88,   96,   80,   33,  259,
 /*   690 */     4,    5,    6,    7,    8,    9,   10,   11,   12,   13,
 /*   700 */    14,   15,   16,   17,   18,   19,   20,   21,   22,  174,
 /*   710 */   259,    4,    5,    6,    7,    8,    9,   10,   11,   12,
 /*   720 */    13,   14,   15,   16,   17,   18,   19,   20,   21,   22,
 /*   730 */   200,  259,    4,    5,    6,    7,    8,    9,   10,   11,
 /*   740 */    12,   13,   14,   15,   16,   17,   18,   19,   20,   21,
 /*   750 */    22,  210,  259,    4,    5,    6,    7,    8,    9,   10,
 /*   760 */    11,   12,   13,   14,   15,   16,   17,   18,   19,   20,
 /*   770 */    21,   22,  218,  259,    4,    5,    6,    7,    8,    9,
 /*   780 */    10,   11,   12,   13,   14,   15,   16,   17,   18,   19,
 /*   790 */    20,   21,   22,  225,  259,    4,    5,    6,    7,    8,
 /*   800 */     9,   10,   11,   12,   13,   14,   15,   16,   17,   18,
 /*   810 */    19,   20,   21,   22,  247,  259,    4,    5,    6,    7,
 /*   820 */     8,    9,   10,   11,   12,   13,   14,   15,   16,   17,
 /*   830 */    18,   19,   20,   21,   22,  262,  259,    4,    5,    6,
 /*   840 */     7,    8,    9,   10,   11,   12,   13,   14,   15,   16,
 /*   850 */    17,   18,   19,   20,   21,   22,  268,  259,    4,    5,
 /*   860 */     6,    7,    8,    9,   10,   11,   12,   13,   14,   15,
 /*   870 */    16,   17,   18,   19,   20,   21,   22,  285,  259,    4,
 /*   880 */     5,    6,    7,    8,    9,   10,   11,   12,   13,   14,
 /*   890 */    15,   16,   17,   18,   19,   20,   21,   22,  304,  259,
 /*   900 */     4,    5,    6,    7,    8,    9,   10,   11,   12,   13,
 /*   910 */    14,   15,   16,   17,   18,   19,   20,   21,   22,  313,
 /*   920 */   259,    4,    5,    6,    7,    8,    9,   10,   11,   12,
 /*   930 */    13,   14,   15,   16,   17,   18,   19,   20,   21,   22,
 /*   940 */   320,  259,    4,    5,    6,    7,    8,    9,   10,   11,
 /*   950 */    12,   13,   14,   15,   16,   17,   18,   19,   20,   21,
 /*   960 */    22,    3,    4,    5,    6,    7,    8,    9,   10,   11,
 /*   970 */    12,   13,   14,   15,   16,   17,   18,   19,   20,   21,
 /*   980 */    22,   44,   41,  197,  103,  199,   98,  203,  291,  100,
 /*   990 */   209,   44,   41,  213,  103,  217,   98,  198,  118,  100,
 /*  1000 */    89,   58,   84,   87,   88,   96,   80,  104,  118,  119,
 /*  1010 */   120,  121,  122,  123,  124,  106,  216,  113,  117,  119,
 /*  1020 */   120,  121,  122,  123,  124,  266,  294,  295,  296,  297,
 /*  1030 */   298,  299,  230,  154,  155,  156,  157,  158,  159,  160,
 /*  1040 */   221,  109,  228,  147,  148,  149,  150,  151,  152,  153,
 /*  1050 */   231,  109,   24,  214,   29,   31,   37,  167,  169,  336,
 /*  1060 */    23,  178,  279,  139,   90,  246,   97,  195,  184,  186,
 /*  1070 */   188,  190,  192,  140,  138,  117,  204,  117,  129,  280,
 /*  1080 */   215,  250,  222,  252,  229,  232,   23,  238,  241,  244,
 /*  1090 */   327,  251,  253,   24,  214,  328,  330,   37,  167,  169,
 /*  1100 */    97,  254,  178,  291,  104,  142,  126,   97,  195,   25,
 /*  1110 */   290,  117,  134,  207,  125,  117,  255,  204,  117,  134,
 /*  1120 */   117,  215,  261,  222,  265,  229,  232,  267,  238,  241,
 /*  1130 */   244,  258,  251,  253,   24,  214,   35,  104,   37,  167,
 /*  1140 */   169,   38,  255,  178,  166,  111,  255,  113,  117,  195,
 /*  1150 */   245,  260,  117,  271,   42,  117,  397,  258,  204,  398,
 /*  1160 */   276,  258,  215,   28,  222,  117,  229,  232,  324,  238,
 /*  1170 */   241,  244,   34,  251,  253,   24,  214,  165,   45,   37,
 /*  1180 */   167,  169,   47,  176,  178,   49,   51,  139,  117,  117,
 /*  1190 */   195,  278,  281,  117,  293,   53,  117,  117,  138,  204,
 /*  1200 */   284,  288,  292,  215,  307,  222,  117,  229,  232,  175,
 /*  1210 */   238,  241,  244,  201,  251,  253,   24,  214,  309,  255,
 /*  1220 */    37,  167,  169,  310,  255,  178,   55,   57,  255,  316,
 /*  1230 */   211,  195,  323,  202,  258,  274,   59,  117,  117,  258,
 /*  1240 */   204,  326,  329,  258,  215,  331,  222,  117,  229,  232,
 /*  1250 */   332,  238,  241,  244,  219,  251,  253,   24,  214,  256,
 /*  1260 */    61,   37,  167,  169,  333,  255,  178,  334,  269,  255,
 /*  1270 */   117,  117,  195,  269,  269,  269,  269,   63,  269,  269,
 /*  1280 */   258,  204,  212,  269,  258,  215,  255,  222,  117,  229,
 /*  1290 */   232,  255,  238,  241,  244,  226,  251,  253,   24,  214,
 /*  1300 */   269,  258,   37,  167,  169,   65,  258,  178,  269,  269,
 /*  1310 */   255,  269,  248,  195,  269,  269,  117,  269,   67,  269,
 /*  1320 */   269,  269,  204,  269,  269,  258,  215,  220,  222,  117,
 /*  1330 */   229,  232,  263,  238,  241,  244,  269,  251,  253,   24,
 /*  1340 */   214,  269,  269,   37,  167,  169,   69,   71,  178,  269,
 /*  1350 */   269,  255,  269,  286,  195,  269,  269,  117,  117,  269,
 /*  1360 */   269,  269,  269,  204,  269,  269,  258,  215,  255,  222,
 /*  1370 */   227,  229,  232,  305,  238,  241,  244,  314,  251,  253,
 /*  1380 */    24,  214,  269,  258,   37,  167,  169,  269,  255,  178,
 /*  1390 */    73,  269,  255,  269,  321,  195,  269,  269,  269,  269,
 /*  1400 */    75,  117,  269,  258,  204,  269,  269,  258,  215,  255,
 /*  1410 */   222,  117,  229,  232,  269,  238,  241,  244,  249,  251,
 /*  1420 */   253,   24,  214,  269,  258,   37,  167,  169,  269,  255,
 /*  1430 */   178,  269,  269,  255,  269,   77,  195,  269,  269,  269,
 /*  1440 */   269,   79,  269,  269,  258,  204,  117,  269,  258,  215,
 /*  1450 */   255,  222,  117,  229,  232,  269,  238,  241,  244,  264,
 /*  1460 */   251,  253,   24,  214,  269,  258,   37,  167,  169,  269,
 /*  1470 */   269,  178,  269,  269,   81,  269,   83,  195,   86,  269,
 /*  1480 */    93,  269,  269,  269,  269,  117,  204,  117,  269,  117,
 /*  1490 */   215,  117,  222,  270,  229,  232,  269,  238,  241,  244,
 /*  1500 */   269,  251,  253,   24,  214,  269,  269,   37,  167,  169,
 /*  1510 */   269,   95,  178,  269,  269,   99,  269,  101,  195,  116,
 /*  1520 */   269,  269,  117,  132,  269,  269,  117,  204,  117,  269,
 /*  1530 */   117,  215,  275,  222,  117,  229,  232,  269,  238,  241,
 /*  1540 */   244,  162,  251,  253,   24,  214,  269,  269,   37,  167,
 /*  1550 */   169,  269,  117,  178,  164,  269,  172,  269,  185,  195,
 /*  1560 */   187,  287,  189,  269,  191,  117,  269,  117,  204,  117,
 /*  1570 */   269,  117,  215,  117,  222,  117,  229,  232,  269,  238,
 /*  1580 */   241,  244,  269,  251,  253,   24,  214,  269,  193,   37,
 /*  1590 */   167,  169,  269,  306,  178,  205,  269,  223,  269,  117,
 /*  1600 */   195,  233,  269,  236,  269,  239,  117,  269,  117,  204,
 /*  1610 */   269,  269,  117,  215,  117,  222,  117,  229,  232,  269,
 /*  1620 */   238,  241,  244,  269,  251,  253,   24,  214,  269,  269,
 /*  1630 */    37,  167,  169,  269,  315,  178,  242,  269,  302,  269,
 /*  1640 */   311,  195,  318,  269,  269,  269,  269,  117,  269,  117,
 /*  1650 */   204,  117,  269,  117,  215,  269,  222,  269,  229,  232,
 /*  1660 */   269,  238,  241,  244,  269,  251,  253,   24,  214,  269,
 /*  1670 */   269,   37,  167,  169,  269,  322,  178,  269,  269,  269,
 /*  1680 */   269,  269,  195,  269,  269,  269,  269,  269,  269,  269,
 /*  1690 */   269,  204,  269,  269,  269,  215,  269,  222,  269,  229,
 /*  1700 */   232,  269,  238,  241,  244,  269,  251,  253,   24,  214,
 /*  1710 */   325,  269,   37,  167,  169,  269,  269,  178,  269,  269,
 /*  1720 */   269,  269,  269,  195,  269,  269,  269,  269,  269,  269,
 /*  1730 */   269,  269,  204,  269,  269,  269,  215,  269,  222,  269,
 /*  1740 */   229,  232,  269,  238,  241,  244,  269,  251,  253,   24,
 /*  1750 */   214,  269,  269,   37,  167,  169,  269,  269,  178,  269,
 /*  1760 */   269,  269,  269,  269,  195,  269,  269,  269,  269,  269,
 /*  1770 */   269,  269,  269,  204,  269,  269,  269,  215,  269,  222,
 /*  1780 */   269,  229,  232,  269,  238,  241,  244,  269,  251,  253,
 /*  1790 */   269,  269,  269,  163,  269,   44,   41,  269,  103,  269,
 /*  1800 */    98,  269,  269,  100,  269,  269,  269,  269,  269,  269,
 /*  1810 */   269,  269,  118,  269,  269,  269,  269,  269,  269,  269,
 /*  1820 */   269,  269,  269,  119,  120,  121,  122,  123,  124,  269,
 /*  1830 */   269,  161,  269,  269,   44,   41,  269,  103,  269,   98,
 /*  1840 */   269,  269,  100,  269,  269,  269,  269,  269,  269,  269,
 /*  1850 */   269,  118,  269,  269,  269,  109,  269,  135,  269,  269,
 /*  1860 */   269,  269,  119,  120,  121,  122,  123,  124,  269,  269,
 /*  1870 */   269,  269,  269,   44,   41,  269,  103,  269,   98,  269,
 /*  1880 */   269,  100,  269,  269,  269,  269,  269,  269,  269,  269,
 /*  1890 */   146,  269,  269,  269,  109,  269,  269,  269,  269,  269,
 /*  1900 */   269,  145,  120,  121,  122,  123,  124,  269,  269,  269,
 /*  1910 */   269,  269,   44,   41,  269,  103,  269,   98,  269,  269,
 /*  1920 */   100,  269,  269,  269,  269,  269,  269,  269,  269,  118,
 /*  1930 */   269,  269,  269,  109,  269,  269,  269,  269,  133,  269,
 /*  1940 */   119,  130,  121,  122,  123,  124,  269,  269,  269,  269,
 /*  1950 */   269,   44,   41,  269,  103,  269,   98,  269,  269,  100,
 /*  1960 */   269,  269,  269,  269,  269,  269,  269,  269,  118,  269,
 /*  1970 */   269,  269,  109,  269,  269,  269,  269,  269,  269,  119,
 /*  1980 */   114,  121,  122,  123,  124,  269,  269,  269,  269,  269,
 /*  1990 */    44,   41,  269,  103,  269,   98,  269,  269,  100,  269,
 /*  2000 */   269,  269,  269,  269,  269,  269,  269,  118,  269,  269,
 /*  2010 */   105,  109,  269,  269,  269,  269,  269,  269,  119,  114,
 /*  2020 */   121,  122,  123,  124,  269,  269,  269,  269,  269,   44,
 /*  2030 */    41,  269,  103,  269,   98,  269,  269,  100,  269,  269,
 /*  2040 */   269,  269,  269,  269,  269,  269,  118,  269,  269,  269,
 /*  2050 */   109,  110,  269,  269,  269,  277,  269,  119,  130,  121,
 /*  2060 */   122,  123,  124,  269,  269,  269,  269,  269,   44,   41,
 /*  2070 */   269,  103,  269,   98,  269,  269,  100,  269,  269,  269,
 /*  2080 */   269,  269,  269,  269,  269,  118,  269,  269,  269,  109,
 /*  2090 */   269,  269,  269,  269,  269,  269,  119,  120,  121,  122,
 /*  2100 */   123,  124,  269,  269,  269,  269,  269,   44,   41,  269,
 /*  2110 */   103,  269,   98,  269,  269,  100,  269,  269,  269,  269,
 /*  2120 */   269,  269,  269,  269,  118,  269,  269,  269,  109,  269,
 /*  2130 */   269,  269,  269,  269,  269,  119,  114,  121,  122,  123,
 /*  2140 */   124,  269,  269,  269,  269,  269,   44,   41,  269,  103,
 /*  2150 */   269,   98,  269,  269,  100,  269,  269,  269,  269,  269,
 /*  2160 */   269,  269,  269,  118,  269,  269,  269,  109,  269,  269,
 /*  2170 */   269,  269,  269,  269,  119,  130,  121,  122,  123,  124,
 /*  2180 */   269,  269,  269,  269,  269,  269,  269,  269,  269,  269,
 /*  2190 */   269,  269,  269,  269,  269,  269,  269,  269,  269,  269,
 /*  2200 */   269,  269,  269,  269,  269,  269,  109,
};
static YYCODETYPE yy_lookahead[] = {
 /*     0 */     3,    4,    2,    6,    7,    8,    9,   10,   11,   12,
 /*    10 */    13,   14,   15,   16,   17,   18,   19,   20,   21,   22,
 /*    20 */    23,   24,   25,   26,   27,   28,   29,   30,    0,   32,
 /*    30 */    26,   27,   28,   29,   30,   38,   21,   22,   23,   24,
 /*    40 */    25,   26,   27,   28,   29,   30,   49,    2,    3,   32,
 /*    50 */     3,    4,   32,    6,    7,    8,    9,   10,   11,   12,
 /*    60 */    13,   14,   15,   16,   17,   18,   19,   20,   21,   22,
 /*    70 */    23,   24,   25,   26,   27,   28,   29,   30,   31,   32,
 /*    80 */     3,    4,   82,    6,    7,    8,    9,   10,   11,   12,
 /*    90 */    13,   14,   15,   16,   17,   18,   19,   20,   21,   22,
 /*   100 */    23,   24,   25,   26,   27,   28,   29,   30,   32,   32,
 /*   110 */     3,    4,    3,    6,    7,    8,    9,   10,   11,   12,
 /*   120 */    13,   14,   15,   16,   17,   18,   19,   20,   21,   22,
 /*   130 */    23,   24,   25,   26,   27,   28,   29,   30,   31,   32,
 /*   140 */     3,    4,   65,    6,    7,    8,    9,   10,   11,   12,
 /*   150 */    13,   14,   15,   16,   17,   18,   19,   20,   21,   22,
 /*   160 */    23,   24,   25,   26,   27,   28,   29,   30,    2,   32,
 /*   170 */     3,    4,  116,    6,    7,    8,    9,   10,   11,   12,
 /*   180 */    13,   14,   15,   16,   17,   18,   19,   20,   21,   22,
 /*   190 */    23,   24,   25,   26,   27,   28,   29,   30,   32,   32,
 /*   200 */     3,    4,    2,    6,    7,    8,    9,   10,   11,   12,
 /*   210 */    13,   14,   15,   16,   17,   18,   19,   20,   21,   22,
 /*   220 */    23,   24,   25,   26,   27,   28,   29,   30,    3,    4,
 /*   230 */     5,    6,    7,    8,    9,   10,   11,   12,   13,   14,
 /*   240 */    15,   16,   17,   18,   19,   20,   21,   22,   23,   24,
 /*   250 */    25,   26,   27,   28,   29,   30,   86,   87,   88,   89,
 /*   260 */    90,   91,   92,   93,   94,   95,   96,   97,   98,   99,
 /*   270 */   100,  101,  102,  103,  104,  105,  106,   80,    2,    2,
 /*   280 */    80,    3,    4,    2,    6,    7,    8,    9,   10,   11,
 /*   290 */    12,   13,   14,   15,   16,   17,   18,   19,   20,   21,
 /*   300 */    22,   23,   24,   25,   26,   27,   28,   29,   30,   18,
 /*   310 */    19,   20,   21,   22,   23,   24,   25,   26,   27,   28,
 /*   320 */    29,   30,    5,   47,   47,   47,  108,  109,   47,    3,
 /*   330 */     4,   80,    6,    7,    8,    9,   10,   11,   12,   13,
 /*   340 */    14,   15,   16,   17,   18,   19,   20,   21,   22,   23,
 /*   350 */    24,   25,   26,   27,   28,   29,   30,    5,   32,    3,
 /*   360 */     4,  116,    6,    7,    8,    9,   10,   11,   12,   13,
 /*   370 */    14,   15,   16,   17,   18,   19,   20,   21,   22,   23,
 /*   380 */    24,   25,   26,   27,   28,   29,   30,   80,   32,    3,
 /*   390 */     4,   80,    6,    7,    8,    9,   10,   11,   12,   13,
 /*   400 */    14,   15,   16,   17,   18,   19,   20,   21,   22,   23,
 /*   410 */    24,   25,   26,   27,   28,   29,   30,    5,   32,    3,
 /*   420 */     4,   80,    6,    7,    8,    9,   10,   11,   12,   13,
 /*   430 */    14,   15,   16,   17,   18,   19,   20,   21,   22,   23,
 /*   440 */    24,   25,   26,   27,   28,   29,   30,    4,   38,    6,
 /*   450 */     7,    8,    9,   10,   11,   12,   13,   14,   15,   16,
 /*   460 */    17,   18,   19,   20,   21,   22,   23,   24,   25,   26,
 /*   470 */    27,   28,   29,   30,   58,  110,  111,    5,    3,    4,
 /*   480 */    32,    6,    7,    8,    9,   10,   11,   12,   13,   14,
 /*   490 */    15,   16,   17,   18,   19,   20,   21,   22,   23,   24,
 /*   500 */    25,   26,   27,   28,   29,   30,    5,   32,    3,    4,
 /*   510 */    32,    6,    7,    8,    9,   10,   11,   12,   13,   14,
 /*   520 */    15,   16,   17,   18,   19,   20,   21,   22,   23,   24,
 /*   530 */    25,   26,   27,   28,   29,   30,   38,   32,    3,    4,
 /*   540 */   109,    6,    7,    8,    9,   10,   11,   12,   13,   14,
 /*   550 */    15,   16,   17,   18,   19,   20,   21,   22,   23,   24,
 /*   560 */    25,   26,   27,   28,   29,   30,    3,    4,   38,    6,
 /*   570 */     7,    8,    9,   10,   11,   12,   13,   14,   15,   16,
 /*   580 */    17,   18,   19,   20,   21,   22,   23,   24,   25,   26,
 /*   590 */    27,   28,   29,   30,    6,    7,    8,    9,   10,   11,
 /*   600 */    12,   13,   14,   15,   16,   17,   18,   19,   20,   21,
 /*   610 */    22,   23,   24,   25,   26,   27,   28,   29,   30,    7,
 /*   620 */     8,    9,   10,   11,   12,   13,   14,   15,   16,   17,
 /*   630 */    18,   19,   20,   21,   22,   23,   24,   25,   26,   27,
 /*   640 */    28,   29,   30,   84,   85,   86,   87,   88,   89,   90,
 /*   650 */    91,   92,   93,   94,   95,   96,   97,   98,   99,  100,
 /*   660 */   101,  102,  103,  104,  105,  106,    9,   10,   11,   12,
 /*   670 */    13,   14,   15,   16,   17,   18,   19,   20,   21,   22,
 /*   680 */    23,   24,   25,   26,   27,   28,   29,   30,   86,   87,
 /*   690 */    88,   89,   90,   91,   92,   93,   94,   95,   96,   97,
 /*   700 */    98,   99,  100,  101,  102,  103,  104,  105,  106,   86,
 /*   710 */    87,   88,   89,   90,   91,   92,   93,   94,   95,   96,
 /*   720 */    97,   98,   99,  100,  101,  102,  103,  104,  105,  106,
 /*   730 */    86,   87,   88,   89,   90,   91,   92,   93,   94,   95,
 /*   740 */    96,   97,   98,   99,  100,  101,  102,  103,  104,  105,
 /*   750 */   106,   86,   87,   88,   89,   90,   91,   92,   93,   94,
 /*   760 */    95,   96,   97,   98,   99,  100,  101,  102,  103,  104,
 /*   770 */   105,  106,   86,   87,   88,   89,   90,   91,   92,   93,
 /*   780 */    94,   95,   96,   97,   98,   99,  100,  101,  102,  103,
 /*   790 */   104,  105,  106,   86,   87,   88,   89,   90,   91,   92,
 /*   800 */    93,   94,   95,   96,   97,   98,   99,  100,  101,  102,
 /*   810 */   103,  104,  105,  106,   86,   87,   88,   89,   90,   91,
 /*   820 */    92,   93,   94,   95,   96,   97,   98,   99,  100,  101,
 /*   830 */   102,  103,  104,  105,  106,   86,   87,   88,   89,   90,
 /*   840 */    91,   92,   93,   94,   95,   96,   97,   98,   99,  100,
 /*   850 */   101,  102,  103,  104,  105,  106,   86,   87,   88,   89,
 /*   860 */    90,   91,   92,   93,   94,   95,   96,   97,   98,   99,
 /*   870 */   100,  101,  102,  103,  104,  105,  106,   86,   87,   88,
 /*   880 */    89,   90,   91,   92,   93,   94,   95,   96,   97,   98,
 /*   890 */    99,  100,  101,  102,  103,  104,  105,  106,   86,   87,
 /*   900 */    88,   89,   90,   91,   92,   93,   94,   95,   96,   97,
 /*   910 */    98,   99,  100,  101,  102,  103,  104,  105,  106,   86,
 /*   920 */    87,   88,   89,   90,   91,   92,   93,   94,   95,   96,
 /*   930 */    97,   98,   99,  100,  101,  102,  103,  104,  105,  106,
 /*   940 */    86,   87,   88,   89,   90,   91,   92,   93,   94,   95,
 /*   950 */    96,   97,   98,   99,  100,  101,  102,  103,  104,  105,
 /*   960 */   106,   87,   88,   89,   90,   91,   92,   93,   94,   95,
 /*   970 */    96,   97,   98,   99,  100,  101,  102,  103,  104,  105,
 /*   980 */   106,   21,   22,   29,   24,   32,   26,   32,   38,   29,
 /*   990 */    32,   21,   22,   32,   24,   32,   26,   47,   38,   29,
 /*  1000 */    24,   25,   26,   27,   28,   29,   30,  107,   38,   49,
 /*  1010 */    50,   51,   52,   53,   54,  115,   38,  117,  118,   49,
 /*  1020 */    50,   51,   52,   53,   54,  114,   49,   50,   51,   52,
 /*  1030 */    53,   54,   50,   73,   74,   75,   76,   77,   78,   79,
 /*  1040 */    32,   81,   32,   73,   74,   75,   76,   77,   78,   79,
 /*  1050 */    32,   81,   31,   32,   33,   34,   35,   36,   37,    0,
 /*  1060 */     1,   40,    1,   38,  107,   32,  107,   46,   41,   42,
 /*  1070 */    43,   44,   45,  116,   49,  118,   55,  118,  119,   56,
 /*  1080 */    59,   32,   61,   32,   63,   64,    1,   66,   67,   68,
 /*  1090 */     1,   70,   71,   31,   32,   33,   34,   35,   36,   37,
 /*  1100 */   107,   32,   40,   38,  107,   80,  113,  107,   46,  107,
 /*  1110 */   111,  118,  119,  113,  117,  118,   57,   55,  118,  119,
 /*  1120 */   118,   59,   32,   61,   32,   63,   64,   32,   66,   67,
 /*  1130 */    68,   72,   70,   71,   31,   32,   33,  107,   35,   36,
 /*  1140 */    37,  107,   57,   40,  107,  115,   57,  117,  118,   46,
 /*  1150 */    53,   54,  118,   32,  107,  118,   32,   72,   55,   32,
 /*  1160 */    32,   72,   59,    1,   61,  118,   63,   64,    1,   66,
 /*  1170 */    67,   68,    1,   70,   71,   31,   32,  107,  107,   35,
 /*  1180 */    36,   37,  107,   39,   40,  107,  107,   38,  118,  118,
 /*  1190 */    46,   32,   32,  118,  112,  107,  118,  118,   49,   55,
 /*  1200 */    32,   32,   41,   59,   32,   61,  118,   63,   64,    1,
 /*  1210 */    66,   67,   68,    1,   70,   71,   31,   32,   38,   57,
 /*  1220 */    35,   36,   37,    3,   57,   40,  107,  107,   57,   32,
 /*  1230 */     1,   46,   32,   48,   72,    1,  107,  118,  118,   72,
 /*  1240 */    55,   32,   32,   72,   59,   32,   61,  118,   63,   64,
 /*  1250 */     1,   66,   67,   68,    1,   70,   71,   31,   32,  107,
 /*  1260 */   107,   35,   36,   37,   33,   57,   40,   32,  120,   57,
 /*  1270 */   118,  118,   46,  120,  120,  120,  120,  107,  120,  120,
 /*  1280 */    72,   55,   56,  120,   72,   59,   57,   61,  118,   63,
 /*  1290 */    64,   57,   66,   67,   68,    1,   70,   71,   31,   32,
 /*  1300 */   120,   72,   35,   36,   37,  107,   72,   40,  120,  120,
 /*  1310 */    57,  120,    1,   46,  120,  120,  118,  120,  107,  120,
 /*  1320 */   120,  120,   55,  120,  120,   72,   59,   60,   61,  118,
 /*  1330 */    63,   64,    1,   66,   67,   68,    1,   70,   71,   31,
 /*  1340 */    32,  120,  120,   35,   36,   37,  107,  107,   40,  120,
 /*  1350 */   120,   57,  120,    1,   46,  120,  120,  118,  118,  120,
 /*  1360 */   120,  120,  120,   55,  120,  120,   72,   59,   57,   61,
 /*  1370 */    62,   63,   64,    1,   66,   67,   68,    1,   70,   71,
 /*  1380 */    31,   32,  120,   72,   35,   36,   37,  120,   57,   40,
 /*  1390 */   107,  120,   57,  120,    1,   46,  120,  120,  120,  120,
 /*  1400 */   107,  118,  120,   72,   55,  120,  120,   72,   59,   57,
 /*  1410 */    61,  118,   63,   64,  120,   66,   67,   68,   69,   70,
 /*  1420 */    71,   31,   32,  120,   72,   35,   36,   37,  120,   57,
 /*  1430 */    40,  120,  120,   57,  120,  107,   46,  120,  120,  120,
 /*  1440 */   120,  107,  120,  120,   72,   55,  118,  120,   72,   59,
 /*  1450 */    57,   61,  118,   63,   64,  120,   66,   67,   68,   69,
 /*  1460 */    70,   71,   31,   32,  120,   72,   35,   36,   37,  120,
 /*  1470 */   120,   40,  120,  120,  107,  120,  107,   46,  107,  120,
 /*  1480 */   107,  120,  120,  120,  120,  118,   55,  118,  120,  118,
 /*  1490 */    59,  118,   61,   62,   63,   64,  120,   66,   67,   68,
 /*  1500 */   120,   70,   71,   31,   32,  120,  120,   35,   36,   37,
 /*  1510 */   120,  107,   40,  120,  120,  107,  120,  107,   46,  107,
 /*  1520 */   120,  120,  118,  107,  120,  120,  118,   55,  118,  120,
 /*  1530 */   118,   59,   60,   61,  118,   63,   64,  120,   66,   67,
 /*  1540 */    68,  107,   70,   71,   31,   32,  120,  120,   35,   36,
 /*  1550 */    37,  120,  118,   40,  107,  120,  107,  120,  107,   46,
 /*  1560 */   107,   48,  107,  120,  107,  118,  120,  118,   55,  118,
 /*  1570 */   120,  118,   59,  118,   61,  118,   63,   64,  120,   66,
 /*  1580 */    67,   68,  120,   70,   71,   31,   32,  120,  107,   35,
 /*  1590 */    36,   37,  120,   39,   40,  107,  120,  107,  120,  118,
 /*  1600 */    46,  107,  120,  107,  120,  107,  118,  120,  118,   55,
 /*  1610 */   120,  120,  118,   59,  118,   61,  118,   63,   64,  120,
 /*  1620 */    66,   67,   68,  120,   70,   71,   31,   32,  120,  120,
 /*  1630 */    35,   36,   37,  120,   39,   40,  107,  120,  107,  120,
 /*  1640 */   107,   46,  107,  120,  120,  120,  120,  118,  120,  118,
 /*  1650 */    55,  118,  120,  118,   59,  120,   61,  120,   63,   64,
 /*  1660 */   120,   66,   67,   68,  120,   70,   71,   31,   32,  120,
 /*  1670 */   120,   35,   36,   37,  120,   39,   40,  120,  120,  120,
 /*  1680 */   120,  120,   46,  120,  120,  120,  120,  120,  120,  120,
 /*  1690 */   120,   55,  120,  120,  120,   59,  120,   61,  120,   63,
 /*  1700 */    64,  120,   66,   67,   68,  120,   70,   71,   31,   32,
 /*  1710 */    33,  120,   35,   36,   37,  120,  120,   40,  120,  120,
 /*  1720 */   120,  120,  120,   46,  120,  120,  120,  120,  120,  120,
 /*  1730 */   120,  120,   55,  120,  120,  120,   59,  120,   61,  120,
 /*  1740 */    63,   64,  120,   66,   67,   68,  120,   70,   71,   31,
 /*  1750 */    32,  120,  120,   35,   36,   37,  120,  120,   40,  120,
 /*  1760 */   120,  120,  120,  120,   46,  120,  120,  120,  120,  120,
 /*  1770 */   120,  120,  120,   55,  120,  120,  120,   59,  120,   61,
 /*  1780 */   120,   63,   64,  120,   66,   67,   68,  120,   70,   71,
 /*  1790 */   120,  120,  120,   19,  120,   21,   22,  120,   24,  120,
 /*  1800 */    26,  120,  120,   29,  120,  120,  120,  120,  120,  120,
 /*  1810 */   120,  120,   38,  120,  120,  120,  120,  120,  120,  120,
 /*  1820 */   120,  120,  120,   49,   50,   51,   52,   53,   54,  120,
 /*  1830 */   120,   18,  120,  120,   21,   22,  120,   24,  120,   26,
 /*  1840 */   120,  120,   29,  120,  120,  120,  120,  120,  120,  120,
 /*  1850 */   120,   38,  120,  120,  120,   81,  120,    5,  120,  120,
 /*  1860 */   120,  120,   49,   50,   51,   52,   53,   54,  120,  120,
 /*  1870 */   120,  120,  120,   21,   22,  120,   24,  120,   26,  120,
 /*  1880 */   120,   29,  120,  120,  120,  120,  120,  120,  120,  120,
 /*  1890 */    38,  120,  120,  120,   81,  120,  120,  120,  120,  120,
 /*  1900 */   120,   49,   50,   51,   52,   53,   54,  120,  120,  120,
 /*  1910 */   120,  120,   21,   22,  120,   24,  120,   26,  120,  120,
 /*  1920 */    29,  120,  120,  120,  120,  120,  120,  120,  120,   38,
 /*  1930 */   120,  120,  120,   81,  120,  120,  120,  120,   47,  120,
 /*  1940 */    49,   50,   51,   52,   53,   54,  120,  120,  120,  120,
 /*  1950 */   120,   21,   22,  120,   24,  120,   26,  120,  120,   29,
 /*  1960 */   120,  120,  120,  120,  120,  120,  120,  120,   38,  120,
 /*  1970 */   120,  120,   81,  120,  120,  120,  120,  120,  120,   49,
 /*  1980 */    50,   51,   52,   53,   54,  120,  120,  120,  120,  120,
 /*  1990 */    21,   22,  120,   24,  120,   26,  120,  120,   29,  120,
 /*  2000 */   120,  120,  120,  120,  120,  120,  120,   38,  120,  120,
 /*  2010 */    80,   81,  120,  120,  120,  120,  120,  120,   49,   50,
 /*  2020 */    51,   52,   53,   54,  120,  120,  120,  120,  120,   21,
 /*  2030 */    22,  120,   24,  120,   26,  120,  120,   29,  120,  120,
 /*  2040 */   120,  120,  120,  120,  120,  120,   38,  120,  120,  120,
 /*  2050 */    81,   82,  120,  120,  120,   47,  120,   49,   50,   51,
 /*  2060 */    52,   53,   54,  120,  120,  120,  120,  120,   21,   22,
 /*  2070 */   120,   24,  120,   26,  120,  120,   29,  120,  120,  120,
 /*  2080 */   120,  120,  120,  120,  120,   38,  120,  120,  120,   81,
 /*  2090 */   120,  120,  120,  120,  120,  120,   49,   50,   51,   52,
 /*  2100 */    53,   54,  120,  120,  120,  120,  120,   21,   22,  120,
 /*  2110 */    24,  120,   26,  120,  120,   29,  120,  120,  120,  120,
 /*  2120 */   120,  120,  120,  120,   38,  120,  120,  120,   81,  120,
 /*  2130 */   120,  120,  120,  120,  120,   49,   50,   51,   52,   53,
 /*  2140 */    54,  120,  120,  120,  120,  120,   21,   22,  120,   24,
 /*  2150 */   120,   26,  120,  120,   29,  120,  120,  120,  120,  120,
 /*  2160 */   120,  120,  120,   38,  120,  120,  120,   81,  120,  120,
 /*  2170 */   120,  120,  120,  120,   49,   50,   51,   52,   53,   54,
 /*  2180 */   120,  120,  120,  120,  120,  120,  120,  120,  120,  120,
 /*  2190 */   120,  120,  120,  120,  120,  120,  120,  120,  120,  120,
 /*  2200 */   120,  120,  120,  120,  120,  120,   81,
};
#define YY_SHIFT_USE_DFLT (-4)
static short yy_shift_ofst[] = {
 /*     0 */  1085,   28, 1059,   -4,   -4,   -4,   -4,   -4,   -4,   -4,
 /*    10 */    -4,   -4,   -4,   -4,   -4,   -4,   -4,   -4,   -4,   -4,
 /*    20 */    -4,   -4,   -4, 1718, 2047,  137, 1089, 1162, 1021,   17,
 /*    30 */    -4,   20, 1167, 1171, 1103,   76,   -4, 2047,  167,   -4,
 /*    40 */  2047, 2047,  976, 2047, 2047,  976, 1774,   15, 1813,   15,
 /*    50 */  2047,   15, 2047,  657, 2047,  657, 2047,  976, 2047,    4,
 /*    60 */  2047,  612, 2047,  291,  960,  291,  970,  291, 2047,  291,
 /*    70 */  2047,  291, 2047,  291, 2047,  291, 2047,  291, 2047,  291,
 /*    80 */  2047,   -4, 2047,  443,  109, 2047,    4,   -4,   -4, 1852,
 /*    90 */   197,   -4, 2047,  225, 2047,  588, 1891,  535, 2047,    4,
 /*   100 */  2047,  278,   -4, 1930,  535,   -4,  200,   -4, 2086, 1969,
 /*   110 */    -4,    0,   -4,   -4,  317, 2047,  535,   -4,   -4,   -4,
 /*   120 */    -4,   -4,   -4,   -4,   -4,   -4,  276,   -4, 2125,   -4,
 /*   130 */   352, 2047,  535,   -4,   -4, 1149,  251,   -4,  307,  311,
 /*   140 */   412, 1025,   -4,  341,   -4,  472,  501,   -4,   -4,   -4,
 /*   150 */    -4,   -4,   -4,   -4,   -4,   -4,   -4,   -4,   -4,   -4,
 /*   160 */    -4, 2047,   15, 2047,   15,  976,  976,  448,   -4,  410,
 /*   170 */    45, 2047,   47, 1085, 1208, 1144,  478,   -4,  498,  166,
 /*   180 */    -4,  498,   -4, 1027, 2047,  535, 2047,  535, 2047,  535,
 /*   190 */  2047,  535, 2047,  535,   -4,  530,  954,  950,  953, 1085,
 /*   200 */  1212, 1185,  955,   -4, 2047,  563, 2008,  277,  958, 1085,
 /*   210 */  1229, 1226,  961,   -4,   -4,  978,  963, 1234, 1253, 1267,
 /*   220 */  1008,   -4, 2047,   -3, 1085, 1294, 1308, 1010,   -4,  982,
 /*   230 */  1018,   -4, 2047,   77,   -4, 2047,  326,   -4, 2047,  356,
 /*   240 */    -4, 2047,  386,   -4, 1097, 1033, 1085, 1311, 1349, 1049,
 /*   250 */    -4, 1051,   -4, 1069,   -4, 2047,  416,   -4,   -4,   -4,
 /*   260 */  1090, 1085, 1331, 1390, 1092,   -4, 1095, 1085, 1335, 1431,
 /*   270 */  1121,   -4, 1124, 1127, 1472, 1128,   -4, 1159, 1061, 1023,
 /*   280 */  1160,   -4,  281, 1168, 1085, 1352, 1513, 1169,   -4, 1065,
 /*   290 */    -4, 1161,  977,   -4,   -4,   -4,   -4,   -4,   -4,   -4,
 /*   300 */    -4, 2047,  475, 1085, 1372, 1554, 1172,   -4, 1180, 1220,
 /*   310 */  2047,  107, 1085, 1376, 1595, 1197,   -4, 2047,  505, 1085,
 /*   320 */  1393, 1636, 1200,   -4, 1677, 1209,   -4, 1062, 1210,   -4,
 /*   330 */  1213, 1249, 1231, 1235,   -4,
};
#define YY_REDUCE_USE_DFLT (-1)
static short yy_reduce_ofst[] = {
 /*     0 */   559,   -1,  874,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
 /*    10 */    -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
 /*    20 */    -1,   -1,   -1,   -1, 1002,   -1,  170,  874,   -1,   -1,
 /*    30 */    -1,   -1,  602,  874,   -1,   -1,   -1, 1034,   -1,   -1,
 /*    40 */  1037, 1047,   -1, 1070, 1071,   -1, 1075,   -1, 1078,   -1,
 /*    50 */  1079,   -1, 1088,   -1, 1119,   -1, 1120,   -1, 1129,   -1,
 /*    60 */  1153,   -1, 1170,   -1, 1198,   -1, 1211,   -1, 1239,   -1,
 /*    70 */  1240,   -1, 1283,   -1, 1293,   -1, 1328,   -1, 1334,   -1,
 /*    80 */  1367,   -1, 1369,   -1,   -1, 1371,   -1,   -1,   -1,  957,
 /*    90 */    -1,   -1, 1373,   -1, 1404,   -1,  993,   -1, 1408,   -1,
 /*   100 */  1410,   -1,   -1,  900,   -1,   -1,   -1,   -1,  997, 1030,
 /*   110 */    -1,   -1,   -1,   -1,   -1, 1412,   -1,   -1,   -1,   -1,
 /*   120 */    -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  959,   -1,
 /*   130 */    -1, 1416,   -1,   -1,   -1,   56,   -1,   -1,   -1,   -1,
 /*   140 */    -1,  245,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
 /*   150 */    -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
 /*   160 */    -1, 1434,   -1, 1447,   -1,   -1,   -1,   -1,   -1,   -1,
 /*   170 */    -1, 1449,   -1,  623,  874,   -1,   -1,   -1,  218,   -1,
 /*   180 */    -1,  431,   -1,   -1, 1451,   -1, 1453,   -1, 1455,   -1,
 /*   190 */  1457,   -1, 1481,   -1,   -1,   -1,   -1,  365,   -1,  644,
 /*   200 */   874,   -1,   -1,   -1, 1488,   -1, 1000,   -1,   -1,  665,
 /*   210 */   874,   -1,   -1,   -1,   -1,   -1,   -1,  686,  874,   -1,
 /*   220 */    -1,   -1, 1490,  911,  707,  874,   -1,   -1,   -1,   -1,
 /*   230 */    -1,   -1, 1494,   -1,   -1, 1496,   -1,   -1, 1498,   -1,
 /*   240 */    -1, 1529,   -1,   -1,   -1,   -1,  728,  874,   -1,   -1,
 /*   250 */    -1,   -1,   -1,   -1,   -1, 1152,   -1,   -1,   -1,   -1,
 /*   260 */    -1,  749,  874,   -1,   -1,   -1,   -1,  770,  874,   -1,
 /*   270 */    -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
 /*   280 */    -1,   -1,   -1,   -1,  791,  874,   -1,   -1,   -1,  999,
 /*   290 */    -1,   -1, 1082,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
 /*   300 */    -1, 1531,   -1,  812,  874,   -1,   -1,   -1,   -1,   -1,
 /*   310 */  1533,   -1,  833,  874,   -1,   -1,   -1, 1535,   -1,  854,
 /*   320 */   874,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
 /*   330 */    -1,   -1,   -1,   -1,   -1,
};
static YYACTIONTYPE yy_default[] = {
 /*     0 */   482,  482,  482,  337,  339,  340,  341,  342,  343,  344,
 /*    10 */   345,  346,  347,  348,  349,  350,  351,  352,  353,  354,
 /*    20 */   355,  356,  357,  482,  482,  482,  482,  482,  482,  482,
 /*    30 */   358,  482,  482,  482,  482,  482,  360,  482,  482,  363,
 /*    40 */   482,  482,  409,  482,  482,  410,  482,  413,  482,  415,
 /*    50 */   482,  417,  482,  418,  482,  419,  482,  420,  482,  421,
 /*    60 */   482,  422,  482,  423,  482,  439,  482,  438,  482,  440,
 /*    70 */   482,  441,  482,  442,  482,  443,  482,  444,  482,  445,
 /*    80 */   482,  446,  482,  447,  482,  482,  448,  450,  451,  482,
 /*    90 */   482,  457,  482,  482,  482,  458,  482,  473,  482,  449,
 /*   100 */   482,  482,  452,  482,  467,  453,  482,  454,  482,  482,
 /*   110 */   455,  482,  456,  465,  477,  482,  466,  468,  475,  476,
 /*   120 */   477,  478,  479,  480,  481,  464,  482,  469,  482,  471,
 /*   130 */   477,  482,  474,  470,  472,  482,  482,  459,  482,  482,
 /*   140 */   482,  482,  460,  482,  461,  476,  475,  425,  427,  429,
 /*   150 */   431,  433,  435,  437,  424,  426,  428,  430,  432,  434,
 /*   160 */   436,  482,  416,  482,  414,  412,  411,  482,  364,  482,
 /*   170 */   482,  482,  482,  482,  482,  482,  482,  365,  482,  482,
 /*   180 */   369,  482,  370,  482,  482,  372,  482,  373,  482,  374,
 /*   190 */   482,  375,  482,  376,  371,  482,  482,  482,  482,  482,
 /*   200 */   482,  482,  482,  377,  482,  482,  482,  482,  469,  482,
 /*   210 */   482,  482,  482,  389,  391,  482,  482,  482,  482,  482,
 /*   220 */   482,  393,  482,  482,  482,  482,  482,  482,  395,  482,
 /*   230 */   482,  399,  482,  482,  400,  482,  482,  401,  482,  482,
 /*   240 */   402,  482,  482,  403,  482,  482,  482,  482,  482,  482,
 /*   250 */   404,  482,  406,  482,  407,  482,  482,  392,  408,  338,
 /*   260 */   482,  482,  482,  482,  482,  405,  482,  482,  482,  482,
 /*   270 */   482,  396,  482,  482,  482,  482,  394,  470,  482,  482,
 /*   280 */   482,  390,  482,  482,  482,  482,  482,  482,  378,  482,
 /*   290 */   379,  381,  482,  382,  383,  384,  385,  386,  387,  388,
 /*   300 */   380,  482,  482,  482,  482,  482,  482,  366,  482,  482,
 /*   310 */   482,  482,  482,  482,  482,  482,  367,  482,  482,  482,
 /*   320 */   482,  482,  482,  368,  482,  482,  361,  482,  482,  359,
 /*   330 */   482,  482,  482,  482,  362,
};
#define YY_SZ_ACTTAB (sizeof(yy_action)/sizeof(yy_action[0]))

/* The next table maps tokens into fallback tokens.  If a construct
** like the following:
** 
**      %fallback ID X Y Z.
**
** appears in the grammer, then ID becomes a fallback token for X, Y,
** and Z.  Whenever one of the tokens X, Y, or Z is input to the parser
** but it does not parse, the type of the token is changed to ID and
** the parse is retried before an error is thrown.
*/
#ifdef YYFALLBACK
static const YYCODETYPE yyFallback[] = {
};
#endif /* YYFALLBACK */

/* The following structure represents a single element of the
** parser's stack.  Information stored includes:
**
**   +  The state number for the parser at this level of the stack.
**
**   +  The value of the token stored at this level of the stack.
**      (In other words, the "major" token.)
**
**   +  The semantic value stored at this level of the stack.  This is
**      the information used by the action routines in the grammar.
**      It is sometimes called the "minor" token.
*/
struct yyStackEntry {
  int stateno;       /* The state-number */
  int major;         /* The major token value.  This is the code
                     ** number for the token at this stack level */
  YYMINORTYPE minor; /* The user-supplied minor token value.  This
                     ** is the value of the token  */
};
typedef struct yyStackEntry yyStackEntry;

/* The state of the parser is completely contained in an instance of
** the following structure */
struct yyParser {
  int yyidx;                    /* Index of top element in stack */
  int yyerrcnt;                 /* Shifts left before out of the error */
  phvolt_ARG_SDECL                /* A place to hold %extra_argument */
  yyStackEntry yystack[YYSTACKDEPTH];  /* The parser's stack */
};
typedef struct yyParser yyParser;

#ifndef NDEBUG
#include <stdio.h>
static FILE *yyTraceFILE = 0;
static char *yyTracePrompt = 0;
#endif /* NDEBUG */

#ifndef NDEBUG
/* 
** Turn parser tracing on by giving a stream to which to write the trace
** and a prompt to preface each trace message.  Tracing is turned off
** by making either argument NULL 
**
** Inputs:
** <ul>
** <li> A FILE* to which trace output should be written.
**      If NULL, then tracing is turned off.
** <li> A prefix string written at the beginning of every
**      line of trace output.  If NULL, then tracing is
**      turned off.
** </ul>
**
** Outputs:
** None.
*/
void phvolt_Trace(FILE *TraceFILE, char *zTracePrompt){
  yyTraceFILE = TraceFILE;
  yyTracePrompt = zTracePrompt;
  if( yyTraceFILE==0 ) yyTracePrompt = 0;
  else if( yyTracePrompt==0 ) yyTraceFILE = 0;
}
#endif /* NDEBUG */

#ifndef NDEBUG
/* For tracing shifts, the names of all terminals and nonterminals
** are required.  The following table supplies these names */
static const char *yyTokenName[] = { 
  "$",             "OPEN_DELIMITER",  "COMMA",         "IN",          
  "QUESTION",      "COLON",         "RANGE",         "AND",         
  "OR",            "IS",            "EQUALS",        "NOTEQUALS",   
  "LESS",          "GREATER",       "GREATEREQUAL",  "LESSEQUAL",   
  "IDENTICAL",     "NOTIDENTICAL",  "DIVIDE",        "TIMES",       
  "MOD",           "PLUS",          "MINUS",         "CONCAT",      
  "SBRACKET_OPEN",  "PIPE",          "NOT",           "INCR",        
  "DECR",          "PARENTHESES_OPEN",  "DOT",           "IF",          
  "CLOSE_DELIMITER",  "ENDIF",         "ELSE",          "ELSEIF",      
  "ELSEFOR",       "FOR",           "IDENTIFIER",    "ENDFOR",      
  "SET",           "ASSIGN",        "ADD_ASSIGN",    "SUB_ASSIGN",  
  "MUL_ASSIGN",    "DIV_ASSIGN",    "MACRO",         "PARENTHESES_CLOSE",
  "ENDMACRO",      "INTEGER",       "STRING",        "DOUBLE",      
  "NULL",          "FALSE",         "TRUE",          "CALL",        
  "ENDCALL",       "OPEN_EDELIMITER",  "CLOSE_EDELIMITER",  "BLOCK",       
  "ENDBLOCK",      "CACHE",         "ENDCACHE",      "EXTENDS",     
  "INCLUDE",       "WITH",          "DO",            "RETURN",      
  "AUTOESCAPE",    "ENDAUTOESCAPE",  "BREAK",         "CONTINUE",    
  "RAW_FRAGMENT",  "DEFINED",       "EMPTY",         "EVEN",        
  "ODD",           "NUMERIC",       "SCALAR",        "ITERABLE",    
  "SBRACKET_CLOSE",  "CBRACKET_OPEN",  "CBRACKET_CLOSE",  "error",       
  "program",       "volt_language",  "statement_list",  "statement",   
  "raw_fragment",  "if_statement",  "elseif_statement",  "elsefor_statement",
  "for_statement",  "set_statement",  "echo_statement",  "block_statement",
  "cache_statement",  "extends_statement",  "include_statement",  "do_statement",
  "return_statement",  "autoescape_statement",  "break_statement",  "continue_statement",
  "macro_statement",  "empty_statement",  "macro_call_statement",  "expr",        
  "set_assignments",  "set_assignment",  "macro_parameters",  "macro_parameter",
  "macro_parameter_default",  "argument_list",  "cache_lifetime",  "array_list",  
  "slice_offset",  "array_item",    "function_call",  "argument_item",
};
#endif /* NDEBUG */

#ifndef NDEBUG
/* For tracing reduce actions, the names of all rules are required.
*/
static const char *yyRuleName[] = {
 /*   0 */ "program ::= volt_language",
 /*   1 */ "volt_language ::= statement_list",
 /*   2 */ "statement_list ::= statement_list statement",
 /*   3 */ "statement_list ::= statement",
 /*   4 */ "statement ::= raw_fragment",
 /*   5 */ "statement ::= if_statement",
 /*   6 */ "statement ::= elseif_statement",
 /*   7 */ "statement ::= elsefor_statement",
 /*   8 */ "statement ::= for_statement",
 /*   9 */ "statement ::= set_statement",
 /*  10 */ "statement ::= echo_statement",
 /*  11 */ "statement ::= block_statement",
 /*  12 */ "statement ::= cache_statement",
 /*  13 */ "statement ::= extends_statement",
 /*  14 */ "statement ::= include_statement",
 /*  15 */ "statement ::= do_statement",
 /*  16 */ "statement ::= return_statement",
 /*  17 */ "statement ::= autoescape_statement",
 /*  18 */ "statement ::= break_statement",
 /*  19 */ "statement ::= continue_statement",
 /*  20 */ "statement ::= macro_statement",
 /*  21 */ "statement ::= empty_statement",
 /*  22 */ "statement ::= macro_call_statement",
 /*  23 */ "if_statement ::= OPEN_DELIMITER IF expr CLOSE_DELIMITER statement_list OPEN_DELIMITER ENDIF CLOSE_DELIMITER",
 /*  24 */ "if_statement ::= OPEN_DELIMITER IF expr CLOSE_DELIMITER OPEN_DELIMITER ENDIF CLOSE_DELIMITER",
 /*  25 */ "if_statement ::= OPEN_DELIMITER IF expr CLOSE_DELIMITER statement_list OPEN_DELIMITER ELSE CLOSE_DELIMITER statement_list OPEN_DELIMITER ENDIF CLOSE_DELIMITER",
 /*  26 */ "if_statement ::= OPEN_DELIMITER IF expr CLOSE_DELIMITER statement_list OPEN_DELIMITER ELSE CLOSE_DELIMITER OPEN_DELIMITER ENDIF CLOSE_DELIMITER",
 /*  27 */ "if_statement ::= OPEN_DELIMITER IF expr CLOSE_DELIMITER OPEN_DELIMITER ELSE CLOSE_DELIMITER OPEN_DELIMITER ENDIF CLOSE_DELIMITER",
 /*  28 */ "elseif_statement ::= OPEN_DELIMITER ELSEIF expr CLOSE_DELIMITER",
 /*  29 */ "elsefor_statement ::= OPEN_DELIMITER ELSEFOR CLOSE_DELIMITER",
 /*  30 */ "for_statement ::= OPEN_DELIMITER FOR IDENTIFIER IN expr CLOSE_DELIMITER statement_list OPEN_DELIMITER ENDFOR CLOSE_DELIMITER",
 /*  31 */ "for_statement ::= OPEN_DELIMITER FOR IDENTIFIER IN expr IF expr CLOSE_DELIMITER statement_list OPEN_DELIMITER ENDFOR CLOSE_DELIMITER",
 /*  32 */ "for_statement ::= OPEN_DELIMITER FOR IDENTIFIER COMMA IDENTIFIER IN expr CLOSE_DELIMITER statement_list OPEN_DELIMITER ENDFOR CLOSE_DELIMITER",
 /*  33 */ "for_statement ::= OPEN_DELIMITER FOR IDENTIFIER COMMA IDENTIFIER IN expr IF expr CLOSE_DELIMITER statement_list OPEN_DELIMITER ENDFOR CLOSE_DELIMITER",
 /*  34 */ "set_statement ::= OPEN_DELIMITER SET set_assignments CLOSE_DELIMITER",
 /*  35 */ "set_assignments ::= set_assignments COMMA set_assignment",
 /*  36 */ "set_assignments ::= set_assignment",
 /*  37 */ "set_assignment ::= IDENTIFIER ASSIGN expr",
 /*  38 */ "set_assignment ::= IDENTIFIER ADD_ASSIGN expr",
 /*  39 */ "set_assignment ::= IDENTIFIER SUB_ASSIGN expr",
 /*  40 */ "set_assignment ::= IDENTIFIER MUL_ASSIGN expr",
 /*  41 */ "set_assignment ::= IDENTIFIER DIV_ASSIGN expr",
 /*  42 */ "macro_statement ::= OPEN_DELIMITER MACRO IDENTIFIER PARENTHESES_OPEN PARENTHESES_CLOSE CLOSE_DELIMITER statement_list OPEN_DELIMITER ENDMACRO CLOSE_DELIMITER",
 /*  43 */ "macro_statement ::= OPEN_DELIMITER MACRO IDENTIFIER PARENTHESES_OPEN macro_parameters PARENTHESES_CLOSE CLOSE_DELIMITER statement_list OPEN_DELIMITER ENDMACRO CLOSE_DELIMITER",
 /*  44 */ "macro_parameters ::= macro_parameters COMMA macro_parameter",
 /*  45 */ "macro_parameters ::= macro_parameter",
 /*  46 */ "macro_parameter ::= IDENTIFIER",
 /*  47 */ "macro_parameter ::= IDENTIFIER ASSIGN macro_parameter_default",
 /*  48 */ "macro_parameter_default ::= INTEGER",
 /*  49 */ "macro_parameter_default ::= STRING",
 /*  50 */ "macro_parameter_default ::= DOUBLE",
 /*  51 */ "macro_parameter_default ::= NULL",
 /*  52 */ "macro_parameter_default ::= FALSE",
 /*  53 */ "macro_parameter_default ::= TRUE",
 /*  54 */ "macro_call_statement ::= OPEN_DELIMITER CALL expr PARENTHESES_OPEN argument_list PARENTHESES_CLOSE CLOSE_DELIMITER statement_list OPEN_DELIMITER ENDCALL CLOSE_DELIMITER",
 /*  55 */ "macro_call_statement ::= OPEN_DELIMITER CALL expr PARENTHESES_OPEN PARENTHESES_CLOSE CLOSE_DELIMITER OPEN_DELIMITER ENDCALL CLOSE_DELIMITER",
 /*  56 */ "empty_statement ::= OPEN_DELIMITER CLOSE_DELIMITER",
 /*  57 */ "echo_statement ::= OPEN_EDELIMITER expr CLOSE_EDELIMITER",
 /*  58 */ "block_statement ::= OPEN_DELIMITER BLOCK IDENTIFIER CLOSE_DELIMITER statement_list OPEN_DELIMITER ENDBLOCK CLOSE_DELIMITER",
 /*  59 */ "block_statement ::= OPEN_DELIMITER BLOCK IDENTIFIER CLOSE_DELIMITER OPEN_DELIMITER ENDBLOCK CLOSE_DELIMITER",
 /*  60 */ "cache_statement ::= OPEN_DELIMITER CACHE expr CLOSE_DELIMITER statement_list OPEN_DELIMITER ENDCACHE CLOSE_DELIMITER",
 /*  61 */ "cache_statement ::= OPEN_DELIMITER CACHE expr cache_lifetime CLOSE_DELIMITER statement_list OPEN_DELIMITER ENDCACHE CLOSE_DELIMITER",
 /*  62 */ "cache_lifetime ::= INTEGER",
 /*  63 */ "cache_lifetime ::= IDENTIFIER",
 /*  64 */ "extends_statement ::= OPEN_DELIMITER EXTENDS STRING CLOSE_DELIMITER",
 /*  65 */ "include_statement ::= OPEN_DELIMITER INCLUDE expr CLOSE_DELIMITER",
 /*  66 */ "include_statement ::= OPEN_DELIMITER INCLUDE expr WITH expr CLOSE_DELIMITER",
 /*  67 */ "do_statement ::= OPEN_DELIMITER DO expr CLOSE_DELIMITER",
 /*  68 */ "return_statement ::= OPEN_DELIMITER RETURN expr CLOSE_DELIMITER",
 /*  69 */ "autoescape_statement ::= OPEN_DELIMITER AUTOESCAPE FALSE CLOSE_DELIMITER statement_list OPEN_DELIMITER ENDAUTOESCAPE CLOSE_DELIMITER",
 /*  70 */ "autoescape_statement ::= OPEN_DELIMITER AUTOESCAPE TRUE CLOSE_DELIMITER statement_list OPEN_DELIMITER ENDAUTOESCAPE CLOSE_DELIMITER",
 /*  71 */ "break_statement ::= OPEN_DELIMITER BREAK CLOSE_DELIMITER",
 /*  72 */ "continue_statement ::= OPEN_DELIMITER CONTINUE CLOSE_DELIMITER",
 /*  73 */ "raw_fragment ::= RAW_FRAGMENT",
 /*  74 */ "expr ::= MINUS expr",
 /*  75 */ "expr ::= PLUS expr",
 /*  76 */ "expr ::= expr MINUS expr",
 /*  77 */ "expr ::= expr PLUS expr",
 /*  78 */ "expr ::= expr TIMES expr",
 /*  79 */ "expr ::= expr TIMES TIMES expr",
 /*  80 */ "expr ::= expr DIVIDE expr",
 /*  81 */ "expr ::= expr DIVIDE DIVIDE expr",
 /*  82 */ "expr ::= expr MOD expr",
 /*  83 */ "expr ::= expr AND expr",
 /*  84 */ "expr ::= expr OR expr",
 /*  85 */ "expr ::= expr CONCAT expr",
 /*  86 */ "expr ::= expr PIPE expr",
 /*  87 */ "expr ::= expr RANGE expr",
 /*  88 */ "expr ::= expr EQUALS expr",
 /*  89 */ "expr ::= expr NOTEQUALS DEFINED",
 /*  90 */ "expr ::= expr IS DEFINED",
 /*  91 */ "expr ::= expr NOTEQUALS EMPTY",
 /*  92 */ "expr ::= expr IS EMPTY",
 /*  93 */ "expr ::= expr NOTEQUALS EVEN",
 /*  94 */ "expr ::= expr IS EVEN",
 /*  95 */ "expr ::= expr NOTEQUALS ODD",
 /*  96 */ "expr ::= expr IS ODD",
 /*  97 */ "expr ::= expr NOTEQUALS NUMERIC",
 /*  98 */ "expr ::= expr IS NUMERIC",
 /*  99 */ "expr ::= expr NOTEQUALS SCALAR",
 /* 100 */ "expr ::= expr IS SCALAR",
 /* 101 */ "expr ::= expr NOTEQUALS ITERABLE",
 /* 102 */ "expr ::= expr IS ITERABLE",
 /* 103 */ "expr ::= expr IS expr",
 /* 104 */ "expr ::= expr NOTEQUALS expr",
 /* 105 */ "expr ::= expr IDENTICAL expr",
 /* 106 */ "expr ::= expr NOTIDENTICAL expr",
 /* 107 */ "expr ::= expr LESS expr",
 /* 108 */ "expr ::= expr GREATER expr",
 /* 109 */ "expr ::= expr GREATEREQUAL expr",
 /* 110 */ "expr ::= expr LESSEQUAL expr",
 /* 111 */ "expr ::= expr DOT expr",
 /* 112 */ "expr ::= expr IN expr",
 /* 113 */ "expr ::= expr NOT IN expr",
 /* 114 */ "expr ::= NOT expr",
 /* 115 */ "expr ::= expr INCR",
 /* 116 */ "expr ::= expr DECR",
 /* 117 */ "expr ::= PARENTHESES_OPEN expr PARENTHESES_CLOSE",
 /* 118 */ "expr ::= SBRACKET_OPEN SBRACKET_CLOSE",
 /* 119 */ "expr ::= SBRACKET_OPEN array_list SBRACKET_CLOSE",
 /* 120 */ "expr ::= CBRACKET_OPEN CBRACKET_CLOSE",
 /* 121 */ "expr ::= CBRACKET_OPEN array_list CBRACKET_CLOSE",
 /* 122 */ "expr ::= expr SBRACKET_OPEN expr SBRACKET_CLOSE",
 /* 123 */ "expr ::= expr QUESTION expr COLON expr",
 /* 124 */ "expr ::= expr SBRACKET_OPEN COLON slice_offset SBRACKET_CLOSE",
 /* 125 */ "expr ::= expr SBRACKET_OPEN slice_offset COLON SBRACKET_CLOSE",
 /* 126 */ "expr ::= expr SBRACKET_OPEN slice_offset COLON slice_offset SBRACKET_CLOSE",
 /* 127 */ "slice_offset ::= INTEGER",
 /* 128 */ "slice_offset ::= IDENTIFIER",
 /* 129 */ "array_list ::= array_list COMMA array_item",
 /* 130 */ "array_list ::= array_item",
 /* 131 */ "array_item ::= STRING COLON expr",
 /* 132 */ "array_item ::= expr",
 /* 133 */ "expr ::= function_call",
 /* 134 */ "function_call ::= expr PARENTHESES_OPEN argument_list PARENTHESES_CLOSE",
 /* 135 */ "function_call ::= expr PARENTHESES_OPEN PARENTHESES_CLOSE",
 /* 136 */ "argument_list ::= argument_list COMMA argument_item",
 /* 137 */ "argument_list ::= argument_item",
 /* 138 */ "argument_item ::= expr",
 /* 139 */ "argument_item ::= STRING COLON expr",
 /* 140 */ "expr ::= IDENTIFIER",
 /* 141 */ "expr ::= INTEGER",
 /* 142 */ "expr ::= STRING",
 /* 143 */ "expr ::= DOUBLE",
 /* 144 */ "expr ::= NULL",
 /* 145 */ "expr ::= FALSE",
 /* 146 */ "expr ::= TRUE",
};
#endif /* NDEBUG */

/*
** This function returns the symbolic name associated with a token
** value.
*/
const char *phvolt_TokenName(int tokenType){
#ifndef NDEBUG
  if( tokenType>0 && tokenType<(sizeof(yyTokenName)/sizeof(yyTokenName[0])) ){
    return yyTokenName[tokenType];
  }else{
    return "Unknown";
  }
#else
  return "";
#endif
}

/* 
** This function allocates a new parser.
** The only argument is a pointer to a function which works like
** malloc.
**
** Inputs:
** A pointer to the function used to allocate memory.
**
** Outputs:
** A pointer to a parser.  This pointer is used in subsequent calls
** to phvolt_ and phvolt_Free.
*/
void *phvolt_Alloc(void *(*mallocProc)(size_t)){
  yyParser *pParser;
  pParser = (yyParser*)(*mallocProc)( (size_t)sizeof(yyParser) );
  if( pParser ){
    pParser->yyidx = -1;
  }
  return pParser;
}

/* The following function deletes the value associated with a
** symbol.  The symbol can be either a terminal or nonterminal.
** "yymajor" is the symbol code, and "yypminor" is a pointer to
** the value.
*/
static void yy_destructor(YYCODETYPE yymajor, YYMINORTYPE *yypminor){
  switch( yymajor ){
    /* Here is inserted the actions which take place when a
    ** terminal or non-terminal is destroyed.  This can happen
    ** when the symbol is popped from the stack during a
    ** reduce or during error processing or when a parser is 
    ** being destroyed before it is finished parsing.
    **
    ** Note: during a reduce, the only symbols destroyed are those
    ** which appear on the RHS of the rule, but which are not used
    ** inside the C code.
    */
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
    case 19:
    case 20:
    case 21:
    case 22:
    case 23:
    case 24:
    case 25:
    case 26:
    case 27:
    case 28:
    case 29:
    case 30:
    case 31:
    case 32:
    case 33:
    case 34:
    case 35:
    case 36:
    case 37:
    case 38:
    case 39:
    case 40:
    case 41:
    case 42:
    case 43:
    case 44:
    case 45:
    case 46:
    case 47:
    case 48:
    case 49:
    case 50:
    case 51:
    case 52:
    case 53:
    case 54:
    case 55:
    case 56:
    case 57:
    case 58:
    case 59:
    case 60:
    case 61:
    case 62:
    case 63:
    case 64:
    case 65:
    case 66:
    case 67:
    case 68:
    case 69:
    case 70:
    case 71:
    case 72:
    case 73:
    case 74:
    case 75:
    case 76:
    case 77:
    case 78:
    case 79:
    case 80:
    case 81:
    case 82:
/* #line 22 "parser.y" */
{
	if ((yypminor->yy0)) {
		if ((yypminor->yy0)->free_flag) {
			efree((yypminor->yy0)->token);
		}
		efree((yypminor->yy0));
	}
}
/* #line 1654 "parser.c" */
      break;
    case 83:
    case 84:
    case 85:
    case 86:
    case 87:
    case 88:
    case 89:
    case 90:
    case 91:
    case 92:
    case 93:
    case 94:
    case 95:
    case 96:
    case 97:
    case 98:
    case 99:
    case 100:
    case 101:
    case 102:
    case 103:
    case 104:
    case 105:
    case 106:
    case 107:
    case 108:
    case 109:
    case 110:
    case 111:
    case 112:
    case 113:
    case 114:
    case 115:
    case 116:
    case 117:
    case 118:
    case 119:
/* #line 31 "parser.y" */
{
	if ((yypminor->yy168)) {
		zval_ptr_dtor(&(yypminor->yy168));
		//efree((yypminor->yy168));
	}
}
/* #line 1700 "parser.c" */
      break;
    default:  break;   /* If no destructor action specified: do nothing */
  }
}

/*
** Pop the parser's stack once.
**
** If there is a destructor routine associated with the token which
** is popped from the stack, then call it.
**
** Return the major token number for the symbol popped.
*/
static int yy_pop_parser_stack(yyParser *pParser){
  YYCODETYPE yymajor;
  yyStackEntry *yytos = &pParser->yystack[pParser->yyidx];

  if( pParser->yyidx<0 ) return 0;
#ifndef NDEBUG
  if( yyTraceFILE && pParser->yyidx>=0 ){
    fprintf(yyTraceFILE,"%sPopping %s\n",
      yyTracePrompt,
      yyTokenName[yytos->major]);
  }
#endif
  yymajor = yytos->major;
  yy_destructor( yymajor, &yytos->minor);
  pParser->yyidx--;
  return yymajor;
}

/* 
** Deallocate and destroy a parser.  Destructors are all called for
** all stack elements before shutting the parser down.
**
** Inputs:
** <ul>
** <li>  A pointer to the parser.  This should be a pointer
**       obtained from phvolt_Alloc.
** <li>  A pointer to a function used to reclaim memory obtained
**       from malloc.
** </ul>
*/
void phvolt_Free(
  void *p,                    /* The parser to be deleted */
  void (*freeProc)(void*)     /* Function used to reclaim memory */
){
  yyParser *pParser = (yyParser*)p;
  if( pParser==0 ) return;
  while( pParser->yyidx>=0 ) yy_pop_parser_stack(pParser);
  (*freeProc)((void*)pParser);
}

/*
** Find the appropriate action for a parser given the terminal
** look-ahead token iLookAhead.
**
** If the look-ahead token is YYNOCODE, then check to see if the action is
** independent of the look-ahead.  If it is, return the action, otherwise
** return YY_NO_ACTION.
*/
static int yy_find_shift_action(
  yyParser *pParser,        /* The parser */
  int iLookAhead            /* The look-ahead token */
){
  int i;
  int stateno = pParser->yystack[pParser->yyidx].stateno;
 
  /* if( pParser->yyidx<0 ) return YY_NO_ACTION;  */
  i = yy_shift_ofst[stateno];
  if( i==YY_SHIFT_USE_DFLT ){
    return yy_default[stateno];
  }
  if( iLookAhead==YYNOCODE ){
    return YY_NO_ACTION;
  }
  i += iLookAhead;
  if( i<0 || i>=YY_SZ_ACTTAB || yy_lookahead[i]!=iLookAhead ){
#ifdef YYFALLBACK
    int iFallback;            /* Fallback token */
    if( iLookAhead<sizeof(yyFallback)/sizeof(yyFallback[0])
           && (iFallback = yyFallback[iLookAhead])!=0 ){
#ifndef NDEBUG
      if( yyTraceFILE ){
        fprintf(yyTraceFILE, "%sFALLBACK %s => %s\n",
           yyTracePrompt, yyTokenName[iLookAhead], yyTokenName[iFallback]);
      }
#endif
      return yy_find_shift_action(pParser, iFallback);
    }
#endif
    return yy_default[stateno];
  }else{
    return yy_action[i];
  }
}

/*
** Find the appropriate action for a parser given the non-terminal
** look-ahead token iLookAhead.
**
** If the look-ahead token is YYNOCODE, then check to see if the action is
** independent of the look-ahead.  If it is, return the action, otherwise
** return YY_NO_ACTION.
*/
static int yy_find_reduce_action(
  yyParser *pParser,        /* The parser */
  int iLookAhead            /* The look-ahead token */
){
  int i;
  int stateno = pParser->yystack[pParser->yyidx].stateno;
 
  i = yy_reduce_ofst[stateno];
  if( i==YY_REDUCE_USE_DFLT ){
    return yy_default[stateno];
  }
  if( iLookAhead==YYNOCODE ){
    return YY_NO_ACTION;
  }
  i += iLookAhead;
  if( i<0 || i>=YY_SZ_ACTTAB || yy_lookahead[i]!=iLookAhead ){
    return yy_default[stateno];
  }else{
    return yy_action[i];
  }
}

/*
** Perform a shift action.
*/
static void yy_shift(
  yyParser *yypParser,          /* The parser to be shifted */
  int yyNewState,               /* The new state to shift in */
  int yyMajor,                  /* The major token to shift in */
  YYMINORTYPE *yypMinor         /* Pointer ot the minor token to shift in */
){
  yyStackEntry *yytos;
  yypParser->yyidx++;
  if( yypParser->yyidx>=YYSTACKDEPTH ){
     phvolt_ARG_FETCH;
     yypParser->yyidx--;
#ifndef NDEBUG
     if( yyTraceFILE ){
       fprintf(yyTraceFILE,"%sStack Overflow!\n",yyTracePrompt);
     }
#endif
     while( yypParser->yyidx>=0 ) yy_pop_parser_stack(yypParser);
     /* Here code is inserted which will execute if the parser
     ** stack every overflows */
     phvolt_ARG_STORE; /* Suppress warning about unused %extra_argument var */
     return;
  }
  yytos = &yypParser->yystack[yypParser->yyidx];
  yytos->stateno = yyNewState;
  yytos->major = yyMajor;
  yytos->minor = *yypMinor;
#ifndef NDEBUG
  if( yyTraceFILE && yypParser->yyidx>0 ){
    int i;
    fprintf(yyTraceFILE,"%sShift %d\n",yyTracePrompt,yyNewState);
    fprintf(yyTraceFILE,"%sStack:",yyTracePrompt);
    for(i=1; i<=yypParser->yyidx; i++)
      fprintf(yyTraceFILE," %s",yyTokenName[yypParser->yystack[i].major]);
    fprintf(yyTraceFILE,"\n");
  }
#endif
}

/* The following table contains information about every rule that
** is used during the reduce.
*/
static struct {
  YYCODETYPE lhs;         /* Symbol on the left-hand side of the rule */
  unsigned char nrhs;     /* Number of right-hand side symbols in the rule */
} yyRuleInfo[] = {
  { 84, 1 },
  { 85, 1 },
  { 86, 2 },
  { 86, 1 },
  { 87, 1 },
  { 87, 1 },
  { 87, 1 },
  { 87, 1 },
  { 87, 1 },
  { 87, 1 },
  { 87, 1 },
  { 87, 1 },
  { 87, 1 },
  { 87, 1 },
  { 87, 1 },
  { 87, 1 },
  { 87, 1 },
  { 87, 1 },
  { 87, 1 },
  { 87, 1 },
  { 87, 1 },
  { 87, 1 },
  { 87, 1 },
  { 89, 8 },
  { 89, 7 },
  { 89, 12 },
  { 89, 11 },
  { 89, 10 },
  { 90, 4 },
  { 91, 3 },
  { 92, 10 },
  { 92, 12 },
  { 92, 12 },
  { 92, 14 },
  { 93, 4 },
  { 108, 3 },
  { 108, 1 },
  { 109, 3 },
  { 109, 3 },
  { 109, 3 },
  { 109, 3 },
  { 109, 3 },
  { 104, 10 },
  { 104, 11 },
  { 110, 3 },
  { 110, 1 },
  { 111, 1 },
  { 111, 3 },
  { 112, 1 },
  { 112, 1 },
  { 112, 1 },
  { 112, 1 },
  { 112, 1 },
  { 112, 1 },
  { 106, 11 },
  { 106, 9 },
  { 105, 2 },
  { 94, 3 },
  { 95, 8 },
  { 95, 7 },
  { 96, 8 },
  { 96, 9 },
  { 114, 1 },
  { 114, 1 },
  { 97, 4 },
  { 98, 4 },
  { 98, 6 },
  { 99, 4 },
  { 100, 4 },
  { 101, 8 },
  { 101, 8 },
  { 102, 3 },
  { 103, 3 },
  { 88, 1 },
  { 107, 2 },
  { 107, 2 },
  { 107, 3 },
  { 107, 3 },
  { 107, 3 },
  { 107, 4 },
  { 107, 3 },
  { 107, 4 },
  { 107, 3 },
  { 107, 3 },
  { 107, 3 },
  { 107, 3 },
  { 107, 3 },
  { 107, 3 },
  { 107, 3 },
  { 107, 3 },
  { 107, 3 },
  { 107, 3 },
  { 107, 3 },
  { 107, 3 },
  { 107, 3 },
  { 107, 3 },
  { 107, 3 },
  { 107, 3 },
  { 107, 3 },
  { 107, 3 },
  { 107, 3 },
  { 107, 3 },
  { 107, 3 },
  { 107, 3 },
  { 107, 3 },
  { 107, 3 },
  { 107, 3 },
  { 107, 3 },
  { 107, 3 },
  { 107, 3 },
  { 107, 3 },
  { 107, 3 },
  { 107, 3 },
  { 107, 4 },
  { 107, 2 },
  { 107, 2 },
  { 107, 2 },
  { 107, 3 },
  { 107, 2 },
  { 107, 3 },
  { 107, 2 },
  { 107, 3 },
  { 107, 4 },
  { 107, 5 },
  { 107, 5 },
  { 107, 5 },
  { 107, 6 },
  { 116, 1 },
  { 116, 1 },
  { 115, 3 },
  { 115, 1 },
  { 117, 3 },
  { 117, 1 },
  { 107, 1 },
  { 118, 4 },
  { 118, 3 },
  { 113, 3 },
  { 113, 1 },
  { 119, 1 },
  { 119, 3 },
  { 107, 1 },
  { 107, 1 },
  { 107, 1 },
  { 107, 1 },
  { 107, 1 },
  { 107, 1 },
  { 107, 1 },
};

static void yy_accept(yyParser*);  /* Forward Declaration */

/*
** Perform a reduce action and the shift that must immediately
** follow the reduce.
*/
static void yy_reduce(
  yyParser *yypParser,         /* The parser */
  int yyruleno                 /* Number of the rule by which to reduce */
){
  int yygoto;                     /* The next state */
  int yyact;                      /* The next action */
  YYMINORTYPE yygotominor;        /* The LHS of the rule reduced */
  yyStackEntry *yymsp;            /* The top of the parser's stack */
  int yysize;                     /* Amount to pop the stack */
  phvolt_ARG_FETCH;
  yymsp = &yypParser->yystack[yypParser->yyidx];
#ifndef NDEBUG
  if( yyTraceFILE && yyruleno>=0 
        && yyruleno<sizeof(yyRuleName)/sizeof(yyRuleName[0]) ){
    fprintf(yyTraceFILE, "%sReduce [%s].\n", yyTracePrompt,
      yyRuleName[yyruleno]);
  }
#endif /* NDEBUG */

  switch( yyruleno ){
  /* Beginning here are the reduction cases.  A typical example
  ** follows:
  **   case 0:
  **  #line <lineno> <grammarfile>
  **     { ... }           // User supplied code
  **  #line <lineno> <thisfile>
  **     break;
  */
      case 0:
/* #line 710 "parser.y" */
{
	status->ret = yymsp[0].minor.yy168;
}
/* #line 2064 "parser.c" */
        break;
      case 1:
      case 4:
      case 5:
      case 6:
      case 7:
      case 8:
      case 9:
      case 10:
      case 11:
      case 12:
      case 13:
      case 14:
      case 15:
      case 16:
      case 17:
      case 18:
      case 19:
      case 20:
      case 21:
      case 22:
      case 133:
/* #line 714 "parser.y" */
{
	yygotominor.yy168 = yymsp[0].minor.yy168;
}
/* #line 2091 "parser.c" */
        break;
      case 2:
/* #line 718 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_zval_list(yymsp[-1].minor.yy168, yymsp[0].minor.yy168);
}
/* #line 2098 "parser.c" */
        break;
      case 3:
      case 36:
      case 45:
      case 130:
      case 137:
/* #line 722 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_zval_list(NULL, yymsp[0].minor.yy168);
}
/* #line 2109 "parser.c" */
        break;
      case 23:
/* #line 802 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_if_statement(yymsp[-5].minor.yy168, yymsp[-3].minor.yy168, NULL, status->scanner_state);
  yy_destructor(1,&yymsp[-7].minor);
  yy_destructor(31,&yymsp[-6].minor);
  yy_destructor(32,&yymsp[-4].minor);
  yy_destructor(1,&yymsp[-2].minor);
  yy_destructor(33,&yymsp[-1].minor);
  yy_destructor(32,&yymsp[0].minor);
}
/* #line 2122 "parser.c" */
        break;
      case 24:
/* #line 806 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_if_statement(yymsp[-4].minor.yy168, NULL, NULL, status->scanner_state);
  yy_destructor(1,&yymsp[-6].minor);
  yy_destructor(31,&yymsp[-5].minor);
  yy_destructor(32,&yymsp[-3].minor);
  yy_destructor(1,&yymsp[-2].minor);
  yy_destructor(33,&yymsp[-1].minor);
  yy_destructor(32,&yymsp[0].minor);
}
/* #line 2135 "parser.c" */
        break;
      case 25:
/* #line 810 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_if_statement(yymsp[-9].minor.yy168, yymsp[-7].minor.yy168, yymsp[-3].minor.yy168, status->scanner_state);
  yy_destructor(1,&yymsp[-11].minor);
  yy_destructor(31,&yymsp[-10].minor);
  yy_destructor(32,&yymsp[-8].minor);
  yy_destructor(1,&yymsp[-6].minor);
  yy_destructor(34,&yymsp[-5].minor);
  yy_destructor(32,&yymsp[-4].minor);
  yy_destructor(1,&yymsp[-2].minor);
  yy_destructor(33,&yymsp[-1].minor);
  yy_destructor(32,&yymsp[0].minor);
}
/* #line 2151 "parser.c" */
        break;
      case 26:
/* #line 814 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_if_statement(yymsp[-8].minor.yy168, yymsp[-6].minor.yy168, NULL, status->scanner_state);
  yy_destructor(1,&yymsp[-10].minor);
  yy_destructor(31,&yymsp[-9].minor);
  yy_destructor(32,&yymsp[-7].minor);
  yy_destructor(1,&yymsp[-5].minor);
  yy_destructor(34,&yymsp[-4].minor);
  yy_destructor(32,&yymsp[-3].minor);
  yy_destructor(1,&yymsp[-2].minor);
  yy_destructor(33,&yymsp[-1].minor);
  yy_destructor(32,&yymsp[0].minor);
}
/* #line 2167 "parser.c" */
        break;
      case 27:
/* #line 818 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_if_statement(yymsp[-7].minor.yy168, NULL, NULL, status->scanner_state);
  yy_destructor(1,&yymsp[-9].minor);
  yy_destructor(31,&yymsp[-8].minor);
  yy_destructor(32,&yymsp[-6].minor);
  yy_destructor(1,&yymsp[-5].minor);
  yy_destructor(34,&yymsp[-4].minor);
  yy_destructor(32,&yymsp[-3].minor);
  yy_destructor(1,&yymsp[-2].minor);
  yy_destructor(33,&yymsp[-1].minor);
  yy_destructor(32,&yymsp[0].minor);
}
/* #line 2183 "parser.c" */
        break;
      case 28:
/* #line 822 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_elseif_statement(yymsp[-1].minor.yy168, status->scanner_state);
  yy_destructor(1,&yymsp[-3].minor);
  yy_destructor(35,&yymsp[-2].minor);
  yy_destructor(32,&yymsp[0].minor);
}
/* #line 2193 "parser.c" */
        break;
      case 29:
/* #line 826 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_elsefor_statement(status->scanner_state);
  yy_destructor(1,&yymsp[-2].minor);
  yy_destructor(36,&yymsp[-1].minor);
  yy_destructor(32,&yymsp[0].minor);
}
/* #line 2203 "parser.c" */
        break;
      case 30:
/* #line 830 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_for_statement(yymsp[-7].minor.yy0, NULL, yymsp[-5].minor.yy168, NULL, yymsp[-3].minor.yy168, status->scanner_state);
  yy_destructor(1,&yymsp[-9].minor);
  yy_destructor(37,&yymsp[-8].minor);
  yy_destructor(3,&yymsp[-6].minor);
  yy_destructor(32,&yymsp[-4].minor);
  yy_destructor(1,&yymsp[-2].minor);
  yy_destructor(39,&yymsp[-1].minor);
  yy_destructor(32,&yymsp[0].minor);
}
/* #line 2217 "parser.c" */
        break;
      case 31:
/* #line 834 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_for_statement(yymsp[-9].minor.yy0, NULL, yymsp[-7].minor.yy168, yymsp[-5].minor.yy168, yymsp[-3].minor.yy168, status->scanner_state);
  yy_destructor(1,&yymsp[-11].minor);
  yy_destructor(37,&yymsp[-10].minor);
  yy_destructor(3,&yymsp[-8].minor);
  yy_destructor(31,&yymsp[-6].minor);
  yy_destructor(32,&yymsp[-4].minor);
  yy_destructor(1,&yymsp[-2].minor);
  yy_destructor(39,&yymsp[-1].minor);
  yy_destructor(32,&yymsp[0].minor);
}
/* #line 2232 "parser.c" */
        break;
      case 32:
/* #line 838 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_for_statement(yymsp[-7].minor.yy0, yymsp[-9].minor.yy0, yymsp[-5].minor.yy168, NULL, yymsp[-3].minor.yy168, status->scanner_state);
  yy_destructor(1,&yymsp[-11].minor);
  yy_destructor(37,&yymsp[-10].minor);
  yy_destructor(2,&yymsp[-8].minor);
  yy_destructor(3,&yymsp[-6].minor);
  yy_destructor(32,&yymsp[-4].minor);
  yy_destructor(1,&yymsp[-2].minor);
  yy_destructor(39,&yymsp[-1].minor);
  yy_destructor(32,&yymsp[0].minor);
}
/* #line 2247 "parser.c" */
        break;
      case 33:
/* #line 842 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_for_statement(yymsp[-9].minor.yy0, yymsp[-11].minor.yy0, yymsp[-7].minor.yy168, yymsp[-5].minor.yy168, yymsp[-3].minor.yy168, status->scanner_state);
  yy_destructor(1,&yymsp[-13].minor);
  yy_destructor(37,&yymsp[-12].minor);
  yy_destructor(2,&yymsp[-10].minor);
  yy_destructor(3,&yymsp[-8].minor);
  yy_destructor(31,&yymsp[-6].minor);
  yy_destructor(32,&yymsp[-4].minor);
  yy_destructor(1,&yymsp[-2].minor);
  yy_destructor(39,&yymsp[-1].minor);
  yy_destructor(32,&yymsp[0].minor);
}
/* #line 2263 "parser.c" */
        break;
      case 34:
/* #line 846 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_set_statement(yymsp[-1].minor.yy168);
  yy_destructor(1,&yymsp[-3].minor);
  yy_destructor(40,&yymsp[-2].minor);
  yy_destructor(32,&yymsp[0].minor);
}
/* #line 2273 "parser.c" */
        break;
      case 35:
      case 44:
      case 129:
      case 136:
/* #line 850 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_zval_list(yymsp[-2].minor.yy168, yymsp[0].minor.yy168);
  yy_destructor(2,&yymsp[-1].minor);
}
/* #line 2284 "parser.c" */
        break;
      case 37:
/* #line 858 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_set_assignment(yymsp[-2].minor.yy0, PHVOLT_T_ASSIGN, yymsp[0].minor.yy168, status->scanner_state);
  yy_destructor(41,&yymsp[-1].minor);
}
/* #line 2292 "parser.c" */
        break;
      case 38:
/* #line 862 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_set_assignment(yymsp[-2].minor.yy0, PHVOLT_T_ADD_ASSIGN, yymsp[0].minor.yy168, status->scanner_state);
  yy_destructor(42,&yymsp[-1].minor);
}
/* #line 2300 "parser.c" */
        break;
      case 39:
/* #line 866 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_set_assignment(yymsp[-2].minor.yy0, PHVOLT_T_SUB_ASSIGN, yymsp[0].minor.yy168, status->scanner_state);
  yy_destructor(43,&yymsp[-1].minor);
}
/* #line 2308 "parser.c" */
        break;
      case 40:
/* #line 870 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_set_assignment(yymsp[-2].minor.yy0, PHVOLT_T_MUL_ASSIGN, yymsp[0].minor.yy168, status->scanner_state);
  yy_destructor(44,&yymsp[-1].minor);
}
/* #line 2316 "parser.c" */
        break;
      case 41:
/* #line 874 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_set_assignment(yymsp[-2].minor.yy0, PHVOLT_T_DIV_ASSIGN, yymsp[0].minor.yy168, status->scanner_state);
  yy_destructor(45,&yymsp[-1].minor);
}
/* #line 2324 "parser.c" */
        break;
      case 42:
/* #line 878 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_macro_statement(yymsp[-7].minor.yy0, NULL, yymsp[-3].minor.yy168, status->scanner_state);
  yy_destructor(1,&yymsp[-9].minor);
  yy_destructor(46,&yymsp[-8].minor);
  yy_destructor(29,&yymsp[-6].minor);
  yy_destructor(47,&yymsp[-5].minor);
  yy_destructor(32,&yymsp[-4].minor);
  yy_destructor(1,&yymsp[-2].minor);
  yy_destructor(48,&yymsp[-1].minor);
  yy_destructor(32,&yymsp[0].minor);
}
/* #line 2339 "parser.c" */
        break;
      case 43:
/* #line 882 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_macro_statement(yymsp[-8].minor.yy0, yymsp[-6].minor.yy168, yymsp[-3].minor.yy168, status->scanner_state);
  yy_destructor(1,&yymsp[-10].minor);
  yy_destructor(46,&yymsp[-9].minor);
  yy_destructor(29,&yymsp[-7].minor);
  yy_destructor(47,&yymsp[-5].minor);
  yy_destructor(32,&yymsp[-4].minor);
  yy_destructor(1,&yymsp[-2].minor);
  yy_destructor(48,&yymsp[-1].minor);
  yy_destructor(32,&yymsp[0].minor);
}
/* #line 2354 "parser.c" */
        break;
      case 46:
/* #line 894 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_macro_parameter(yymsp[0].minor.yy0, NULL, status->scanner_state);
}
/* #line 2361 "parser.c" */
        break;
      case 47:
/* #line 898 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_macro_parameter(yymsp[-2].minor.yy0, yymsp[0].minor.yy168, status->scanner_state);
  yy_destructor(41,&yymsp[-1].minor);
}
/* #line 2369 "parser.c" */
        break;
      case 48:
      case 62:
      case 127:
      case 141:
/* #line 902 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_literal_zval(PHVOLT_T_INTEGER, yymsp[0].minor.yy0, status->scanner_state);
}
/* #line 2379 "parser.c" */
        break;
      case 49:
      case 142:
/* #line 906 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_literal_zval(PHVOLT_T_STRING, yymsp[0].minor.yy0, status->scanner_state);
}
/* #line 2387 "parser.c" */
        break;
      case 50:
      case 143:
/* #line 910 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_literal_zval(PHVOLT_T_DOUBLE, yymsp[0].minor.yy0, status->scanner_state);
}
/* #line 2395 "parser.c" */
        break;
      case 51:
      case 144:
/* #line 914 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_literal_zval(PHVOLT_T_NULL, NULL, status->scanner_state);
  yy_destructor(52,&yymsp[0].minor);
}
/* #line 2404 "parser.c" */
        break;
      case 52:
      case 145:
/* #line 918 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_literal_zval(PHVOLT_T_FALSE, NULL, status->scanner_state);
  yy_destructor(53,&yymsp[0].minor);
}
/* #line 2413 "parser.c" */
        break;
      case 53:
      case 146:
/* #line 922 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_literal_zval(PHVOLT_T_TRUE, NULL, status->scanner_state);
  yy_destructor(54,&yymsp[0].minor);
}
/* #line 2422 "parser.c" */
        break;
      case 54:
/* #line 926 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_macro_call_statement(yymsp[-8].minor.yy168, yymsp[-6].minor.yy168, yymsp[-3].minor.yy168, status->scanner_state);
  yy_destructor(1,&yymsp[-10].minor);
  yy_destructor(55,&yymsp[-9].minor);
  yy_destructor(29,&yymsp[-7].minor);
  yy_destructor(47,&yymsp[-5].minor);
  yy_destructor(32,&yymsp[-4].minor);
  yy_destructor(1,&yymsp[-2].minor);
  yy_destructor(56,&yymsp[-1].minor);
  yy_destructor(32,&yymsp[0].minor);
}
/* #line 2437 "parser.c" */
        break;
      case 55:
/* #line 930 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_macro_call_statement(yymsp[-6].minor.yy168, NULL, NULL, status->scanner_state);
  yy_destructor(1,&yymsp[-8].minor);
  yy_destructor(55,&yymsp[-7].minor);
  yy_destructor(29,&yymsp[-5].minor);
  yy_destructor(47,&yymsp[-4].minor);
  yy_destructor(32,&yymsp[-3].minor);
  yy_destructor(1,&yymsp[-2].minor);
  yy_destructor(56,&yymsp[-1].minor);
  yy_destructor(32,&yymsp[0].minor);
}
/* #line 2452 "parser.c" */
        break;
      case 56:
/* #line 934 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_empty_statement(status->scanner_state);
  yy_destructor(1,&yymsp[-1].minor);
  yy_destructor(32,&yymsp[0].minor);
}
/* #line 2461 "parser.c" */
        break;
      case 57:
/* #line 938 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_echo_statement(yymsp[-1].minor.yy168, status->scanner_state);
  yy_destructor(57,&yymsp[-2].minor);
  yy_destructor(58,&yymsp[0].minor);
}
/* #line 2470 "parser.c" */
        break;
      case 58:
/* #line 942 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_block_statement(yymsp[-5].minor.yy0, yymsp[-3].minor.yy168, status->scanner_state);
  yy_destructor(1,&yymsp[-7].minor);
  yy_destructor(59,&yymsp[-6].minor);
  yy_destructor(32,&yymsp[-4].minor);
  yy_destructor(1,&yymsp[-2].minor);
  yy_destructor(60,&yymsp[-1].minor);
  yy_destructor(32,&yymsp[0].minor);
}
/* #line 2483 "parser.c" */
        break;
      case 59:
/* #line 946 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_block_statement(yymsp[-4].minor.yy0, NULL, status->scanner_state);
  yy_destructor(1,&yymsp[-6].minor);
  yy_destructor(59,&yymsp[-5].minor);
  yy_destructor(32,&yymsp[-3].minor);
  yy_destructor(1,&yymsp[-2].minor);
  yy_destructor(60,&yymsp[-1].minor);
  yy_destructor(32,&yymsp[0].minor);
}
/* #line 2496 "parser.c" */
        break;
      case 60:
/* #line 950 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_cache_statement(yymsp[-5].minor.yy168, NULL, yymsp[-3].minor.yy168, status->scanner_state);
  yy_destructor(1,&yymsp[-7].minor);
  yy_destructor(61,&yymsp[-6].minor);
  yy_destructor(32,&yymsp[-4].minor);
  yy_destructor(1,&yymsp[-2].minor);
  yy_destructor(62,&yymsp[-1].minor);
  yy_destructor(32,&yymsp[0].minor);
}
/* #line 2509 "parser.c" */
        break;
      case 61:
/* #line 954 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_cache_statement(yymsp[-6].minor.yy168, yymsp[-5].minor.yy168, yymsp[-3].minor.yy168, status->scanner_state);
  yy_destructor(1,&yymsp[-8].minor);
  yy_destructor(61,&yymsp[-7].minor);
  yy_destructor(32,&yymsp[-4].minor);
  yy_destructor(1,&yymsp[-2].minor);
  yy_destructor(62,&yymsp[-1].minor);
  yy_destructor(32,&yymsp[0].minor);
}
/* #line 2522 "parser.c" */
        break;
      case 63:
      case 128:
      case 140:
/* #line 962 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_literal_zval(PHVOLT_T_IDENTIFIER, yymsp[0].minor.yy0, status->scanner_state);
}
/* #line 2531 "parser.c" */
        break;
      case 64:
/* #line 966 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_extends_statement(yymsp[-1].minor.yy0, status->scanner_state);
  yy_destructor(1,&yymsp[-3].minor);
  yy_destructor(63,&yymsp[-2].minor);
  yy_destructor(32,&yymsp[0].minor);
}
/* #line 2541 "parser.c" */
        break;
      case 65:
/* #line 970 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_include_statement(yymsp[-1].minor.yy168, NULL, status->scanner_state);
  yy_destructor(1,&yymsp[-3].minor);
  yy_destructor(64,&yymsp[-2].minor);
  yy_destructor(32,&yymsp[0].minor);
}
/* #line 2551 "parser.c" */
        break;
      case 66:
/* #line 974 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_include_statement(yymsp[-3].minor.yy168, yymsp[-1].minor.yy168, status->scanner_state);
  yy_destructor(1,&yymsp[-5].minor);
  yy_destructor(64,&yymsp[-4].minor);
  yy_destructor(65,&yymsp[-2].minor);
  yy_destructor(32,&yymsp[0].minor);
}
/* #line 2562 "parser.c" */
        break;
      case 67:
/* #line 978 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_do_statement(yymsp[-1].minor.yy168, status->scanner_state);
  yy_destructor(1,&yymsp[-3].minor);
  yy_destructor(66,&yymsp[-2].minor);
  yy_destructor(32,&yymsp[0].minor);
}
/* #line 2572 "parser.c" */
        break;
      case 68:
/* #line 982 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_return_statement(yymsp[-1].minor.yy168, status->scanner_state);
  yy_destructor(1,&yymsp[-3].minor);
  yy_destructor(67,&yymsp[-2].minor);
  yy_destructor(32,&yymsp[0].minor);
}
/* #line 2582 "parser.c" */
        break;
      case 69:
/* #line 986 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_autoescape_statement(0, yymsp[-3].minor.yy168, status->scanner_state);
  yy_destructor(1,&yymsp[-7].minor);
  yy_destructor(68,&yymsp[-6].minor);
  yy_destructor(53,&yymsp[-5].minor);
  yy_destructor(32,&yymsp[-4].minor);
  yy_destructor(1,&yymsp[-2].minor);
  yy_destructor(69,&yymsp[-1].minor);
  yy_destructor(32,&yymsp[0].minor);
}
/* #line 2596 "parser.c" */
        break;
      case 70:
/* #line 990 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_autoescape_statement(1, yymsp[-3].minor.yy168, status->scanner_state);
  yy_destructor(1,&yymsp[-7].minor);
  yy_destructor(68,&yymsp[-6].minor);
  yy_destructor(54,&yymsp[-5].minor);
  yy_destructor(32,&yymsp[-4].minor);
  yy_destructor(1,&yymsp[-2].minor);
  yy_destructor(69,&yymsp[-1].minor);
  yy_destructor(32,&yymsp[0].minor);
}
/* #line 2610 "parser.c" */
        break;
      case 71:
/* #line 994 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_break_statement(status->scanner_state);
  yy_destructor(1,&yymsp[-2].minor);
  yy_destructor(70,&yymsp[-1].minor);
  yy_destructor(32,&yymsp[0].minor);
}
/* #line 2620 "parser.c" */
        break;
      case 72:
/* #line 998 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_continue_statement(status->scanner_state);
  yy_destructor(1,&yymsp[-2].minor);
  yy_destructor(71,&yymsp[-1].minor);
  yy_destructor(32,&yymsp[0].minor);
}
/* #line 2630 "parser.c" */
        break;
      case 73:
/* #line 1002 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_literal_zval(PHVOLT_T_RAW_FRAGMENT, yymsp[0].minor.yy0, status->scanner_state);
}
/* #line 2637 "parser.c" */
        break;
      case 74:
/* #line 1006 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_expr(PHVOLT_T_MINUS, NULL, yymsp[0].minor.yy168, NULL, status->scanner_state);
  yy_destructor(22,&yymsp[-1].minor);
}
/* #line 2645 "parser.c" */
        break;
      case 75:
/* #line 1010 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_expr(PHVOLT_T_PLUS, NULL, yymsp[0].minor.yy168, NULL, status->scanner_state);
  yy_destructor(21,&yymsp[-1].minor);
}
/* #line 2653 "parser.c" */
        break;
      case 76:
/* #line 1014 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_expr(PHVOLT_T_SUB, yymsp[-2].minor.yy168, yymsp[0].minor.yy168, NULL, status->scanner_state);
  yy_destructor(22,&yymsp[-1].minor);
}
/* #line 2661 "parser.c" */
        break;
      case 77:
/* #line 1018 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_expr(PHVOLT_T_ADD, yymsp[-2].minor.yy168, yymsp[0].minor.yy168, NULL, status->scanner_state);
  yy_destructor(21,&yymsp[-1].minor);
}
/* #line 2669 "parser.c" */
        break;
      case 78:
/* #line 1022 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_expr(PHVOLT_T_MUL, yymsp[-2].minor.yy168, yymsp[0].minor.yy168, NULL, status->scanner_state);
  yy_destructor(19,&yymsp[-1].minor);
}
/* #line 2677 "parser.c" */
        break;
      case 79:
/* #line 1026 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_expr(PHVOLT_T_POW, yymsp[-3].minor.yy168, yymsp[0].minor.yy168, NULL, status->scanner_state);
  yy_destructor(19,&yymsp[-2].minor);
  yy_destructor(19,&yymsp[-1].minor);
}
/* #line 2686 "parser.c" */
        break;
      case 80:
/* #line 1030 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_expr(PHVOLT_T_DIV, yymsp[-2].minor.yy168, yymsp[0].minor.yy168, NULL, status->scanner_state);
  yy_destructor(18,&yymsp[-1].minor);
}
/* #line 2694 "parser.c" */
        break;
      case 81:
/* #line 1034 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_expr(PHVOLT_T_MOD, yymsp[-3].minor.yy168, yymsp[0].minor.yy168, NULL, status->scanner_state);
  yy_destructor(18,&yymsp[-2].minor);
  yy_destructor(18,&yymsp[-1].minor);
}
/* #line 2703 "parser.c" */
        break;
      case 82:
/* #line 1038 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_expr(PHVOLT_T_MOD, yymsp[-2].minor.yy168, yymsp[0].minor.yy168, NULL, status->scanner_state);
  yy_destructor(20,&yymsp[-1].minor);
}
/* #line 2711 "parser.c" */
        break;
      case 83:
/* #line 1042 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_expr(PHVOLT_T_AND, yymsp[-2].minor.yy168, yymsp[0].minor.yy168, NULL, status->scanner_state);
  yy_destructor(7,&yymsp[-1].minor);
}
/* #line 2719 "parser.c" */
        break;
      case 84:
/* #line 1046 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_expr(PHVOLT_T_OR, yymsp[-2].minor.yy168, yymsp[0].minor.yy168, NULL, status->scanner_state);
  yy_destructor(8,&yymsp[-1].minor);
}
/* #line 2727 "parser.c" */
        break;
      case 85:
/* #line 1050 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_expr(PHVOLT_T_CONCAT, yymsp[-2].minor.yy168, yymsp[0].minor.yy168, NULL, status->scanner_state);
  yy_destructor(23,&yymsp[-1].minor);
}
/* #line 2735 "parser.c" */
        break;
      case 86:
/* #line 1054 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_expr(PHVOLT_T_PIPE, yymsp[-2].minor.yy168, yymsp[0].minor.yy168, NULL, status->scanner_state);
  yy_destructor(25,&yymsp[-1].minor);
}
/* #line 2743 "parser.c" */
        break;
      case 87:
/* #line 1058 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_expr(PHVOLT_T_RANGE, yymsp[-2].minor.yy168, yymsp[0].minor.yy168, NULL, status->scanner_state);
  yy_destructor(6,&yymsp[-1].minor);
}
/* #line 2751 "parser.c" */
        break;
      case 88:
/* #line 1062 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_expr(PHVOLT_T_EQUALS, yymsp[-2].minor.yy168, yymsp[0].minor.yy168, NULL, status->scanner_state);
  yy_destructor(10,&yymsp[-1].minor);
}
/* #line 2759 "parser.c" */
        break;
      case 89:
/* #line 1066 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_expr(PHVOLT_T_NOT_ISSET, yymsp[-2].minor.yy168, NULL, NULL, status->scanner_state);
  yy_destructor(11,&yymsp[-1].minor);
  yy_destructor(73,&yymsp[0].minor);
}
/* #line 2768 "parser.c" */
        break;
      case 90:
/* #line 1070 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_expr(PHVOLT_T_ISSET, yymsp[-2].minor.yy168, NULL, NULL, status->scanner_state);
  yy_destructor(9,&yymsp[-1].minor);
  yy_destructor(73,&yymsp[0].minor);
}
/* #line 2777 "parser.c" */
        break;
      case 91:
/* #line 1074 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_expr(PHVOLT_T_NOT_ISEMPTY, yymsp[-2].minor.yy168, NULL, NULL, status->scanner_state);
  yy_destructor(11,&yymsp[-1].minor);
  yy_destructor(74,&yymsp[0].minor);
}
/* #line 2786 "parser.c" */
        break;
      case 92:
/* #line 1078 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_expr(PHVOLT_T_ISEMPTY, yymsp[-2].minor.yy168, NULL, NULL, status->scanner_state);
  yy_destructor(9,&yymsp[-1].minor);
  yy_destructor(74,&yymsp[0].minor);
}
/* #line 2795 "parser.c" */
        break;
      case 93:
/* #line 1082 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_expr(PHVOLT_T_NOT_ISEVEN, yymsp[-2].minor.yy168, NULL, NULL, status->scanner_state);
  yy_destructor(11,&yymsp[-1].minor);
  yy_destructor(75,&yymsp[0].minor);
}
/* #line 2804 "parser.c" */
        break;
      case 94:
/* #line 1086 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_expr(PHVOLT_T_ISEVEN, yymsp[-2].minor.yy168, NULL, NULL, status->scanner_state);
  yy_destructor(9,&yymsp[-1].minor);
  yy_destructor(75,&yymsp[0].minor);
}
/* #line 2813 "parser.c" */
        break;
      case 95:
/* #line 1090 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_expr(PHVOLT_T_NOT_ISODD, yymsp[-2].minor.yy168, NULL, NULL, status->scanner_state);
  yy_destructor(11,&yymsp[-1].minor);
  yy_destructor(76,&yymsp[0].minor);
}
/* #line 2822 "parser.c" */
        break;
      case 96:
/* #line 1094 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_expr(PHVOLT_T_ISODD, yymsp[-2].minor.yy168, NULL, NULL, status->scanner_state);
  yy_destructor(9,&yymsp[-1].minor);
  yy_destructor(76,&yymsp[0].minor);
}
/* #line 2831 "parser.c" */
        break;
      case 97:
/* #line 1098 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_expr(PHVOLT_T_NOT_ISNUMERIC, yymsp[-2].minor.yy168, NULL, NULL, status->scanner_state);
  yy_destructor(11,&yymsp[-1].minor);
  yy_destructor(77,&yymsp[0].minor);
}
/* #line 2840 "parser.c" */
        break;
      case 98:
/* #line 1102 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_expr(PHVOLT_T_ISNUMERIC, yymsp[-2].minor.yy168, NULL, NULL, status->scanner_state);
  yy_destructor(9,&yymsp[-1].minor);
  yy_destructor(77,&yymsp[0].minor);
}
/* #line 2849 "parser.c" */
        break;
      case 99:
/* #line 1106 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_expr(PHVOLT_T_NOT_ISSCALAR, yymsp[-2].minor.yy168, NULL, NULL, status->scanner_state);
  yy_destructor(11,&yymsp[-1].minor);
  yy_destructor(78,&yymsp[0].minor);
}
/* #line 2858 "parser.c" */
        break;
      case 100:
/* #line 1110 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_expr(PHVOLT_T_ISSCALAR, yymsp[-2].minor.yy168, NULL, NULL, status->scanner_state);
  yy_destructor(9,&yymsp[-1].minor);
  yy_destructor(78,&yymsp[0].minor);
}
/* #line 2867 "parser.c" */
        break;
      case 101:
/* #line 1114 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_expr(PHVOLT_T_NOT_ISITERABLE, yymsp[-2].minor.yy168, NULL, NULL, status->scanner_state);
  yy_destructor(11,&yymsp[-1].minor);
  yy_destructor(79,&yymsp[0].minor);
}
/* #line 2876 "parser.c" */
        break;
      case 102:
/* #line 1118 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_expr(PHVOLT_T_ISITERABLE, yymsp[-2].minor.yy168, NULL, NULL, status->scanner_state);
  yy_destructor(9,&yymsp[-1].minor);
  yy_destructor(79,&yymsp[0].minor);
}
/* #line 2885 "parser.c" */
        break;
      case 103:
/* #line 1122 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_expr(PHVOLT_T_IS, yymsp[-2].minor.yy168, yymsp[0].minor.yy168, NULL, status->scanner_state);
  yy_destructor(9,&yymsp[-1].minor);
}
/* #line 2893 "parser.c" */
        break;
      case 104:
/* #line 1126 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_expr(PHVOLT_T_NOTEQUALS, yymsp[-2].minor.yy168, yymsp[0].minor.yy168, NULL, status->scanner_state);
  yy_destructor(11,&yymsp[-1].minor);
}
/* #line 2901 "parser.c" */
        break;
      case 105:
/* #line 1130 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_expr(PHVOLT_T_IDENTICAL, yymsp[-2].minor.yy168, yymsp[0].minor.yy168, NULL, status->scanner_state);
  yy_destructor(16,&yymsp[-1].minor);
}
/* #line 2909 "parser.c" */
        break;
      case 106:
/* #line 1134 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_expr(PHVOLT_T_NOTIDENTICAL, yymsp[-2].minor.yy168, yymsp[0].minor.yy168, NULL, status->scanner_state);
  yy_destructor(17,&yymsp[-1].minor);
}
/* #line 2917 "parser.c" */
        break;
      case 107:
/* #line 1138 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_expr(PHVOLT_T_LESS, yymsp[-2].minor.yy168, yymsp[0].minor.yy168, NULL, status->scanner_state);
  yy_destructor(12,&yymsp[-1].minor);
}
/* #line 2925 "parser.c" */
        break;
      case 108:
/* #line 1142 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_expr(PHVOLT_T_GREATER, yymsp[-2].minor.yy168, yymsp[0].minor.yy168, NULL, status->scanner_state);
  yy_destructor(13,&yymsp[-1].minor);
}
/* #line 2933 "parser.c" */
        break;
      case 109:
/* #line 1146 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_expr(PHVOLT_T_GREATEREQUAL, yymsp[-2].minor.yy168, yymsp[0].minor.yy168, NULL, status->scanner_state);
  yy_destructor(14,&yymsp[-1].minor);
}
/* #line 2941 "parser.c" */
        break;
      case 110:
/* #line 1150 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_expr(PHVOLT_T_LESSEQUAL, yymsp[-2].minor.yy168, yymsp[0].minor.yy168, NULL, status->scanner_state);
  yy_destructor(15,&yymsp[-1].minor);
}
/* #line 2949 "parser.c" */
        break;
      case 111:
/* #line 1154 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_expr(PHVOLT_T_DOT, yymsp[-2].minor.yy168, yymsp[0].minor.yy168, NULL, status->scanner_state);
  yy_destructor(30,&yymsp[-1].minor);
}
/* #line 2957 "parser.c" */
        break;
      case 112:
/* #line 1158 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_expr(PHVOLT_T_IN, yymsp[-2].minor.yy168, yymsp[0].minor.yy168, NULL, status->scanner_state);
  yy_destructor(3,&yymsp[-1].minor);
}
/* #line 2965 "parser.c" */
        break;
      case 113:
/* #line 1162 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_expr(PHVOLT_T_NOT_IN, yymsp[-3].minor.yy168, yymsp[0].minor.yy168, NULL, status->scanner_state);
  yy_destructor(26,&yymsp[-2].minor);
  yy_destructor(3,&yymsp[-1].minor);
}
/* #line 2974 "parser.c" */
        break;
      case 114:
/* #line 1166 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_expr(PHVOLT_T_NOT, NULL, yymsp[0].minor.yy168, NULL, status->scanner_state);
  yy_destructor(26,&yymsp[-1].minor);
}
/* #line 2982 "parser.c" */
        break;
      case 115:
/* #line 1170 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_expr(PHVOLT_T_INCR, yymsp[-1].minor.yy168, NULL, NULL, status->scanner_state);
  yy_destructor(27,&yymsp[0].minor);
}
/* #line 2990 "parser.c" */
        break;
      case 116:
/* #line 1174 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_expr(PHVOLT_T_DECR, yymsp[-1].minor.yy168, NULL, NULL, status->scanner_state);
  yy_destructor(28,&yymsp[0].minor);
}
/* #line 2998 "parser.c" */
        break;
      case 117:
/* #line 1178 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_expr(PHVOLT_T_ENCLOSED, yymsp[-1].minor.yy168, NULL, NULL, status->scanner_state);
  yy_destructor(29,&yymsp[-2].minor);
  yy_destructor(47,&yymsp[0].minor);
}
/* #line 3007 "parser.c" */
        break;
      case 118:
/* #line 1182 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_expr(PHVOLT_T_ARRAY, NULL, NULL, NULL, status->scanner_state);
  yy_destructor(24,&yymsp[-1].minor);
  yy_destructor(80,&yymsp[0].minor);
}
/* #line 3016 "parser.c" */
        break;
      case 119:
/* #line 1186 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_expr(PHVOLT_T_ARRAY, yymsp[-1].minor.yy168, NULL, NULL, status->scanner_state);
  yy_destructor(24,&yymsp[-2].minor);
  yy_destructor(80,&yymsp[0].minor);
}
/* #line 3025 "parser.c" */
        break;
      case 120:
/* #line 1190 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_expr(PHVOLT_T_ARRAY, NULL, NULL, NULL, status->scanner_state);
  yy_destructor(81,&yymsp[-1].minor);
  yy_destructor(82,&yymsp[0].minor);
}
/* #line 3034 "parser.c" */
        break;
      case 121:
/* #line 1194 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_expr(PHVOLT_T_ARRAY, yymsp[-1].minor.yy168, NULL, NULL, status->scanner_state);
  yy_destructor(81,&yymsp[-2].minor);
  yy_destructor(82,&yymsp[0].minor);
}
/* #line 3043 "parser.c" */
        break;
      case 122:
/* #line 1198 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_expr(PHVOLT_T_ARRAYACCESS, yymsp[-3].minor.yy168, yymsp[-1].minor.yy168, NULL, status->scanner_state);
  yy_destructor(24,&yymsp[-2].minor);
  yy_destructor(80,&yymsp[0].minor);
}
/* #line 3052 "parser.c" */
        break;
      case 123:
/* #line 1202 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_expr(PHVOLT_T_TERNARY, yymsp[-2].minor.yy168, yymsp[0].minor.yy168, yymsp[-4].minor.yy168, status->scanner_state);
  yy_destructor(4,&yymsp[-3].minor);
  yy_destructor(5,&yymsp[-1].minor);
}
/* #line 3061 "parser.c" */
        break;
      case 124:
/* #line 1206 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_slice(yymsp[-4].minor.yy168, NULL, yymsp[-1].minor.yy168, status->scanner_state);
  yy_destructor(24,&yymsp[-3].minor);
  yy_destructor(5,&yymsp[-2].minor);
  yy_destructor(80,&yymsp[0].minor);
}
/* #line 3071 "parser.c" */
        break;
      case 125:
/* #line 1210 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_slice(yymsp[-4].minor.yy168, yymsp[-2].minor.yy168, NULL, status->scanner_state);
  yy_destructor(24,&yymsp[-3].minor);
  yy_destructor(5,&yymsp[-1].minor);
  yy_destructor(80,&yymsp[0].minor);
}
/* #line 3081 "parser.c" */
        break;
      case 126:
/* #line 1214 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_slice(yymsp[-5].minor.yy168, yymsp[-3].minor.yy168, yymsp[-1].minor.yy168, status->scanner_state);
  yy_destructor(24,&yymsp[-4].minor);
  yy_destructor(5,&yymsp[-2].minor);
  yy_destructor(80,&yymsp[0].minor);
}
/* #line 3091 "parser.c" */
        break;
      case 131:
      case 139:
/* #line 1234 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_named_item(yymsp[-2].minor.yy0, yymsp[0].minor.yy168, status->scanner_state);
  yy_destructor(5,&yymsp[-1].minor);
}
/* #line 3100 "parser.c" */
        break;
      case 132:
      case 138:
/* #line 1238 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_named_item(NULL, yymsp[0].minor.yy168, status->scanner_state);
}
/* #line 3108 "parser.c" */
        break;
      case 134:
/* #line 1246 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_func_call(yymsp[-3].minor.yy168, yymsp[-1].minor.yy168, status->scanner_state);
  yy_destructor(29,&yymsp[-2].minor);
  yy_destructor(47,&yymsp[0].minor);
}
/* #line 3117 "parser.c" */
        break;
      case 135:
/* #line 1250 "parser.y" */
{
	yygotominor.yy168 = phvolt_ret_func_call(yymsp[-2].minor.yy168, NULL, status->scanner_state);
  yy_destructor(29,&yymsp[-1].minor);
  yy_destructor(47,&yymsp[0].minor);
}
/* #line 3126 "parser.c" */
        break;
  };
  yygoto = yyRuleInfo[yyruleno].lhs;
  yysize = yyRuleInfo[yyruleno].nrhs;
  yypParser->yyidx -= yysize;
  yyact = yy_find_reduce_action(yypParser,yygoto);
  if( yyact < YYNSTATE ){
    yy_shift(yypParser,yyact,yygoto,&yygotominor);
  }else if( yyact == YYNSTATE + YYNRULE + 1 ){
    yy_accept(yypParser);
  }
}

/*
** The following code executes when the parse fails
*/
static void yy_parse_failed(
  yyParser *yypParser           /* The parser */
){
  phvolt_ARG_FETCH;
#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sFail!\n",yyTracePrompt);
  }
#endif
  while( yypParser->yyidx>=0 ) yy_pop_parser_stack(yypParser);
  /* Here code is inserted which will be executed whenever the
  ** parser fails */
  phvolt_ARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/*
** The following code executes when a syntax error first occurs.
*/
static void yy_syntax_error(
  yyParser *yypParser,           /* The parser */
  int yymajor,                   /* The major type of the error token */
  YYMINORTYPE yyminor            /* The minor type of the error token */
){
  phvolt_ARG_FETCH;
#define TOKEN (yyminor.yy0)
/* #line 612 "parser.y" */

	{

		smart_str error_str = {0};

		char *token_name = NULL;
		const phvolt_token_names *tokens = phvolt_tokens;
		int token_len = 0;
		uint active_token = status->scanner_state->active_token;

		if (status->scanner_state->start_length) {

			if (active_token) {

				do {
					if (tokens->code == active_token) {
						token_name = tokens->name;
						token_len = tokens->len;
						break;
					}
					++tokens;
				} while (tokens[0].code != 0);

			}

			smart_str_appendl(&error_str, "Syntax error, unexpected token ", sizeof("Syntax error, unexpected token ") - 1);
			if (!token_name) {
				smart_str_appendl(&error_str, "UNKNOWN", sizeof("UNKNOWN") - 1);
			} else {
				smart_str_appendl(&error_str, token_name, token_len);
			}
			if (status->token->value) {
				smart_str_appendc(&error_str, '(');
				smart_str_appendl(&error_str, status->token->value, status->token->len);
				smart_str_appendc(&error_str, ')');
			}
			smart_str_appendl(&error_str, " in ", sizeof(" in ") - 1);
			smart_str_appendl(&error_str, Z_STRVAL_P(status->scanner_state->active_file), Z_STRLEN_P(status->scanner_state->active_file));
			smart_str_appendl(&error_str, " on line ", sizeof(" on line ") - 1);
			{
				char stmp[MAX_LENGTH_OF_LONG + 1];
				int str_len;
				str_len = slprintf(stmp, sizeof(stmp), "%ld", status->scanner_state->active_line);
				smart_str_appendl(&error_str, stmp, str_len);
			}

		} else {

			smart_str_appendl(&error_str, "Syntax error, unexpected EOF in ", sizeof("Syntax error, unexpected EOF in ") - 1);
			smart_str_appendl(&error_str, Z_STRVAL_P(status->scanner_state->active_file), Z_STRLEN_P(status->scanner_state->active_file));

			/* Report unclosed 'if' blocks */
			if ((status->scanner_state->if_level + status->scanner_state->old_if_level) > 0) {
				if ((status->scanner_state->if_level + status->scanner_state->old_if_level) == 1) {
					smart_str_appendl(&error_str, ", there is one 'if' block without close", sizeof(", there is one 'if' block without close") - 1);
				} else {
					smart_str_appendl(&error_str, ", there are ", sizeof(", there are ") - 1);
					{
						char stmp[MAX_LENGTH_OF_LONG + 1];
						int str_len;
						str_len = slprintf(stmp, sizeof(stmp), "%ld", status->scanner_state->if_level + status->scanner_state->old_if_level);
						smart_str_appendl(&error_str, stmp, str_len);
					}
					smart_str_appendl(&error_str, " 'if' blocks without close", sizeof(" 'if' blocks without close") - 1);
				}
			}

			/* Report unclosed 'for' blocks */
			if (status->scanner_state->for_level > 0) {
				if (status->scanner_state->for_level == 1) {
					smart_str_appendl(&error_str, ", there is one 'for' block without close", sizeof(", there is one 'for' block without close") - 1);
				} else {
					smart_str_appendl(&error_str, ", there are ", sizeof(", there are ") - 1);
					{
						char stmp[MAX_LENGTH_OF_LONG + 1];
						int str_len;
						str_len = slprintf(stmp, sizeof(stmp), "%ld", status->scanner_state->if_level);
						smart_str_appendl(&error_str, stmp, str_len);
					}
					smart_str_appendl(&error_str, " 'for' blocks without close", sizeof(" 'for' blocks without close") - 1);
				}
			}
		}

		smart_str_0(&error_str);

		if (error_str.len) {
			status->syntax_error = error_str.c;
			status->syntax_error_len = error_str.len;
		} else {
			status->syntax_error = NULL;
		}

	}

	status->status = PHVOLT_PARSING_FAILED;

/* #line 3266 "parser.c" */
  phvolt_ARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/*
** The following is executed when the parser accepts
*/
static void yy_accept(
  yyParser *yypParser           /* The parser */
){
  phvolt_ARG_FETCH;
#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sAccept!\n",yyTracePrompt);
  }
#endif
  while( yypParser->yyidx>=0 ) yy_pop_parser_stack(yypParser);
  /* Here code is inserted which will be executed whenever the
  ** parser accepts */
  phvolt_ARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/* The main parser program.
** The first argument is a pointer to a structure obtained from
** "phvolt_Alloc" which describes the current state of the parser.
** The second argument is the major token number.  The third is
** the minor token.  The fourth optional argument is whatever the
** user wants (and specified in the grammar) and is available for
** use by the action routines.
**
** Inputs:
** <ul>
** <li> A pointer to the parser (an opaque structure.)
** <li> The major token number.
** <li> The minor token number.
** <li> An option argument of a grammar-specified type.
** </ul>
**
** Outputs:
** None.
*/
void phvolt_(
  void *yyp,                   /* The parser */
  int yymajor,                 /* The major token code number */
  phvolt_TOKENTYPE yyminor       /* The value for the token */
  phvolt_ARG_PDECL               /* Optional %extra_argument parameter */
){
  YYMINORTYPE yyminorunion;
  int yyact;            /* The parser action. */
  int yyendofinput;     /* True if we are at the end of input */
  int yyerrorhit = 0;   /* True if yymajor has invoked an error */
  yyParser *yypParser;  /* The parser */

  /* (re)initialize the parser, if necessary */
  yypParser = (yyParser*)yyp;
  if( yypParser->yyidx<0 ){
    if( yymajor==0 ) return;
    yypParser->yyidx = 0;
    yypParser->yyerrcnt = -1;
    yypParser->yystack[0].stateno = 0;
    yypParser->yystack[0].major = 0;
  }
  yyminorunion.yy0 = yyminor;
  yyendofinput = (yymajor==0);
  phvolt_ARG_STORE;

#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sInput %s\n",yyTracePrompt,yyTokenName[yymajor]);
  }
#endif

  do{
    yyact = yy_find_shift_action(yypParser,yymajor);
    if( yyact<YYNSTATE ){
      yy_shift(yypParser,yyact,yymajor,&yyminorunion);
      yypParser->yyerrcnt--;
      if( yyendofinput && yypParser->yyidx>=0 ){
        yymajor = 0;
      }else{
        yymajor = YYNOCODE;
      }
    }else if( yyact < YYNSTATE + YYNRULE ){
      yy_reduce(yypParser,yyact-YYNSTATE);
    }else if( yyact == YY_ERROR_ACTION ){
      int yymx;
#ifndef NDEBUG
      if( yyTraceFILE ){
        fprintf(yyTraceFILE,"%sSyntax Error!\n",yyTracePrompt);
      }
#endif
#ifdef YYERRORSYMBOL
      /* A syntax error has occurred.
      ** The response to an error depends upon whether or not the
      ** grammar defines an error token "ERROR".  
      **
      ** This is what we do if the grammar does define ERROR:
      **
      **  * Call the %syntax_error function.
      **
      **  * Begin popping the stack until we enter a state where
      **    it is legal to shift the error symbol, then shift
      **    the error symbol.
      **
      **  * Set the error count to three.
      **
      **  * Begin accepting and shifting new tokens.  No new error
      **    processing will occur until three tokens have been
      **    shifted successfully.
      **
      */
      if( yypParser->yyerrcnt<0 ){
        yy_syntax_error(yypParser,yymajor,yyminorunion);
      }
      yymx = yypParser->yystack[yypParser->yyidx].major;
      if( yymx==YYERRORSYMBOL || yyerrorhit ){
#ifndef NDEBUG
        if( yyTraceFILE ){
          fprintf(yyTraceFILE,"%sDiscard input token %s\n",
             yyTracePrompt,yyTokenName[yymajor]);
        }
#endif
        yy_destructor(yymajor,&yyminorunion);
        yymajor = YYNOCODE;
      }else{
         while(
          yypParser->yyidx >= 0 &&
          yymx != YYERRORSYMBOL &&
          (yyact = yy_find_shift_action(yypParser,YYERRORSYMBOL)) >= YYNSTATE
        ){
          yy_pop_parser_stack(yypParser);
        }
        if( yypParser->yyidx < 0 || yymajor==0 ){
          yy_destructor(yymajor,&yyminorunion);
          yy_parse_failed(yypParser);
          yymajor = YYNOCODE;
        }else if( yymx!=YYERRORSYMBOL ){
          YYMINORTYPE u2;
          u2.YYERRSYMDT = 0;
          yy_shift(yypParser,yyact,YYERRORSYMBOL,&u2);
        }
      }
      yypParser->yyerrcnt = 3;
      yyerrorhit = 1;
#else  /* YYERRORSYMBOL is not defined */
      /* This is what we do if the grammar does not define ERROR:
      **
      **  * Report an error message, and throw away the input token.
      **
      **  * If the input token is $, then fail the parse.
      **
      ** As before, subsequent error messages are suppressed until
      ** three input tokens have been successfully shifted.
      */
      if( yypParser->yyerrcnt<=0 ){
        yy_syntax_error(yypParser,yymajor,yyminorunion);
      }
      yypParser->yyerrcnt = 3;
      yy_destructor(yymajor,&yyminorunion);
      if( yyendofinput ){
        yy_parse_failed(yypParser);
      }
      yymajor = YYNOCODE;
#endif
    }else{
      yy_accept(yypParser);
      yymajor = YYNOCODE;
    }
  }while( yymajor!=YYNOCODE && yypParser->yyidx>=0 );
  return;
}

/*
  +------------------------------------------------------------------------+
  | Phalcon Framework                                                      |
  +------------------------------------------------------------------------+
  | Copyright (c) 2011-2014 Phalcon Team (http://www.phalconphp.com)       |
  +------------------------------------------------------------------------+
  | This source file is subject to the New BSD License that is bundled     |
  | with this package in the file docs/LICENSE.txt.                        |
  |                                                                        |
  | If you did not receive a copy of the license and are unable to         |
  | obtain it through the world-wide-web, please send an email             |
  | to license@phalconphp.com so we can send you a copy immediately.       |
  +------------------------------------------------------------------------+
  | Authors: Andres Gutierrez <andres@phalconphp.com>                      |
  |          Eduar Carvajal <eduar@phalconphp.com>                         |
  +------------------------------------------------------------------------+
*/

const phvolt_token_names phvolt_tokens[] =
{
  { SL("INTEGER"),        PHVOLT_T_INTEGER },
  { SL("DOUBLE"),         PHVOLT_T_DOUBLE },
  { SL("STRING"),         PHVOLT_T_STRING },
  { SL("IDENTIFIER"),     PHVOLT_T_IDENTIFIER },
  { SL("MINUS"),          PHVOLT_T_MINUS },
  { SL("+"),              PHVOLT_T_ADD },
  { SL("-"),              PHVOLT_T_SUB },
  { SL("*"),              PHVOLT_T_MUL },
  { SL("/"),              PHVOLT_T_DIV },
  { SL("%%"),             PHVOLT_T_MOD },
  { SL("!"),              PHVOLT_T_NOT },
  { SL("~"),              PHVOLT_T_CONCAT },
  { SL("AND"),            PHVOLT_T_AND },
  { SL("OR"),             PHVOLT_T_OR },
  { SL("DOT"),            PHVOLT_T_DOT },
  { SL("COMMA"),          PHVOLT_T_COMMA },
  { SL("EQUALS"),         PHVOLT_T_EQUALS },
  { SL("NOT EQUALS"),     PHVOLT_T_NOTEQUALS },
  { SL("IDENTICAL"),      PHVOLT_T_IDENTICAL },
  { SL("NOT IDENTICAL"),  PHVOLT_T_NOTIDENTICAL },
  { SL("NOT"),            PHVOLT_T_NOT },
  { SL("RANGE"),          PHVOLT_T_RANGE },
  { SL("COLON"),          PHVOLT_T_COLON },
  { SL("QUESTION MARK"),  PHVOLT_T_QUESTION },
  { SL("<"),              PHVOLT_T_LESS },
  { SL("<="),             PHVOLT_T_LESSEQUAL },
  { SL(">"),              PHVOLT_T_GREATER },
  { SL(">="),             PHVOLT_T_GREATEREQUAL },
  { SL("("),              PHVOLT_T_PARENTHESES_OPEN },
  { SL(")"),              PHVOLT_T_PARENTHESES_CLOSE },
  { SL("["),              PHVOLT_T_SBRACKET_OPEN },
  { SL("]"),              PHVOLT_T_SBRACKET_CLOSE },
  { SL("{"),              PHVOLT_T_CBRACKET_OPEN },
  { SL("}"),              PHVOLT_T_CBRACKET_CLOSE },
  { SL("{%"),             PHVOLT_T_OPEN_DELIMITER },
  { SL("%}"),             PHVOLT_T_CLOSE_DELIMITER },
  { SL("{{"),             PHVOLT_T_OPEN_EDELIMITER },
  { SL("}}"),             PHVOLT_T_CLOSE_EDELIMITER },
  { SL("IF"),             PHVOLT_T_IF },
  { SL("ELSE"),           PHVOLT_T_ELSE },
  { SL("ELSEIF"),         PHVOLT_T_ELSEIF },
  { SL("ELSEFOR"),        PHVOLT_T_ELSEFOR },
  { SL("ENDIF"),          PHVOLT_T_ENDIF },
  { SL("FOR"),            PHVOLT_T_FOR },
  { SL("IN"),             PHVOLT_T_IN },
  { SL("ENDFOR"),         PHVOLT_T_ENDFOR },
  { SL("SET"),            PHVOLT_T_SET },
  { SL("ASSIGN"),         PHVOLT_T_ASSIGN },
  { SL("+="),             PHVOLT_T_ADD_ASSIGN },
  { SL("-="),             PHVOLT_T_SUB_ASSIGN },
  { SL("*="),             PHVOLT_T_MUL_ASSIGN },
  { SL("/="),             PHVOLT_T_DIV_ASSIGN },
  { SL("++"),             PHVOLT_T_INCR },
  { SL("--"),             PHVOLT_T_DECR },
  { SL("BLOCK"),          PHVOLT_T_BLOCK },
  { SL("ENDBLOCK"),       PHVOLT_T_ENDBLOCK },
  { SL("CACHE"),          PHVOLT_T_CACHE },
  { SL("ENDCACHE"),       PHVOLT_T_ENDCACHE },
  { SL("EXTENDS"),        PHVOLT_T_EXTENDS },
  { SL("IS"),             PHVOLT_T_IS },
  { SL("DEFINED"),        PHVOLT_T_DEFINED },
  { SL("EMPTY"),          PHVOLT_T_EMPTY },
  { SL("EVEN"),           PHVOLT_T_EVEN },
  { SL("ODD"),            PHVOLT_T_ODD },
  { SL("NUMERIC"),        PHVOLT_T_NUMERIC },
  { SL("SCALAR"),         PHVOLT_T_SCALAR },
  { SL("ITERABLE"),       PHVOLT_T_ITERABLE },
  { SL("INCLUDE"),        PHVOLT_T_INCLUDE },
  { SL("DO"),             PHVOLT_T_DO },
  { SL("WHITESPACE"),     PHVOLT_T_IGNORE },
  { SL("AUTOESCAPE"),     PHVOLT_T_AUTOESCAPE },
  { SL("ENDAUTOESCAPE"),  PHVOLT_T_ENDAUTOESCAPE },
  { SL("CONTINUE"),       PHVOLT_T_CONTINUE },
  { SL("BREAK"),          PHVOLT_T_BREAK },
  { SL("WITH"),           PHVOLT_T_WITH },
  { SL("RETURN"),         PHVOLT_T_RETURN },
  { SL("MACRO"),          PHVOLT_T_MACRO },
  { SL("ENDMACRO"),       PHVOLT_T_ENDMACRO },
  { SL("CALL"),           PHVOLT_T_CALL },
  { SL("WITH"),           PHVOLT_T_WITH },
  { NULL, 0, 0 }
};

/**
 * Wrapper to alloc memory within the parser
 */
static void *phvolt_wrapper_alloc(size_t bytes){
	return emalloc(bytes);
}

/**
 * Wrapper to free memory within the parser
 */
static void phvolt_wrapper_free(void *pointer){
	efree(pointer);
}

/**
 * Creates a parser_token to be passed to the parser
 */
static void phvolt_parse_with_token(void* phvolt_parser, int opcode, int parsercode, phvolt_scanner_token *token, phvolt_parser_status *parser_status){

	phvolt_parser_token *pToken;

	pToken = emalloc(sizeof(phvolt_parser_token));
	pToken->opcode = opcode;
	pToken->token = token->value;
	pToken->token_len = token->len;
	pToken->free_flag = 1;

	phvolt_(phvolt_parser, parsercode, pToken, parser_status);

	token->value = NULL;
	token->len = 0;
}

/**
 * Creates an error message
 */
static void phvolt_create_error_msg(phvolt_parser_status *parser_status, char *message){

	unsigned int length = (128 + Z_STRLEN_P(parser_status->scanner_state->active_file));
	char *str = emalloc(sizeof(char) * length);

	snprintf(str, length, "%s in %s on line %d", message, Z_STRVAL_P(parser_status->scanner_state->active_file), parser_status->scanner_state->active_line);
	str[length - 1] = '\0';

	parser_status->syntax_error = estrndup(str, strlen(str));
	efree(str);
}

/**
 * Creates an error message when it's triggered by the scanner
 */
static void phvolt_scanner_error_msg(phvolt_parser_status *parser_status, zval **error_msg TSRMLS_DC){

	char *error, *error_part;
	int length;
	phvolt_scanner_state *state = parser_status->scanner_state;

	MAKE_STD_ZVAL(*error_msg);
	if (state->start) {
		error = emalloc(sizeof(char) * 72 + state->start_length +  Z_STRLEN_P(state->active_file));
		if (state->start_length > 16) {
			length = 72 + Z_STRLEN_P(state->active_file);
			error_part = estrndup(state->start, 16);
			snprintf(error, length, "Scanning error before '%s...' in %s on line %d", error_part, Z_STRVAL_P(state->active_file), state->active_line);
			error[length - 1] = '\0';
		} else {
			length = 48 + state->start_length + Z_STRLEN_P(state->active_file);
			snprintf(error, length, "Scanning error before '%s' in %s on line %d", state->start, Z_STRVAL_P(state->active_file), state->active_line);
		}
	} else {
		error = emalloc(sizeof(char) * (32 + Z_STRLEN_P(state->active_file)));
		length = 32 + Z_STRLEN_P(state->active_file);
		snprintf(error, length, "Scanning error near to EOF in %s", Z_STRVAL_P(state->active_file));
	}

	error[length - 1] = '\0';
	ZVAL_STRING(*error_msg, error, 1);
	efree(error);
}

/**
 * Receives the volt code tokenizes and parses it
 */
int phvolt_parse_view(zval *result, zval *view_code, zval *template_path TSRMLS_DC){

	zval *error_msg = NULL;

	ZVAL_NULL(result);

	if (Z_TYPE_P(view_code) != IS_STRING) {
		PHALCON_THROW_EXCEPTION_STR(phalcon_mvc_view_exception_ce, "View code must be a string");
		return FAILURE;
	}

	if (phvolt_internal_parse_view(&result, view_code, template_path, &error_msg TSRMLS_CC) == FAILURE) {
		if (likely(error_msg != NULL)) {
			PHALCON_THROW_EXCEPTION_STRW(phalcon_mvc_view_exception_ce, Z_STRVAL_P(error_msg));
			zval_ptr_dtor(&error_msg);
		}
		else {
			PHALCON_THROW_EXCEPTION_STRW(phalcon_mvc_view_exception_ce, "Error parsing the view");
		}

		return FAILURE;
	}

	return SUCCESS;
}

/**
 * Checks whether the token has only blank characters
 */
int phvolt_is_blank_string(phvolt_scanner_token *token){

	char *marker = token->value;
	int i;

	for (i = 0; i < token->len; i++) {
		char ch = *marker;
		if (ch != ' ' && ch != '\t' && ch != '\n' && ch != '\r' && ch != 11) {
			return 0;
		}
		marker++;
	}

	return 1;
}

/**
 * Parses a volt template returning an intermediate array representation
 */
int phvolt_internal_parse_view(zval **result, zval *view_code, zval *template_path, zval **error_msg TSRMLS_DC) {

	char *error;
	phvolt_scanner_state *state;
	phvolt_scanner_token token;
	int scanner_status, status = SUCCESS;
	phvolt_parser_status *parser_status = NULL;
	void* phvolt_parser;

	/** Check if the view has code */
	if (!Z_STRVAL_P(view_code)) {
		MAKE_STD_ZVAL(*error_msg);
		ZVAL_STRING(*error_msg, "View code cannot be null", 1);
		return FAILURE;
	}

	if (!Z_STRLEN_P(view_code)) {
		array_init(*result);
		return SUCCESS;
	}

	/** Start the reentrant parser */
	phvolt_parser = phvolt_Alloc(phvolt_wrapper_alloc);
	if (unlikely(!phvolt_parser)) {
		MAKE_STD_ZVAL(*error_msg);
		ZVAL_STRING(*error_msg, "Memory allocation error", 1);
		return FAILURE;
	}

	parser_status = emalloc(sizeof(phvolt_parser_status));
	state = emalloc(sizeof(phvolt_scanner_state));

	parser_status->status = PHVOLT_PARSING_OK;
	parser_status->scanner_state = state;
	parser_status->ret = NULL;
	parser_status->token = &token;
	parser_status->syntax_error = NULL;

	/** Initialize the scanner state */
	state->active_token = 0;
	state->start = Z_STRVAL_P(view_code);
	state->mode = PHVOLT_MODE_RAW;
	state->raw_buffer = emalloc(sizeof(char) * PHVOLT_RAW_BUFFER_SIZE);
	state->raw_buffer_size = PHVOLT_RAW_BUFFER_SIZE;
	state->raw_buffer_cursor = 0;
	state->active_file = template_path;
	state->active_line = 1;
	state->statement_position = 0;
	state->extends_mode = 0;
	state->block_level = 0;
	state->macro_level = 0;
	state->start_length = 0;
	state->old_if_level = 0;
	state->if_level = 0;
	state->for_level = 0;
	state->whitespace_control = 0;

	state->end = state->start;

	token.value = NULL;
	token.len = 0;

	while (0 <= (scanner_status = phvolt_get_token(state, &token))) {

		state->active_token = token.opcode;

		state->start_length = (Z_STRVAL_P(view_code) + Z_STRLEN_P(view_code) - state->start);

		switch (token.opcode) {

			case PHVOLT_T_IGNORE:
				break;

			case PHVOLT_T_ADD:
				phvolt_(phvolt_parser, PHVOLT_PLUS, NULL, parser_status);
				break;
			case PHVOLT_T_SUB:
				phvolt_(phvolt_parser, PHVOLT_MINUS, NULL, parser_status);
				break;
			case PHVOLT_T_MUL:
				phvolt_(phvolt_parser, PHVOLT_TIMES, NULL, parser_status);
				break;
			case PHVOLT_T_DIV:
				phvolt_(phvolt_parser, PHVOLT_DIVIDE, NULL, parser_status);
				break;
			case PHVOLT_T_MOD:
				phvolt_(phvolt_parser, PHVOLT_MOD, NULL, parser_status);
				break;
			case PHVOLT_T_AND:
				phvolt_(phvolt_parser, PHVOLT_AND, NULL, parser_status);
				break;
			case PHVOLT_T_OR:
				phvolt_(phvolt_parser, PHVOLT_OR, NULL, parser_status);
				break;
			case PHVOLT_T_IS:
				phvolt_(phvolt_parser, PHVOLT_IS, NULL, parser_status);
				break;
			case PHVOLT_T_EQUALS:
				phvolt_(phvolt_parser, PHVOLT_EQUALS, NULL, parser_status);
				break;
			case PHVOLT_T_NOTEQUALS:
				phvolt_(phvolt_parser, PHVOLT_NOTEQUALS, NULL, parser_status);
				break;
			case PHVOLT_T_LESS:
				phvolt_(phvolt_parser, PHVOLT_LESS, NULL, parser_status);
				break;
			case PHVOLT_T_GREATER:
				phvolt_(phvolt_parser, PHVOLT_GREATER, NULL, parser_status);
				break;
			case PHVOLT_T_GREATEREQUAL:
				phvolt_(phvolt_parser, PHVOLT_GREATEREQUAL, NULL, parser_status);
				break;
			case PHVOLT_T_LESSEQUAL:
				phvolt_(phvolt_parser, PHVOLT_LESSEQUAL, NULL, parser_status);
				break;
			case PHVOLT_T_IDENTICAL:
				phvolt_(phvolt_parser, PHVOLT_IDENTICAL, NULL, parser_status);
				break;
			case PHVOLT_T_NOTIDENTICAL:
				phvolt_(phvolt_parser, PHVOLT_NOTIDENTICAL, NULL, parser_status);
				break;
			case PHVOLT_T_NOT:
				phvolt_(phvolt_parser, PHVOLT_NOT, NULL, parser_status);
				break;
			case PHVOLT_T_DOT:
				phvolt_(phvolt_parser, PHVOLT_DOT, NULL, parser_status);
				break;
			case PHVOLT_T_CONCAT:
				phvolt_(phvolt_parser, PHVOLT_CONCAT, NULL, parser_status);
				break;
			case PHVOLT_T_RANGE:
				phvolt_(phvolt_parser, PHVOLT_RANGE, NULL, parser_status);
				break;
			case PHVOLT_T_PIPE:
				phvolt_(phvolt_parser, PHVOLT_PIPE, NULL, parser_status);
				break;
			case PHVOLT_T_COMMA:
				phvolt_(phvolt_parser, PHVOLT_COMMA, NULL, parser_status);
				break;
			case PHVOLT_T_COLON:
				phvolt_(phvolt_parser, PHVOLT_COLON, NULL, parser_status);
				break;
			case PHVOLT_T_QUESTION:
				phvolt_(phvolt_parser, PHVOLT_QUESTION, NULL, parser_status);
				break;

			case PHVOLT_T_PARENTHESES_OPEN:
				phvolt_(phvolt_parser, PHVOLT_PARENTHESES_OPEN, NULL, parser_status);
				break;
			case PHVOLT_T_PARENTHESES_CLOSE:
				phvolt_(phvolt_parser, PHVOLT_PARENTHESES_CLOSE, NULL, parser_status);
				break;
			case PHVOLT_T_SBRACKET_OPEN:
				phvolt_(phvolt_parser, PHVOLT_SBRACKET_OPEN, NULL, parser_status);
				break;
			case PHVOLT_T_SBRACKET_CLOSE:
				phvolt_(phvolt_parser, PHVOLT_SBRACKET_CLOSE, NULL, parser_status);
				break;
			case PHVOLT_T_CBRACKET_OPEN:
				phvolt_(phvolt_parser, PHVOLT_CBRACKET_OPEN, NULL, parser_status);
				break;
			case PHVOLT_T_CBRACKET_CLOSE:
				phvolt_(phvolt_parser, PHVOLT_CBRACKET_CLOSE, NULL, parser_status);
				break;

			case PHVOLT_T_OPEN_DELIMITER:
				phvolt_(phvolt_parser, PHVOLT_OPEN_DELIMITER, NULL, parser_status);
				break;
			case PHVOLT_T_CLOSE_DELIMITER:
				phvolt_(phvolt_parser, PHVOLT_CLOSE_DELIMITER, NULL, parser_status);
				break;

			case PHVOLT_T_OPEN_EDELIMITER:
				if (state->extends_mode == 1 && state->block_level == 0) {
					phvolt_create_error_msg(parser_status, "Child templates only may contain blocks");
					parser_status->status = PHVOLT_PARSING_FAILED;
					break;
				}
				phvolt_(phvolt_parser, PHVOLT_OPEN_EDELIMITER, NULL, parser_status);
				break;
			case PHVOLT_T_CLOSE_EDELIMITER:
				phvolt_(phvolt_parser, PHVOLT_CLOSE_EDELIMITER, NULL, parser_status);
				break;

			case PHVOLT_T_NULL:
				phvolt_(phvolt_parser, PHVOLT_NULL, NULL, parser_status);
				break;
			case PHVOLT_T_TRUE:
				phvolt_(phvolt_parser, PHVOLT_TRUE, NULL, parser_status);
				break;
			case PHVOLT_T_FALSE:
				phvolt_(phvolt_parser, PHVOLT_FALSE, NULL, parser_status);
				break;

			case PHVOLT_T_INTEGER:
				phvolt_parse_with_token(phvolt_parser, PHVOLT_T_INTEGER, PHVOLT_INTEGER, &token, parser_status);
				break;
			case PHVOLT_T_DOUBLE:
				phvolt_parse_with_token(phvolt_parser, PHVOLT_T_DOUBLE, PHVOLT_DOUBLE, &token, parser_status);
				break;
			case PHVOLT_T_STRING:
				phvolt_parse_with_token(phvolt_parser, PHVOLT_T_STRING, PHVOLT_STRING, &token, parser_status);
				break;
			case PHVOLT_T_IDENTIFIER:
				phvolt_parse_with_token(phvolt_parser, PHVOLT_T_IDENTIFIER, PHVOLT_IDENTIFIER, &token, parser_status);
				break;

			case PHVOLT_T_IF:
				if (state->extends_mode == 1 && state->block_level == 0){
					phvolt_create_error_msg(parser_status, "Child templates only may contain blocks");
					parser_status->status = PHVOLT_PARSING_FAILED;
					break;
				} else {
					state->if_level++;
					state->block_level++;
				}
				phvolt_(phvolt_parser, PHVOLT_IF, NULL, parser_status);
				break;

			case PHVOLT_T_ELSE:
				if (state->if_level == 0 && state->for_level > 0) {
					phvolt_(phvolt_parser, PHVOLT_ELSEFOR, NULL, parser_status);
				} else {
					phvolt_(phvolt_parser, PHVOLT_ELSE, NULL, parser_status);
				}
				break;

			case PHVOLT_T_ELSEFOR:
				phvolt_(phvolt_parser, PHVOLT_ELSEFOR, NULL, parser_status);
				break;

			case PHVOLT_T_ELSEIF:
				if (state->if_level == 0) {
					phvolt_create_error_msg(parser_status, "Unexpected ENDIF");
					parser_status->status = PHVOLT_PARSING_FAILED;
					break;
				}
				phvolt_(phvolt_parser, PHVOLT_ELSEIF, NULL, parser_status);
				break;

			case PHVOLT_T_ENDIF:
				state->block_level--;
				state->if_level--;
				phvolt_(phvolt_parser, PHVOLT_ENDIF, NULL, parser_status);
				break;

			case PHVOLT_T_FOR:
				if (state->extends_mode == 1 && state->block_level == 0){
					phvolt_create_error_msg(parser_status, "Child templates only may contain blocks");
					parser_status->status = PHVOLT_PARSING_FAILED;
					break;
				} else {
					state->old_if_level = state->if_level;
					state->if_level = 0;
					state->for_level++;
					state->block_level++;
				}
				phvolt_(phvolt_parser, PHVOLT_FOR, NULL, parser_status);
				break;

			case PHVOLT_T_IN:
				phvolt_(phvolt_parser, PHVOLT_IN, NULL, parser_status);
				break;

			case PHVOLT_T_ENDFOR:
				state->block_level--;
				state->for_level--;
				state->if_level = state->old_if_level;
				phvolt_(phvolt_parser, PHVOLT_ENDFOR, NULL, parser_status);
				break;

			case PHVOLT_T_RAW_FRAGMENT:
				if (token.len > 0) {
					if (state->extends_mode == 1 && state->block_level == 0){
						if (!phvolt_is_blank_string(&token)) {
							phvolt_create_error_msg(parser_status, "Child templates only may contain blocks");
							parser_status->status = PHVOLT_PARSING_FAILED;
						}
						efree(token.value);
						break;
					} else {
						if (!phvolt_is_blank_string(&token)) {
							state->statement_position++;
						}
					}
					phvolt_parse_with_token(phvolt_parser, PHVOLT_T_RAW_FRAGMENT, PHVOLT_RAW_FRAGMENT, &token, parser_status);
				} else {
					efree(token.value);
				}
				break;

			case PHVOLT_T_SET:
				if (state->extends_mode == 1 && state->block_level == 0){
					phvolt_create_error_msg(parser_status, "Child templates only may contain blocks");
					parser_status->status = PHVOLT_PARSING_FAILED;
					break;
				}
				phvolt_(phvolt_parser, PHVOLT_SET, NULL, parser_status);
				break;
			case PHVOLT_T_ASSIGN:
				phvolt_(phvolt_parser, PHVOLT_ASSIGN, NULL, parser_status);
				break;
			case PHVOLT_T_ADD_ASSIGN:
				phvolt_(phvolt_parser, PHVOLT_ADD_ASSIGN, NULL, parser_status);
				break;
			case PHVOLT_T_SUB_ASSIGN:
				phvolt_(phvolt_parser, PHVOLT_SUB_ASSIGN, NULL, parser_status);
				break;
			case PHVOLT_T_MUL_ASSIGN:
				phvolt_(phvolt_parser, PHVOLT_MUL_ASSIGN, NULL, parser_status);
				break;
			case PHVOLT_T_DIV_ASSIGN:
				phvolt_(phvolt_parser, PHVOLT_DIV_ASSIGN, NULL, parser_status);
				break;

			case PHVOLT_T_INCR:
				phvolt_(phvolt_parser, PHVOLT_INCR, NULL, parser_status);
				break;
			case PHVOLT_T_DECR:
				phvolt_(phvolt_parser, PHVOLT_DECR, NULL, parser_status);
				break;

			case PHVOLT_T_BLOCK:
				if (state->block_level > 0) {
					phvolt_create_error_msg(parser_status, "Embedding blocks into other blocks is not supported");
					parser_status->status = PHVOLT_PARSING_FAILED;
					break;
				} else {
					state->block_level++;
				}
				phvolt_(phvolt_parser, PHVOLT_BLOCK, NULL, parser_status);
				break;
			case PHVOLT_T_ENDBLOCK:
				state->block_level--;
				phvolt_(phvolt_parser, PHVOLT_ENDBLOCK, NULL, parser_status);
				break;

			case PHVOLT_T_MACRO:
				if (state->macro_level > 0) {
					phvolt_create_error_msg(parser_status, "Embedding macros into other macros is not allowed");
					parser_status->status = PHVOLT_PARSING_FAILED;
					break;
				} else {
					state->macro_level++;
				}
				phvolt_(phvolt_parser, PHVOLT_MACRO, NULL, parser_status);
				break;
			case PHVOLT_T_ENDMACRO:
				state->macro_level--;
				phvolt_(phvolt_parser, PHVOLT_ENDMACRO, NULL, parser_status);
				break;

			case PHVOLT_T_CALL:
				phvolt_(phvolt_parser, PHVOLT_CALL, NULL, parser_status);
				break;
			case PHVOLT_T_ENDCALL:
				phvolt_(phvolt_parser, PHVOLT_ENDCALL, NULL, parser_status);
				break;

			case PHVOLT_T_CACHE:
				phvolt_(phvolt_parser, PHVOLT_CACHE, NULL, parser_status);
				break;
			case PHVOLT_T_ENDCACHE:
				phvolt_(phvolt_parser, PHVOLT_ENDCACHE, NULL, parser_status);
				break;

			case PHVOLT_T_INCLUDE:
				phvolt_(phvolt_parser, PHVOLT_INCLUDE, NULL, parser_status);
				break;

			case PHVOLT_T_WITH:
				phvolt_(phvolt_parser, PHVOLT_WITH, NULL, parser_status);
				break;

			case PHVOLT_T_DEFINED:
				phvolt_(phvolt_parser, PHVOLT_DEFINED, NULL, parser_status);
				break;

			case PHVOLT_T_EMPTY:
				phvolt_(phvolt_parser, PHVOLT_EMPTY, NULL, parser_status);
				break;

			case PHVOLT_T_EVEN:
				phvolt_(phvolt_parser, PHVOLT_EVEN, NULL, parser_status);
				break;

			case PHVOLT_T_ODD:
				phvolt_(phvolt_parser, PHVOLT_ODD, NULL, parser_status);
				break;

			case PHVOLT_T_NUMERIC:
				phvolt_(phvolt_parser, PHVOLT_NUMERIC, NULL, parser_status);
				break;

			case PHVOLT_T_SCALAR:
				phvolt_(phvolt_parser, PHVOLT_SCALAR, NULL, parser_status);
				break;

			case PHVOLT_T_ITERABLE:
				phvolt_(phvolt_parser, PHVOLT_ITERABLE, NULL, parser_status);
				break;

			case PHVOLT_T_DO:
				phvolt_(phvolt_parser, PHVOLT_DO, NULL, parser_status);
				break;
			case PHVOLT_T_RETURN:
				phvolt_(phvolt_parser, PHVOLT_RETURN, NULL, parser_status);
				break;

			case PHVOLT_T_AUTOESCAPE:
				phvolt_(phvolt_parser, PHVOLT_AUTOESCAPE, NULL, parser_status);
				break;

			case PHVOLT_T_ENDAUTOESCAPE:
				phvolt_(phvolt_parser, PHVOLT_ENDAUTOESCAPE, NULL, parser_status);
				break;

			case PHVOLT_T_BREAK:
				phvolt_(phvolt_parser, PHVOLT_BREAK, NULL, parser_status);
				break;

			case PHVOLT_T_CONTINUE:
				phvolt_(phvolt_parser, PHVOLT_CONTINUE, NULL, parser_status);
				break;

			case PHVOLT_T_EXTENDS:
				if (state->statement_position != 1) {
					phvolt_create_error_msg(parser_status, "Extends statement must be placed at the first line in the template");
					parser_status->status = PHVOLT_PARSING_FAILED;
					break;
				} else {
					state->extends_mode = 1;
				}
				phvolt_(phvolt_parser, PHVOLT_EXTENDS, NULL, parser_status);
				break;

			default:
				parser_status->status = PHVOLT_PARSING_FAILED;
				if (!*error_msg) {
					error = emalloc(sizeof(char) * (48 + Z_STRLEN_P(state->active_file)));
					snprintf(error, 48 + Z_STRLEN_P(state->active_file) + state->active_line, "Scanner: unknown opcode %d on in %s line %d", token.opcode, Z_STRVAL_P(state->active_file), state->active_line);
					MAKE_STD_ZVAL(*error_msg);
					ZVAL_STRING(*error_msg, error, 1);
					efree(error);
				}
				break;
		}

		if (parser_status->status != PHVOLT_PARSING_OK) {
			status = FAILURE;
			break;
		}

		state->end = state->start;
	}

	if (status != FAILURE) {
		switch (scanner_status) {
			case PHVOLT_SCANNER_RETCODE_ERR:
			case PHVOLT_SCANNER_RETCODE_IMPOSSIBLE:
				if (!*error_msg) {
					phvolt_scanner_error_msg(parser_status, error_msg TSRMLS_CC);
				}
				status = FAILURE;
				break;
			default:
				phvolt_(phvolt_parser, 0, NULL, parser_status);
		}
	}

	state->active_token = 0;
	state->start = NULL;
	efree(state->raw_buffer);

	if (parser_status->status != PHVOLT_PARSING_OK) {
		status = FAILURE;
		if (parser_status->syntax_error) {
			if (!*error_msg) {
				MAKE_STD_ZVAL(*error_msg);
				ZVAL_STRING(*error_msg, parser_status->syntax_error, 1);
			}
			efree(parser_status->syntax_error);
		}
	}

	phvolt_Free(phvolt_parser, phvolt_wrapper_free);

	if (status != FAILURE) {
		if (parser_status->status == PHVOLT_PARSING_OK) {
			if (parser_status->ret) {
				ZVAL_ZVAL(*result, parser_status->ret, 0, 0);
				ZVAL_NULL(parser_status->ret);
				zval_ptr_dtor(&parser_status->ret);
			} else {
				array_init(*result);
			}
		}
	}

	efree(parser_status);
	efree(state);

	return status;
}
