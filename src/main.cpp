#include <iostream>
#include "Game.h"
#include "LevelScene.h"

int main()
{
    std::cout 
        << "==================" << std::endl
        << "Skeleton To Nigromant --- raylib version --- by Daniel Gonzalez" << std::endl
        << "==================" << std::endl;

    Game game(1280, 920, "Skeleton To Nigromant");
    game.SetScene(std::make_unique<LevelScene>(0));
    return game.Run() ? 0 : 1;
}