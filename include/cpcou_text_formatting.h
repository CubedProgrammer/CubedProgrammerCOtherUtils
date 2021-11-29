#ifndef __cplusplus
#ifndef Included_cpcou_text_formatting_h
#define Included_cpcou_text_formatting_h
#include<stdio.h>
#define cpcou_format(fmt)cpcou_fformat(stdout, fmt)

typedef const char *cpcou_text_formatter_type;

extern cpcou_text_formatter_type cpcou_fmt_reset;
extern cpcou_text_formatter_type cpcou_fmt_bold;
extern cpcou_text_formatter_type cpcou_fmt_faint;
extern cpcou_text_formatter_type cpcou_fmt_italic;
extern cpcou_text_formatter_type cpcou_fmt_underlined;
extern cpcou_text_formatter_type cpcou_fmt_crossed;
extern cpcou_text_formatter_type cpcou_fmt_overlined;
extern cpcou_text_formatter_type cpcou_fmt_def_background;
extern cpcou_text_formatter_type cpcou_fmt_def_foreground;

/**
 * Sets the text formatting of a file
 */
void cpcou_fformat(FILE *fh, cpcou_text_formatter_type fmt);

#endif
#endif
