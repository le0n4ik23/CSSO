#pragma once
#include "../interfaces/interfaces.h"
#include "../util/vector.h"
#include "../util/usercmd.h"
#include "i_player_movement.hpp"
namespace prediction {
	void start(UserCmd* cmd);
	void end();

	inline player_move_data data;
	inline float old_cur_time;
	inline float old_frame_time;
	inline int* prediction_random_seed;
	//void restoreentitytopredictedframe(int predictedFrame) noexcept;
	void restore(int predicted_frame);
	inline Vector3 origin;
	inline Vector3 velocity;
	inline int flags;
};