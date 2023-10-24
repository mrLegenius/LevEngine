#include "../Quad.hlsl"
#include "PostProcessing.hlsl"

Texture2D<float> lastLuminance : register(t0);
Texture2D<float> currentLuminance : register(t1);

float PSMain(PS_IN input) : SV_Target
{
	float TimeDelta = constants.TimeDelta;
	float Tau = constants.Tau;

	float lastLum = lastLuminance.Load(uint3(0, 0, 0));
	float currentLum = currentLuminance.Load(uint3(0, 0, 10));
	currentLum = exp(currentLum);

	// Adapt the luminance using Pattanaik's technique
	float adaptedLum = lastLum + (currentLum - lastLum) * (1 - exp(-TimeDelta * Tau));

	return adaptedLum;
}