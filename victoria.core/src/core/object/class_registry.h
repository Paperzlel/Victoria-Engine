#pragma once

#include "callable_method_pointer.h" // IWYU pragma: keep

#include "core/data/hashtable.h"
#include "core/object/object.h"
#include "core/string/print_string.h"
#include "core/typedefs.h"

class VAPI ClassRegistry {
private:
	struct ClassInfo {
		Object *(*creation_func)() = nullptr;
		String name;
		String inherits;
		List<String> signals;
		bool is_registered = false;
	};
	static HashTable<String, ClassInfo> classes;

	template <typename T>
	static Object *creator() {
		Object *obj = vnew(T);
		return obj;
	}

public:
	template <typename T>
	static void register_class() {
		ClassInfo ci;
		String cname = T::get_class_name_static();
		ci.name = cname;
		ci.inherits = T::get_inherited_class_name_static();
		ci.creation_func = &creator<T>;
		ci.is_registered = true;
		print_verbose(vformat("Registering class %s", cname.get_data()));
		classes.insert(cname, ci);
		T::initialize_class();
	}

	template <typename T>
	static void register_abstract_class() {
		ClassInfo ci;
		String cname = T::get_class_name_static();
		ci.name = cname;
		ci.inherits = T::get_inherited_class_name_static();
		ci.is_registered = true;
		print_verbose(vformat("Registering abstract class %s", cname.get_data()));
		classes.insert(cname, ci);
		T::initialize_class();
	}

	static void add_signal(const String &p_class, const String &p_signal);
	static bool has_signal(const String &p_class, const String &p_signal);

	static Object *instantiate(const String &p_class);
};

#define REGISTER_CLASS(m_class) ClassRegistry::register_class<m_class>();

#define REGISTER_ABSTRACT_CLASS(m_class) ClassRegistry::register_abstract_class<m_class>()
