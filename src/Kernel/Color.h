#pragma once

namespace LevEngine
{
struct Color
{
	float r = 0, g = 0, b = 0, a = 0;

    Color() = default;

    Color(const float r, const float g, const float b, const float a = 1)
        : r(r)
        , g(g)
        , b(b)
		, a(a) { }

    explicit Color(const uint32_t hex)
        : r(((hex >> 24) & 0xff) / 255.0f)
        , g(((hex >> 16) & 0xff) / 255.0f)
        , b(((hex >> 8) & 0xff) / 255.0f)
        , a(((hex >> 0) & 0xff) / 255.0f)
	{ }

    float* Raw() { return &r; }

    static const Color White;
    static const Color Silver;
    static const Color Gray;
    static const Color Black;
    static const Color Red;
    static const Color Maroon;
    static const Color Yellow;
    static const Color Olive;
    static const Color Lime;
    static const Color Green;
    static const Color Aqua;
    static const Color Teal;
    static const Color Blue;
    static const Color Navy;
    static const Color Pink;
    static const Color Purple;
};
}
