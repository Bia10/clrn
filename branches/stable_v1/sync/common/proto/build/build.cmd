:: Define variables
@set protocompiler=protoc.exe
@set indir=..\messages
@set outdir=..\inc
@set filesmask=*.proto

:: Use this variable for changing additional directories
@set aid=..\ext_tools\protobuf\include


::@if "%stdout_redirected%" == "yes" goto make_projectinfo


::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::::
::   RESOURCE COMPILING
::::
::::::::::::::::::::::::::::::::::::::::::::::::::::::::

:: Make output directory
@md %outdir%

:: Here we go
@echo Compiling proto files ...


::Makefull path for output directory
@set outdirfull=%~dp0%outdir%
@echo outdirfull=%outdirfull%


:: Change directory to input
@set basedir=%~dp0
@cd %indir%

:: Find all protobuf files
@for %%F in (%filesmask%) do (
        
    @echo "%basedir%%protocompiler%" "--cpp_out=%outdirfull%" --error_format=msvs %%~nxF
    @call "%basedir%%protocompiler%" "--cpp_out=%outdirfull%" --error_format=msvs %%~nxF
    @if errorlevel 1 goto compile_error
    )
    
:: Restore previous directory
@cd %basedir%
    
@echo OK: Proto files have been compiled


::::Make project info
::@if "%stdout_redirected%" == "" (
::    @set stdout_redirected=yes
::    @call cmd.exe /c %0 %* > %projectinfofile%
::    @exit /b %error_level%
::)

@goto succeeded

:succeeded
:: Go outta here
@exit /b 0





:: Whata fa..
:compile_error
@echo ERROR: Failed to compile proto file(s)
pause
@exit /b 1