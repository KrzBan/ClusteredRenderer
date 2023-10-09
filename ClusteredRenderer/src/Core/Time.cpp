#include "Time.hpp"

struct TimeData {
	Timestep lastTime = 0.0;
	Timestep deltaTime = 0.0;
	Timestep currentTime = 0.0;

	uint32_t avgCount = 1;

	uint32_t currentDeltaIndex = 0;
	std::vector<Timestep> deltaSamples = std::vector<Timestep>(avgCount);
};

static TimeData s_TimeData{};

Timestep Time::DeltaTime() {
	return s_TimeData.deltaTime;
}

Timestep Time::RealTime() {
	return s_TimeData.currentTime;
}

void Time::UpdateTime(Timestep time) {
	s_TimeData.currentTime = time;

	s_TimeData.deltaTime = s_TimeData.currentTime - s_TimeData.lastTime;
	s_TimeData.lastTime = s_TimeData.currentTime;

	if (s_TimeData.currentDeltaIndex == s_TimeData.avgCount) s_TimeData.currentDeltaIndex = 0;
	s_TimeData.deltaSamples[s_TimeData.currentDeltaIndex++] = s_TimeData.deltaTime;
}

uint32 Time::FramesPerSecond() {
	return static_cast<uint32>(Timestep{ 1.0 } / s_TimeData.deltaTime);
}

Timestep Time::FrameTimeAvg() {
	return std::accumulate(s_TimeData.deltaSamples.begin(), s_TimeData.deltaSamples.end(), 0.0) / s_TimeData.avgCount;
}