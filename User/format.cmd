pushd "%~dp0"
clang-format -i -style=file array_init.c array_init.h Config.h main.c main.h uart_protocol.c uart_protocol.h MCUConfig.h
popd
