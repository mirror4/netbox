// fmtHTML.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "..\BEncoding.h"
#include "..\BMStream.h"
#include "..\BCodepage.h"
#include "fmtHTML.h"

// This function is called by the lexer when the end-of-file
// is reached; you can reset yyin (the input FILE*) and return 0
// if you want to process another file; otherwise just return 1.
extern "C" int fht_wrap(void) {
  return 1;
}

extern char *fht_text;

static struct _tagName{
	char* name;
	int	length;
	unsigned int   mode:2;
	unsigned int   before:1;
	unsigned int   inside:1;
	unsigned int   after:1;
	unsigned int   isPRE:1;
	char*		modes;
}s_tags[] = 
{
	{"a", 1, 2, 0, 0, 0, 0, "00000000302000020000020000000000000000220000000100000000210200000"},
	{"acronym", 7, 2, 0, 0, 0, 0, "00000000322000020000020000000000000000220000000100000000210200000"},
	{"address", 7, 2, 1, 1, 1, 0, "20000002322000220022220000000000000000222200320100000022212220202"},
	{"b", 1, 2, 0, 0, 0, 0, "00000000322000020000020000000000000000220000000100000000210200000"},
	{"bdo", 3, 2, 1, 0, 1, 0, "00000000322000020000020000000000000000220000000100000000210200000"},
	{"big", 3, 2, 0, 0, 0, 0, "00000000322000020000020000000000000000220000000100000000210200000"},
	{"blockquote", 10, 2, 1, 0, 1, 0, "20020000322000220022220000000000000000222200320100000022212220202"},
	{"br", 2, 0, 0, 1, 0, 0, "00000000000000000000000000000000000000000000000100000000010000000"},
	{"button", 6, 2, 1, 0, 1, 0, "30030003302300000033300003333330000000223300330100000022212220203"},
	{"caption", 7, 2, 1, 1, 1, 0, "11111111300111001111101011111110100111001100101111101100010000111"},
	{"center", 6, 2, 1, 1, 1, 0, "00000000322000020000020000000000000000220000300100000000210200000"},
	{"cite", 4, 2, 0, 0, 0, 0, "00000000322000020000020000000000000000220000000100000000210200000"},
	{"code", 4, 2, 0, 0, 0, 0, "00000000322000020000020000000000000000220000000100000000210200000"},
	{"colgroup", 8, 2, 1, 1, 1, 0, "11111111300111001111101011111110100111001100101111101100010000111"},
	{"dd", 2, 2, 1, 0, 1, 0, "20020002322000000022220000000000000000222200220100000022212220202"},
	{"del", 3, 2, 0, 0, 0, 0, "00000000322000020000020000000000000000220000000100000000210200000"},
	{"dfn", 3, 2, 0, 0, 0, 0, "00000000322000020000020000000000000000220000000100000000210200000"},
	{"dir", 3, 2, 1, 0, 1, 0, "20020002322000220002220000000000000000222200320100000022212220202"},
	{"div", 3, 2, 1, 0, 1, 0, "00020002322000020020220000000000000000222200320100000022212220202"},
	{"dl", 2, 2, 1, 1, 1, 0, "20020002322000220022020000000000000000222200320100000022212220202"},
	{"dt", 2, 2, 1, 0, 1, 0, "20020002322000020022200000000000000000222200220100000022212220202"},
	{"em", 2, 2, 0, 0, 0, 0, "00000000322000020000020000000000000000220000000100000000210200000"},
	{"embed", 5, 0, 1, 1, 0, 0, "00000000300000000000000000000000000000000000000100000000010000000"},
	{"font", 4, 2, 0, 0, 0, 0, "00000000322000020000020000000000000000220000000100000000210200000"},
	{"h1", 2, 2, 1, 0, 1, 0, "00000000322000020000020000222220000000220000300100000000210200000"},
	{"h2", 2, 2, 1, 0, 1, 0, "00000000322000020000020002022220000000220000300100000000210200000"},
	{"h3", 2, 2, 1, 0, 1, 0, "00000000322000020000020002202220000000220000300100000000210200000"},
	{"h4", 2, 2, 1, 0, 1, 0, "00000000322000020000020002220220000000220000300100000000210200000"},
	{"h5", 2, 2, 1, 0, 1, 0, "00000000322000020000020002222020000000220000300100000000210200000"},
	{"h6", 2, 2, 1, 0, 1, 0, "00000000322000020000020002222200000000220000300100000000210200000"},
	{"hr", 2, 0, 1, 1, 0, 0, "00000000300000000000000000000000000000000000000100000000010000000"},
	{"i", 1, 2, 0, 0, 0, 0, "00000000322000020000020000000000000000220000000100000000210200000"},
	{"img", 3, 0, 0, 0, 0, 0, "00000000300000000000000000000000000000000000000100000000010000000"},
	{"input", 5, 0, 1, 1, 0, 0, "00000000300000000000000000000000000000000000000000000000010000000"},
	{"ins", 3, 2, 0, 0, 0, 0, "00000000322000020000020000000000000000220000000100000000210200000"},
	{"kbd", 3, 2, 0, 0, 0, 0, "00000000322000020000020000000000000000220000000100000000210200000"},
	{"label", 5, 2, 1, 0, 1, 0, "00000000322000020000020000000000000000220000000100000000210200000"},
	{"li", 2, 0, 1, 0, 0, 0, "20020002322000020022220000000000000000022200220100000022212220202"},
	{"marquee", 7, 2, 1, 1, 1, 0, "30030003302300000033300003333330000000203300330100000022212220203"},
	{"menu", 4, 2, 1, 1, 1, 0, "20020002322000220022220000000000000000220200320100000022212220202"},
	{"ol", 2, 2, 1, 1, 1, 0, "20020002322000220022220000000000000000222000320100000022212220202"},
	{"optgroup", 8, 2, 1, 0, 1, 0, "11111111300111001111101011111110100111001100101211101100010000011"},
	{"option", 6, 2, 1, 0, 1, 0, "11111111300111001111101011111110100111001100101211101100010000011"},
	{"p", 1, 0, 1, 0, 0, 0, "20020002322000220022220000000000000000222200020100000022212220202"},
	{"pre", 3, 2, 1, 0, 1, 1, "20020002322000020022220000000000000000222200200100000022212220202"},
	{"q", 1, 2, 0, 0, 0, 0, "00000000322000020000020000000000000000220000000100000000210200000"},
	{"select", 6, 2, 1, 1, 1, 0, "22222222202222222222222222222222222222222222222122202222212220222"},
	{"small", 5, 2, 0, 0, 0, 0, "00000000322000020000020000000000000000220000000100000000210200000"},
	{"span", 4, 2, 0, 0, 0, 0, "00000000322000020000020000000000000000220000000100000000210200000"},
	{"strong", 6, 2, 0, 0, 0, 0, "00000000322000020000020000000000000000220000000100000000210200000"},
	{"style", 5, 1, 0, 0, 0, 0, "11111111000111001111101011111110100111001100101111101100010000011"},
	{"sub", 3, 2, 0, 0, 0, 0, "00000000322000020000020000000000000000220000000100000000210200000"},
	{"sup", 3, 2, 0, 0, 0, 0, "00000000322000020000020000000000000000220000000100000000210200000"},
	{"table", 5, 2, 1, 1, 1, 0, "30030003300000200033300000000000000000223322330100000001012020103"},
	{"tbody", 5, 2, 1, 1, 1, 0, "11111111300111001111101011111110100111001100101111101120010000111"},
	{"td", 2, 2, 1, 0, 1, 0, "11111111300111001111101011111110100111001100101111101100010000211"},
	{"textarea", 8, 2, 1, 0, 1, 0, "22222222202222222222222222222222222222222222222222202222212220222"},
	{"tfoot", 5, 2, 1, 1, 1, 0, "11111111300111001111101011111110100111001100101111101100010000111"},
	{"th", 2, 2, 1, 0, 1, 0, "11111111300111001111101011111110100111001100101111101100010000011"},
	{"thead", 5, 2, 1, 1, 1, 0, "11111111300111001111101011111110100111001100101111101100010000111"},
	{"title", 5, 1, 0, 0, 0, 0, "11111111111111111111111111111111111111111111111111101111111110111"},
	{"tr", 2, 2, 1, 0, 1, 0, "11111111300111001111101011111110100111001100101111101122012020011"},
	{"tt", 2, 2, 0, 0, 0, 0, "00000000322000020000020000000000000000220000000100000000210200000"},
	{"ul", 2, 2, 1, 1, 1, 0, "200200023220002200222200000000000000002222003201000000222122202000"}
};

static char* s_attrs[] =
{
	"align", "alt", "background", "bgcolor", "border", "bordercolor", "bordercolordark", "bordercolorlight",
	"cellpadding", "cellspacing", "clear", "color", "cols", "colspan", "dir", "direction", "disabled",
	"face", "frame", "galleryimg", "headers", "height", "href", "loop", "lowsrc", "rows", "rowspan",
	"rules", "scope", "scrollamount", "scrolldelay", "size", "span", "src", "start", "summary", "target",
	"text", "title", "truespeed", "type", "units", "unselectable", "urn", "valign", "value", "volume",
	"vspace", "width", "wrap"
};

static char* s_styles[] =
{
	"background", "background-attachment", "background-color", "background-image", "background-position",
	"background-position-x", "background-position-y", "background-repeat", 	"border", "border-bottom",
	"border-bottom-color", "border-bottom-style", "border-bottom-width", "border-collapse",	"border-color",
	"border-left", "border-left-color", "border-left-style", "border-left-width", "border-right",
	"border-right-color", "border-right-style", "border-right-width", "border-style", "border-top", "border-top-color", 
	"border-top-style", "border-top-width", "border-width", "clear", "color", "direction", "float", "font",
	"font-family", "font-size", "font-style", "font-variant", "font-weight", "height", "layout-flow", "layout-grid",
	"layout-grid-char", "layout-grid-line", "layout-grid-mode", "layout-grid-type", "letter-spacing", "line-break",
	"line-height", "margin", "margin-bottom", "margin-left", "margin-right", "margin-top", "min-height", "padding",
	"padding-bottom", "padding-left", "padding-right", "padding-top", "ruby-align",	"ruby-overhang", "ruby-position",
	"scrollbar-3dlight-color", "scrollbar-arrow-color", "scrollbar-base-color", "scrollbar-darkshadow-color",
	"scrollbar-face-color", "scrollbar-highlight-color", "scrollbar-shadow-color", "scrollbar-track-color",
	"table-layout", "text-align", "text-align-last", "text-autospace", "text-decoration", "text-indent", "text-justify",
	"text-kashida-space", "text-overflow", "text-transform", "text-underline-position", "unicode-bidi",
	"vertical-align", "white-space", "width", "word-break", "word-spacing", "word-wrap"
};

static int str_compare( char **arg1, char **arg2 )
{
   /* Compare all of both strings: */
   return _strcmpi( *arg1, *arg2 );
}

extern char* fht_text;
extern int fht_leng;
static CAtlArray<int> arrayTagStack;
static int bSpace, bSkipNewLine, bNewLine, bSkipSpace, nPRE;
static int nLinePos;
static CBStringA s_fht_Buffer;
static int s_fnt_Position;
static CBCriticalSection s_ms;
static IStream* s_pStream;

inline void putSpace()
{
	if(bNewLine)
	{
		if(!bSkipNewLine)
		{
			s_pStream->Write("\r\n", 2, NULL);
			nLinePos = 0;
		}
		bSpace = 0;
		bNewLine = 0;
	}else if(bSpace)
	{
		if(!bSkipSpace)
		{
			if(nLinePos > 77)
			{
				s_pStream->Write("\r\n", 2, NULL);
				nLinePos = 0;
			}else
			{
				s_pStream->Write(" ", 1, NULL);
				nLinePos ++;
			}
		}
		bSpace = 0;
	}
	bSkipSpace = 0;
	bSkipNewLine = 0;
}

int fht_input(char *buf, int ms)
{
	int n = min(ms, s_fht_Buffer.GetLength() - s_fnt_Position);

	if(n > 0)
	{
		memcpy(buf, (LPCSTR)s_fht_Buffer + s_fnt_Position, n);
		s_fnt_Position += n;
	}

	return n;
}

inline void CloseTag(int i)
{
	_tagName* pTag = &s_tags[arrayTagStack[i]];
	int j = (int)arrayTagStack.GetCount() - 1;
	int nTag = arrayTagStack[i];

	while(j >= 0 && arrayTagStack[j] != nTag)
	{
		_tagName* pTag1 = &s_tags[arrayTagStack[j]];
		if(pTag1->modes[nTag + 1] == '2' ||
			(pTag1->modes[nTag + 1] != '1' && pTag1->modes[0] == '1'))
			CloseTag(j);
		j --;
	}

	if(!nPRE && pTag->inside)bNewLine = 1;
	if(!nPRE && !pTag->inside && pTag->before)bSkipSpace = 1;
	putSpace();
	s_pStream->Write("</", 2, NULL);
	s_pStream->Write(pTag->name, pTag->length, NULL);
	s_pStream->Write(">", 1, NULL);
	nLinePos += pTag->length + 3;
	if(pTag->isPRE)nPRE --;
	if(!nPRE && pTag->after)bNewLine = 1;

	arrayTagStack.RemoveAt(i);
}

void fmtHTML()
{
	int n;
	const char* ptr;
	_tagName* pTag;

	bSpace = 0;
	bNewLine = 0;
	bSkipSpace = 1;
	bSkipNewLine = 1;
	nPRE = 0;
	nLinePos = 0;
	s_fnt_Position = 0;

	n = fht_lex();
	while(n)
	{
		switch(n)
		{
		case SPACETEXT: 
			if(nPRE)
			{
				s_pStream->Write(fht_text, fht_leng, NULL);
				nLinePos += fht_leng;
			}else bSpace = 1;

			n = fht_lex();
			break;

		case PLANTEXT: 
			putSpace();
			s_pStream->Write(fht_text, fht_leng, NULL);
			nLinePos += fht_leng;

			n = fht_lex();
			break;

		case TAG_START_ID:
			ptr = fht_text + 1;
			pTag = (_tagName*)bsearch( (char *) &ptr, (char *)s_tags, sizeof(s_tags) / sizeof(s_tags[0]),
								sizeof( s_tags[0] ), (int (*)(const void*, const void*))str_compare );

			if(!pTag)
				do{n = fht_lex();}while(n != 0 && n != '>');
			else
			{
				if(pTag->mode == 1)
				{
					do{n = fht_lex();}while(n != 0 && n != '>');
					do{n = fht_lex();}while(n == PLANTEXT || n == SPACETEXT);
					break;
				}

				int nTag = int(pTag - s_tags);
				char bDel = '1';

				if(arrayTagStack.GetCount() == 0)
					bDel = pTag->modes[0];
				else
					for(int i = (int)arrayTagStack.GetCount() - 1; i >= 0 && bDel == '1'; i --)
						bDel = pTag->modes[arrayTagStack[i] + 1];

				if(bDel == '1')
				{
					do{n = fht_lex();}while(n != 0 && n != '>');
					n = fht_lex();
					break;
				}

				if(pTag->modes[0] == '1')
				{
					int i = (int)arrayTagStack.GetCount() - 1;
					while(i >= 0 && pTag->modes[arrayTagStack[i] + 1] != '2')
					{
						if(arrayTagStack[i] == nTag || pTag->modes[arrayTagStack[i] + 1] == '1')
							CloseTag(i);
						i --;
					}
				}

				if(!nPRE && pTag->before)bNewLine = 1;
				putSpace();
				s_pStream->Write("<", 1, NULL);
				s_pStream->Write(pTag->name, pTag->length, NULL);
				nLinePos += pTag->length + 1;
				if(pTag->isPRE)nPRE ++;

				n = fht_lex();
				while(1)
				{
					if(n == 0 || n == '>')break;

					if(n == STYLE_BEGIN)
					{
						CString strStyle;

						n = fht_lex();
						while(1)
						{
							if(n == 0 || n == STYLE_END)break;

							if(n != STYLE_ID)
							{
								n = fht_lex();
								continue;
							}

							CString strTemp(fht_text);
							strTemp.MakeLower();

							n = fht_lex();
							if(n != STYLE_VALUE)continue;

							ptr = strTemp;
							if(!bsearch( (char *) &ptr, (char *)s_styles, sizeof(s_styles) / sizeof(char *),
										sizeof( char * ), (int (*)(const void*, const void*))str_compare ))
							{
								n = fht_lex();
								continue;
							}

							strStyle.Append(strTemp);
							strStyle.Append(fht_text);
							strStyle.AppendChar(';');
						}

						if(!strStyle.IsEmpty())
						{
							s_pStream->Write(" style=\"", 8, NULL);
							s_pStream->Write(strStyle, strStyle.GetLength(), NULL);
							s_pStream->Write("\"", 1, NULL);
							nLinePos += strStyle.GetLength() + 9;
						}

						n = fht_lex();
						continue;
					}

					if(n != TAG_ID)
					{
						n = fht_lex();
						continue;
					}

					CString strTemp(fht_text);
					strTemp.MakeLower();

					n = fht_lex();

					if(n != '=')continue;

					n = fht_lex();
					if(n == '=')
					{
						n = fht_lex();
						continue;
					}

					ptr = strTemp;
					if(!bsearch( (char *) &ptr, (char *)s_attrs, sizeof(s_attrs) / sizeof(char *),
								sizeof( char * ), (int (*)(const void*, const void*))str_compare ))
					{
						n = fht_lex();
						continue;
					}

					CString strTemp1(fht_text);

					if(n == TAG_STR)
						strTemp1 = strTemp1.Mid(1, strTemp1.GetLength() - 2);

					if(!strTemp.Compare("href") && !_strnicmp(strTemp1, "javascript:", 11))
					{
						n = fht_lex();
						continue;
					}

					if(strTemp1.Find('\"') != -1)
						strTemp1 = '\'' + strTemp1 + '\'';
					else strTemp1 = '\"' + strTemp1 + '\"';

					s_pStream->Write(" ", 1, NULL);
					s_pStream->Write(strTemp, strTemp.GetLength(), NULL);
					s_pStream->Write("=", 1, NULL);
					s_pStream->Write(strTemp1, strTemp1.GetLength(), NULL);
					nLinePos += strTemp.GetLength() + strTemp1.GetLength() + 2;

					n = fht_lex();
				}

				s_pStream->Write(">", 1, NULL);
				nLinePos ++;

				if(!nPRE && pTag->inside)bNewLine = 1;
				if(!nPRE && !pTag->inside && pTag->before)bSkipSpace = 1;

				if(pTag->mode > 1)arrayTagStack.Add(nTag);
			}

			n = fht_lex();

			break;

		case TAG_CLOSE_ID:
			ptr = fht_text + 2;
			pTag = (_tagName*)bsearch( (char *) &ptr, (char *)s_tags, sizeof(s_tags) / sizeof(s_tags[0]),
								sizeof( s_tags[0] ), (int (*)(const void*, const void*))str_compare );

			if(pTag && pTag->mode > 1)
			{
				int nTag = int(pTag - s_tags);
				int i;
				char bDel = pTag->modes[0];

				i = (int)arrayTagStack.GetCount() - 1;
				if(bDel == '1')
					while(i >= 0 && arrayTagStack[i] != nTag && pTag->modes[arrayTagStack[i] + 1] == '1')i --;
				else
					while(i >= 0 && arrayTagStack[i] != nTag && s_tags[arrayTagStack[i]].modes[nTag + 1] != '3')i --;

				if(i < 0 || arrayTagStack[i] != nTag)
				{
					do{n = fht_lex();}while(n != 0 && n != '>');
					n = fht_lex();
					break;
				}

				CloseTag(i);
			}

			do{n = fht_lex();}while(n != 0 && n != '>');

			n = fht_lex();
			break;
	   };
   }

	int i = (int)arrayTagStack.GetCount() - 1;
	while(i >= 0)
	{
		CloseTag(i);
		i --;
	}
}

STDMETHODIMP CBEncoding::HtmlFormat(BSTR TextString, BSTR *retVal)
{
	CBCodePage cp(CP_UTF8);
	CBStringA strTemp(TextString, ::SysStringLen(TextString));
	CBTempStream mStream;

	s_ms.Enter();

	s_pStream = &mStream;
	s_fht_Buffer = strTemp;

	fmtHTML();

	s_pStream = NULL;
	s_fht_Buffer.Empty();

	s_ms.Leave();

	int nSize = (int)mStream.GetLength();

	mStream.SeekToBegin();
	mStream.Read(strTemp.GetBuffer(nSize), nSize);
	strTemp.ReleaseBuffer(nSize);

	*retVal = strTemp.AllocSysString();

	return S_OK;
}

