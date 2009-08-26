@echo off
setlocal enabledelayedexpansion

rem Global configuration
rem set DEBUG=1
rem set PACK_UPX=1
rem set PACK_FSG=1

set SRC=
set OBJ=
for %%i in ( *.cpp .\win\*.cpp ) do (

  set SRC=%%~fi !SRC!
  set T=%%~nxi
  set OBJ=out\!T:cpp=obj! !OBJ!
)

rem Set up environment variables for cl

CALL vcvars32.bat

NMAKE.EXE %1 /NOLOGO /C /F NMakefile
