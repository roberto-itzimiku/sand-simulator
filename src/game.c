#include "raylib.h"
#include "game.h"
#include "particles.h"
#include <stdlib.h>
#include <stdio.h>

#include <time.h>

#ifndef MIN_MAX_CLAMP_H
#define MIN_MAX_CLAMP_H
	#define MIN(a,b) (((a)<(b))?(a):(b))
	#define MAX(a,b) (((a)>(b))?(a):(b))
	#define CLAMP(a, b, c) MAX(MIN(a, c), b)
#endif // MIN_MAX_CLAMP_H

///////////////////////////////////////////////////////////////////////////////////////////////////
//
//
// VARIABLES
//
//
///////////////////////////////////////////////////////////////////////////////////////////////////

const uint32_t screenWidth = 640, screenHeight = 360;
const uint32_t mapWidth    = 140, mapHeight    = 90;
const uint32_t cellWidth   = 4;
Particle_t *particles     = NULL;
Particle_t *particlesNext = NULL;
bool pause = false;

uint8_t size_part_current = 5;
Particle_t *part_current = NULL;

///////////////////////////////////////////////////////////////////////////////////////////////////
//
//
// MANAGE GAME
//
//
///////////////////////////////////////////////////////////////////////////////////////////////////
void initializeGame()
{
	part_current = malloc(sizeof(Particle_t));
	set_particle(part_current, ROCK_ID, ROCK_COLOR);

	// create particles
	particles     = malloc(mapWidth * mapHeight * sizeof(Particle_t));
	particlesNext = malloc(mapWidth * mapHeight * sizeof(Particle_t));

	// init render
	InitWindow(screenWidth, screenHeight, "Sand simulator");
	SetTargetFPS(30);

	// initialize particles
	for (uint32_t i = 0; i < mapWidth*mapHeight; ++i)
	{
		set_particle(&particles[i], AIR_ID, BLACK);
		set_particle(&particlesNext[i], AIR_ID, BLACK);
	}
}

void freeGame()
{
	free(particles);
	free(particlesNext);
	free(part_current);
}

void loop()
{
	// main loop
	while(!WindowShouldClose())
	{
		clock_t start = clock();
		logic();
		clock_t end = clock();
		//printf("se tarda en ejecutar %d milisegundos\n", end - start);
		draw();
	}
	CloseWindow();
}




///////////////////////////////////////////////////////////////////////////////////////////////////
//
// OTHER FUNCTIONS
//
///////////////////////////////////////////////////////////////////////////////////////////////////
void set_current_particle(void)
{
	switch(GetKeyPressed())
	{
		case KEY_ONE:
			set_particle(part_current, ROCK_ID, ROCK_COLOR);
			size_part_current = 5;
			break;
		case KEY_TWO:
			set_particle(part_current, SAND_ID, SAND_COLOR);
			size_part_current = 10;
			break;
		case KEY_THREE:
			set_particle(part_current, WATER_ID, WATER_COLOR);
			size_part_current = 1;
			break;
		case KEY_FOUR:
			set_particle(part_current, 0, GREEN);
			size_part_current = 10;
			break;
	}	
}

void spawnMaterial(uint32_t xs, uint32_t ys, uint32_t w, uint32_t h, uint8_t id_type, Color color) 
{
	for (uint32_t y = 0; y < w; y++)
	{
		for (uint32_t x = 0; x < h; x++)
		{
			// arreglar x e y para que no se pasen
			uint32_t xf = CLAMP(x, 0, mapWidth);
			uint32_t yf = CLAMP(y, 0, mapHeight);

			uint32_t id = ( xs + ys*mapWidth )  +  (xf + yf * mapWidth );
			uint32_t id_fix = CLAMP(id, 0, mapWidth*mapHeight-1);
			set_particle(&particles[id_fix], id_type, color);
		}
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////
//
//
// LOGIC AND DRAWING
//
//
///////////////////////////////////////////////////////////////////////////////////////////////////

void logic()
{ 
	// mouse position
	Vector2 pos_mouse     = GetMousePosition();
	Vector2 pos_mouse_fix = pos_mouse;

	pos_mouse_fix.x = CLAMP(pos_mouse.x / (float)cellWidth, 0, mapWidth);
	pos_mouse_fix.y = CLAMP(pos_mouse.y / (float)cellWidth, 0, mapHeight);

	// pause
	if (IsKeyPressed(KEY_ENTER)) pause = !pause;

	// spawn materials
	set_current_particle();

	if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
	{
		spawnMaterial(pos_mouse_fix.x, pos_mouse_fix.y, size_part_current, size_part_current, part_current->id_type, part_current->color);
	}
	if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
	{
		spawnMaterial(pos_mouse_fix.x, pos_mouse_fix.y, 10, 10, AIR_ID, AIR_COLOR);
	}

	// initialize next particles
	for (uint32_t i = 0; i < mapWidth*mapHeight; i++)
		copy_particle(&particlesNext[i], &particles[i]);

	// logic
	if (!pause)
	{
		for (int32_t y = mapHeight-1; y >= 0; --y)
		{
			for (uint32_t x = 0; x < mapWidth; ++x)
			{
				// obtener vecinos y a mi
				uint32_t xf = CLAMP(x, 0, mapWidth-1);
				uint32_t yf = CLAMP(y, 0, mapHeight-1);

				// USAMOS CLAMP PARA QUE NO SE PASE DE INDICE YASE QUE QUEDA ORRIBLE :,V
				uint32_t me   = CLAMP( yf    * mapWidth + xf, 0, mapWidth*mapHeight);
				uint32_t nbD  = CLAMP((yf+1) * mapWidth + xf, 0, mapWidth*mapHeight);

				uint32_t nbL  = CLAMP( me - 1, 0, mapWidth*mapHeight-1);
				uint32_t nbR  = CLAMP( me + 1, 0, mapWidth*mapHeight-1);
				uint32_t nbDL = CLAMP(nbD - 1, 0, mapWidth*mapHeight-1);
				uint32_t nbDR = CLAMP(nbD + 1, 0, mapWidth*mapHeight-1);

				// evaluar estados
				switch(particles[me].id_type)
				{
					case SAND_ID:
					{
						particle_sand_update(particles, particlesNext, me, nbD, nbDL, nbDR, nbL, nbR);
						break;
					}
					case WATER_ID:
					{
						particle_water_update(particles, particlesNext, me, nbD, nbDL, nbDR, nbL, nbR);
						break;
					}
					default:
						break;
				}
			}
		}
	}

	// actualizar estados de las particulas
	for (int i = 0; i < mapWidth*mapHeight; ++i)
	{
		copy_particle(&particles[i], &particlesNext[i]);
	}
}

void draw()
{	
	BeginDrawing();
	ClearBackground(BLACK);

	// draw particles
	for (int y = 0; y < mapHeight; ++y)
	{
		for (int x = 0; x < mapWidth; ++x)
		{
			Color color = particles[y * mapWidth + x].color;
			DrawRectangle(x*cellWidth, y*cellWidth, cellWidth, cellWidth, color);
		}
	}

	EndDrawing();
}

// 25 milis dibujar
// 01 milis logica