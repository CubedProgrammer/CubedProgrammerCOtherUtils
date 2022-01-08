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
cpcou_text_formatter_type cpcou_fmt_black_background = "\033\13340m";
cpcou_text_formatter_type cpcou_fmt_red_background = "\033\13341m";
cpcou_text_formatter_type cpcou_fmt_green_background = "\033\13342m";
cpcou_text_formatter_type cpcou_fmt_yellow_background = "\033\13343m";
cpcou_text_formatter_type cpcou_fmt_blue_background = "\033\13344m";
cpcou_text_formatter_type cpcou_fmt_magenta_background = "\033\13345m";
cpcou_text_formatter_type cpcou_fmt_cyan_background = "\033\13346m";
cpcou_text_formatter_type cpcou_fmt_white_background = "\033\13347m";
cpcou_text_formatter_type cpcou_fmt_black_foreground = "\033\13330m";
cpcou_text_formatter_type cpcou_fmt_red_foreground = "\033\13331m";
cpcou_text_formatter_type cpcou_fmt_green_foreground = "\033\13332m";
cpcou_text_formatter_type cpcou_fmt_yellow_foreground = "\033\13333m";
cpcou_text_formatter_type cpcou_fmt_blue_foreground = "\033\13334m";
cpcou_text_formatter_type cpcou_fmt_magenta_foreground = "\033\13335m";
cpcou_text_formatter_type cpcou_fmt_cyan_foreground = "\033\13336m";
cpcou_text_formatter_type cpcou_fmt_white_foreground = "\033\13337m";
cpcou_text_formatter_type cpcou_terminal_up = "\033\133A";
cpcou_text_formatter_type cpcou_terminal_up_return = "\033\133F";

char cpcou____fmt_any_colour_str[20];
char cpcou____terminal_movement_str[20];

/**
 * Moves the caret a certain direction
 * For the returns, moves the caret vertically as well as moving it to the far left
 */
cpcou_text_formatter_type cpcou_terminal_left_n(size_t n)
{
	sprintf(cpcou____terminal_movement_str, "\033\133%zuD", n);
	return cpcou____terminal_movement_str;
}

cpcou_text_formatter_type cpcou_terminal_right_n(size_t n)
{
	sprintf(cpcou____terminal_movement_str, "\033\133%zuC", n);
	return cpcou____terminal_movement_str;
}

cpcou_text_formatter_type cpcou_terminal_up_n(size_t n)
{
	sprintf(cpcou____terminal_movement_str, "\033\133%zuA", n);
	return cpcou____terminal_movement_str;
}

cpcou_text_formatter_type cpcou_terminal_down_n(size_t n)
{
	sprintf(cpcou____terminal_movement_str, "\033\133%zuB", n);
	return cpcou____terminal_movement_str;
}

cpcou_text_formatter_type cpcou_terminal_up_return_n(size_t n)
{
	sprintf(cpcou____terminal_movement_str, "\033\133%zuF", n);
	return cpcou____terminal_movement_str;
}

cpcou_text_formatter_type cpcou_terminal_down_return_n(size_t n)
{
	sprintf(cpcou____terminal_movement_str, "\033\133%zuE", n);
	return cpcou____terminal_movement_str;
}

/**
 * Get formatter for any colour.
 * A call to one of these functions will modify the return values of previous calls
 * Therefore, it is wise to use the return value of these functions immediately
 * If a colour is used often, its call to one of these functions should be defined as a macro
 */
cpcou_text_formatter_type cpcou_fmt_background_cmy(int c, int m, int y)
{
	return cpcou_fmt_background_rgb(255 - c, 255 - m, 255 - y);
}

cpcou_text_formatter_type cpcou_fmt_background_rgb(int r, int g, int b)
{
	sprintf(cpcou____fmt_any_colour_str, "\033\13348;2;%i;%i;%im", r, g, b);
	return cpcou____fmt_any_colour_str;
}

cpcou_text_formatter_type cpcou_fmt_foreground_cmy(int c, int m, int y)
{
	return cpcou_fmt_foreground_rgb(255 - c, 255 - m, 255 - y);
}

cpcou_text_formatter_type cpcou_fmt_foreground_rgb(int r, int g, int b)
{
	sprintf(cpcou____fmt_any_colour_str, "\033\13338;2;%i;%i;%im", r, g, b);
	return cpcou____fmt_any_colour_str;
}

/**
 * Sets the text formatting of a file
 */
void cpcou_fformat(FILE *fh, cpcou_text_formatter_type fmt)
{
	fputs(fmt, fh);
}

#endif
#endif
