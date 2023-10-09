#pragma once

#include <Core.hpp>

using Timestep = float;

struct Time {
	static Timestep DeltaTime();
	static Timestep RealTime();

	static uint32 FramesPerSecond();
	static Timestep FrameTimeAvg();

	static void UpdateTime(Timestep time);
};