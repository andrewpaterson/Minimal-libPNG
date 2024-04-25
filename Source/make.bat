call clean.bat
cls
@echo %time%

wdc816cc.exe  -bs -DUSING_816 -ml -wr -wu -so -sp -lt -pb -pp -px -o adler32.obj adler32.c || exit /b
wdc816cc.exe  -bs -DUSING_816 -ml -wr -wu -so -sp -lt -pb -pp -px -o compress.obj compress.c || exit /b
wdc816cc.exe  -bs -DUSING_816 -ml -wr -wu -so -sp -lt -pb -pp -px -o deflate.obj deflate.c || exit /b
wdcln.exe -C20000,00000 -g -t -sz -HB -obin\LibPNG.bin adler32.obj compress.obj deflate.obj -lcl -lml || exit /b
