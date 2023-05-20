#include "Time.hpp"

struct TimeData {
	double lastTime = 0.0;
	double deltaTime = 0.0;
	double currentTime = 0.0;

	uint32_t avgCount = 1;

	uint32_t currentDeltaIndex = 0;
	std::vector<double> deltaSamples = std::vector<double>(avgCount);
};

static TimeData s_TimeData{};

double Time::DeltaTime() {
	return s_TimeData.deltaTime;
}

double Time::RealTime() {
	return s_TimeData.currentTime;
}

float Time::DeltaTimeF() {
	return static_cast<float>(s_TimeData.deltaTime);
}

float Time::RealTimeF() {
	return static_cast<float>(s_TimeData.currentTime);
}

void Time::UpdateTime(double time) {
	s_TimeData.currentTime = time;

	s_TimeData.deltaTime = s_TimeData.currentTime - s_TimeData.lastTime;
	s_TimeData.lastTime = s_TimeData.currentTime;

	if (s_TimeData.currentDeltaIndex == s_TimeData.avgCount) s_TimeData.currentDeltaIndex = 0;
	s_TimeData.deltaSamples[s_TimeData.currentDeltaIndex++] = s_TimeData.deltaTime;
}

uint32_t Time::FramesPerSecond() {
	return static_cast<int>(1.0 / s_TimeData.deltaTime);
}

double Time::FrameTimeAvg() {
	return std::accumulate(s_TimeData.deltaSamples.begin(), s_TimeData.deltaSamples.end(), 0.0) / s_TimeData.avgCount;
}