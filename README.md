# Gassy Bird

Oh shit!

## Overview

You are a bird who enjoys doing the doo doo. Fly above people’s heads and swerve between natural obstacles and thrown objects as you gleefully squeeze out juicy gifts for those below. Why are there thrown objects? Because for some reason, people who receive your generous white presents get mad and throw rocks at you.

Poop with precision; missing two fecal discharges in a row causes you to lose. Poop with expedience; holding your churning mass of pressurized turds inside for too long causes you to lose. And don't forget -- try to avoid flying into things.

## Controls

### Gameplay

- `Space`: Shoot forth a gleaming poopy mess
- `P`: Pause the game (can also click on the pause button)
- Any other key: Fly upward

### Menu Naviagtion

You can use the mouse to click on buttons. But if you're a pro keyboard gamer, then you know any time where your hands aren't on the keyboard is time wasted that could be spent pooping on people. So the following controls also work to navigate the button interface:

- (`W`, `A`, `S`, `D`) and (`Up`, `Left`, `Down`, `Right`): Select button
- `Space` and `Enter`: Click selected button

## Installation

### Windows

Start up a command promp in [administrator mode](https://www.isunshare.com/windows-10/2-ways-to-run-command-prompt-as-administrator-in-win-10.html), then do the following:

1. Install SFML

    ```sh
    git clone https://github.com/SFML/SFML.git
    cd SFML
    mkdir build
    cd build
    cmake -G "MinGW Makefiles" ..
    make install
    ```

2. Add the SFML `bin` directory to your `PATH` environment vairable ([instructions](https://www.architectryan.com/2018/03/17/add-to-the-path-on-windows-10/)). For example, if you installed SFML to `C:\Program Files (x86)\SFML`, then add `C:\Program Files (x86)\SFML\bin` to `PATH`.

    You may have to restart your command prompt for this change to take effect. Remember to restart it in administrator mode!

3. Install Box2D

    ```sh
    git clone https://github.com/erincatto/box2d.git
    cd box2d
    mkdir build
    cd build
    cmake -G "MinGW Makefiles" ..
    make install
    ```

4. Install and run Gassy Bird

    ```sh
    git clone https://github.com/noooooobage/GassyBird.git
    cd GassyBird
    mkdir Release
    cd Release
    cmake -G "MinGW Makefiles" ..
    make
    gassy_bird
    ```

### Linux

1. Install SFML

    ```sh
    git clone https://github.com/SFML/SFML.git
    cd SFML
    mkdir build
    cd build
    cmake ..
    sudo make install
    ```

2. Add the SFML `bin` directory to your `PATH` environment vairable ([instructions](https://linuxize.com/post/how-to-add-directory-to-path-in-linux/)). For example, if you installed SFML to `/usr/SFML`, then add `/usr/SFML/bin` to `PATH`.

3. Install Box2D

    ```sh
    git clone https://github.com/erincatto/box2d.git
    cd box2d
    mkdir build
    cd build
    cmake ..
    sudo make install
    ```

4. Install and run Gassy Bird

    ```sh
    git clone https://github.com/noooooobage/GassyBird.git
    cd GassyBird
    mkdir Release
    cd Release
    cmake ..
    make
    ./gassy_bird
    ```

### Troubleshooting

If you get CMAKE errors like a package wasn't able to be found, then you may have to set one or more of the following environment variables:

- `BOX2DINCLUDE`: The location where the Box2D header files are located. For example, if you installed Box2D to `C:\Program Files (x86)\box2d`, then set `BOX2DINCLUDE` to `C:\Program Files (x86)\box2d\include`.
- `BOX2DLIB`: The location where the box2d libraries are located. For example, if you installed Box2D to `C:\Program Files (x86)\box2d`, then set `BOX2DLIB` to `C:\Program Files (x86)\box2d\lib`.
- `INCLUDE`: The location where the SFML header files are located. For example, if you installed SFML to `C:\Program Files (x86)\SFML`, then set `INCLUDE` to `C:\Program Files (x86)\SFML\include`.
- `LIB`: The location where the SFML libraries are installed. For example, if you installed SFML to `C:\Program Files (x86)\SFML`, then set `LIB` to `C:\Program Files (x86)\SFML\lib`.
