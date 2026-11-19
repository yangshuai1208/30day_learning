
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef enum
{
    ACK_IN_PROGRESS = 0,
    ACK_OK,
    ACK_ERROR,
    ACK_PREEMPTED
} AckStatus;

typedef struct
{
    uint32_t seq;
    AckStatus status;
    bool valid;
} AckRecord;

/*
 * 更新ACK记录。
 *
 * 返回true：允许本次操作。
 * 返回false：非法或冲突的状态变化。
 *
 * 返回false时不得修改原记录。
 */

bool ack_record_update(
    AckRecord *record,
    uint32_t seq,
    AckStatus new_status)
{
    // 1. 参数合法性检查
    if (record == NULL ||
        new_status < ACK_IN_PROGRESS ||
        new_status > ACK_PREEMPTED)
    {
        return false;
    }

    // 2. 首次创建记录
    if (!record->valid)
    {
        if (new_status != ACK_IN_PROGRESS)
        {
            return false;
        }

        record->seq = seq;
        record->status = ACK_IN_PROGRESS;
        record->valid = true;
        return true;
    }

    // 3. 序列号检查
    if (record->seq != seq)
    {
        return false;
    }

    // 4. 当前状态合法性检查
    if (record->status < ACK_IN_PROGRESS ||
        record->status > ACK_PREEMPTED)
    {
        return false;
    }

    // 5. 同状态重复更新：幂等
    if (record->status == new_status)
    {
        return true;
    }

    // 6. 终态不能转换
    if (record->status != ACK_IN_PROGRESS)
    {
        return false;
    }

    // 7. IN_PROGRESS转换为终态
    record->status = new_status;
    return true;
}
