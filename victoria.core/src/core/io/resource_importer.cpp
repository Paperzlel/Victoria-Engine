#include "core/io/resource_importer.h"

ResourceImporter *ResourceImporter::singleton = nullptr;

ResourceImporter *ResourceImporter::get_singleton() {
	return singleton;
}
void ResourceImporter::add_importer(Ref<ResourceFormatImporter> p_importer) {
	importers.push_back(p_importer);
}

/**
 * @brief Takes a raw file from the user filesystem and outputs a resource for the engine to use.
 * @param p_path The path (relative or absolute) to the desired file to import, including the file extension.
 * @param p_argc The number additional arguments that want to be passed to the importer, depending on its import type.
 * @param p_args A pointer to an array of arguments that can be used by the importer.
 * @param r_error The returning error message to output if something goes wrong.
 * @returns A resource file of the desired type if successful, and otherwise a null resource if the operation fails.
 */
Ref<Resource> ResourceImporter::import(const String &p_path, int p_argc, Variant *p_args, Error *r_error) {
	for (const Ref<ResourceFormatImporter> &i : importers) {
		List<String> supported_formats = i->get_extension_list();
		List<String>::Element *e = supported_formats.front();

		while (e) {
			if (p_path.get_file_extension() == e->get()) {
				Ref<Resource> ret = i->_import(p_path, p_argc, p_args);
				if (ret == Ref<Resource>()) {
					if (r_error) {
						*r_error = ERR_CANT_CREATE;
					}
					ERR_FAIL_MSG_R("Could not properly import the resource.", Ref<Resource>());
				}
				if (r_error) {
					*r_error = OK;
				}
				return ret;
			}
			e = e->next();
		}
	}

	return Ref<Resource>();
}

ResourceImporter::ResourceImporter() {
	singleton = this;
}
