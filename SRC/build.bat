%CD%\..\cc65\bin\cl65.exe -O -o MAIN.PRG -t cx16 wait.c joy.c controller.c objects.c levelloader/levels.c gameobjs/player.c main.c
%CD%\..\x16emu\x16emu.exe -prg MAIN.PRG -run
