
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "servo_scheduler.c"

int main(void)
{
    ServoScheduler sched = {100U, 20U, true};

    /* 参数检查 */
    assert(!servo_scheduler_should_run(NULL, 120U));
    assert(!servo_scheduler_should_run(&sched, 119U));
    assert(servo_scheduler_should_run(&sched, 120U));

    /* 写入失败不更新时间 */
    servo_scheduler_commit(&sched, 130U, false);
    assert(sched.last_tick == 100U);

    /* 写入成功，按完成时间更新 */
    servo_scheduler_commit(&sched, 130U, true);
    assert(sched.last_tick == 130U);
    assert(!servo_scheduler_should_run(&sched, 149U));
    assert(servo_scheduler_should_run(&sched, 150U));

    /* 禁用及零间隔 */
    sched.enabled = false;
    assert(!servo_scheduler_should_run(&sched, 150U));
    sched.enabled = true;
    sched.interval_ms = 0U;
    assert(!servo_scheduler_should_run(&sched, 150U));

    /* 测试Tick回绕 */
    sched.interval_ms = 20U;
    sched.last_tick = UINT32_MAX - 5U;
    assert(!servo_scheduler_should_run(&sched, 13U));
    assert(servo_scheduler_should_run(&sched, 14U));

    /* 空指针提交 */
    servo_scheduler_commit(NULL, 200U, true);

    puts("SERVO_SCHEDULER_TEST_PASS");
    return 0;
}
