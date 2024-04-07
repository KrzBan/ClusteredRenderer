#pragma once

#include <Core.hpp>

static std::random_device randomDevice;
static std::mt19937 randomEngine(randomDevice());
static std::uniform_real_distribution<float> distribution(0.0f, 1.0f);

inline float GetRandomFloat() {
	return distribution(randomEngine);
}
