#include "game.h"

int main(int argc, char* args[])
{
	initializeGame();
	loop();
	freeGame();
	
	return 0;
}