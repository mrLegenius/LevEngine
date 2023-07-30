#pragma once

namespace LevEngine
{
struct Color
{
	float R = 0, G = 0, B = 0, A = 0;

    Color() = default;

    Color(const float r, const float g, const float b, const float a = 1)
        : R(r)
        , G(g)
        , B(b)
		, A(a) { }

    explicit Color(const uint32_t hex)
        : R(((hex >> 24) & 0xff) / 255.0f)
        , G(((hex >> 16) & 0xff) / 255.0f)
        , B(((hex >> 8) & 0xff) / 255.0f)
        , A(((hex >> 0) & 0xff) / 255.0f)
	{ }

    float* Raw() { return &R; }

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
