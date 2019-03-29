#include "hydroponics.h"
#include "debug.h"
#include "game.h"
#include "messages.h"
#include "output.h"
#include "iexamine.h"
#include "vehicle.h"

const skill_id skill_survival( "survival" );

itype_id hydroponics::query_seed()
{
    // Copypasted from iexamine::dirtmound
    // @todo Deduplicate
    std::vector<item *> seed_inv = g->u.items_with( []( const item &itm ) {
        return itm.is_seed();
    } );
    if( seed_inv.empty() ) {
        add_msg( m_info, _( "You have no seeds to plant." ) );
        return itype_id( "null" );
    }

    auto seed_entries = iexamine::get_seed_entries( seed_inv );
    int seed_index = iexamine::query_seed( seed_entries );

    if( seed_index < 0 || seed_index >= static_cast< int >( seed_entries.size() ) ) {
        add_msg( _( "You saved your seeds for later." ) );
        return itype_id( "null" );
    }
    return std::get<0>( seed_entries[seed_index] );
}

void hydroponics::plant_hydroponics( vehicle &veh, const tripoint &, int hydroponic_index )
{
    const auto &seed_id = query_seed();
    if( seed_id == itype_id( "null" ) ) {
        return;
    }
    // Copypasted from iexamine::plant_seed
    // @todo Deduplicate
    std::list<item> used_seed;
    if( item::count_by_charges( seed_id ) ) {
        used_seed = g->u.use_charges( seed_id, 1 );
    }
    else {
        used_seed = g->u.use_amount( seed_id, 1 );
    }

    // @todo Proper "replace seed here"
    veh.parts[hydroponic_index].ammo_set( seed_id, 1 );
    g->u.moves -= 500;
    add_msg( _( "Planted %s in %s." ), item::nname( seed_id ).c_str(),
        veh.parts[hydroponic_index].name().c_str() );
}

void hydroponics::harvest_hydroponics( vehicle &veh, const tripoint &pos, int hydroponic_index )
{
    auto items = veh.get_items( hydroponic_index );
    if( items.size() != 1 ) {
        debugmsg( "Tried to harvest hydroponics with non-1 item count" );
        return;
    }
    const itype &type = *items.front().type;
    items.clear();
    // Copypasted from iexamine::harvest_plant
    // @todo Deduplicate
    int skillLevel = g->u.get_skill_level( skill_survival );
    ///\EFFECT_SURVIVAL increases number of plants harvested from a seed
    int plantCount = rng( skillLevel / 2, skillLevel );
    if( plantCount >= 12 ) {
        plantCount = 12;
    }
    else if( plantCount <= 0 ) {
        plantCount = 1;
    }
    const int seedCount = std::max( 1l, rng( plantCount / 4, plantCount / 2 ) );
    for( auto &i : iexamine::get_harvest_items( type, plantCount, seedCount, true ) ) {
        g->m.add_item_or_charges( pos, i );
    }
    g->u.moves -= 500;
}

hydroponics::stage hydroponics::get_stage( vehicle & veh, int hydroponics_part )
{
    const item &hydro = *veh.part_base( hydroponics_part );
    if( hydro.contents.empty() ) {
        return stage::empty;
            seeded,
            seedling,
            immature,
            harvestable
        selectmenu.addentry( PLANT_HYDROPONICS, true, 'y', _( "Plant a seed in hydroponic tray" ) );
    }
    else if( maybe_plant.front().get_var( "growth_stage", 0 ) >= 3 ) {
        selectmenu.addentry( HARVEST_HYDROPONICS, true, 'Y', _( "Harvest plant from hydroponic tray" ) );
    }
    else {
        std::array<std::string, 3> staged_plant_names = { {
            _( "seed" ),
            _( "seedling" ),
            _( "immature plant" )
        } };
        int stage = maybe_plant.front().get_var( "growth_stage", 0 );
        selectmenu.addentry( KILL_HYDROPONICS, true, 'n',
            string_format( _( "Remove the %s from the hydroponic tray" ),
                staged_plant_names[stage] ).c_str() );
    }
}
