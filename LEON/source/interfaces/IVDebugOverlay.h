#pragma once
class Vector3;

class IVDebugOverlay
{
public:
    virtual void AddEntityTextOverlay(int ent_index, int line_offset, float duration, int r, int g, int b, int a...) = 0;
    virtual void AddBoxOverlay(const Vector3& origin, const Vector3& mins, const Vector3& max, Vector3 const& orientation, int r, int g, int b, int a, float duration) = 0;
    virtual void AddSphereOverlay(const Vector3& vOrigin, float flRadius, int nTheta, int nPhi, int r, int g, int b, int a, float flDuration) = 0;
    virtual void AddTriangleOverlay(const Vector3& p1, const Vector3& p2, const Vector3& p3, int r, int g, int b, int a, bool noDepthTest, float duration) = 0;
    virtual void AddLineOverlay(const Vector3& origin, const Vector3& dest, int r, int g, int b, bool noDepthTest, float duration) = 0;
    virtual void AddTextOverlay(const Vector3& origin, float duration...) = 0;
    virtual void AddTextOverlay(const Vector3& origin, int line_offset, float duration ...) = 0;
    virtual void AddScreenTextOverlay(float flXPos, float flYPos, float flDuration, int r, int g, int b, int a, const char* text) = 0;
    virtual void AddSweptBoxOverlay(const Vector3& start, const Vector3& end, const Vector3& mins, const Vector3& max, const Vector3& angles, int r, int g, int b, int a, float flDuration) = 0;
    virtual void AddGridOverlay(const Vector3& origin) = 0;
    virtual int ScreenPosition(const Vector3& point, Vector3& screen) = 0;
    virtual void AddCoordFrameOverlay(const void* frame, float flScale, int vColorTable[3][3] = 0) = 0;


    virtual int ScreenPosition(float flXPos, float flYPos, Vector3& screen) = 0;
    /*int ScreenPosition(const Vector3& point, Vector3& screen)
    {
        return util::get_vfunc<11, int>(this, point, screen);
    }

    int ScreenPosition(float flXPos, float flYPos, Vector3& screen)
    {
        return util::get_vfunc<12, int>(this, flXPos, flYPos, screen);
    }*/
};