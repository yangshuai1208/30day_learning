#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

/* Compile this test file alone; it includes and exercises production code. */
#include "servo_step.c"

static void assert_servo_state(const ServoMotion *servo,
                               uint8_t current_angle,
                               uint8_t target_angle,
                               uint32_t last_tick)
{
    assert(servo->current_angle == current_angle);
    assert(servo->target_angle == target_angle);
    assert(servo->last_tick == last_tick);
}

static void test_null_pointer(void)
{
    assert(servo_step(NULL, 100U, 10U) == false);
}

static void test_zero_interval(void)
{
    ServoMotion servo = { 20U, 25U, 100U };

    assert(servo_step(&servo, 200U, 0U) == false);
    assert_servo_state(&servo, 20U, 25U, 100U);
}

static void test_target_already_reached(void)
{
    ServoMotion servo = { 45U, 45U, 100U };

    assert(servo_step(&servo, 200U, 10U) == false);
    assert_servo_state(&servo, 45U, 45U, 100U);
}

static void test_interval_one_millisecond_short(void)
{
    ServoMotion servo = { 20U, 22U, 100U };

    assert(servo_step(&servo, 109U, 10U) == false);
    assert_servo_state(&servo, 20U, 22U, 100U);
}

static void test_interval_exact_and_increments(void)
{
    ServoMotion servo = { 20U, 22U, 100U };

    assert(servo_step(&servo, 110U, 10U) == true);
    assert_servo_state(&servo, 21U, 22U, 110U);
}

static void test_decrements_toward_target(void)
{
    ServoMotion servo = { 22U, 20U, 50U };

    assert(servo_step(&servo, 60U, 10U) == true);
    assert_servo_state(&servo, 21U, 20U, 60U);
}

static void test_tick_wraparound(void)
{
    ServoMotion servo = { 30U, 31U, UINT32_MAX - 4U };

    /* Unsigned elapsed time is 8 ms across wrap: one short, then exact. */
    assert(servo_step(&servo, 2U, 8U) == false);
    assert_servo_state(&servo, 30U, 31U, UINT32_MAX - 4U);

    assert(servo_step(&servo, 3U, 8U) == true);
    assert_servo_state(&servo, 31U, 31U, 3U);
}

static void test_consecutive_calls_and_last_tick(void)
{
    ServoMotion servo = { 10U, 12U, 100U };

    assert(servo_step(&servo, 109U, 10U) == false);
    assert_servo_state(&servo, 10U, 12U, 100U);

    assert(servo_step(&servo, 110U, 10U) == true);
    assert_servo_state(&servo, 11U, 12U, 110U);

    assert(servo_step(&servo, 119U, 10U) == false);
    assert_servo_state(&servo, 11U, 12U, 110U);

    assert(servo_step(&servo, 120U, 10U) == true);
    assert_servo_state(&servo, 12U, 12U, 120U);

    assert(servo_step(&servo, 130U, 10U) == false);
    assert_servo_state(&servo, 12U, 12U, 120U);
}

int main(void)
{
    test_null_pointer();
    test_zero_interval();
    test_target_already_reached();
    test_interval_one_millisecond_short();
    test_interval_exact_and_increments();
    test_decrements_toward_target();
    test_tick_wraparound();
    test_consecutive_calls_and_last_tick();

    return 0;
}
