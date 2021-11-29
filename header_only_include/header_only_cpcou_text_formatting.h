#ifndef __cplusplus
#ifndef Included_header_only_cpcou_text_formatting_h
#define Included_header_only_cpcou_text_formatting_h
#include<cpcou_text_formatting.h>

cpcou_text_formatter_type cpcou_fmt_reset = "\033\1330m";
cpcou_text_formatter_type cpcou_fmt_bold = "\033\1331m";
cpcou_text_formatter_type cpcou_fmt_faint = "\033\1332m";
cpcou_text_formatter_type cpcou_fmt_italic = "\033\1333m";
cpcou_text_formatter_type cpcou_fmt_underlined = "\033\1334m";
cpcou_text_formatter_type cpcou_fmt_crossed = "\033\1339m";
cpcou_text_formatter_type cpcou_fmt_overlined = "\033\13353m";
cpcou_text_formatter_type cpcou_fmt_def_background = "\033\13349m";
cpcou_text_formatter_type cpcou_fmt_def_foreground = "\033\13339m";

/**
 * Sets the text formatting of a file
 */
void cpcou_fformat(FILE *fh, cpcou_text_formatter_type fmt)
{
	fputs(fmt, fh);
}

#endif
#endif
