#ifndef EVENT_H
#define EVENT_H

#include "core/typedefs.h"
#include "core/error/error_macros.h"
#include "core/data/vector.h"

/**
 * @brief The basic event class. This class is designed so that when a given situation occurs, several callback methods can happen at once, allowing for a transfer of 
 * communication between two parts of the application that may not otherwise be able to contact one another. Uses a variatic argument template in order for events to 
 * have any given amount of arguments to oneself.
 */
template <typename... Args>
class Event {
public:

    // A typedef that exists for each individual class, which is set when the class is constructed.
    typedef void (*listener)(Args...);
private:

    // A vector of function pointers that are called when the event activates.
    Vector<listener> listeners;
public:

    FORCE_INLINE void connect(Event::listener list, bool priority);
    FORCE_INLINE void fire(Args... p_args);
    FORCE_INLINE Error disconnect(Event::listener list);

    Event() {}
    ~Event() {};
};

/**
 * @brief Gives the address of a function to an event for it to call at a later point.
 * @param list The function pointer to add into the vector
 * @param priority Some functions may need to be called first, and as a result a function with priority is pushed to the front of the queue ahead of other functions.
 */
template <typename... Args>
void Event<Args...>::connect(Event::listener list, bool priority) {
    if (priority) {
        listeners.push_front(list);
    } else {
        listeners.push_back(list);
    }
}

/**
 * @brief Sets of every function pointer the vector has obtained with the given arguments.
 * @param p_args The arguments to send off in the event. The number of arguments sent here must be the same as that on creation of the event
 */
template <typename... Args>
void Event<Args...>::fire(Args... p_args) {
    uint64 list_size = listeners.size();
    if (!list_size) {
        return;
    }
    for (int i = 0; i < list_size; i++) {
        listeners[i](p_args...);
    }
}

/**
 * @brief Disconnects a given listener from the vector.
 * @param list The listener to remove from the event
 * @returns `OK` if it was able to find and remove the listener, and `ERR_CANT_FIND` if the listener was not found in the vector.
 */
template <typename... Args>
Error Event<Args...>::disconnect(Event::listener list) {
    int loc = listeners.find(list);
    if (loc == -1) {
        return ERR_CANT_FIND;
    }
    listeners.remove_at(loc);
    return OK;
}


#endif // EVENT_H