
set DIR=../Debug
set OUTNANE=STM32F1xx_System

arm-none-eabi-size --format=berkeley %DIR%/*.elf
arm-none-eabi-objcopy -O binary      %DIR%/*.elf %OUTNANE%.bin
arm-none-eabi-objcopy -O ihex      %DIR%/*.elf %OUTNANE%.hex
pause

