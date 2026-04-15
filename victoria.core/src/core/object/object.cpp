#include "core/object/object.h"

#include "core/object/class_registry.h"

bool Object::_predelete() {
	notification(NOTIFICATION_PREDELETE);
	return true;
}

void Object::notification(int p_what, bool p_reversed) {
	if (p_reversed) {
		_notification_backwardv(p_what);
	} else {
		_notification_forwardv(p_what);
	}
}

Error Object::connect_method(const String &p_name, const CallableMethod &p_method) {
	// Avoid crashes by inserting the method beforehand
	if (callables.get_element_count() == 0 && ClassRegistry::has_signal(get_class_name(), p_name)) {
		callables.insert(p_name, List<CallableMethod>());
	}

	List<CallableMethod> *methods = callables.get_ptr(p_name);
	if (!methods->has(p_method)) {
		methods->push_back(p_method);
	} else {
		ERR_FAIL_MSG_R(vformat("Method already connected to function \'%s\'.", p_name.get_data()), ERR_ALREADY_EXISTS);
	}

	return OK;
}

Error Object::emit_methodp(const String &p_name, const Variant **p_args, int p_argc) {
	List<CallableMethod> *list = callables.get_ptr(p_name);
	if (!list) {
		return ERR_UNAVAILABLE;
	}

	for (const CallableMethod &c : *list) {
		Error ret = OK;
		c.callp(p_args, ret);

		if (ret != OK) {
			return ret;
		}
	}

	return OK;
}

void Object::initialize_class() {
	static bool init = false;
	if (init) {
		return;
	}

	_bind_methods();
	init = true;
}

Object::Object() {}

Object::~Object() {}

bool predelete(Object *p_item) {
	return p_item->_predelete();
}