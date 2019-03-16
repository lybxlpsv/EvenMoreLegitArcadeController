# This fork is just for testing/experimenting.
Also most of the new things here is spaghetti-code quality. ![:slowpoke:](https://cdn.discordapp.com/emojis/531734661865668628.png)


[Download binaries here](https://ci.appveyor.com/api/projects/lybxlpsv/evenmorelegitarcadecontroller/artifacts/release.zip?branch=experimental) [![Build status](https://ci.appveyor.com/api/projects/status/github/lybxlpsv/evenmorelegitarcadecontroller?svg=true)](https://ci.appveyor.com/project/lybxlpsv/evenmorelegitarcadecontroller)

# Even More Legit Arcade Controller (ELAC)
A collection of emulation components written in C++ for the purpose of operating Project DIVA Arcade on unintended hardware and generally improve the user experience.

This is still a work in progress program.

## Usage:
* Once per release patch your `diva.exe` using the `prepatch` program by either dropping it on the executable inside your file explorer 
or manually passing it in as the first command line argument to the program.

* Once the diva executable is appropriately patched start it like normal and *while* the diva process is running start `elac`.
This has to be done every time diva is restarted.

* Keyboard bindings can be changed inside the `keyconfig.ini` text file.
Alternative input methods are not yet natively supported but using 3rd party software is of course an option.
If a keybinding is pressed while a different one is already held down, the target key will be automatically repressed.
This *will* cancel hold combos so for now do not expect a behavior as sophisticated as in the PS4 release of Future Tone.
