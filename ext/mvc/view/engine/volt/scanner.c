/* Generated by re2c 0.13.5 on Sun Oct  7 22:43:37 2012 */
// 1 "scanner.re"

/*
  +------------------------------------------------------------------------+
  | Phalcon Framework                                                      |
  +------------------------------------------------------------------------+
  | Copyright (c) 2011-2012 Phalcon Team (http://www.phalconphp.com)       |
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_phalcon.h"

#include "scanner.h"

#define KKCTYPE char
#define KKCURSOR (s->start)
#define KKLIMIT (s->end)
#define KKMARKER q

int phvolt_get_token(phvolt_scanner_state *s, phvolt_scanner_token *token) {

	char *q = KKCURSOR, *start = KKCURSOR;
	int status = PHVOLT_SCANNER_RETCODE_IMPOSSIBLE;

	while(PHVOLT_SCANNER_RETCODE_IMPOSSIBLE == status) {

		if (s->mode == PHVOLT_MODE_RAW) {

			if (*KKCURSOR == '\0' || (*KKCURSOR == '{' && (*(KKCURSOR+1) == '%' || *(KKCURSOR+1) == '{'))) {

				s->mode = PHVOLT_MODE_CODE;

				if (s->raw_buffer_cursor > 0) {
					token->opcode = PHVOLT_T_RAW_FRAGMENT;
					token->value = emalloc(sizeof(char) * s->raw_buffer_cursor+1);
					memcpy(token->value, s->raw_buffer, s->raw_buffer_cursor);
					token->value[s->raw_buffer_cursor] = 0;
					token->len = s->raw_buffer_cursor;
					s->raw_buffer_cursor = 0;
					q = KKCURSOR;
				} else {
					token->opcode = PHVOLT_T_IGNORE;
				}

				return 0;

			} else {

				if (s->raw_buffer_cursor == s->raw_buffer_size) {
					s->raw_buffer_size += PHVOLT_RAW_BUFFER_SIZE;
					s->raw_buffer = erealloc(s->raw_buffer, s->raw_buffer_size);
				}

				memcpy(s->raw_buffer+s->raw_buffer_cursor, KKCURSOR, 1);
				s->raw_buffer_cursor++;

				++KKCURSOR;
			}

		} else {

		
// 80 "scanner.c"
		{
			KKCTYPE kkch;
			unsigned int kkaccept = 0;

			kkch = *KKCURSOR;
			switch (kkch) {
			case 0x00:	goto kk57;
			case '\t':
			case '\n':
			case '\r':
			case ' ':	goto kk55;
			case '!':	goto kk49;
			case '"':	goto kk18;
			case '%':	goto kk15;
			case '\'':	goto kk19;
			case '(':	goto kk35;
			case ')':	goto kk37;
			case '*':	goto kk25;
			case '+':	goto kk21;
			case ',':	goto kk33;
			case '-':	goto kk23;
			case '.':	goto kk31;
			case '/':	goto kk27;
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':	goto kk2;
			case ':':	goto kk53;
			case '<':	goto kk43;
			case '=':	goto kk45;
			case '>':	goto kk47;
			case 'A':
			case 'a':	goto kk11;
			case 'B':
			case 'C':
			case 'D':
			case 'G':
			case 'H':
			case 'J':
			case 'K':
			case 'L':
			case 'M':
			case 'P':
			case 'Q':
			case 'R':
			case 'U':
			case 'V':
			case 'W':
			case 'X':
			case 'Y':
			case 'Z':
			case 'b':
			case 'c':
			case 'd':
			case 'g':
			case 'h':
			case 'j':
			case 'k':
			case 'l':
			case 'm':
			case 'p':
			case 'q':
			case 'r':
			case 'u':
			case 'v':
			case 'w':
			case 'x':
			case 'y':
			case 'z':	goto kk20;
			case 'E':
			case 'e':	goto kk6;
			case 'F':
			case 'f':	goto kk7;
			case 'I':
			case 'i':	goto kk4;
			case 'N':
			case 'n':	goto kk9;
			case 'O':
			case 'o':	goto kk12;
			case 'S':
			case 's':	goto kk8;
			case 'T':
			case 't':	goto kk10;
			case '[':	goto kk39;
			case ']':	goto kk41;
			case '{':	goto kk13;
			case '|':	goto kk51;
			case '}':	goto kk17;
			case '~':	goto kk29;
			default:	goto kk59;
			}
kk2:
			kkaccept = 0;
			kkch = *(KKMARKER = ++KKCURSOR);
			goto kk140;
kk3:
// 81 "scanner.re"
			{
			token->opcode = PHVOLT_T_INTEGER;
			token->value = estrndup(start, KKCURSOR - start);
			token->len = KKCURSOR - start;
			q = KKCURSOR;
			return 0;
		}
// 191 "scanner.c"
kk4:
			++KKCURSOR;
			switch ((kkch = *KKCURSOR)) {
			case 'F':
			case 'f':	goto kk134;
			case 'N':
			case 'n':	goto kk136;
			default:	goto kk79;
			}
kk5:
// 190 "scanner.re"
			{
			token->opcode = PHVOLT_T_IDENTIFIER;
			token->value = estrndup(start, KKCURSOR - start);
			token->len = KKCURSOR - start;
			q = KKCURSOR;
			return 0;
		}
// 210 "scanner.c"
kk6:
			kkch = *++KKCURSOR;
			switch (kkch) {
			case 'L':
			case 'l':	goto kk121;
			case 'N':
			case 'n':	goto kk122;
			default:	goto kk79;
			}
kk7:
			kkch = *++KKCURSOR;
			switch (kkch) {
			case 'A':
			case 'a':	goto kk113;
			case 'O':
			case 'o':	goto kk114;
			default:	goto kk79;
			}
kk8:
			kkch = *++KKCURSOR;
			switch (kkch) {
			case 'E':
			case 'e':	goto kk110;
			default:	goto kk79;
			}
kk9:
			kkch = *++KKCURSOR;
			switch (kkch) {
			case 'U':
			case 'u':	goto kk106;
			default:	goto kk79;
			}
kk10:
			kkch = *++KKCURSOR;
			switch (kkch) {
			case 'R':
			case 'r':	goto kk102;
			default:	goto kk79;
			}
kk11:
			kkch = *++KKCURSOR;
			switch (kkch) {
			case 'N':
			case 'n':	goto kk99;
			default:	goto kk79;
			}
kk12:
			kkch = *++KKCURSOR;
			switch (kkch) {
			case 'R':
			case 'r':	goto kk97;
			default:	goto kk79;
			}
kk13:
			++KKCURSOR;
			switch ((kkch = *KKCURSOR)) {
			case '%':	goto kk95;
			case '{':	goto kk93;
			default:	goto kk14;
			}
kk14:
// 338 "scanner.re"
			{
			status = PHVOLT_SCANNER_RETCODE_ERR;
			break;
		}
// 277 "scanner.c"
kk15:
			++KKCURSOR;
			switch ((kkch = *KKCURSOR)) {
			case '}':	goto kk91;
			default:	goto kk16;
			}
kk16:
// 218 "scanner.re"
			{
			token->opcode = PHVOLT_T_MOD;
			return 0;
		}
// 290 "scanner.c"
kk17:
			kkch = *++KKCURSOR;
			switch (kkch) {
			case '}':	goto kk89;
			default:	goto kk14;
			}
kk18:
			kkaccept = 1;
			kkch = *(KKMARKER = ++KKCURSOR);
			if (kkch <= 0x00) goto kk14;
			goto kk87;
kk19:
			kkaccept = 1;
			kkch = *(KKMARKER = ++KKCURSOR);
			if (kkch <= 0x00) goto kk14;
			goto kk81;
kk20:
			kkch = *++KKCURSOR;
			goto kk79;
kk21:
			++KKCURSOR;
// 198 "scanner.re"
			{
			token->opcode = PHVOLT_T_ADD;
			return 0;
		}
// 317 "scanner.c"
kk23:
			++KKCURSOR;
// 203 "scanner.re"
			{
			token->opcode = PHVOLT_T_SUB;
			return 0;
		}
// 325 "scanner.c"
kk25:
			++KKCURSOR;
// 208 "scanner.re"
			{
			token->opcode = PHVOLT_T_MUL;
			return 0;
		}
// 333 "scanner.c"
kk27:
			++KKCURSOR;
// 213 "scanner.re"
			{
			token->opcode = PHVOLT_T_DIV;
			return 0;
		}
// 341 "scanner.c"
kk29:
			++KKCURSOR;
// 223 "scanner.re"
			{
			token->opcode = PHVOLT_T_CONCAT;
			return 0;
		}
// 349 "scanner.c"
kk31:
			++KKCURSOR;
			switch ((kkch = *KKCURSOR)) {
			case '.':	goto kk76;
			default:	goto kk32;
			}
kk32:
// 233 "scanner.re"
			{
			token->opcode = PHVOLT_T_DOT;
			return 0;
		}
// 362 "scanner.c"
kk33:
			++KKCURSOR;
// 238 "scanner.re"
			{
			token->opcode = PHVOLT_T_COMMA;
			return 0;
		}
// 370 "scanner.c"
kk35:
			++KKCURSOR;
// 243 "scanner.re"
			{
			token->opcode = PHVOLT_T_BRACKET_OPEN;
			return 0;
		}
// 378 "scanner.c"
kk37:
			++KKCURSOR;
// 248 "scanner.re"
			{
			token->opcode = PHVOLT_T_BRACKET_CLOSE;
			return 0;
		}
// 386 "scanner.c"
kk39:
			++KKCURSOR;
// 253 "scanner.re"
			{
			token->opcode = PHVOLT_T_SBRACKET_OPEN;
			return 0;
		}
// 394 "scanner.c"
kk41:
			++KKCURSOR;
// 258 "scanner.re"
			{
			token->opcode = PHVOLT_T_SBRACKET_CLOSE;
			return 0;
		}
// 402 "scanner.c"
kk43:
			++KKCURSOR;
			switch ((kkch = *KKCURSOR)) {
			case '=':	goto kk74;
			case '>':	goto kk72;
			default:	goto kk44;
			}
kk44:
// 308 "scanner.re"
			{
			token->opcode = PHVOLT_T_LESS;
			return 0;
		}
// 416 "scanner.c"
kk45:
			++KKCURSOR;
			switch ((kkch = *KKCURSOR)) {
			case '=':	goto kk68;
			default:	goto kk46;
			}
kk46:
// 268 "scanner.re"
			{
			token->opcode = PHVOLT_T_ASSIGN;
			return 0;
		}
// 429 "scanner.c"
kk47:
			++KKCURSOR;
			switch ((kkch = *KKCURSOR)) {
			case '=':	goto kk66;
			default:	goto kk48;
			}
kk48:
// 313 "scanner.re"
			{
			token->opcode = PHVOLT_T_GREATER;
			return 0;
		}
// 442 "scanner.c"
kk49:
			++KKCURSOR;
			switch ((kkch = *KKCURSOR)) {
			case '=':	goto kk62;
			default:	goto kk50;
			}
kk50:
// 303 "scanner.re"
			{
			token->opcode = PHVOLT_T_NOT;
			return 0;
		}
// 455 "scanner.c"
kk51:
			++KKCURSOR;
// 318 "scanner.re"
			{
			token->opcode = PHVOLT_T_PIPE;
			return 0;
		}
// 463 "scanner.c"
kk53:
			++KKCURSOR;
// 323 "scanner.re"
			{
			token->opcode = PHVOLT_T_DOUBLECOLON;
			return 0;
		}
// 471 "scanner.c"
kk55:
			++KKCURSOR;
			kkch = *KKCURSOR;
			goto kk61;
kk56:
// 328 "scanner.re"
			{
			token->opcode = PHVOLT_T_IGNORE;
			return 0;
		}
// 482 "scanner.c"
kk57:
			++KKCURSOR;
// 333 "scanner.re"
			{
			status = PHVOLT_SCANNER_RETCODE_EOF;
			break;
		}
// 490 "scanner.c"
kk59:
			kkch = *++KKCURSOR;
			goto kk14;
kk60:
			++KKCURSOR;
			kkch = *KKCURSOR;
kk61:
			switch (kkch) {
			case '\t':
			case '\n':
			case '\r':
			case ' ':	goto kk60;
			default:	goto kk56;
			}
kk62:
			++KKCURSOR;
			switch ((kkch = *KKCURSOR)) {
			case '=':	goto kk64;
			default:	goto kk63;
			}
kk63:
// 283 "scanner.re"
			{
			token->opcode = PHVOLT_T_NOTEQUALS;
			return 0;
		}
// 517 "scanner.c"
kk64:
			++KKCURSOR;
// 298 "scanner.re"
			{
			token->opcode = PHVOLT_T_NOTIDENTICAL;
			return 0;
		}
// 525 "scanner.c"
kk66:
			++KKCURSOR;
// 273 "scanner.re"
			{
			token->opcode = PHVOLT_T_GREATEREQUAL;
			return 0;
		}
// 533 "scanner.c"
kk68:
			++KKCURSOR;
			switch ((kkch = *KKCURSOR)) {
			case '=':	goto kk70;
			default:	goto kk69;
			}
kk69:
// 278 "scanner.re"
			{
			token->opcode = PHVOLT_T_EQUALS;
			return 0;
		}
// 546 "scanner.c"
kk70:
			++KKCURSOR;
// 293 "scanner.re"
			{
			token->opcode = PHVOLT_T_IDENTICAL;
			return 0;
		}
// 554 "scanner.c"
kk72:
			++KKCURSOR;
// 288 "scanner.re"
			{
			token->opcode = PHVOLT_T_NOTEQUALS;
			return 0;
		}
// 562 "scanner.c"
kk74:
			++KKCURSOR;
// 263 "scanner.re"
			{
			token->opcode = PHVOLT_T_LESSEQUAL;
			return 0;
		}
// 570 "scanner.c"
kk76:
			++KKCURSOR;
// 228 "scanner.re"
			{
			token->opcode = PHVOLT_T_RANGE;
			return 0;
		}
// 578 "scanner.c"
kk78:
			++KKCURSOR;
			kkch = *KKCURSOR;
kk79:
			switch (kkch) {
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			case 'A':
			case 'B':
			case 'C':
			case 'D':
			case 'E':
			case 'F':
			case 'G':
			case 'H':
			case 'I':
			case 'J':
			case 'K':
			case 'L':
			case 'M':
			case 'N':
			case 'O':
			case 'P':
			case 'Q':
			case 'R':
			case 'S':
			case 'T':
			case 'U':
			case 'V':
			case 'W':
			case 'X':
			case 'Y':
			case 'Z':
			case '\\':
			case '_':
			case 'a':
			case 'b':
			case 'c':
			case 'd':
			case 'e':
			case 'f':
			case 'g':
			case 'h':
			case 'i':
			case 'j':
			case 'k':
			case 'l':
			case 'm':
			case 'n':
			case 'o':
			case 'p':
			case 'q':
			case 'r':
			case 's':
			case 't':
			case 'u':
			case 'v':
			case 'w':
			case 'x':
			case 'y':
			case 'z':	goto kk78;
			default:	goto kk5;
			}
kk80:
			++KKCURSOR;
			kkch = *KKCURSOR;
kk81:
			switch (kkch) {
			case 0x00:	goto kk82;
			case '\'':	goto kk84;
			case '\\':	goto kk83;
			default:	goto kk80;
			}
kk82:
			KKCURSOR = KKMARKER;
			switch (kkaccept) {
			case 0: 	goto kk3;
			case 1: 	goto kk14;
			}
kk83:
			++KKCURSOR;
			kkch = *KKCURSOR;
			switch (kkch) {
			case '\n':	goto kk82;
			default:	goto kk80;
			}
kk84:
			++KKCURSOR;
// 181 "scanner.re"
			{
			token->opcode = PHVOLT_T_STRING;
			token->value = estrndup(q, KKCURSOR - q - 1);
			token->len = KKCURSOR - q - 1;
			q = KKCURSOR;
			return 0;
		}
// 683 "scanner.c"
kk86:
			++KKCURSOR;
			kkch = *KKCURSOR;
kk87:
			switch (kkch) {
			case 0x00:	goto kk82;
			case '"':	goto kk84;
			case '\\':	goto kk88;
			default:	goto kk86;
			}
kk88:
			++KKCURSOR;
			kkch = *KKCURSOR;
			switch (kkch) {
			case '\n':	goto kk82;
			default:	goto kk86;
			}
kk89:
			++KKCURSOR;
// 174 "scanner.re"
			{
			s->mode = PHVOLT_MODE_RAW;
			token->opcode = PHVOLT_T_CLOSE_EDELIMITER;
			return 0;
		}
// 709 "scanner.c"
kk91:
			++KKCURSOR;
// 163 "scanner.re"
			{
			s->mode = PHVOLT_MODE_RAW;
			token->opcode = PHVOLT_T_CLOSE_DELIMITER;
			return 0;
		}
// 718 "scanner.c"
kk93:
			++KKCURSOR;
// 169 "scanner.re"
			{
			token->opcode = PHVOLT_T_OPEN_EDELIMITER;
			return 0;
		}
// 726 "scanner.c"
kk95:
			++KKCURSOR;
// 158 "scanner.re"
			{
			token->opcode = PHVOLT_T_OPEN_DELIMITER;
			return 0;
		}
// 734 "scanner.c"
kk97:
			++KKCURSOR;
			switch ((kkch = *KKCURSOR)) {
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			case 'A':
			case 'B':
			case 'C':
			case 'D':
			case 'E':
			case 'F':
			case 'G':
			case 'H':
			case 'I':
			case 'J':
			case 'K':
			case 'L':
			case 'M':
			case 'N':
			case 'O':
			case 'P':
			case 'Q':
			case 'R':
			case 'S':
			case 'T':
			case 'U':
			case 'V':
			case 'W':
			case 'X':
			case 'Y':
			case 'Z':
			case '\\':
			case '_':
			case 'a':
			case 'b':
			case 'c':
			case 'd':
			case 'e':
			case 'f':
			case 'g':
			case 'h':
			case 'i':
			case 'j':
			case 'k':
			case 'l':
			case 'm':
			case 'n':
			case 'o':
			case 'p':
			case 'q':
			case 'r':
			case 's':
			case 't':
			case 'u':
			case 'v':
			case 'w':
			case 'x':
			case 'y':
			case 'z':	goto kk78;
			default:	goto kk98;
			}
kk98:
// 153 "scanner.re"
			{
			token->opcode = PHVOLT_T_OR;
			return 0;
		}
// 810 "scanner.c"
kk99:
			kkch = *++KKCURSOR;
			switch (kkch) {
			case 'D':
			case 'd':	goto kk100;
			default:	goto kk79;
			}
kk100:
			++KKCURSOR;
			switch ((kkch = *KKCURSOR)) {
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			case 'A':
			case 'B':
			case 'C':
			case 'D':
			case 'E':
			case 'F':
			case 'G':
			case 'H':
			case 'I':
			case 'J':
			case 'K':
			case 'L':
			case 'M':
			case 'N':
			case 'O':
			case 'P':
			case 'Q':
			case 'R':
			case 'S':
			case 'T':
			case 'U':
			case 'V':
			case 'W':
			case 'X':
			case 'Y':
			case 'Z':
			case '\\':
			case '_':
			case 'a':
			case 'b':
			case 'c':
			case 'd':
			case 'e':
			case 'f':
			case 'g':
			case 'h':
			case 'i':
			case 'j':
			case 'k':
			case 'l':
			case 'm':
			case 'n':
			case 'o':
			case 'p':
			case 'q':
			case 'r':
			case 's':
			case 't':
			case 'u':
			case 'v':
			case 'w':
			case 'x':
			case 'y':
			case 'z':	goto kk78;
			default:	goto kk101;
			}
kk101:
// 148 "scanner.re"
			{
			token->opcode = PHVOLT_T_AND;
			return 0;
		}
// 893 "scanner.c"
kk102:
			kkch = *++KKCURSOR;
			switch (kkch) {
			case 'U':
			case 'u':	goto kk103;
			default:	goto kk79;
			}
kk103:
			kkch = *++KKCURSOR;
			switch (kkch) {
			case 'E':
			case 'e':	goto kk104;
			default:	goto kk79;
			}
kk104:
			++KKCURSOR;
			switch ((kkch = *KKCURSOR)) {
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			case 'A':
			case 'B':
			case 'C':
			case 'D':
			case 'E':
			case 'F':
			case 'G':
			case 'H':
			case 'I':
			case 'J':
			case 'K':
			case 'L':
			case 'M':
			case 'N':
			case 'O':
			case 'P':
			case 'Q':
			case 'R':
			case 'S':
			case 'T':
			case 'U':
			case 'V':
			case 'W':
			case 'X':
			case 'Y':
			case 'Z':
			case '\\':
			case '_':
			case 'a':
			case 'b':
			case 'c':
			case 'd':
			case 'e':
			case 'f':
			case 'g':
			case 'h':
			case 'i':
			case 'j':
			case 'k':
			case 'l':
			case 'm':
			case 'n':
			case 'o':
			case 'p':
			case 'q':
			case 'r':
			case 's':
			case 't':
			case 'u':
			case 'v':
			case 'w':
			case 'x':
			case 'y':
			case 'z':	goto kk78;
			default:	goto kk105;
			}
kk105:
// 143 "scanner.re"
			{
			token->opcode = PHVOLT_T_TRUE;
			return 0;
		}
// 983 "scanner.c"
kk106:
			kkch = *++KKCURSOR;
			switch (kkch) {
			case 'L':
			case 'l':	goto kk107;
			default:	goto kk79;
			}
kk107:
			kkch = *++KKCURSOR;
			switch (kkch) {
			case 'L':
			case 'l':	goto kk108;
			default:	goto kk79;
			}
kk108:
			++KKCURSOR;
			switch ((kkch = *KKCURSOR)) {
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			case 'A':
			case 'B':
			case 'C':
			case 'D':
			case 'E':
			case 'F':
			case 'G':
			case 'H':
			case 'I':
			case 'J':
			case 'K':
			case 'L':
			case 'M':
			case 'N':
			case 'O':
			case 'P':
			case 'Q':
			case 'R':
			case 'S':
			case 'T':
			case 'U':
			case 'V':
			case 'W':
			case 'X':
			case 'Y':
			case 'Z':
			case '\\':
			case '_':
			case 'a':
			case 'b':
			case 'c':
			case 'd':
			case 'e':
			case 'f':
			case 'g':
			case 'h':
			case 'i':
			case 'j':
			case 'k':
			case 'l':
			case 'm':
			case 'n':
			case 'o':
			case 'p':
			case 'q':
			case 'r':
			case 's':
			case 't':
			case 'u':
			case 'v':
			case 'w':
			case 'x':
			case 'y':
			case 'z':	goto kk78;
			default:	goto kk109;
			}
kk109:
// 133 "scanner.re"
			{
			token->opcode = PHVOLT_T_NULL;
			return 0;
		}
// 1073 "scanner.c"
kk110:
			kkch = *++KKCURSOR;
			switch (kkch) {
			case 'T':
			case 't':	goto kk111;
			default:	goto kk79;
			}
kk111:
			++KKCURSOR;
			switch ((kkch = *KKCURSOR)) {
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			case 'A':
			case 'B':
			case 'C':
			case 'D':
			case 'E':
			case 'F':
			case 'G':
			case 'H':
			case 'I':
			case 'J':
			case 'K':
			case 'L':
			case 'M':
			case 'N':
			case 'O':
			case 'P':
			case 'Q':
			case 'R':
			case 'S':
			case 'T':
			case 'U':
			case 'V':
			case 'W':
			case 'X':
			case 'Y':
			case 'Z':
			case '\\':
			case '_':
			case 'a':
			case 'b':
			case 'c':
			case 'd':
			case 'e':
			case 'f':
			case 'g':
			case 'h':
			case 'i':
			case 'j':
			case 'k':
			case 'l':
			case 'm':
			case 'n':
			case 'o':
			case 'p':
			case 'q':
			case 'r':
			case 's':
			case 't':
			case 'u':
			case 'v':
			case 'w':
			case 'x':
			case 'y':
			case 'z':	goto kk78;
			default:	goto kk112;
			}
kk112:
// 128 "scanner.re"
			{
			token->opcode = PHVOLT_T_SET;
			return 0;
		}
// 1156 "scanner.c"
kk113:
			kkch = *++KKCURSOR;
			switch (kkch) {
			case 'L':
			case 'l':	goto kk117;
			default:	goto kk79;
			}
kk114:
			kkch = *++KKCURSOR;
			switch (kkch) {
			case 'R':
			case 'r':	goto kk115;
			default:	goto kk79;
			}
kk115:
			++KKCURSOR;
			switch ((kkch = *KKCURSOR)) {
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			case 'A':
			case 'B':
			case 'C':
			case 'D':
			case 'E':
			case 'F':
			case 'G':
			case 'H':
			case 'I':
			case 'J':
			case 'K':
			case 'L':
			case 'M':
			case 'N':
			case 'O':
			case 'P':
			case 'Q':
			case 'R':
			case 'S':
			case 'T':
			case 'U':
			case 'V':
			case 'W':
			case 'X':
			case 'Y':
			case 'Z':
			case '\\':
			case '_':
			case 'a':
			case 'b':
			case 'c':
			case 'd':
			case 'e':
			case 'f':
			case 'g':
			case 'h':
			case 'i':
			case 'j':
			case 'k':
			case 'l':
			case 'm':
			case 'n':
			case 'o':
			case 'p':
			case 'q':
			case 'r':
			case 's':
			case 't':
			case 'u':
			case 'v':
			case 'w':
			case 'x':
			case 'y':
			case 'z':	goto kk78;
			default:	goto kk116;
			}
kk116:
// 113 "scanner.re"
			{
			token->opcode = PHVOLT_T_FOR;
			return 0;
		}
// 1246 "scanner.c"
kk117:
			kkch = *++KKCURSOR;
			switch (kkch) {
			case 'S':
			case 's':	goto kk118;
			default:	goto kk79;
			}
kk118:
			kkch = *++KKCURSOR;
			switch (kkch) {
			case 'E':
			case 'e':	goto kk119;
			default:	goto kk79;
			}
kk119:
			++KKCURSOR;
			switch ((kkch = *KKCURSOR)) {
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			case 'A':
			case 'B':
			case 'C':
			case 'D':
			case 'E':
			case 'F':
			case 'G':
			case 'H':
			case 'I':
			case 'J':
			case 'K':
			case 'L':
			case 'M':
			case 'N':
			case 'O':
			case 'P':
			case 'Q':
			case 'R':
			case 'S':
			case 'T':
			case 'U':
			case 'V':
			case 'W':
			case 'X':
			case 'Y':
			case 'Z':
			case '\\':
			case '_':
			case 'a':
			case 'b':
			case 'c':
			case 'd':
			case 'e':
			case 'f':
			case 'g':
			case 'h':
			case 'i':
			case 'j':
			case 'k':
			case 'l':
			case 'm':
			case 'n':
			case 'o':
			case 'p':
			case 'q':
			case 'r':
			case 's':
			case 't':
			case 'u':
			case 'v':
			case 'w':
			case 'x':
			case 'y':
			case 'z':	goto kk78;
			default:	goto kk120;
			}
kk120:
// 138 "scanner.re"
			{
			token->opcode = PHVOLT_T_FALSE;
			return 0;
		}
// 1336 "scanner.c"
kk121:
			kkch = *++KKCURSOR;
			switch (kkch) {
			case 'S':
			case 's':	goto kk131;
			default:	goto kk79;
			}
kk122:
			kkch = *++KKCURSOR;
			switch (kkch) {
			case 'D':
			case 'd':	goto kk123;
			default:	goto kk79;
			}
kk123:
			kkch = *++KKCURSOR;
			switch (kkch) {
			case 'F':
			case 'f':	goto kk124;
			case 'I':
			case 'i':	goto kk125;
			default:	goto kk79;
			}
kk124:
			kkch = *++KKCURSOR;
			switch (kkch) {
			case 'O':
			case 'o':	goto kk128;
			default:	goto kk79;
			}
kk125:
			kkch = *++KKCURSOR;
			switch (kkch) {
			case 'F':
			case 'f':	goto kk126;
			default:	goto kk79;
			}
kk126:
			++KKCURSOR;
			switch ((kkch = *KKCURSOR)) {
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			case 'A':
			case 'B':
			case 'C':
			case 'D':
			case 'E':
			case 'F':
			case 'G':
			case 'H':
			case 'I':
			case 'J':
			case 'K':
			case 'L':
			case 'M':
			case 'N':
			case 'O':
			case 'P':
			case 'Q':
			case 'R':
			case 'S':
			case 'T':
			case 'U':
			case 'V':
			case 'W':
			case 'X':
			case 'Y':
			case 'Z':
			case '\\':
			case '_':
			case 'a':
			case 'b':
			case 'c':
			case 'd':
			case 'e':
			case 'f':
			case 'g':
			case 'h':
			case 'i':
			case 'j':
			case 'k':
			case 'l':
			case 'm':
			case 'n':
			case 'o':
			case 'p':
			case 'q':
			case 'r':
			case 's':
			case 't':
			case 'u':
			case 'v':
			case 'w':
			case 'x':
			case 'y':
			case 'z':	goto kk78;
			default:	goto kk127;
			}
kk127:
// 108 "scanner.re"
			{
			token->opcode = PHVOLT_T_ENDIF;
			return 0;
		}
// 1449 "scanner.c"
kk128:
			kkch = *++KKCURSOR;
			switch (kkch) {
			case 'R':
			case 'r':	goto kk129;
			default:	goto kk79;
			}
kk129:
			++KKCURSOR;
			switch ((kkch = *KKCURSOR)) {
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			case 'A':
			case 'B':
			case 'C':
			case 'D':
			case 'E':
			case 'F':
			case 'G':
			case 'H':
			case 'I':
			case 'J':
			case 'K':
			case 'L':
			case 'M':
			case 'N':
			case 'O':
			case 'P':
			case 'Q':
			case 'R':
			case 'S':
			case 'T':
			case 'U':
			case 'V':
			case 'W':
			case 'X':
			case 'Y':
			case 'Z':
			case '\\':
			case '_':
			case 'a':
			case 'b':
			case 'c':
			case 'd':
			case 'e':
			case 'f':
			case 'g':
			case 'h':
			case 'i':
			case 'j':
			case 'k':
			case 'l':
			case 'm':
			case 'n':
			case 'o':
			case 'p':
			case 'q':
			case 'r':
			case 's':
			case 't':
			case 'u':
			case 'v':
			case 'w':
			case 'x':
			case 'y':
			case 'z':	goto kk78;
			default:	goto kk130;
			}
kk130:
// 118 "scanner.re"
			{
			token->opcode = PHVOLT_T_ENDFOR;
			return 0;
		}
// 1532 "scanner.c"
kk131:
			kkch = *++KKCURSOR;
			switch (kkch) {
			case 'E':
			case 'e':	goto kk132;
			default:	goto kk79;
			}
kk132:
			++KKCURSOR;
			switch ((kkch = *KKCURSOR)) {
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			case 'A':
			case 'B':
			case 'C':
			case 'D':
			case 'E':
			case 'F':
			case 'G':
			case 'H':
			case 'I':
			case 'J':
			case 'K':
			case 'L':
			case 'M':
			case 'N':
			case 'O':
			case 'P':
			case 'Q':
			case 'R':
			case 'S':
			case 'T':
			case 'U':
			case 'V':
			case 'W':
			case 'X':
			case 'Y':
			case 'Z':
			case '\\':
			case '_':
			case 'a':
			case 'b':
			case 'c':
			case 'd':
			case 'e':
			case 'f':
			case 'g':
			case 'h':
			case 'i':
			case 'j':
			case 'k':
			case 'l':
			case 'm':
			case 'n':
			case 'o':
			case 'p':
			case 'q':
			case 'r':
			case 's':
			case 't':
			case 'u':
			case 'v':
			case 'w':
			case 'x':
			case 'y':
			case 'z':	goto kk78;
			default:	goto kk133;
			}
kk133:
// 103 "scanner.re"
			{
			token->opcode = PHVOLT_T_ELSE;
			return 0;
		}
// 1615 "scanner.c"
kk134:
			++KKCURSOR;
			switch ((kkch = *KKCURSOR)) {
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			case 'A':
			case 'B':
			case 'C':
			case 'D':
			case 'E':
			case 'F':
			case 'G':
			case 'H':
			case 'I':
			case 'J':
			case 'K':
			case 'L':
			case 'M':
			case 'N':
			case 'O':
			case 'P':
			case 'Q':
			case 'R':
			case 'S':
			case 'T':
			case 'U':
			case 'V':
			case 'W':
			case 'X':
			case 'Y':
			case 'Z':
			case '\\':
			case '_':
			case 'a':
			case 'b':
			case 'c':
			case 'd':
			case 'e':
			case 'f':
			case 'g':
			case 'h':
			case 'i':
			case 'j':
			case 'k':
			case 'l':
			case 'm':
			case 'n':
			case 'o':
			case 'p':
			case 'q':
			case 'r':
			case 's':
			case 't':
			case 'u':
			case 'v':
			case 'w':
			case 'x':
			case 'y':
			case 'z':	goto kk78;
			default:	goto kk135;
			}
kk135:
// 98 "scanner.re"
			{
			token->opcode = PHVOLT_T_IF;
			return 0;
		}
// 1691 "scanner.c"
kk136:
			++KKCURSOR;
			switch ((kkch = *KKCURSOR)) {
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			case 'A':
			case 'B':
			case 'C':
			case 'D':
			case 'E':
			case 'F':
			case 'G':
			case 'H':
			case 'I':
			case 'J':
			case 'K':
			case 'L':
			case 'M':
			case 'N':
			case 'O':
			case 'P':
			case 'Q':
			case 'R':
			case 'S':
			case 'T':
			case 'U':
			case 'V':
			case 'W':
			case 'X':
			case 'Y':
			case 'Z':
			case '\\':
			case '_':
			case 'a':
			case 'b':
			case 'c':
			case 'd':
			case 'e':
			case 'f':
			case 'g':
			case 'h':
			case 'i':
			case 'j':
			case 'k':
			case 'l':
			case 'm':
			case 'n':
			case 'o':
			case 'p':
			case 'q':
			case 'r':
			case 's':
			case 't':
			case 'u':
			case 'v':
			case 'w':
			case 'x':
			case 'y':
			case 'z':	goto kk78;
			default:	goto kk137;
			}
kk137:
// 123 "scanner.re"
			{
			token->opcode = PHVOLT_T_IN;
			return 0;
		}
// 1767 "scanner.c"
kk138:
			kkch = *++KKCURSOR;
			switch (kkch) {
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':	goto kk141;
			default:	goto kk82;
			}
kk139:
			kkaccept = 0;
			KKMARKER = ++KKCURSOR;
			kkch = *KKCURSOR;
kk140:
			switch (kkch) {
			case '.':	goto kk138;
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':	goto kk139;
			default:	goto kk3;
			}
kk141:
			++KKCURSOR;
			kkch = *KKCURSOR;
			switch (kkch) {
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':	goto kk141;
			default:	goto kk143;
			}
kk143:
// 90 "scanner.re"
			{
			token->opcode = PHVOLT_T_DOUBLE;
			token->value = estrndup(start, KKCURSOR - start);
			token->len = KKCURSOR - start;
			q = KKCURSOR;
			return 0;
		}
// 1827 "scanner.c"
		}
// 343 "scanner.re"


		}
	}

	return status;
}