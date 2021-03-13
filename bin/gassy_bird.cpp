#include <iostream>

#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>

#include "Game.hpp"

int main(int argc, char** argv) {

    // create and initialize game
    Game game;
    game.init();

    // game loop
    while (game.update())
        game.draw();
    

    // exit
    return 0;
}
