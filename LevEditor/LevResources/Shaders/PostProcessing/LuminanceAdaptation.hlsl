#include "../Quad.hlsl"
#include "PostProcessing.hlsl"

Texture2D<float> lastLuminance : register(t0);
Texture2D<float> currentLuminance : register(t1);

float PSMain(PS_IN input) : SV_Target
{
	float TimeDelta = constants.TimeDelta;
	float Tau = constants.Tau;

	//LuminancePass writes log(luminance) and generates the full mip chain, so the smallest
	//mip is the geometric mean over the frame. Its index follows RenderSettings::LuminanceMapSize
	//and must not be hardcoded.
	uint width, height, mipLevels;
	currentLuminance.GetDimensions(0, width, height, mipLevels);

	float lastLum = lastLuminance.Load(uint3(0, 0, 0));
	float currentLum = currentLuminance.Load(uint3(0, 0, mipLevels - 1));
	currentLum = exp(currentLum);

	// Adapt the luminance using Pattanaik's technique
	float adaptedLum = lastLum + (currentLum - lastLum) * (1 - exp(-TimeDelta * Tau));

	return adaptedLum;
}