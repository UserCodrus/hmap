#pragma once

#include <vector>

// Linear interpolation
inline float lerp(float t, float a, float b);
// Perlin fade
inline float fade(float t);
// Perlin gradient
inline float grad(int hash, float x, float y);

// Generate a set of permutations for hashed perlin noise gradients
void permutation(std::vector<int>& p, unsigned size, unsigned seed);

// Perlin noise function with hashed gradients
float perlin(const std::vector<int>& p, float x, float y);