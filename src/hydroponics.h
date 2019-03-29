#pragma once
#ifndef HYDROPONICS_H
#define HYDROPONICS_H

#include "item.h"

class vehicle;
class tripoint;

namespace hydroponics
{

enum class stage
{
    empty,
    seeded,
    seedling,
    immature,
    harvestable
};

itype_id query_seed();
void plant_hydroponics( vehicle &veh, const tripoint &pos, int hydroponics_part );
void harvest_hydroponics( vehicle &veh, const tripoint &pos, int hydroponics_part );
stage get_stage( vehicle &veh, int hydroponics_part );

};

#endif
