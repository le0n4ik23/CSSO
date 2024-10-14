#pragma once

#include "../classes/CEntity.h"
#include "../util/usercmd.h"

#include "../classes/singleton.h"

#include "../classes/c_move_data.h"


class c_engine_prediction : public singleton<c_engine_prediction> {
public:

	c_engine_prediction();

	void start(UserCmd*, CEntity*);
	void end(UserCmd*, CEntity*);

public:

	int flags;
	Vector3 predicted_velocity;
	Vector3 predicted_position;
	
private:

	float old_current_time;
	float old_frametime;

	c_move_data move_data;

	void* move_helper;

};
