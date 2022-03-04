# full-anticheat
WIP of a full kernel anticheat. Use on any game you want to protect

AntiCheat is the EXE, anticheat_x64 is the dll, and achelper is the driver
The DLL can be injected into the game process alone and it will do most of the work, the EXE will load and communicate with the driver so if you would like to strip handles the exe will need to be running.

To build:
in visual studio, select Batch build and build x64 release for each of the three projects
