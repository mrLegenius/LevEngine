#include "levpch.h"
#include "Bone.h"
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
            double timeStamp = channel->mPositionKeys[positionIndex].mTime;
            KeyPosition data;
            data.position = AssimpConverter::ToVector3(aiPosition);
            data.timeStamp = timeStamp;
            m_Positions.push_back(data);
        }

        for (unsigned int rotationIndex = 0; rotationIndex < channel->mNumRotationKeys; ++rotationIndex)
        {
            aiQuaternion aiOrientation = channel->mRotationKeys[rotationIndex].mValue;
            double timeStamp = channel->mRotationKeys[rotationIndex].mTime;
            KeyRotation data;
            data.orientation = AssimpConverter::ToQuaternion(aiOrientation);
            data.timeStamp = timeStamp;
            m_Rotations.push_back(data);
        }

        for (unsigned int scaleIndex = 0; scaleIndex < channel->mNumScalingKeys; ++scaleIndex)
        {
            aiVector3D scale = channel->mScalingKeys[scaleIndex].mValue;
            double timeStamp = channel->mScalingKeys[scaleIndex].mTime;
            KeyScale data;
            data.scale = AssimpConverter::ToVector3(scale);
            data.timeStamp = timeStamp;
            m_Scales.push_back(data);
        }

        // Set bone to starting pose
        Update(0.0f);
    }

    /*interpolates  b/w positions,rotations & scaling keys based on the curren time of
    the animation and prepares the local transformation matrix by combining all keys
    transformations*/
    void Bone::Update(double animationTime)
    {
        const Matrix translation = InterpolatePosition(animationTime);
        const Matrix rotation = InterpolateRotation(animationTime);
        const Matrix scale = InterpolateScaling(animationTime);
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
        
        LEV_CORE_ASSERT(0)
        return -1;
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
        
        LEV_CORE_ASSERT(0)
        return -1;
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
        
        LEV_CORE_ASSERT(0)
        return -1;
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
        if (1 == m_Positions.size())
        {
            return Matrix::CreateTranslation(m_Positions[0].position);
        }

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
        if (1 == m_Rotations.size())
        {
            Quaternion rotation = m_Rotations[0].orientation;
            rotation.Normalize();
            return Matrix::CreateFromQuaternion(rotation);
        }

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
        if (1 == m_Scales.size())
        {
            return Matrix::CreateScale(m_Scales[0].scale);
        }

        const size_t p0Index = GetScaleIndex(animationTime);
        const size_t p1Index = p0Index + 1;
        const double scaleFactor = GetScaleFactor(m_Scales[p0Index].timeStamp,
                                                 m_Scales[p1Index].timeStamp, animationTime);
        const Vector3 finalScale = Vector3::Lerp(m_Scales[p0Index].scale, m_Scales[p1Index].scale,
                                                 scaleFactor);
        return Matrix::CreateScale(finalScale);
    }
}
