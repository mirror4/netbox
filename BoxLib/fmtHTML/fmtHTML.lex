%{

#include "fmtHTML.h"

#define isatty(p) 0

void yyerror(char *msg);

void fht_exit()
{
	yy_delete_buffer(yy_current_buffer);
}

#undef YY_INPUT
#define YY_INPUT(b, r, ms) (r = fht_input(b, ms))

extern int fht_input(char *buf, int ms);
%}

%x TAG_MODE STYLE_MODE SCRIPT_MODE COMMENT_MODE

%%

[ \t\r\n]+  {  return SPACETEXT; }
[^ \t\r\n<]* { return PLANTEXT; }

"<script"([ \r\n\t][^>]*)?">" BEGIN SCRIPT_MODE;
<SCRIPT_MODE>"</script"([ \r\n\t][^>]*)?">" BEGIN INITIAL;
<SCRIPT_MODE>. ;
<SCRIPT_MODE>\n ;

"<!--" BEGIN COMMENT_MODE;
<COMMENT_MODE>"-->" BEGIN INITIAL;
<COMMENT_MODE>. ;
<COMMENT_MODE>\n ;

"<"[a-z!][^ \t\r\n\"\'\\\/>=]* { BEGIN TAG_MODE; return TAG_START_ID; }
"<"\/[a-z!][^ \t\r\n\"\'\\\/>=]* { BEGIN TAG_MODE; return TAG_CLOSE_ID; }
<TAG_MODE>= return yytext[0];
<TAG_MODE>[ \t\r\n]+ ;
<TAG_MODE>style[ \t\r\n>=]*=[ \t\r\n>=]*[\"\'] BEGIN STYLE_MODE; return STYLE_BEGIN;
<TAG_MODE>[^ \t\r\n\"\'>=]+ return TAG_ID;
<TAG_MODE>\"[^\r\n\"]*\" return TAG_STR;
<TAG_MODE>\'[^\r\n\']*\' return TAG_STR;
<TAG_MODE>">" BEGIN INITIAL; return yytext[0];
<TAG_MODE>. ;

<STYLE_MODE>[\r\n\"\'] BEGIN TAG_MODE; return STYLE_END;
<STYLE_MODE>[ \t]+ ;
<STYLE_MODE>[^ \t\r\n\"\'\:;]+ return STYLE_ID;
<STYLE_MODE>\:[^\r\n\"\'\:;]+ return STYLE_VALUE;
<STYLE_MODE>. ;

"<" { return PLANTEXT; }

%%
