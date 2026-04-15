#pragma once

#include "core/data/rid.h" // IWYU pragma: keep
#include "core/object/ref_counted.h"
#include "core/typedefs.h"

class VAPI Resource : public RefCounted {
	VREGISTER_CLASS(Resource, RefCounted);

	String name = "";
	String path = "";

public:
	String get_name() const;
	void set_name(const String &p_name);

	String get_path() const;
	void set_path(const String &p_path);

	Resource() {}
	~Resource() {}
};