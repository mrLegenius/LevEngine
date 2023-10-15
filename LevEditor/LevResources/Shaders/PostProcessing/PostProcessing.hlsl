struct Constants
{
    float BloomThreshold;
    float BloomMagnitude;
    float BloomBlurSigma;
    float Tau;
    float TimeDelta;
    float KeyValue;
};

cbuffer ScreenToViewParams : register(b8)
{
    Constants constants;
}