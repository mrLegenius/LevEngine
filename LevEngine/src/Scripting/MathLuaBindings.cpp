#include "levpch.h"
#include "MathLuaBindings.h"
#include "Math/Math.h"


namespace LevEngine::Scripting 
{
	void CreateVector3Bind(sol::state& lua) 
	{
		auto vec3_multiplication_overload = sol::overload(
			[](const Vector3& v1, const Vector3& v2) {
				return v1 * v2;
			},
			[](const Vector3& v1, float value) {
				return v1 * value;
			},
			[](float value, const Vector3& v1) {
				return v1 * value;
			}
		);

		auto vec3_division_overload = sol::overload(
			[](const Vector3& v1, const Vector3& v2) {
				return v1 / v2;
			},
			[](const Vector3& v1, float value) {
				return v1 / value;
			}
		);

		auto vec3_addition_overload = sol::overload(
			[](const Vector3& v1, const Vector3& v2) {
				return v1 + v2;
			}
		);

		auto vec3_substraction_overload = sol::overload(
			[](const Vector3& v1, const Vector3& v2) {
				return v1 - v2;
			}
		);

		lua.new_usertype<Vector3>(
			"Vector3",
			sol::call_constructor,
			sol::constructors<Vector3(float), Vector3(float, float, float)>(),
			"x", &Vector3::x,
			"y", &Vector3::y,
			"z", &Vector3::z,
			sol::meta_function::addition, vec3_addition_overload,
			sol::meta_function::subtraction, vec3_substraction_overload,
			sol::meta_function::multiplication, vec3_multiplication_overload,
			sol::meta_function::division, vec3_division_overload
		);
	}

	void CreateVector2Bind(sol::state& lua)
	{
		auto vec2_multiplication_overload = sol::overload(
			[](const Vector2& v1, const Vector2& v2) {
				return v1 * v2;
			},
			[](const Vector2& v1, float value) {
				return v1 * value;
			},
			[](float value, const Vector2& v1) {
				return v1 * value;
			}
		);

		auto vec2_division_overload = sol::overload(
			[](const Vector2& v1, const Vector2& v2) {
				return v1 / v2;
			},
			[](const Vector2& v1, float value) {
				return v1 / value;
			}
		);

		auto vec2_addition_overload = sol::overload(
			[](const Vector2& v1, const Vector2& v2) {
				return v1 + v2;
			}
		);

		auto vec2_substraction_overload = sol::overload(
			[](const Vector2& v1, const Vector2& v2) {
				return v1 - v2;
			}
		);

		lua.new_usertype<Vector2>(
			"Vector2",
			sol::call_constructor,
			sol::constructors<Vector2(float), Vector2(float, float)>(),
			"x", &Vector2::x,
			"y", &Vector2::y,
			sol::meta_function::addition, vec2_addition_overload,
			sol::meta_function::subtraction, vec2_substraction_overload,
			sol::meta_function::multiplication, vec2_multiplication_overload,
			sol::meta_function::division, vec2_division_overload
		);
	}

	void CreateVector4Bind(sol::state& lua)
	{
		auto vec4_multiplication_overload = sol::overload(
			[](const Vector4& v1, const Vector4& v2) {
				return v1 * v2;
			},
			[](const Vector4& v1, float value) {
				return v1 * value;
			},
			[](float value, const Vector4& v1) {
				return v1 * value;
			}
		);

		auto vec4_division_overload = sol::overload(
			[](const Vector4& v1, const Vector4& v2) {
				return v1 / v2;
			},
			[](const Vector4& v1, float value) {
				return v1 / value;
			}
		);

		auto vec4_addition_overload = sol::overload(
			[](const Vector4& v1, const Vector4& v2) {
				return v1 + v2;
			}
		);

		auto vec4_substraction_overload = sol::overload(
			[](const Vector4& v1, const Vector4& v2) {
				return v1 - v2;
			}
		);

		lua.new_usertype<Vector4>(
			"Vector4",
			sol::call_constructor,
			sol::constructors<Vector4(float), Vector4(float, float, float, float)>(),
			"x", &Vector4::x,
			"y", &Vector4::y,
			"z", &Vector4::z,
			"w", &Vector4::w,
			sol::meta_function::addition, vec4_addition_overload,
			sol::meta_function::subtraction, vec4_substraction_overload,
			sol::meta_function::multiplication, vec4_multiplication_overload,
			sol::meta_function::division, vec4_division_overload
		);
	}

	void CreateQuaternionBind(sol::state& lua) 
	{
		auto vec4_multiplication_overload = sol::overload(
			[](const Quaternion& v1, const Quaternion& v2) {
				return v1 * v2;
			},
			[](const Quaternion& v1, float value) {
				return v1 * value;
			},
			[](float value, const Quaternion& v1) {
				return value * v1;
			}
		);

		auto vec4_division_overload = sol::overload(
			[](const Quaternion& v1, const Quaternion& v2) {
				return v1 / v2;
			}
		);

		auto vec4_addition_overload = sol::overload(
			[](const Quaternion& v1, const Quaternion& v2) {
				return v1 + v2;
			}
		);

		auto vec4_substraction_overload = sol::overload(
			[](const Quaternion& v1, const Quaternion& v2) {
				return v1 - v2;
			}
		);

		lua.new_usertype<Quaternion>(
			"Quaternion",
			sol::call_constructor,
			sol::constructors<Quaternion(), Quaternion(float, float, float, float)>(),
			"x", &Quaternion::x,
			"y", &Quaternion::y,
			"z", &Quaternion::z,
			"w", &Quaternion::w,
			sol::meta_function::addition, vec4_addition_overload,
			sol::meta_function::subtraction, vec4_substraction_overload,
			sol::meta_function::multiplication, vec4_multiplication_overload,
			sol::meta_function::division, vec4_division_overload,
			"toEuler", [](Quaternion& quaternion) { return quaternion.ToEuler(); }
		);
	}

	void MathFreeFunctions(sol::state& lua) {
		auto math = lua["Math"].get_or_create<sol::table>();
		math.set_function("distance", sol::overload(
			[](const Vector2& a, const Vector2& b) {
				return Vector2::Distance(a, b);
			},
			[](const Vector3& a, const Vector3& b) {
				return Vector3::Distance(a, b);
			},
			[](const Vector4& a, const Vector4& b) {
				return Vector4::Distance(a, b);
			}
		));

		math.set_function("lerp", [](float a, float b, float t) {return std::lerp(a, b, t); });
		math.set_function("clamp", sol::overload(
			[](float value, float min, float max) {return std::clamp(value, min, max); },
			[](double value, double min, double max) {return std::clamp(value, min, max); },
			[](int value, int min, int max) {return std::clamp(value, min, max); }
		));

		math.set_function("createQuaternionFromYawPitchRoll", sol::overload(
			[](float yaw, float pitch, float roll) { return Quaternion::CreateFromYawPitchRoll(yaw, pitch, roll); },
			[](const Vector3& angles) { return Quaternion::CreateFromYawPitchRoll(angles); }
		));

		math["radToDeg"] = Math::RadToDeg;
		math["degToRad"] = Math::DegToRad;
	}

	void MathLuaBindings::CreateLuaBindings(sol::state& lua)
	{
		CreateVector2Bind(lua);
		CreateVector3Bind(lua);
		CreateVector4Bind(lua);
		
		CreateQuaternionBind(lua);

		MathFreeFunctions(lua);
	}
}
