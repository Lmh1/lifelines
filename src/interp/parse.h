/* 
   Copyright (c) 2001 Perry Rapp
   "The MIT license"
   Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
   The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
/*======================================================================
 * parse.h -- types & functions for parser & lexical analyzer
 * Copyright(c) 2002 by Perry Rapp; all rights reserved
 *====================================================================*/

#ifndef parse_h_included
#define parse_h_included

/* semantic value type used by bison parser -- needs to hold any pointer */
#define YYSTYPE void *

/* lexical analyzer function (provided by lifelines code in lex.c */
int yylex(YYSTYPE * lvalp, void * parm);

/* parser entry point (for bison parser generated from yacc.y */
int yyparse(PACTX pactx);

/* function to report parse errors */
void parse_error(PACTX pactx, STRING str);


#endif /* parse_h_included */

