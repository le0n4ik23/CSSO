#pragma once

#include "../util/vector.h"


class c_move_data
{
public:
    bool            m_bFirstRunOfFunctions : 1;
    bool            m_bGameCodeMovedPlayer : 1;
    int    m_nPlayerHandle;
    int                m_nImpulseCommand;
    Vector3          m_vecviewAngles;
    Vector3           m_vecAbsviewAngles;
    int                m_nButtons;
    int                m_nOldButtons;
    float            m_flForwardMove;
    float            m_flSideMove;
    float            m_flUpMove;
    float            m_flMaxSpeed;
    float            m_flClientMaxSpeed;
    Vector3            m_vecVelocity;
    Vector3            m_vecAngles;
    Vector3            m_vecOldAngles;
    float            m_outStepHeight;
    Vector3            m_outWishVel;
    Vector3            m_outJumpVel;
    Vector3            m_vecConstraintCenter;
    float            m_flConstraintRadius;
    float            m_flConstraintWidth;
    float            m_flConstraintSpeedFactor;
    float            m_flUnknown[5];
    Vector3            m_vecAbsOrigin;
};
