#include "catch/catch.hpp"
#include "game.h"
#include "map.h"
#include "player.h"
#include "veh_type.h"
#include "vehicle.h"
#include "vpart_range.h"
#include "map_helpers.h"
#include "player_helpers.h"

TEST_CASE( "hydroponics" )
{
    GIVEN( "A vehicle with a hydroponic tray" ) {

        clear_player();
        clear_map();

        const tripoint vehicle_origin( 60, 60, 0 );
        vehicle *veh_ptr = g->m.add_vehicle( vproto_id( "hydroponics_test" ), vehicle_origin, -90, 100, 0 );
        REQUIRE( veh_ptr != nullptr );
        vehicle &veh = *veh_ptr;
        REQUIRE( veh.fuel_left( "water" ) > 0 );
        REQUIRE( veh.fuel_left( "fertilizer_liquid" ) > 0 );
        REQUIRE( veh.fuel_left( "battery" ) > 0 );
        auto hydro_parts = veh.get_avail_parts( "HYDROPONICS" );
        // Exactly one hydro
        REQUIRE( std::distance( std::begin( hydro_parts ), std::end( hydro_parts ) ) == 1 );
        vpart_reference hydro = *std::begin( hydro_parts );
        WHEN( "A seed is added into the tray" ) {
            hydro.part().ammo_set( "seed_oats", 1 );
            auto item_stack = veh.get_items( hydro.part_index() );
            REQUIRE( item_stack.size() == 1 );
            THEN( "The player's grab is released" ) {
            }
        }
    }
}
