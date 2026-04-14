#pragma once

#include "resource.h"

#include "core/data/list.h"
#include "core/typedefs.h"

class VAPI ResourceFormatImporter : public Instance {
protected:
	List<String> supported_formats;

public:
	virtual Ref<Resource> _import(const String &p_path, int p_argc, Variant *p_args) {
		return Ref<Resource>();
	}
	/**
	 * @brief Obtains a reference to the extension list held by the format importer.
	 * @return The list of supported formats for the given importer.
	 */
	const List<String> &get_extension_list() const {
		return supported_formats;
	}
};
