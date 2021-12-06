@echo off

set CommonCompilerFlags=-Od -FC -GR- -Oi -MP -WL -Z7 -nologo -EHsc -Fobin/ -DOS_WINDOWS 
set CommonLinkerFlags=-incremental:no -opt:ref "User32.lib" "Gdi32.lib" "kernel32.lib"

cl %CommonCompilerFlags% day01/day01.cpp /Feday01/day01 /Fdday/day01 /link %CommonLinkerFlags%
