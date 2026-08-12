#ifndef LEV_PARTICLES_RANDOM_HLSL
#define LEV_PARTICLES_RANDOM_HLSL

#define RANDOM_IA 16807
#define RANDOM_IM 2147483647
#define RANDOM_AM (1.0f/float(RANDOM_IM))
#define RANDOM_IQ 127773
#define RANDOM_IR 2836
#define RANDOM_MASK 123459876

// Constant-time thread-unique seed. Cycling the generator N times to get there is O(N) per
// thread, which turns a dispatch of N threads into O(N^2).
uint WangHash(uint seed)
{
    seed = (seed ^ 61u) ^ (seed >> 16u);
    seed *= 9u;
    seed = seed ^ (seed >> 4u);
    seed *= 0x27d4eb2du;
    seed = seed ^ (seed >> 15u);
    return seed;
}

struct NumberGenerator {
    int seed; // Used to generate values.

    // Returns the current random float.
    float GetCurrentFloat() {
        Cycle();
        return RANDOM_AM * seed;
    }

    // Returns the current random int.
    int GetCurrentInt() {
        Cycle();
        return seed;
    }

    // Generates the next number in the sequence.
    void Cycle() {
        seed ^= RANDOM_MASK;
        // RANDOM_IQ has to stay a signed literal: an unsigned one promotes this division to
        // uint and Park-Miller breaks for every negative seed.
        int k = seed / RANDOM_IQ;
        seed = RANDOM_IA * (seed - k * RANDOM_IQ) - RANDOM_IR * k;

        if (seed < 0)
            seed += RANDOM_IM;

        seed ^= RANDOM_MASK;
    }

    // Returns a random float within the input range.
    float GetRandomFloat(const float low, const float high) {
        float v = GetCurrentFloat();
        return low * (1.0f - v) + high * v;
    }

    // Sets the seed
    void SetSeed(const uint value) {
        seed = int(WangHash(value));
        Cycle();
    }
};

#endif
