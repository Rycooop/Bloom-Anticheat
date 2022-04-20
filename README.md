# Bloom Anticheat


![Animation](https://user-images.githubusercontent.com/45408021/164273282-2c4d67a9-8c49-46ee-8002-055aed0387da.gif)

This Anticheat can be used on any windows x64 process. It is currently configured to protect notepad, but changing the macro to the desired process is all you will need to do to change it. The main executable has a basic error handler and should be good enough for debugging.

Once loaded, the driver will use ObRegisterCallbacks() to protect both the anticheat and target process, and begin registering its callbacks.

For those looking for a simple usermode solution, injecting just the DLL will work as it is not reliant on the other modules. Make sure you LoadLibrary inject however, as manual mapping it will result in it detecting itself.

IMPORTANT: the driver binary must be put inside your System32\drivers folder(an error will be outputted if not).

To build:
in visual studio, select Batch build and build x64 release for each of the three projects
