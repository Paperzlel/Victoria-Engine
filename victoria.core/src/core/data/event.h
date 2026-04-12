#pragma once

#include "list.h"

#include "core/config/callable_method_pointer.h" // IWYU pragma: keep
#include "core/error/error_macros.h"
#include "core/typedefs.h"

/**
 * @brief The basic event class. This class is designed so that when a given situation occurs, several callback methods
 * can happen at once, allowing for a transfer of communication between two parts of the application that may not
 * otherwise be able to contact one another. Uses a variatic argument template in order for events to have any given
 * amount of arguments to oneself.
 */
template <typename... Args>
class Event {
private:
	// A vector of function pointers that are called when the event activates.
	List<CallableMethod> listeners;

public:
	FORCE_INLINE void connect(const CallableMethod &p_method, bool priority = false);
	FORCE_INLINE void fire(Args... p_args);
	FORCE_INLINE Error disconnect(const CallableMethod &p_method);

	FORCE_INLINE bool has(const CallableMethod &p_method);

	Event() {}
	~Event(){};
};

/**
 * @brief Gives the address of a function to an event for it to call at a later point.
 * @param list The function pointer to add into the vector
 * @param priority Some functions may need to be called first, and as a result a function with priority is pushed to
 * the front of the queue ahead of other functions.
 */
template <typename... Args>
void Event<Args...>::connect(const CallableMethod &p_method, bool priority) {
	if (priority) {
		listeners.push_front(p_method);
	} else {
		listeners.push_back(p_method);
	}
}

/**
 * @brief Sets of every function pointer the vector has obtained with the given arguments.
 * @param p_args The arguments to send off in the event. The number of arguments sent here must be the same as that on
 * creation of the event
 */
template <typename... Args>
void Event<Args...>::fire(Args... p_args) {
	uint64_t list_size = listeners.size();
	if (!list_size) {
		return;
	}
	List<CallableMethod>::Element *e = listeners.front();
	while (e) {
		CallableMethod &c = e->get();
		c.call(p_args...);
		e = e->next();
	}
}

/**
 * @brief Disconnects a given listener from the vector.
 * @param list The listener to remove from the event
 * @returns `OK` if it was able to find and remove the listener, and `ERR_CANT_FIND` if the listener was not found in
 * the vector.
 */
template <typename... Args>
Error Event<Args...>::disconnect(const CallableMethod &p_method) {
	List<CallableMethod>::Element *e = listeners.find(p_method);
	ERR_COND_NULL_R(e, ERR_CANT_FIND);

	if (listeners.erase(e)) {
		return OK;
	}

	return ERR_CANT_DELETE;
}

/**
 * @brief Checks to see if the current listener is held by the event.
 * @param list The listener to check
 * @returns `true` if the listener is held by the event, and `false` if not.
 */
template <typename... Args>
bool Event<Args...>::has(const CallableMethod &p_method) {
	List<CallableMethod>::Element *e = listeners.find(p_method);
	if (e != nullptr) {
		return true;
	}

	return false;
}
