# CubedProgrammerCOtherUtils
Other utilities for C

File system, string and process utilities, plans for regexes in the future.

gcc -Iinclude -Iheader_only_include -O3 -c src/cpcou_file_system.c -fPIC

gcc -Iinclude -Iheader_only_include -O3 -c src/cpcou_misc_utils.c -fPIC

gcc -Iinclude -Iheader_only_include -O3 -c src/cpcou_process_basic.c -fPIC

gcc -Iinclude -Iheader_only_include -O3 -c src/cpcou_process_streams.c -fPIC

gcc -Iinclude -Iheader_only_include -O3 -c src/cpcou_str_algo.c -fPIC

gcc -Iinclude -Iheader_only_include -O3 -c src/cpcou_text_formatting.c -fPIC

gcc -shared -o libcpcou cpcou_file_system.o cpcou_misc_utils.o cpcou_process_basic.o cpcou_process_streams.o cpcou_str_algo.o cpcou_text_formatting.o
