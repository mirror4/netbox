// lex.h: lexer defines & declarations

#ifndef _LEX_H_
#define _LEX_H_

#include <stdio.h>

// Function prototype
int fht_lex ();
void fht_exit();

#define	ERROR_TOKEN	258
#define	SPACETEXT	259
#define	PLANTEXT	260
#define	TAG_ID		261
#define	TAG_STR		262
#define	TAG_START_ID	263
#define	TAG_CLOSE_ID	264
#define STYLE_BEGIN		265
#define STYLE_END		266
#define STYLE_ID		267
#define STYLE_VALUE		268

#endif