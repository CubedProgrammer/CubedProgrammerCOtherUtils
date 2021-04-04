# CubedProgrammerCOtherUtils
Other utilities for C

So far working on filesystems, plans for process builidng, maybe regexes and http in the far future.

gcc -I. -O3 -c src/cpcou_file_system.c -fPIC

gcc -I. -O3 -c src/cpcou_str_algo.c -fPIC

gcc -shared -o libcpcou cpcou_file_system.o cpcou_str_algo.o
