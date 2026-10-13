#include <iostream>
#include "Game.h"
#include "Level1.h"
#include "Grid.h"

int main()
{
    std::cout 
        << "==================" << std::endl
        << "Skeleton To Nigromant --- raylib version --- by Daniel Gonzalez" << std::endl
        << "==================" << std::endl;
    /*
    Game game(500, 500, "Skeleton To Nigromant");
    game.SetScene(std::make_unique<Level1>());
    game.Run();
    */
    
    Grid grid;
    grid.ScanLevel("Level1.txt");

    return 0;
}