#pragma once
#include "vector.h"
#include "crc.h"

enum CommandButtons : int
{
    IN_ATTACK = (1 << 0),
    IN_JUMP = (1 << 1),
    IN_DUCK = (1 << 2),
    IN_FORWARD = (1 << 3),
    IN_BACK = (1 << 4),
    IN_USE = (1 << 5),
    IN_CANCEL = (1 << 6),
    IN_LEFT = (1 << 7),
    IN_RIGHT = (1 << 8),
    IN_MOVELEFT = (1 << 9),
    IN_MOVERIGHT = (1 << 10),
    IN_SECOND_ATTACK = (1 << 11),
    IN_RUN = (1 << 12),
    IN_RELOAD = (1 << 13),
    IN_LEFT_ALT = (1 << 14),
    IN_RIGHT_ALT = (1 << 15),
    IN_SCORE = (1 << 16),
    IN_SPEED = (1 << 17),
    IN_WALK = (1 << 18),
    IN_ZOOM = (1 << 19),
    IN_FIRST_WEAPON = (1 << 20),
    IN_SECOND_WEAPON = (1 << 21),
    IN_BULLRUSH = (1 << 22),
    IN_FIRST_GRENADE = (1 << 23),
    IN_SECOND_GRENADE = (1 << 24),
    IN_MIDDLE_ATTACK = (1 << 25)
};

/*
struct UserCmd
{
    void* vmt;
    std::int32_t commandNumber;
    std::int32_t tickCount;
    Vector3 viewAngles;
    float forwardMove;
    float sideMove;
    float upMove;
    std::int32_t buttons;
    char impulse;
    std::int32_t weaponSelect;
    std::int32_t weaponSubType;
    std::int32_t randomSeed;
    short mouseDeltaX;
    short mouseDeltaY;
    bool hasBeenPredicted;
    Vector3 headAngles;
    Vector3 headOffset;
};
*/

class UserCmd {
public:
	UserCmd()
	{
		reset();
	}

	virtual ~UserCmd() { };

	void reset()
	{
		commandNumber = 0;
		tick_count = 0;
		viewAngles.Init(0.f, 0.f, 0.f);
		forwardMove = 0.0f;
		sideMove = 0.0f;
		upmove = 0.0f;
		buttons = 0;
		impulse = 0;
		weaponselect = 0;
		weaponsubtype = 0;
		randomSeed = 0;
		mousedx = 0;
		mousedy = 0;

		hasbeenpredicted = false;
	}

	UserCmd& operator =(const UserCmd& src)
	{
		if (this == &src)
			return *this;

		commandNumber = src.commandNumber;
		tick_count = src.tick_count;
		viewAngles = src.viewAngles;
		forwardMove = src.forwardMove;
		sideMove = src.sideMove;
		upmove = src.upmove;
		buttons = src.buttons;
		impulse = src.impulse;
		weaponselect = src.weaponselect;
		weaponsubtype = src.weaponsubtype;
		randomSeed = src.randomSeed;
		mousedx = src.mousedx;
		mousedy = src.mousedy;

		hasbeenpredicted = src.hasbeenpredicted;

		return *this;
	}

	UserCmd(const UserCmd& src)
	{
		*this = src;
	}

	CRC32_t GetChecksum(void) const
	{
		CRC32_t crc;

		CRC32_Init(&crc);
		CRC32_ProcessBuffer(&crc, &commandNumber, sizeof(commandNumber));
		CRC32_ProcessBuffer(&crc, &tick_count, sizeof(tick_count));
		CRC32_ProcessBuffer(&crc, &viewAngles, sizeof(viewAngles));
		CRC32_ProcessBuffer(&crc, &forwardMove, sizeof(forwardMove));
		CRC32_ProcessBuffer(&crc, &sideMove, sizeof(sideMove));
		CRC32_ProcessBuffer(&crc, &upmove, sizeof(upmove));
		CRC32_ProcessBuffer(&crc, &buttons, sizeof(buttons));
		CRC32_ProcessBuffer(&crc, &impulse, sizeof(impulse));
		CRC32_ProcessBuffer(&crc, &weaponselect, sizeof(weaponselect));
		CRC32_ProcessBuffer(&crc, &weaponsubtype, sizeof(weaponsubtype));
		CRC32_ProcessBuffer(&crc, &randomSeed, sizeof(randomSeed));
		CRC32_ProcessBuffer(&crc, &mousedx, sizeof(mousedx));
		CRC32_ProcessBuffer(&crc, &mousedy, sizeof(mousedy));
		CRC32_Final(&crc);

		return crc;
	}

	// Allow command, but negate gameplay-affecting values
	void MakeInert(void)
	{
		viewAngles = { 0.f, 0.f, 0.f };
		forwardMove = 0.f;
		sideMove = 0.f;
		upmove = 0.f;
		buttons = 0;
		impulse = 0;
	}

	// For matching server and client commands for debugging
	int		commandNumber;

	// the tick the client created this command
	int		tick_count;

	// Player instantaneous view angles.
	Vector3	viewAngles;
	// Intended velocities
	//	IN_FORWARD velocity.
	float	forwardMove;
	//  sideways velocity.
	float	sideMove;
	//  upward velocity.
	float	upmove;
	// Attack button states
	int		buttons;
	// Impulse command issued.
	unsigned char    impulse;
	// Current weapon id
	int		weaponselect;
	int		weaponsubtype;

	int		randomSeed;	// For shared random functions

	short	mousedx;		// mouse accum in x from create move
	short	mousedy;		// mouse accum in y from create move

	// Client only, tracks whether we've predicted this command at least once
	bool	hasbeenpredicted;


};