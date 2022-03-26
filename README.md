# Bloom Anticheat
Once loaded, the driver will use ObRegisterCallbacks to register a callback and protect the anticheat process as well as the game process

The DLL can be injected into the game process alone and it will do most of the work, otherwise the EXE will load the DLL into the process you want to protect as well as load and communicate with the driver.

IMPORTANT: the driver binary must be put inside your System32\drivers folder. Otherwise it will not load.

To build:
in visual studio, select Batch build and build x64 release for each of the three projects
