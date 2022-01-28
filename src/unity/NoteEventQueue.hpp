#include <queue>
#include <array>

enum EventType {
    NoteStart,
    NoteRelease
};

struct NoteEvent {
    EventType event_type;
    double wait_for_beat;
    int instance;
    int note;
    int velocity;

    NoteEvent(EventType event_type, double wait_for_beat, int instance, int note, int velocity) :
        event_type (event_type),
        wait_for_beat (wait_for_beat),
        instance (instance),
        note (note),
        velocity (velocity)
    {
    }
};

// This structure suggested from https://www.geeksforgeeks.org/stl-priority-queue-for-structure-or-class/
struct PrioritizeEarlierTime {
    bool operator()(NoteEvent const& e1, NoteEvent const& e2) {
        // Larger (later) time goes to end of queue.
        return e1.wait_for_beat > e2.wait_for_beat ||
        // ...or if the two are the same, one that's starting goes to the end
        (e1.wait_for_beat == e2.wait_for_beat && e1.event_type == NoteStart);
        // This helps make sure that if one key has two notes perfectly
        // consecutively, the release of the first is processed before the start
        // of the next. Note that this means a note where start==end should
        // never be scheduled.
    }
};