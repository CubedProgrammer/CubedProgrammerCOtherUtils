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
extern cpcou_text_formatter_type cpcou_fmt_black_background;
extern cpcou_text_formatter_type cpcou_fmt_red_background;
extern cpcou_text_formatter_type cpcou_fmt_green_background;
extern cpcou_text_formatter_type cpcou_fmt_yellow_background;
extern cpcou_text_formatter_type cpcou_fmt_blue_background;
extern cpcou_text_formatter_type cpcou_fmt_magenta_background;
extern cpcou_text_formatter_type cpcou_fmt_cyan_background;
extern cpcou_text_formatter_type cpcou_fmt_white_background;
extern cpcou_text_formatter_type cpcou_fmt_black_foreground;
extern cpcou_text_formatter_type cpcou_fmt_red_foreground;
extern cpcou_text_formatter_type cpcou_fmt_green_foreground;
extern cpcou_text_formatter_type cpcou_fmt_yellow_foreground;
extern cpcou_text_formatter_type cpcou_fmt_blue_foreground;
extern cpcou_text_formatter_type cpcou_fmt_magenta_foreground;
extern cpcou_text_formatter_type cpcou_fmt_cyan_foreground;
extern cpcou_text_formatter_type cpcou_fmt_white_foreground;
extern cpcou_text_formatter_type cpcou_terminal_up;
extern cpcou_text_formatter_type cpcou_terminal_up_return;

/**
 * Moves the caret a certain direction
 * For the returns, moves the caret vertically as well as moving it to the far left
 */
cpcou_text_formatter_type cpcou_terminal_left_n(size_t n);
cpcou_text_formatter_type cpcou_terminal_right_n(size_t n);
cpcou_text_formatter_type cpcou_terminal_up_n(size_t n);
cpcou_text_formatter_type cpcou_terminal_down_n(size_t n);
cpcou_text_formatter_type cpcou_terminal_up_return_n(size_t n);
cpcou_text_formatter_type cpcou_terminal_down_return_n(size_t n);

/**
 * Get formatter for any colour.
 * A call to one of these functions will modify the return values of previous calls
 * Therefore, it is wise to use the return value of these functions immediately
 * If a colour is used often, its call to one of these functions should be defined as a macro
 */
cpcou_text_formatter_type cpcou_fmt_background_cmy(int c, int m, int y);
cpcou_text_formatter_type cpcou_fmt_background_rgb(int r, int g, int b);
cpcou_text_formatter_type cpcou_fmt_foreground_cmy(int c, int m, int y);
cpcou_text_formatter_type cpcou_fmt_foreground_rgb(int r, int g, int b);

/**
 * Sets the text formatting of a file
 */
void cpcou_fformat(FILE *fh, cpcou_text_formatter_type fmt);

#endif
#endif
