#include <iostream>
#include "Game.h"
#include "Level1.h"

int main()
{
    std::cout 
        << "==================" << std::endl
        << "Skeleton To Nigromant --- raylib version --- by Daniel Gonzalez" << std::endl
        << "==================" << std::endl;

    Game game(1280, 920, "Skeleton To Nigromant");
    game.SetScene(std::make_unique<Level1>());
    game.Run();

    return 0;
}