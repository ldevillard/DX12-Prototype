#include "PrecompiledHeaders.h"
#include "helpers/Time.h"

std::chrono::steady_clock::time_point Time::time;
std::chrono::nanoseconds Time::deltaTime;
std::chrono::high_resolution_clock Time::clock;
double Time::timeElapsed;

#pragma region Public Methods

void Time::OnInit()
{
	timeElapsed = 0;
}

void Time::OnUpdate()
{
	std::chrono::steady_clock::time_point currentTime = clock.now();
	deltaTime = currentTime - time;
	time = currentTime;

	timeElapsed += deltaTime.count() * 1e-9;
}

const float Time::GetFrameRate()
{
	return 1.0f / (deltaTime.count() * 1e-9f); // delta time is in nano seconds
}

const double Time::GetTimeElapsed()
{
	return timeElapsed;
}

const float Time::GetDeltaTime()
{
	return deltaTime.count() * 1e-9f;
}

#pragma endregion