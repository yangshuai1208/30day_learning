
#include <assert.h>
#include <stdio.h>

#include "ack_state.c"

static void assert_record_unchanged(const AckRecord *record, AckRecord before)
{
    assert(record->seq == before.seq);
    assert(record->status == before.status);
    assert(record->valid == before.valid);
}

static void test_terminal_is_irreversible(AckStatus terminal)
{
    AckRecord record = {0};
    const uint32_t seq = 41U;

    assert(ack_record_update(&record, seq, ACK_IN_PROGRESS));
    assert(ack_record_update(&record, seq, terminal));
    assert(ack_record_update(&record, seq, terminal));

    for (int status = ACK_IN_PROGRESS; status <= ACK_PREEMPTED; status++)
    {
        if ((AckStatus)status == terminal)
        {
            continue;
        }

        AckRecord before = record;
        assert(!ack_record_update(&record, seq, (AckStatus)status));
        assert_record_unchanged(&record, before);
    }

    assert(record.seq == seq);
    assert(record.status == terminal);
    assert(record.valid);
}

static void test_different_seq_cannot_overwrite(void)
{
    AckRecord record = {0};

    assert(ack_record_update(&record, 51U, ACK_IN_PROGRESS));
    AckRecord before = record;
    assert(!ack_record_update(&record, 52U, ACK_IN_PROGRESS));
    assert_record_unchanged(&record, before);
    assert(!ack_record_update(&record, 52U, ACK_OK));
    assert_record_unchanged(&record, before);

    assert(ack_record_update(&record, 51U, ACK_OK));
    before = record;
    assert(!ack_record_update(&record, 52U, ACK_OK));
    assert_record_unchanged(&record, before);
}

static void test_invalid_status_never_changes_record(void)
{
    AckRecord record = {0};
    AckRecord before = record;

    assert(!ack_record_update(&record, 61U, (AckStatus)-1));
    assert_record_unchanged(&record, before);
    assert(!ack_record_update(&record, 61U, (AckStatus)99));
    assert_record_unchanged(&record, before);
    assert(!ack_record_update(&record, 61U, ACK_ERROR));
    assert_record_unchanged(&record, before);

    assert(ack_record_update(&record, 61U, ACK_IN_PROGRESS));
    before = record;
    assert(!ack_record_update(&record, 61U, (AckStatus)-1));
    assert_record_unchanged(&record, before);
    assert(!ack_record_update(&record, 61U, (AckStatus)99));
    assert_record_unchanged(&record, before);

    record.status = (AckStatus)99;
    before = record;
    assert(!ack_record_update(&record, 61U, ACK_OK));
    assert_record_unchanged(&record, before);
}

int main(void)
{
    AckRecord r = {0};

    assert(!ack_record_update(NULL, 1, ACK_IN_PROGRESS));
    assert(!ack_record_update(&r, 1, ACK_OK));
    assert(!r.valid);

    assert(ack_record_update(&r, 1, ACK_IN_PROGRESS));
    assert(ack_record_update(&r, 1, ACK_IN_PROGRESS));
    assert(!ack_record_update(&r, 2, ACK_OK));

    assert(ack_record_update(&r, 1, ACK_PREEMPTED));
    assert(ack_record_update(&r, 1, ACK_PREEMPTED));
    assert(!ack_record_update(&r, 1, ACK_OK));
    assert(!ack_record_update(&r, 1, ACK_ERROR));
    assert(!ack_record_update(&r, 1, ACK_IN_PROGRESS));

    assert(r.seq == 1);
    assert(r.status == ACK_PREEMPTED);
    assert(!ack_record_update(&r, 1, (AckStatus)99));

    test_terminal_is_irreversible(ACK_OK);
    test_terminal_is_irreversible(ACK_ERROR);
    test_terminal_is_irreversible(ACK_PREEMPTED);
    test_different_seq_cannot_overwrite();
    test_invalid_status_never_changes_record();

    puts("DAY09_ACK_TEST_PASS");
    return 0;
}
