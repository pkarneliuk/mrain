@echo off
setlocal enabledelayedexpansion


set SRC=
set OBJ=
rem set DEBUG=1
for %%i in ( *.cpp .\win\*.cpp ) do (

  set SRC=%%~fi !SRC!
  set T=%%~nxi
  set OBJ=out\!T:cpp=obj! !OBJ!
)

rem Set up environment variables for cl

CALL vcvars32.bat

NMAKE.EXE %1 /NOLOGO /C /F NMakefile
