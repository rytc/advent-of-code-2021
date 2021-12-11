@echo off

set CommonCompilerFlags=-Od -FC -GR- -Oi -MP -WL -Z7 -nologo -EHsc -Fobin/ -DOS_WINDOWS 
set CommonLinkerFlags=-incremental:no -opt:ref "User32.lib" "Gdi32.lib" "kernel32.lib"

IF "%1"=="day01" cl %CommonCompilerFlags% day01/day01.cpp /Feday01/day01 /Fdday01/day01 /link %CommonLinkerFlags%
IF "%1"=="day02" cl %CommonCompilerFlags% day02/day02.cpp /Feday02/day02 /Fdday02/day02 /link %CommonLinkerFlags%
IF "%1"=="day03" cl %CommonCompilerFlags% day03/day03.cpp /Feday03/day03 /Fdday03/day03 /link %CommonLinkerFlags%
IF "%1"=="day04" cl %CommonCompilerFlags% day04/day04.cpp /Feday04/day04 /Fdday04/day04 /link %CommonLinkerFlags%
IF "%1"=="day05" cl %CommonCompilerFlags% day05/day05.cpp /Feday05/day05 /Fdday05/day05 /link %CommonLinkerFlags%
IF "%1"=="day06" cl %CommonCompilerFlags% day06/day06.cpp /Feday06/day06 /Fdday06/day06 /link %CommonLinkerFlags%
IF "%1"=="day07" cl %CommonCompilerFlags% day07/day07.cpp /Feday07/day07 /Fdday07/day07 /link %CommonLinkerFlags%
IF "%1"=="day08" cl %CommonCompilerFlags% day08/day08.cpp /Feday08/day08 /Fdday08/day08 /link %CommonLinkerFlags%
IF "%1"=="day09" cl %CommonCompilerFlags% day09/day09.cpp /Feday09/day09 /Fdday08/day09 /link %CommonLinkerFlags%
IF "%1"=="day10" cl %CommonCompilerFlags% day10/day10.cpp /Feday10/day10 /Fdday10/day10 /link %CommonLinkerFlags%
IF "%1"=="day11" cl %CommonCompilerFlags% day11/day11.cpp /Feday11/day11 /Fdday11/day11 /link %CommonLinkerFlags%










