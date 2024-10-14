#pragma once
#include "../util/vector.h"
#include "../util/usercmd.h"
#include "../classes/CEntity.h"


class player_move_helper {
public:
	bool	first_run_of_iunctions : 1;
	bool	game_code_moved_player : 1;
	int	player_handle;
	int	impulse_command;
	Vector3	view_angles;
	Vector3	abs_view_angles;
	int	buttons;
	int	old_buttons;
	float	forward_move;
	float	side_move;
	float	up_move;
	float	max_speed;
	float	client_max_speed;
	Vector3	velocity;
	Vector3	angles;
	Vector3	old_angles;
	float	out_step_height;
	Vector3	wish_velocity;
	Vector3	jump_velocity;
	Vector3	constraint_center;
	float	constraint_radius;
	float	constraint_width;
	float	constraint_speed_factor;
	float	u0[5];
	Vector3	abs_origin;
	virtual	void u1() = 0;
	virtual void set_host(CEntity* host) = 0;
};

extern player_move_helper* move_helper;

class player_move_data {
public:
	bool    first_run_of_instructions : 1;
	bool    game_code_moved_player : 1;
	int     player_handle;
	int     impulse_command;
	Vector3	view_angles;
	Vector3	abs_view_angles;
	int     buttons;
	int     old_buttons;
	float   fw_move;
	float   sd_move;
	float   up_move;
	float   max_speed;
	float   client_max_speed;
	Vector3	velocity;
	Vector3	angles;
	Vector3	old_angles;
	float   step_height;
	Vector3	wish_velocity;
	Vector3	jump_velocity;
	Vector3	constraint_center;
	float   constraint_radius;
	float   constraint_width;
	float   constraint_speed_factor;
	float   u0[5];
	Vector3	abs_origin;
};

extern player_move_data* move_data;

class virtual_game_movement {

public:
	virtual				~virtual_game_movement(void) {}
	virtual void			process_movement(CEntity* player, player_move_data* move) = 0;
	virtual void			reset(void) = 0;
	virtual void			start_track_prediction_errors(CEntity* player) = 0;
	virtual void			finish_track_prediction_errors(CEntity* player) = 0;
	virtual void			diff_print(char const* fmt, ...) = 0;
	virtual Vector3 const& get_player_mins(bool ducked) const = 0;
	virtual Vector3 const& get_player_maxs(bool ducked) const = 0;
	virtual Vector3 const& get_player_view_offset(bool ducked) const = 0;
	virtual bool			is_moving_player_stuck(void) const = 0;
	virtual CEntity* get_moving_player(void) const = 0;
	virtual void			unblock_posher(CEntity* player, CEntity* pusher) = 0;
	virtual void			setup_movement_bounds(player_move_data* move) = 0;
};

extern virtual_game_movement* game_movement;

class player_game_movement : public virtual_game_movement {
public:
	virtual ~player_game_movement(void) { }
};

class player_prediction {
public:
	std::byte		pad0[0x4];						// 0x0000
	std::uintptr_t	hLastGround;					// 0x0004
	bool			bInPrediction;					// 0x0008
	bool			bIsFirstTimePredicted;			// 0x0009
	bool			bEnginePaused;					// 0x000A
	bool			bOldCLPredictValue;				// 0x000B
	int				iPreviousStartFrame;			// 0x000C
	int				nIncomingPacketNumber;			// 0x0010
	float			flLastServerWorldTimeStamp;		// 0x0014

	struct Split_t
	{
		bool		bIsFirstTimePredicted;			// 0x0018
		std::byte	pad0[0x3];						// 0x0019
		int			nCommandsPredicted;				// 0x001C
		int			nServerCommandsAcknowledged;	// 0x0020
		int			iPreviousAckHadErrors;			// 0x0024
		float		flIdealPitch;					// 0x0028
		int			iLastCommandAcknowledged;		// 0x002C
		bool		bPreviousAckErrorTriggersFullLatchReset; // 0x0030
	};
	Split_t			Split[1];						// 0x0018

public:
	bool in_prediction() {
		typedef bool(__thiscall* fn)(void*);
		return util::call_virtual<fn>(this, 14)(this);
	}
	void run_command(CEntity* player, UserCmd* cmd, player_move_helper* helper) {
		typedef void(__thiscall* fn)(void*, CEntity*, UserCmd*, player_move_helper*);
		return util::call_virtual<fn>(this, 19)(this, player, cmd, helper);
	}
	void check_moving_ground(CEntity* player, double frametime) {
		typedef void(__thiscall* fn)(void*, CEntity*, double);
		return util::call_virtual<fn>(this, 18)(this, player, frametime);
	}
	void set_local_view_angles(Vector3& ang) {
		typedef void(__thiscall* fn)(void*, Vector3&);
		return util::call_virtual<fn>(this, 13)(this, ang);
	}
	void setup_move(CEntity* player, UserCmd* cmd, player_move_helper* helper, player_move_data* data) {
		typedef void(__thiscall* fn)(void*, CEntity*, UserCmd*, player_move_helper*, player_move_data*);
		return util::call_virtual<fn>(this, 20)(this, player, cmd, helper, data);
	}
	void finish_move(CEntity* player, UserCmd* cmd, player_move_data* data) {
		typedef void(__thiscall* fn)(void*, CEntity*, UserCmd*, player_move_data*);
		return util::call_virtual<fn>(this, 21)(this, player, cmd, data);
	}
};
extern player_prediction* prediction1;