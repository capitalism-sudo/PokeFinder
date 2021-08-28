#include "CGearTimer.hpp"

CGearTimer::CGearTimer() : currentTime(0), timerState(TimerState::First)
{
}

u32 CGearTimer::getTime(u64 seed)
{
    switch (timerState)
    {
    case TimerState::First:
    {
        timerState = TimerState::Skip;
        currentTime += 21;
        return currentTime;
    }
    case TimerState::Long:
    {
        timerState = TimerState::Short;
        currentTime = currentTime + ((seed * 152) >> 32) + 60;
        return currentTime;
    }
    case TimerState::Short:
    {
        timerState = TimerState::Skip;
        currentTime = currentTime + ((seed * 40) >> 32) + 60;
        return currentTime;
    }
    case TimerState::Skip:
    {
        timerState = TimerState::Long;
        return 0;
    }
    }

    return 0;
}
