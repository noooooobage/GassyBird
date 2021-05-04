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

This project utilizes [SFML](https://www.sfml-dev.org/tutorials/2.5/compile-with-cmake.php) and [Box2D](https://github.com/erincatto/box2d). Follow those links to install the libraries using CMAKE.

Once SFML and Box2D are installed, run the following to compile and launch the game:

```sh
git clone https://github.com/noooooobage/GassyBird.git
cd GassyBird
mkdir Release
cmake ..
make
./gassy_bird
```

The above runs fine as long as SFML and Box2D are installed in their standard locations. If you get CMAKE errors like a package wasn't able to be found, then you may have to set some environment variables which tell CMAKE how to find your SFML and Box2D locations. The CMAKE error messages tell you which variables need to be set and to what values.
