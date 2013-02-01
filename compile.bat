@ECHO OFF
ECHO Copyright 1998-2006, Peter J. Jensen
ECHO All Rights Reserved.
ECHO 
ECHO This is UNPUBLISHED PROPRIETARY SOURCE CODE of Pete Jensen
ECHO the contents of this file may not be disclosed to third parties, copied or
ECHO duplicated in any form, in whole or in part, without the prior written
ECHO permission of Hairylogic, Inc.
ECHO 
ECHO RESTRICTED RIGHTS LEGEND:
ECHO Use, duplication or disclosure by the Government is subject to restrictions
ECHO as set forth in subdivision (c)(1)(ii) of the Rights in Technical Data
ECHO and Computer Software clause at DFARS 252.227-7013, and/or in similar or
ECHO successor clauses in the FAR, DOD or NASA FAR Supplement. Unpublished -
ECHO rights reserved under the Copyright Laws of the United States.
ECHO        ______
ECHO       /_____/\
ECHO      /_____\\ \      FILE:    compile.bat
ECHO     /_____\ \\ /     AUTHOR:  Pete Jensen
ECHO    /_____/ \/ / /    DATE:    Oct 19th, 2006
ECHO   /_____/ /   \//\   VERSION: 1.0.0
ECHO   \_____\//\   / /   
ECHO    \_____/ / /\ /    
ECHO     \_____/ \\ \     HTTP://euler.acadiau.ca/~064881j
ECHO      \_____\ \\ 
ECHO       \_____\/
ECHO Compiling ...
goto START

:NO_COMPILER
echo [ERROR]: Borland C Compiler (BCC) not in path.
exit

:MISSING_FILES
echo [ERROR]: Missing files!
exit

:COMPILE_ERROR
echo [ERROR]: Unable to compile Game of Life!
exit

:START
echo [COMPILE:] Checking for bcc.exe
if exist bcc.exe goto NEXT_1 else goto NO_COMPILER
:NEXT_1
echo [COMPILE:] Checking for GOL.c
if exist GOL.c goto NEXT_2 else goto MISSING_FILES
:NEXT_2
echo [COMPILE:] Checking for Graph.h
if exist graph.h goto NEXT_3 else goto MISSING_FILES
:NEXT_3
echo [COMPILE:] Checking for mouse.h
if exist mouse.h goto NEXT_4 else goto MISSING_FILES
:NEXT_4

echo [COMPILE:] _________________________________________
echo [COMPILE:] Finding if previos compile...
if exist GameOfLife.exe goto NEXT_7 goto DEL_EXES
:NEXT_7
echo [COMPILE:] Previous compilation exists. Deleting executables
del GameOfLife.exe
del GameOfLife_Mouse.exe
echo [COMPILE:] Operation DONE
:DEL_EXES
echo _
echo _
echo [COMPILE:] _________________________________________
echo [COMPILE:] Compiling mouse enabled/controled version.
bcc -D__MOUSE_ENABLED gol.c
if exist gol.exe goto NEXT_5 else goto COMPILE_ERROR
:NEXT_5
rename gol.exe GameOfLife_Mouse.exe
echo _
echo _
echo [COMPILE:] _________________________________________
echo [COMPILE:] Compiling automatic version.
bcc gol.c
if exist gol.exe goto NEXT_6 else goto COMPILE_ERROR
:NEXT_6
rename gol.exe GameOfLife.exe

echo [COMPILE:] Operation DONE

