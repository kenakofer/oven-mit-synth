#include <queue>
#include <array>

enum EventType {
    NoteStart,
    NoteRelease
};

struct NoteEvent {
    EventType event_type;
    long wait_for_sample;
    int instance;
    int note;
    int velocity;

    NoteEvent(EventType event_type, long wait_for_sample, int instance, int note, int velocity) :
        event_type (event_type),
        wait_for_sample (wait_for_sample),
        instance (instance),
        note (note),
        velocity (velocity)
    {
    }
};

// This written with help from https://www.geeksforgeeks.org/stl-priority-queue-for-structure-or-class/
struct PrioritizeEarlierTime {
    bool operator()(NoteEvent const& e1, NoteEvent const& e2) {
        // Larger (later) time goes to end of queue.
        return e1.wait_for_sample > e2.wait_for_sample;
    }
};