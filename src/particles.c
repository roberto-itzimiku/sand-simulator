#include "particles.h"
#include <stdlib.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// MAKE, FREE, MANAGE PARTICLES, ETC...
//
///////////////////////////////////////////////////////////////////////////////////////////////////
Particle_t*
make_particle(uint8_t id_type, Color color)
{
	Particle_t* particle = malloc(sizeof(Particle_t));
	particle->id_type = id_type;
	particle->color   = color;

	return particle;
}

void
free_particle(Particle_t* particle)
{
	free(particle);
}

void
set_particle(Particle_t* particle, uint8_t id_type, Color color)
{
	particle->id_type = id_type;
	particle->color   = color;
}

void
copy_particle(Particle_t* particle, Particle_t* to_copy)
{
	particle->id_type  = to_copy->id_type;
	particle->color    = to_copy->color;
}




///////////////////////////////////////////////////////////////////////////////////////////////////
//
// OTHERS
//
///////////////////////////////////////////////////////////////////////////////////////////////////
bool
move_particle(Particle_t* parts, Particle_t* partN, uint32_t origin, uint32_t obj, uint8_t free_id)
{
    if (parts[obj].id_type == free_id)
    {
        copy_particle(&partN[obj], &parts[origin]);
        copy_particle(&partN[origin], &parts[obj]);
        return true;
    } 

    return false;
}

bool
can_move(Particle_t* parts, uint32_t obj, uint8_t free_id)
{
    return (parts[obj].id_type == free_id);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// 	LOGIC PARTICLES
//
///////////////////////////////////////////////////////////////////////////////////////////////////
void
particle_sand_update(Particle_t* parts, Particle_t* partN, uint32_t me, uint32_t nbD, uint32_t nbDL, uint32_t nbDR, uint32_t nbL, uint32_t nbR)
{
    if (!move_particle(parts, partN, me,  nbD, AIR_ID))
    if (!move_particle(parts, partN, me, nbDL, AIR_ID))
         move_particle(parts, partN, me, nbDR, AIR_ID);
}

void
particle_water_update(Particle_t* parts, Particle_t* partN, uint32_t me, uint32_t nbD, uint32_t nbDL, uint32_t nbDR, uint32_t nbL, uint32_t nbR)
{
   	if (!move_particle(parts, partN, me,  nbD, AIR_ID))
   	    if (!move_particle(parts, partN, me,  nbL, AIR_ID))
            if (can_move(partN, nbR, AIR_ID))
                move_particle(parts, partN, me,  nbR, AIR_ID);

            else if (can_move(partN, nbDL, AIR_ID))
                move_particle(parts, partN, me, nbDL, AIR_ID);
            else if (can_move(partN, nbDR, AIR_ID))
                move_particle(parts, partN, me, nbDR, AIR_ID);
}