#pragma once

#include "c_move_data.h"
#include "../util/util.h"
#include "../hooks.h"
#include "CEntity.h"


class v_client_prediction {
public:
    void setup_move(CEntity* player, UserCmd* cmd, void* move_helper, c_move_data* move_data) {
        typedef void(__thiscall* o_fn)(void*, CEntity*, UserCmd*, void*, c_move_data*);
        util::get_vfunc<o_fn>(18, this)(this, player, cmd, move_helper, move_data);
    }

    void finish_move(CEntity* player, UserCmd* cmd, c_move_data* move_data) {
        typedef void(__thiscall* o_fn)(void*, CEntity*, UserCmd*, c_move_data*);
        util::get_vfunc<o_fn>(19, this)(this, player, cmd, move_data);
    }
};
