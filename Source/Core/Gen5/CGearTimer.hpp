#ifndef CGEARTIMER_HPP
#define CGEARTIMER_HPP

#include <Core/Util/Global.hpp>

enum TimerState : u8
{
    First,
    Long,
    Skip,
    Short
};

class CGearTimer
{
public:
    CGearTimer();
    u32 getTime(u64 seed);

private:
    u32 currentTime;
    TimerState timerState;
};

#endif // CGEARTIMER_HPP
