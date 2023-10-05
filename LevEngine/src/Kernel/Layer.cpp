#include "levpch.h"
#include "Layer.h"

#include "DataTypes/Utility.h"

namespace LevEngine
{
Layer::Layer(String name) : m_Name(Move(name))
{
}
}
