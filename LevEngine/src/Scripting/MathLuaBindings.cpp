#include "levpch.h"
#include "MathLuaBindings.h"
#include "Math/Math.h"
#include "Math/Random.h"


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

		auto vec3_subtraction_overload = sol::overload(
			[](const Vector3& v1, const Vector3& v2) {
				return v1 - v2;
			}
		);

		lua.new_usertype<Vector3>(
			"Vector3",
			sol::call_constructor,
			sol::constructors<Vector3(), Vector3(float), Vector3(float, float, float)>(),
			"x", &Vector3::x,
			"y", &Vector3::y,
			"z", &Vector3::z,
			sol::meta_function::addition, vec3_addition_overload,
			sol::meta_function::subtraction, vec3_subtraction_overload,
			sol::meta_function::multiplication, vec3_multiplication_overload,
			sol::meta_function::division, vec3_division_overload,
			"normalize", [] (Vector3& vector) { vector.Normalize(); },
			"length", &Vector3::Length,
			"lengthSquared", &Vector3::LengthSquared,
			sol::meta_function::to_string, [] (const Vector3& vector)
			{
				return "Vector3 [" + std::to_string(vector.x) + ", "
				+ std::to_string(vector.y) + ", "
				+ std::to_string(vector.z) + "]";
			},
			"Zero", sol::var(Vector3::Zero),
			"Up", sol::var(Vector3::Up),
			"Right", sol::var(Vector3::Right)
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

		auto vec2_subtraction_overload = sol::overload(
			[](const Vector2& v1, const Vector2& v2) {
				return v1 - v2;
			}
		);

		lua.new_usertype<Vector2>(
			"Vector2",
			sol::call_constructor,
			sol::constructors<Vector2(), Vector2(float), Vector2(float, float)>(),
			"x", &Vector2::x,
			"y", &Vector2::y,
			sol::meta_function::addition, vec2_addition_overload,
			sol::meta_function::subtraction, vec2_subtraction_overload,
			sol::meta_function::multiplication, vec2_multiplication_overload,
			sol::meta_function::division, vec2_division_overload,
			sol::meta_function::to_string, [] (const Vector2& vector)
			{
				return "Vector2 [" + std::to_string(vector.x) + ", "
				+ std::to_string(vector.y) + "]";
			},
			"normalize", [] (Vector2& vector) { vector.Normalize(); },
			"length", &Vector2::Length,
			"lengthSquared", &Vector2::LengthSquared
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

		auto vec4_subtraction_overload = sol::overload(
			[](const Vector4& v1, const Vector4& v2) {
				return v1 - v2;
			}
		);

		lua.new_usertype<Vector4>(
			"Vector4",
			sol::call_constructor,
			sol::constructors<Vector4(), Vector4(float), Vector4(float, float, float, float)>(),
			"x", &Vector4::x,
			"y", &Vector4::y,
			"z", &Vector4::z,
			"w", &Vector4::w,
			sol::meta_function::addition, vec4_addition_overload,
			sol::meta_function::subtraction, vec4_subtraction_overload,
			sol::meta_function::multiplication, vec4_multiplication_overload,
			sol::meta_function::division, vec4_division_overload,
			sol::meta_function::to_string, [] (const Vector4& vector)
			{
				return "Vector4 [" + std::to_string(vector.x) + ", "
				+ std::to_string(vector.y) + ", "
				+ std::to_string(vector.z) + ", "
				+ std::to_string(vector.w) + "]";
			},
			"normalize", [] (Vector4& vector) { vector.Normalize(); },
			"length", &Vector4::Length,
			"lengthSquared", &Vector4::LengthSquared
		);
	}

	void CreateColorBind(sol::state& lua)
	{
		lua.new_usertype<Color>(
			"Color",
			sol::call_constructor,
			sol::constructors<Color(), Color(float, float, float, float), Color(uint32_t), Color(const Vector3&), Color(const Vector4&)>(),
			"r", &Color::r,
			"g", &Color::g,
			"b", &Color::b,
			"a", &Color::a,

			sol::meta_function::to_string, [] (const Color& color)
			{
				return "Color [" + std::to_string(color.r) + ", "
				+ std::to_string(color.g) + ", "
				+ std::to_string(color.b) + ", "
				+ std::to_string(color.a) + "]";
			},
			"toVector4", [](const Color& color)
			{
				return static_cast<Vector4>(color);
			},
			"toVector3", [](const Color& color)
			{
				return static_cast<Vector3>(color);
			},
			"Clear", sol::var(&Color::Clear),
			"White", sol::var(&Color::White),
			"Silver", sol::var(&Color::Silver),
			"Gray", sol::var(&Color::Gray),
			"Black", sol::var(&Color::Black),
			"Red", sol::var(&Color::Red),
			"Maroon", sol::var(&Color::Maroon),
			"Yellow", sol::var(&Color::Yellow),
			"Olive", sol::var(&Color::Olive),
			"Lime", sol::var(&Color::Lime),
			"Green", sol::var(&Color::Green),
			"Aqua", sol::var(&Color::Aqua),
			"Teal", sol::var(&Color::Teal),
			"Blue", sol::var(&Color::Blue),
			"Navy", sol::var(&Color::Navy),
			"Pink", sol::var(&Color::Pink),
			"Purple", sol::var(&Color::Purple)
		);
	}

	void CreateQuaternionBind(sol::state& lua) 
	{
		auto quaternion_multiplication_overload = sol::overload(
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

		auto quaternion_division_overload = sol::overload(
			[](const Quaternion& v1, const Quaternion& v2) {
				return v1 / v2;
			}
		);

		auto quaternion_addition_overload = sol::overload(
			[](const Quaternion& v1, const Quaternion& v2) {
				return v1 + v2;
			}
		);

		auto quaternion_subtraction_overload = sol::overload(
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
			sol::meta_function::addition, quaternion_addition_overload,
			sol::meta_function::subtraction, quaternion_subtraction_overload,
			sol::meta_function::multiplication, quaternion_multiplication_overload,
			sol::meta_function::division, quaternion_division_overload,
			sol::meta_function::to_string, [] (const Quaternion& quaternion)
			{
				const auto vector = quaternion.ToEuler();
				return "Euler Angles [" + std::to_string(vector.x) + ", "
				+ std::to_string(vector.y) + ", "
				+ std::to_string(vector.z) + "]";
			},
			"toEuler", [](const Quaternion& quaternion)
			{
				return quaternion.ToEuler();
			},
			"lookRotation", [](const Vector3& forward, const Vector3& up)
			{
				return Quaternion::LookRotation(forward, up);
			},
			"identity", sol::var(Quaternion::Identity)
		);
	}

	void MathFreeFunctions(sol::state& lua) {
		auto math = lua["Math"].get_or_create<sol::table>();
		math.set_function("distance", sol::overload(
			sol::resolve<float(const Vector2&, const Vector2&)>(&Vector2::Distance),
			sol::resolve<float(const Vector3&, const Vector3&)>(&Vector3::Distance),
			sol::resolve<float(const Vector4&, const Vector4&)>(&Vector4::Distance)
		));

		math.set_function("lerp", [](float a, float b, float t) {return std::lerp(a, b, t); });
		math.set_function("sqrt", [](float a) { return std::sqrtf(a); });
		math.set_function("pow", [](float a, float power) { return std::powf(a, power); });
		math.set_function("exp", [](float power) { return std::expf(power); });
		math.set_function("clamp", sol::overload(
			[](float value, float min, float max) {return std::clamp(value, min, max); },
			[](double value, double min, double max) {return std::clamp(value, min, max); },
			[](int value, int min, int max) {return std::clamp(value, min, max); }
		));

		math.set_function("createQuaternionFromYawPitchRoll", sol::overload(
			sol::resolve<Quaternion(float, float, float)>(&Quaternion::CreateFromYawPitchRoll),
			sol::resolve<Quaternion(const Vector3&)>(&Quaternion::CreateFromYawPitchRoll)
		));

		math.set_function("createQuaternionFromAxisAngle", sol::overload(
			sol::resolve<Quaternion(const Vector3&, float)>(&Quaternion::CreateFromAxisAngle)
		));

		math["radToDeg"] = Math::RadToDeg;
		math["degToRad"] = Math::DegToRad;
	}

	void CreateRandomBind(sol::state& lua)
	{
		lua.new_usertype<Random>(
			"Random",
			sol::no_constructor,
			"bool", &Random::Bool,
			"int", &Random::Int, //[min, max)
			"float", sol::overload(
				sol::resolve<float(float, float)>(&Random::Float), // [min, max]
				sol::resolve<float()>(&Random::Float)), //[0, 1]
			"vector3", sol::overload(
				sol::resolve<Vector3()>(&Random::Vec3),
				sol::resolve<Vector3(float, float)>(&Random::Vec3),
				sol::resolve<Vector3(const Vector3, const Vector3)>(&Random::Vec3)),
			"vector4", sol::overload(
				sol::resolve<Vector4()>(&Random::Vec4),
				sol::resolve<Vector4(float, float)>(&Random::Vec4),
				sol::resolve<Vector4(const Vector4, const Vector4)>(&Random::Vec4)),
			"rotation", &Random::Rotation,
			"color", sol::overload(
				sol::resolve<Color(float, float, float)>(&Random::Color),
				sol::resolve<Color(const Color&, const Color&)>(&Random::Color)),
			"colorSmooth", &Random::ColorSmooth, //Random lerp. ColorSmooth(Color, Color)
			"colorGrayScale", &Random::ColorSmoothGrayscale //ColorSmoothGrayscale(Color, Color, alpha)
		);
	}

	void MathLuaBindings::CreateLuaBindings(sol::state& lua)
	{
		CreateVector2Bind(lua);
		CreateVector3Bind(lua);
		CreateVector4Bind(lua);
		
		CreateQuaternionBind(lua);

		CreateColorBind(lua);

		CreateRandomBind(lua);

		MathFreeFunctions(lua);
	}
}

