# full-anticheat
WIP of a full kernel anticheat. Use on any game you want to protect

AntiCheat is the EXE, anticheat_x64 is the dll, and achelper is the driver
The DLL can be injected into the game process alone and it will do most of the work, the EXE will load the DLL into the process you want to protect as well as communicate with the driver.

To build:
in visual studio, select Batch build and build x64 release for each of the three projects

test signing must be on to use the driver, which is automatically loaded from the exe since the driver is not signed
