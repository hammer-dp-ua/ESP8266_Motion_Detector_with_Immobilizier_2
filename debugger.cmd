set GDB_PATH=C:\Users\USER\ESP8266\xtensa-lx106-elf-win32-1.22.0-100-ge567ec7-5.2.0\bin
set CURRENT_DIR=%~dp0

%GDB_PATH%\xtensa-lx106-elf-gdb.exe %CURRENT_DIR%cmake-build-debug\Motion_Detector_with_Immobilizier.elf

:: Then:
:: > info symbol 0x<address>

:: Or better show the source line number and lines:
:: > list *0x<address>