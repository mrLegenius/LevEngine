#pragma once

namespace LevEngine
{
	class Math
	{
	public:

		template<typename T>
		static T Max(T a, T b) { return ((a) > (b)) ? (a) : (b); }
	};
}
