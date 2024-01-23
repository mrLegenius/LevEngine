#pragma once
#include <DataTypes/Vector.h>
#include <Math/Matrix.h>
#include <Math/Vector3.h>
#include <Math/Quaternion.h>
#include <assimp/anim.h>
#include "cereal/access.hpp"

namespace LevEngine
{
    struct KeyPosition
    {
        Vector3 position;
        double timeStamp;

    private:
        friend class cereal::access;
        template <class Archive>
        void serialize(Archive& archive)
        {
            archive(position);
            archive(timeStamp);
        }
    };

    struct KeyRotation
    {
        Quaternion orientation;
        double timeStamp;

    private:
        friend class cereal::access;
        template <class Archive>
        void serialize(Archive& archive)
        {
            archive(orientation);
            archive(timeStamp);
        }
    };

    struct KeyScale
    {
        Vector3 scale;
        double timeStamp;

    private:
        friend class cereal::access;
        template <class Archive>
        void serialize(Archive& archive)
        {
            archive(scale);
            archive(timeStamp);
        }
    };

    class Bone
    {
    public:
        Bone() = default;

        /*reads keyframes from aiNodeAnim*/
        Bone(const String& name, size_t ID, const aiNodeAnim* channel);
        
        void Init();

        /*interpolates  b/w positions,rotations & scaling keys based on the curren time of
        the animation and prepares the local transformation matrix by combining all keys
        tranformations*/
        void Update(double animationTime);

        [[nodiscard]] const Matrix& GetLocalTransform() const;
        [[nodiscard]] const String& GetBoneName() const;
        [[nodiscard]] size_t GetBoneID() const;


        /* Gets the current index on mKeyPositions to interpolate to based on
        the current animation time*/
        [[nodiscard]] size_t GetPositionIndex(double animationTime);

        /* Gets the current index on mKeyRotations to interpolate to based on the
        current animation time*/
        [[nodiscard]] size_t GetRotationIndex(double animationTime);

        /* Gets the current index on mKeyScalings to interpolate to based on the
        current animation time */
        [[nodiscard]] size_t GetScaleIndex(double animationTime);

    private:
        friend class cereal::access;
        template <class Archive>
        void save(Archive& output) const
        {
            output(m_Positions);
            output(m_Rotations);
            output(m_Scales);
            output(m_Name);
            output(m_ID);
        }

        template <class Archive>
        void load(Archive& input)
        {
            input(m_Positions);
            input(m_Rotations);
            input(m_Scales);
            input(m_Name);
            input(m_ID);

            Init();
        }
        
        /* Gets normalized value for Lerp & Slerp*/
        [[nodiscard]] double GetScaleFactor(double lastTimeStamp, double nextTimeStamp, double animationTime) const;

        /*figures out which position keys to interpolate b/w and performs the interpolation
        and returns the translation matrix*/
        Matrix InterpolatePosition(double animationTime);

        /*figures out which rotations keys to interpolate b/w and performs the interpolation
        and returns the rotation matrix*/
        Matrix InterpolateRotation(double animationTime);

        /*figures out which scaling keys to interpolate b/w and performs the interpolation
        and returns the scale matrix*/
        Matrix InterpolateScaling(double animationTime);


        Vector<KeyPosition> m_Positions;
        Vector<KeyRotation> m_Rotations;
        Vector<KeyScale> m_Scales;
        String m_Name;
        size_t m_ID;

        Matrix m_LocalTransform;

        // Used when respective keyframes amount == 1 to accelerate updates
        Matrix m_ConstantTranslation;
        Matrix m_ConstantRotation;
        Matrix m_ConstantScale;
    };
}
