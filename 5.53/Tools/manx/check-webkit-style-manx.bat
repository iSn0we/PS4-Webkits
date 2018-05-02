@echo off
REM Copyright (C) 2016 Sony Interactive Entertainment Inc.

setlocal
set webkit_dir=%~dp0..\..
set PYTHON=python
set script=%webkit_dir%\Tools\Scripts\check-webkit-style
set args=--filter=-readability/enum_casing
set error=0

if "%1" == "" goto :CHECK_MANX_DIRS
if "%1" == "--help" goto :SHOW_HELP
if "%1" == "-h" goto :SHOW_HELP
goto :CHECK_GIT_COMMIT

:CHECK_GIT_COMMIT
  "%PYTHON%" "%script%" %args% --git-commit "%1"
  exit /b %ERRORLEVEL%

:CHECK_MANX_DIRS
  cd /d "%webkit_dir%\Source"
  for /r /d %%A in (*manx*) do (
    "%PYTHON%" "%script%" %args% %%A
    if ERRORLEVEL 1 if %error% equ 0 set error=1
  )
  exit /b %error%

:SHOW_HELP
  @echo usage:
  @echo   %~nx0 $GIT_REVISION
  @echo     Check a git commit.
  @echo   %~nx0
  @echo     Check sources in the manx directories.
  @echo   %~nx0 --help
  @echo     Show this message.
  exit /b 0
