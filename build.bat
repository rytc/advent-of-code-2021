@echo off

set CommonCompilerFlags=-Od -FC -GR- -Oi -MP -WL -Z7 -nologo -EHsc -Fobin/ -DOS_WINDOWS 
set CommonLinkerFlags=-incremental:no -opt:ref "User32.lib" "Gdi32.lib" "kernel32.lib"

cl %CommonCompilerFlags% day01/day01.cpp /Feday01/day01 /Fdday01/day01 /link %CommonLinkerFlags%
cl %CommonCompilerFlags% day02/day02.cpp /Feday02/day02 /Fdday02/day02 /link %CommonLinkerFlags%
cl %CommonCompilerFlags% day03/day03.cpp /Feday03/day03 /Fdday03/day03 /link %CommonLinkerFlags%
cl %CommonCompilerFlags% day04/day04.cpp /Feday04/day04 /Fdday04/day04 /link %CommonLinkerFlags%
cl %CommonCompilerFlags% day05/day05.cpp /Feday05/day05 /Fdday05/day05 /link %CommonLinkerFlags%




