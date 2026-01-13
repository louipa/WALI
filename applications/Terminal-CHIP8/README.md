```
             _____ _    _ _____ _____        ___    ______                 _       _                 
    /\      / ____| |  | |_   _|  __ \      / _ \  |  ____|               | |     | |              _ 
   /  \    | |    | |__| | | | | |__) |____| (_) | | |__   _ __ ___  _   _| | __ _| |_ ___  _ __  (_)
  / /\ \   | |    |  __  | | | |  ___/______> _ <  |  __| | '_ ` _ \| | | | |/ _` | __/ _ \| '__|    
 / ____ \  | |____| |  | |_| |_| |         | (_) | | |____| | | | | | |_| | | (_| | || (_) | |     _ 
/_/    \_\  \_____|_|  |_|_____|_|          \___/  |______|_| |_| |_|\__,_|_|\__,_|\__\___/|_|    (_)
                                                                                                     
                                                                                                     
Using conio.h and printf making it (probably) very Windows backwards compatible!
```
to build and run in pwr shell with tcc use
> .\tcc.exe <path-to_repo>/main.c -run
>
> then copy path to the .ch8 file (no spaces, no more than 1024 character path)

also build with the DB flag set to 1 to get instruction by instruction debug info

----
TODO:
  - [ ] a more GDB like debug interface
  - [ ] optimise a little
  - [ ] be able to select dark and light pixel color palette
  - [ ] be able to select game speed
  - [ ] better rom selection
  - [ ] sound? (have not figured out easiest way for this yet)
  - [ ] learn window.h or sdl2
----
font generated with [http://www.network-science.de/ASCII/](http://www.network-science.de/ascii/)

chip-8 resources:

  - [https://github.com/mattmikolay/chip-8/wiki/CHIP%E2%80%908-Instruction-Set](https://github.com/mattmikolay/chip-8/wiki/CHIP%E2%80%908-Instruction-Set)
  - [https://chip-8.github.io/links/](https://chip-8.github.io/links/)
  - [https://github.com/Timendus/chip8-test-suite](https://github.com/Timendus/chip8-test-suite)
  - [http://www.multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/](http://www.multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/)
    
