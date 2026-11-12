#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct
{
    uint32_t last_tick;
    uint32_t interval_ms;
    bool enabled;
} ServoScheduler;

bool servo_scheduler_should_run(
    const ServoScheduler *sched,
    uint32_t now_tick)
{
    if (sched == NULL || !sched->enabled || sched->interval_ms == 0)
    {
        return false;
    }

    return (uint32_t)(now_tick - sched->last_tick) >= sched->interval_ms;
}




void servo_scheduler_commit(
    ServoScheduler *sched,
    uint32_t completion_tick,
    bool write_ok)
{
    if (sched == NULL || !write_ok)
    {
        return;
    }

    sched->last_tick = completion_tick;
}