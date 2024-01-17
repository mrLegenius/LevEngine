#include "levpch.h"
#include "Bone.h"

#include "Math/Math.h"
#include "Renderer/3D/MeshLoading/AssimpConverter.h"

namespace LevEngine
{
    /*reads keyframes from aiNodeAnim*/
    Bone::Bone(const String& name, int ID, const aiNodeAnim* channel)
        :
        m_Name(name),
        m_ID(ID)
    {
        for (unsigned int positionIndex = 0; positionIndex < channel->mNumPositionKeys; ++positionIndex)
        {
            aiVector3D aiPosition = channel->mPositionKeys[positionIndex].mValue;
            const double timeStamp = channel->mPositionKeys[positionIndex].mTime;
            const Vector3 position = AssimpConverter::ToVector3(aiPosition);

            if (positionIndex == 0
                || (position - m_Positions.back().position).LengthSquared() > Math::FloatEpsilon)
            {
                KeyPosition data;
                data.position = position;
                data.timeStamp = timeStamp;
                m_Positions.push_back(data);   
            }
        }

        if (m_Positions.size() == 1)
        {
            m_ConstantTranslation = Matrix::CreateTranslation(m_Positions[0].position);
        }

        
        for (unsigned int rotationIndex = 0; rotationIndex < channel->mNumRotationKeys; ++rotationIndex)
        {
            aiQuaternion aiOrientation = channel->mRotationKeys[rotationIndex].mValue;
            const double timeStamp = channel->mRotationKeys[rotationIndex].mTime;
            const Quaternion quaternion = AssimpConverter::ToQuaternion(aiOrientation);

            if (rotationIndex == 0
                || Quaternion::Angle(quaternion, m_Rotations.back().orientation) > Math::FloatEpsilon)
            {
                KeyRotation data;
                data.orientation = quaternion;
                data.timeStamp = timeStamp;
                m_Rotations.push_back(data);
            }
        }
        
        if (m_Rotations.size() == 1)
        {
            Quaternion rotation = m_Rotations[0].orientation;
            rotation.Normalize();
            m_ConstantRotation = Matrix::CreateFromQuaternion(rotation);
        }
        

        for (unsigned int scaleIndex = 0; scaleIndex < channel->mNumScalingKeys; ++scaleIndex)
        {
            aiVector3D aiScale = channel->mScalingKeys[scaleIndex].mValue;
            const double timeStamp = channel->mScalingKeys[scaleIndex].mTime;

            const Vector3 scale = AssimpConverter::ToVector3(aiScale);
            
            if (scaleIndex == 0
                || (scale - m_Scales.back().scale).LengthSquared() > Math::FloatEpsilon)
            {
                KeyScale data;
                data.scale = scale;
                data.timeStamp = timeStamp;
                m_Scales.push_back(data); 
            }

            if (m_Scales.size() == 1)
            {
                m_ConstantScale = Matrix::CreateScale(m_Scales[0].scale);
            }
        }
    }

    /*interpolates  b/w positions,rotations & scaling keys based on the curren time of
    the animation and prepares the local transformation matrix by combining all keys
    transformations*/
    void Bone::Update(double animationTime)
    {
        const Matrix& translation = m_Positions.size() == 1 ? m_ConstantTranslation : InterpolatePosition(animationTime);
        const Matrix& rotation = m_Rotations.size() == 1 ? m_ConstantRotation : InterpolateRotation(animationTime);
        const Matrix& scale = m_Scales.size() == 1 ? m_ConstantScale : InterpolateScaling(animationTime);
        m_LocalTransform = (scale * rotation * translation);
    }

    const Matrix& Bone::GetLocalTransform() const { return m_LocalTransform; }
    const String& Bone::GetBoneName() const { return m_Name; }
    int Bone::GetBoneID() const { return m_ID; }


    /* Gets the current index on mKeyPositions to interpolate to based on
    the current animation time*/
    size_t Bone::GetPositionIndex(double animationTime)
    {
        for (size_t index = 0; index < m_Positions.size() - 1; ++index)
        {
            if (animationTime < m_Positions[index + 1].timeStamp)
            {
                return index;
            }
        }
        
        LEV_CORE_ASSERT(m_Positions.size() > 1);
        return m_Positions.size() - 2;
    }

    /* Gets the current index on mKeyRotations to interpolate to based on the
    current animation time*/
    size_t Bone::GetRotationIndex(double animationTime)
    {
        for (size_t index = 0; index < m_Rotations.size() - 1; ++index)
        {
            if (animationTime < m_Rotations[index + 1].timeStamp)
            {
                return index;
            }
        }
        
        LEV_CORE_ASSERT(m_Rotations.size() > 1);
        return m_Rotations.size() - 2;
    }

    /* Gets the current index on m_Scales to interpolate to based on the
    current animation time */
    size_t Bone::GetScaleIndex(double animationTime)
    {
        for (size_t index = 0; index < m_Scales.size() - 1; ++index)
        {
            if (animationTime < m_Scales[index + 1].timeStamp)
            {
                return index;
            }
        }
        
        LEV_CORE_ASSERT(m_Scales.size() > 1);
        return m_Scales.size() - 2;
    }


    /* Gets normalized value for Lerp & Slerp*/
    double Bone::GetScaleFactor(double lastTimeStamp, double nextTimeStamp, double animationTime) const
    {
        const double midWayLength = animationTime - lastTimeStamp;
        const double framesDiff = nextTimeStamp - lastTimeStamp;
        return midWayLength / framesDiff;
    }

    /*figures out which position keys to interpolate b/w and performs the interpolation
    and returns the translation matrix*/
    Matrix Bone::InterpolatePosition(double animationTime)
    {
        const size_t p0Index = GetPositionIndex(animationTime);
        const size_t p1Index = p0Index + 1;
        const double scaleFactor = GetScaleFactor(m_Positions[p0Index].timeStamp,
                                                 m_Positions[p1Index].timeStamp, animationTime);
        const Vector3 finalPosition = Vector3::Lerp(m_Positions[p0Index].position,
                                                    m_Positions[p1Index].position, scaleFactor);

        return Matrix::CreateTranslation(finalPosition);
    }

    /*figures out which rotations keys to interpolate b/w and performs the interpolation
    and returns the rotation matrix*/
    Matrix Bone::InterpolateRotation(double animationTime)
    {
        const size_t p0Index = GetRotationIndex(animationTime);
        const size_t p1Index = p0Index + 1;
        const double scaleFactor = GetScaleFactor(m_Rotations[p0Index].timeStamp,
                                                 m_Rotations[p1Index].timeStamp, animationTime);
        Quaternion finalRotation = Quaternion::Slerp(m_Rotations[p0Index].orientation,
            m_Rotations[p1Index].orientation, scaleFactor);
        finalRotation.Normalize();
        return Matrix::CreateFromQuaternion(finalRotation);
    }

    /*figures out which scaling keys to interpolate b/w and performs the interpolation
    and returns the scale matrix*/
    Matrix Bone::InterpolateScaling(double animationTime)
    {
        const size_t p0Index = GetScaleIndex(animationTime);
        const size_t p1Index = p0Index + 1;
        const double scaleFactor = GetScaleFactor(m_Scales[p0Index].timeStamp,
                                                 m_Scales[p1Index].timeStamp, animationTime);
        const Vector3 finalScale = Vector3::Lerp(m_Scales[p0Index].scale, m_Scales[p1Index].scale,
                                                 scaleFactor);
        return Matrix::CreateScale(finalScale);
    }
}
