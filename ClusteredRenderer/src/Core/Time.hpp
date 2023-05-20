#pragma once

#include <Core.hpp>

struct Time {
	static double DeltaTime();
	static double RealTime();

	static float DeltaTimeF();
	static float RealTimeF();

	static uint32_t FramesPerSecond();
	static double FrameTimeAvg();

	static void UpdateTime(double time);
};