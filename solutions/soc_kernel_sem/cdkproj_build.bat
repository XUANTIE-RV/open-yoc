@echo off
setlocal enableDelayedExpansion

set filename=%0
set CURDIR=%~dp0
set BOARD_COMP_DIR=%CURDIR%..\..\boards\board_riscv_dummy
set CHIP_COMP_DIR=%CURDIR%..\..\components\chip_riscv_dummy
set SDK_COMP_DIR=%CURDIR%..\..\components\sdk_chip_riscv_dummy
set RULE_CONFIG_FILE=%CURDIR%..\..\components\xuantie_cpu_sdk\xt_rtos_sdk.csv

set remain=%CURDIR%
:loop
for /f "tokens=1* delims=\" %%a in ("%remain%") do (
    set solution_name=%%a
	set remain=%%b
)
if defined remain goto :loop

@REM echo %CURDIR%
@REM echo %BOARD_COMP_DIR%
@REM echo %CHIP_COMP_DIR%
@REM echo %SDK_COMP_DIR%
@REM echo %RULE_CONFIG_FILE%
@REM echo %solution_name%

if not exist %RULE_CONFIG_FILE% (
    echo "rule file: %RULE_CONFIG_FILE% is not exist, xuantie_cpu_sdk component may be not installed yet!"
    pause
    exit /b 0
)

set cpu_name=%1
set board=%2
set rtos=%3

set count=0
for %%i in (%*) do (
  set /a count+=1
)
@REM echo %count%
if not %count% equ 3 (
    call :help
    pause
    exit /b 0
)

call :check_cpu %cpu_name%
IF %ERRORLEVEL% NEQ 0 (
    exit /b 1
)
call :check_board %board%
IF %ERRORLEVEL% NEQ 0 (
    exit /b 1
)
call :check_rtos %rtos%
IF %ERRORLEVEL% NEQ 0 (
    exit /b 1
)

copy %SDK_COMP_DIR%\package.yaml.%rtos% %SDK_COMP_DIR%\package.yaml > nul
copy %CHIP_COMP_DIR%\package.yaml.%cpu_name% %CHIP_COMP_DIR%\package.yaml > nul
copy %BOARD_COMP_DIR%\package.yaml.%board% %BOARD_COMP_DIR%\package.yaml > nul

if exist Obj ( rd /s /q Obj )
if exist Lst ( rd /s /q Lst )

echo "build %cpu_name% cdkproj success."

exit /b 0


:help
	echo "%filename% cpu_name board rtos"
	echo "eg:"
	echo "%filename% c906fd xiaohui freertos"
	echo "%filename% e906fdp smartl rtthread"
goto:eof

:check_cpu
    set cpu=%1
    echo "the cpu is "%cpu%
    for /f %%a in (%RULE_CONFIG_FILE%) do (
        @REM echo %%a
        echo %%a | findstr %solution_name% >nul && (
            @REM echo -----------------find-----------------
            set linestr=%%a
            @REM echo !linestr!
            for /f "tokens=2* delims=," %%i in ("!linestr!") do (
                set _cpu_list=%%i
                @REM echo !_cpu_list!
                :loop_cpu
                for /f "tokens=1* delims=/" %%a in ("!_cpu_list!") do (
                    @REM echo %%a
                    if /i %%a equ %cpu% (
                        echo found cpu: %cpu%
                        goto:eof
                    )
                    set _cpu_list=%%b
                )
                if defined _cpu_list goto :loop_cpu
            )
        )
    )
    echo "this solution is not support for the cpu, may be rule file is not update yet!"
    pause
    exit /b 1
goto:eof

:check_board
	set board=%1
	set board_list='smartl' 'xiaohui'
	echo "the board is "%board%
	for %%a in (%board_list%) do (
        @REM echo %%a
        if /i %%a equ '%board%' (
            echo found board: %board%
            goto:eof
        )
    )

	echo "the board is not support for the cpu. optional boards are:"
	echo %board_list%
    pause
	exit /b 1
goto:eof

:check_rtos
	set rtos=%1
	set rtos_list='rhino' 'freertos' 'rtthread' 'ucos3'
	echo "the rtos is "%rtos%
	for %%a in (%rtos_list%) do (
        @REM echo %%a
        if /i %%a equ '%rtos%' (
            goto:eof
        )
    )

	echo "the rtos is not support. optional rtos are:"
	echo %rtos_list%
    pause
	exit /b 1
goto:eof
