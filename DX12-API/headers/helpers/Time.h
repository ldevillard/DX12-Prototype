#pragma once

#include <chrono>

class Time
{
public:
	static void OnInit();
	static void OnUpdate();

	static const float GetFrameRate();
	static const double GetTimeElapsed();
	static const float GetDeltaTime();

private:
	static std::chrono::steady_clock::time_point time;
	static std::chrono::nanoseconds deltaTime;
	static std::chrono::high_resolution_clock clock;

	static double timeElapsed;
};