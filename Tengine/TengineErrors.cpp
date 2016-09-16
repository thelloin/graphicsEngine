#include "TengineErrors.h"

#include <cstdlib> // Used for exit??

#include <iostream>
#include <SDL/SDL.h>

namespace Tengine {

	// Prints out an error message and exists the game
	void fatalError(std::string errorString)
	{
		std::cout << errorString << std::endl;
		std::cout << "Enter any key to exit... ";
		int tmp;
		std::cin >> tmp;
		SDL_Quit();
		exit(1);
	}

}