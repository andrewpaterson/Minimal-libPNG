call clean.bat
cls
@echo %time%

wdc816cc.exe  -bs -DUSING_816 -ml -wr -wu -so -sp -lt -pb -pp -px -o adler32.obj adler32.c || exit /b
wdc816cc.exe  -bs -DUSING_816 -ml -wr -wu -so -sp -lt -pb -pp -px -o compress.obj compress.c || exit /b
wdc816cc.exe  -bs -DUSING_816 -ml -wr -wu -so -sp -lt -pb -pp -px -o deflate.obj deflate.c || exit /b
wdc816cc.exe  -bs -DUSING_816 -ml -wr -wu -so -sp -lt -pb -pp -px -o trees.obj trees.c || exit /b
wdc816cc.exe  -bs -DUSING_816 -ml -wr -wu -so -sp -lt -pb -pp -px -o crc32.obj crc32.c || exit /b
wdc816cc.exe  -bs -DUSING_816 -ml -wr -wu -so -sp -lt -pb -pp -px -o zutil.obj zutil.c || exit /b
wdc816cc.exe  -bs -DUSING_816 -ml -wr -wu -so -sp -lt -pb -pp -px -o uncompr.obj uncompr.c || exit /b
wdc816cc.exe  -bs -DUSING_816 -ml -wr -wu -so -sp -lt -pb -pp -px -o inflate.obj inflate.c || exit /b
wdc816cc.exe  -bs -DUSING_816 -ml -wr -wu -so -sp -lt -pb -pp -px -o inffast.obj inffast.c || exit /b
wdc816cc.exe  -bs -DUSING_816 -ml -wr -wu -so -sp -lt -pb -pp -px -o inftrees.obj inftrees.c || exit /b
wdcln.exe -C20000,00000 -g -t -sz -HB -obin\LibPNG.bin adler32.obj compress.obj deflate.obj trees.obj crc32.obj zutil.obj uncompr.obj inflate.obj inffast.obj inftrees.obj -lcl -lml || exit /b
