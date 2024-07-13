#pragma once

#include <chrono>

class Timer
{
public:
	Timer();

	const float GetFrameRate() const;
	void OnUpdate();

private:
	std::chrono::steady_clock::time_point time;
	std::chrono::nanoseconds deltaTime;
	std::chrono::high_resolution_clock clock;
};