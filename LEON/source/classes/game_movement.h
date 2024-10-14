#pragma once

#include "../util/util.h"
#include "c_move_data.h"
#include "CEntity.h"

class i_game_movement {
public:
    void process_movement(CEntity* player, c_move_data* move_data) {
        typedef void(__thiscall* o_fn)(void*, CEntity*, c_move_data*);
        util::get_vfunc<o_fn>(1, this)(this, player, move_data);
    }

    void start_track_prediction_errors(CEntity* player) {
        typedef void(__thiscall* o_fn)(void*, CEntity*);
        util::get_vfunc<o_fn>(3, this)(this, player);
    }

    void finish_track_prediction_errors(CEntity* player) {
        typedef void(__thiscall* o_fn)(void*, CEntity*);
        util::get_vfunc<o_fn>(4, this)(this, player);
    }

    // Другие методы, если нужно
};


