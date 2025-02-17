#pragma once

#include <limits>
#include "../util/vector.h"
#include "QAngle.hpp"
struct Matrix4x4 {
    union {
        struct {
            float _11, _12, _13, _14;
            float _21, _22, _23, _24;
            float _31, _32, _33, _34;
            float _41, _42, _43, _44;

        };
        float m[4][4];
    };
};
struct cplane_t
{
    Vector3 normal;
    float dist;
    uint8_t type;   // for fast side tests
    uint8_t signbits;  // signx + (signy<<1) + (signz<<1)
    uint8_t pad[2];

};

class matrix3x4_t
{
public:
    matrix3x4_t() {}
    matrix3x4_t(
        float m00, float m01, float m02, float m03,
        float m10, float m11, float m12, float m13,
        float m20, float m21, float m22, float m23)
    {
        m_flMatVal[0][0] = m00; m_flMatVal[0][1] = m01; m_flMatVal[0][2] = m02; m_flMatVal[0][3] = m03;
        m_flMatVal[1][0] = m10; m_flMatVal[1][1] = m11; m_flMatVal[1][2] = m12; m_flMatVal[1][3] = m13;
        m_flMatVal[2][0] = m20; m_flMatVal[2][1] = m21; m_flMatVal[2][2] = m22; m_flMatVal[2][3] = m23;
    }
    //-----------------------------------------------------------------------------
    // Creates a matrix where the X axis = forward
    // the Y axis = left, and the Z axis = up
    //-----------------------------------------------------------------------------
    void Init(const Vector3& xAxis, const Vector3& yAxis, const Vector3& zAxis, const Vector3& vecOrigin)
    {
        m_flMatVal[0][0] = xAxis.x; m_flMatVal[0][1] = yAxis.x; m_flMatVal[0][2] = zAxis.x; m_flMatVal[0][3] = vecOrigin.x;
        m_flMatVal[1][0] = xAxis.y; m_flMatVal[1][1] = yAxis.y; m_flMatVal[1][2] = zAxis.y; m_flMatVal[1][3] = vecOrigin.y;
        m_flMatVal[2][0] = xAxis.z; m_flMatVal[2][1] = yAxis.z; m_flMatVal[2][2] = zAxis.z; m_flMatVal[2][3] = vecOrigin.z;
    }

    //-----------------------------------------------------------------------------
    // Creates a matrix where the X axis = forward
    // the Y axis = left, and the Z axis = up
    //-----------------------------------------------------------------------------
    matrix3x4_t(const Vector3& xAxis, const Vector3& yAxis, const Vector3& zAxis, const Vector3& vecOrigin)
    {
        Init(xAxis, yAxis, zAxis, vecOrigin);
    }

    inline void SetOrigin(Vector3 const& p)
    {
        m_flMatVal[0][3] = p.x;
        m_flMatVal[1][3] = p.y;
        m_flMatVal[2][3] = p.z;
    }

    inline void Invalidate(void)
    {
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 4; j++) {
                m_flMatVal[i][j] = std::numeric_limits<float>::infinity();;
            }
        }
    }

    Vector3 GetXAxis()  const { return at(0); }
    Vector3 GetYAxis()  const { return at(1); }
    Vector3 GetZAxis()  const { return at(2); }
    Vector3 GetOrigin() const { return at(3); }

    Vector3 at(int i) const { return Vector3{ m_flMatVal[0][i], m_flMatVal[1][i], m_flMatVal[2][i] }; }

    float* operator[](int i) { return m_flMatVal[i]; }
    const float* operator[](int i) const { return m_flMatVal[i]; }
    float* Base() { return &m_flMatVal[0][0]; }
    const float* Base() const { return &m_flMatVal[0][0]; }

    float m_flMatVal[3][4];
};
class VMatrix
{
public:

    VMatrix();
    VMatrix(
        float m00, float m01, float m02, float m03,
        float m10, float m11, float m12, float m13,
        float m20, float m21, float m22, float m23,
        float m30, float m31, float m32, float m33
    );

    // Creates a matrix where the X axis = forward
    // the Y axis = left, and the Z axis = up
    VMatrix(const Vector3& forward, const Vector3& left, const Vector3& up);

    // Construct from a 3x4 matrix
    VMatrix(const matrix3x4_t& matrix3x4);

    // Set the values in the matrix.
    void  Init(
        float m00, float m01, float m02, float m03,
        float m10, float m11, float m12, float m13,
        float m20, float m21, float m22, float m23,
        float m30, float m31, float m32, float m33
    );


    // Initialize from a 3x4
    void  Init(const matrix3x4_t& matrix3x4);

    // array access
    inline float* operator[](int i)
    {
        return m[i];
    }

    inline const float* operator[](int i) const
    {
        return m[i];
    }

    // Get a pointer to m[0][0]
    inline float* Base()
    {
        return &m[0][0];
    }

    inline const float* Base() const
    {
        return &m[0][0];
    }

    void  SetLeft(const Vector3& vLeft);
    void  SetUp(const Vector3& vUp);
    void  SetForward(const Vector3& vForward);

    void  GetBasisVectors(Vector3& vForward, Vector3& vLeft, Vector3& vUp) const;
    void  SetBasisVectors(const Vector3& vForward, const Vector3& vLeft, const Vector3& vUp);

    // Get/Set the translation.
    Vector3& GetTranslation(Vector3& vTrans) const;
    void  SetTranslation(const Vector3& vTrans);

    void  PreTranslate(const Vector3& vTrans);
    void  PostTranslate(const Vector3& vTrans);

    matrix3x4_t& As3x4();
    const matrix3x4_t& As3x4() const;
    void  CopyFrom3x4(const matrix3x4_t& m3x4);
    void  Set3x4(matrix3x4_t& matrix3x4) const;

    bool  operator==(const VMatrix& src) const;
    bool  operator!=(const VMatrix& src) const { return !(*this == src); }

    // Access the basis Vector3s.
    Vector3  GetLeft() const;
    Vector3  GetUp() const;
    Vector3  GetForward() const;
    Vector3  GetTranslation() const;


    // Matrix->Vector3 operations.
public:
    // Multiply by a 3D Vector3 (same as operator*).
    void  V3Mul(const Vector3& vIn, Vector3& vOut) const;

    // Multiply by a 4D Vector3.
    //void  V4Mul( const Vector34D &vIn, Vector34D &vOut ) const;

    // Applies the rotation (ignores translation in the matrix). (This just calls VMul3x3).
    Vector3  ApplyRotation(const Vector3& vVec) const;

    // Multiply by a Vector3 (divides by w, assumes input w is 1).
    Vector3  operator*(const Vector3& vVec) const;

    // Multiply by the upper 3x3 part of the matrix (ie: only apply rotation).
    Vector3  VMul3x3(const Vector3& vVec) const;

    // Apply the inverse (transposed) rotation (only works on pure rotation matrix)
    Vector3  VMul3x3Transpose(const Vector3& vVec) const;

    // Multiply by the upper 3 rows.
    Vector3  VMul4x3(const Vector3& vVec) const;

    // Apply the inverse (transposed) transformation (only works on pure rotation/translation)
    Vector3  VMul4x3Transpose(const Vector3& vVec) const;


    // Matrix->plane operations.
    //public:
    // Transform the plane. The matrix can only contain translation and rotation.
    //void  TransformPlane( const VPlane &inPlane, VPlane &outPlane ) const;

    // Just calls TransformPlane and returns the result.
    //VPlane  operator*(const VPlane &thePlane) const;

    // Matrix->matrix operations.
public:

    VMatrix& operator=(const VMatrix& mOther);

    // Add two matrices.
    const VMatrix& operator+=(const VMatrix& other);

    // Add/Subtract two matrices.
    VMatrix  operator+(const VMatrix& other) const;
    VMatrix  operator-(const VMatrix& other) const;

    // Negation.
    VMatrix  operator-() const;

    // Return inverse matrix. Be careful because the results are undefined 
    // if the matrix doesn't have an inverse (ie: InverseGeneral returns false).
    VMatrix  operator~() const;

    // Matrix operations.
public:
    // Set to identity.
    void  Identity();
    bool  IsIdentity() const;
public:
    // The matrix.
    float  m[4][4];
};

inline void MatrixGetColumn(const matrix3x4_t& src, int nCol, Vector3& pColumn)
{
    pColumn.x = src[0][nCol];
    pColumn.y = src[1][nCol];
    pColumn.z = src[2][nCol];
}

inline void MatrixPosition(const matrix3x4_t& matrix, Vector3& position)
{
    MatrixGetColumn(matrix, 3, position);
}