mkdir obj32
gcc -c -o obj32\swnd.o -Wall -funsigned-char -O2  -Isrc -Ires -Iinclude  src\swnd.c
gcc -c -o obj32\nargv.o -Wall -funsigned-char -O2  -Isrc -Ires -Iinclude  src\nargv.c
windres.exe res\res.rc obj32\res.o
gcc -c -o obj32\dirwalk.o -Wall -funsigned-char -O2  -Isrc -Ires -Iinclude  src\dirwalk.c
gcc -c -o obj32\recent.o -Wall -funsigned-char -O2  -Isrc -Ires -Iinclude  src\recent.c
gcc -c -o obj32\mstr.o -Wall -funsigned-char -O2  -Isrc -Ires -Iinclude  src\mstr.c
gcc -c -o obj32\array.o -Wall -funsigned-char -O2  -Isrc -Ires -Iinclude  src\array.c
gcc -c -o obj32\ini.o -Wall -funsigned-char -O2  -Isrc -Ires -Iinclude  src\ini.c
gcc -c -o obj32\config.o -Wall -funsigned-char -O2  -Isrc -Ires -Iinclude  src\config.c
gcc -c -o obj32\search.o -Wall -funsigned-char -O2  -Isrc -Ires -Iinclude  src\search.c
gcc -c -o obj32\mask.o -Wall -funsigned-char -O2  -Isrc -Ires -Iinclude  src\mask.c
gcc -c -o obj32\kmem_cache.o -Wall -funsigned-char -O2  -Isrc -Ires -Iinclude  src\kmem_cache.c
gcc -c -o obj32\save.o -Wall -funsigned-char -O2  -Isrc -Ires -Iinclude  src\save.c
gcc -c -o obj32\elapsed.o -Wall -funsigned-char -O2  -Isrc -Ires -Iinclude  src\elapsed.c
gcc -c -o obj32\shlink.o -Wall -funsigned-char -O2  -Isrc -Ires -Iinclude  src\shlink.c
gcc -mwindows -static -s -Xlinker --no-insert-timestamp -o obj32\sif2.exe  obj32\swnd.o obj32\nargv.o obj32\res.o obj32\dirwalk.o obj32\recent.o obj32\mstr.o obj32\array.o obj32\ini.o obj32\config.o obj32\search.o obj32\mask.o obj32\kmem_cache.o obj32\save.o obj32\elapsed.o obj32\shlink.o  -lcomctl32 -lole32 -luuid -lshlwapi
