#include "PrecompiledHeaders.h"
#include "helpers/Timer.h"

#pragma region Public Methods

Timer::Timer()
	: deltaTime(0)
{
	std::chrono::steady_clock::time_point time = clock.now();
}

const float Timer::GetFrameRate() const
{
	return 1.0f / (deltaTime.count() * 1e-9f); // delta time is in nano seconds
}

void Timer::OnUpdate()
{
	std::chrono::steady_clock::time_point currentTime = clock.now();
	deltaTime = currentTime - time;
	time = currentTime;
}

#pragma endregion