#pragma once

#include <chrono>

class Timer
{
public:
	Timer();

	void OnUpdate();

	const float GetFrameRate() const;
	const double GetTimeElapsed() const;

private:
	std::chrono::steady_clock::time_point time;
	std::chrono::nanoseconds deltaTime;
	std::chrono::high_resolution_clock clock;

	double timeElapsed;
};