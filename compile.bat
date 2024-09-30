@echo off
set CC65=cc65
set CA65=ca65
set LD65=ld65
set NAME=game
set CFG=nrom_32k_vert.cfg

if "%1"=="clean" goto clean

rem Build steps
%CC65% -Oirs %NAME%.c --add-source
%CA65% %NAME%.s -g
%CA65% crt0.s
%LD65% -C %CFG% -o %NAME%.nes crt0.o %NAME%.o nes.lib -Ln labels.txt --dbgfile dbg.txt

del *.o
move %NAME%.nes rom\%NAME%.nes
move %NAME%.s rom\%NAME%.s
move labels.txt rom\labels.txt
move dbg.txt rom\dbg.txt

echo %NAME%.nes created
goto end

:clean
del %NAME%.nes

:end