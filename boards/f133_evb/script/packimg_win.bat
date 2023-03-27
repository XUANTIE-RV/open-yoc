@echo off
setlocal enableDelayedExpansion

set CURDIR=%~dp0
set BOARD_DIR=..\..\boards\d1_evb
set PACK_DIR=%BOARD_DIR%\pack
set OUT_DIR=%CURDIR%out
set RTOS_IMG=%CURDIR%yoc.bin
set FS_DATA_DIR=%CURDIR%data
set MK_GENERATED_IMGS_PATH=%CURDIR%generated
set CONFIG_YAML_FILE=%CURDIR%\configs\config.yaml
set FACTORY_ZIP=%CURDIR%%1

@REM echo %CURDIR%
@REM echo %BOARD_DIR%
@REM echo %PACK_DIR%
@REM echo %FACTORY_ZIP%

%PACK_DIR%\pack.exe -d 0 -r %RTOS_IMG% -f %FS_DATA_DIR% -o %OUT_DIR% -m %MK_GENERATED_IMGS_PATH% -c %CONFIG_YAML_FILE% -z %FACTORY_ZIP%