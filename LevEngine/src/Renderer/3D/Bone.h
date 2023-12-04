#pragma once
#include <DataTypes/Vector.h>
#include <Math/Matrix.h>
#include <Math/Vector3.h>
#include <Math/Quaternion.h>
#include <assimp/anim.h>

namespace LevEngine
{
    struct KeyPosition
    {
        Vector3 position;
        double timeStamp;
    };

    struct KeyRotation
    {
        Quaternion orientation;
        double timeStamp;
    };

    struct KeyScale
    {
        Vector3 scale;
        double timeStamp;
    };

    class Bone
    {
    public:
        /*reads keyframes from aiNodeAnim*/
        Bone(const String& name, int ID, const aiNodeAnim* channel);

        /*interpolates  b/w positions,rotations & scaling keys based on the curren time of
        the animation and prepares the local transformation matrix by combining all keys
        tranformations*/
        void Update(float animationTime);

        [[nodiscard]] const Matrix& GetLocalTransform() const;
        [[nodiscard]] const String& GetBoneName() const;
        [[nodiscard]] int GetBoneID() const;


        /* Gets the current index on mKeyPositions to interpolate to based on
        the current animation time*/
        [[nodiscard]] int GetPositionIndex(float animationTime);

        /* Gets the current index on mKeyRotations to interpolate to based on the
        current animation time*/
        [[nodiscard]] int GetRotationIndex(float animationTime);

        /* Gets the current index on mKeyScalings to interpolate to based on the
        current animation time */
        [[nodiscard]] int GetScaleIndex(float animationTime);

    private:

        /* Gets normalized value for Lerp & Slerp*/
        [[nodiscard]] float GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime) const;

        /*figures out which position keys to interpolate b/w and performs the interpolation
        and returns the translation matrix*/
        Matrix InterpolatePosition(float animationTime);

        /*figures out which rotations keys to interpolate b/w and performs the interpolation
        and returns the rotation matrix*/
        Matrix InterpolateRotation(float animationTime);

        /*figures out which scaling keys to interpolate b/w and performs the interpolation
        and returns the scale matrix*/
        Matrix InterpolateScaling(float animationTime);


        Vector<KeyPosition> m_Positions;
        Vector<KeyRotation> m_Rotations;
        Vector<KeyScale> m_Scales;

        Matrix m_LocalTransform;
        String m_Name;
        int m_ID;

    };
}
