#pragma once

#include "core/io/resource_format_importer.h"
#include "core/typedefs.h"

/**
 * @brief Importer class for a font.
 * Arguments:
 * 1. bitmap (boolean): Whether to import the font as grid of bitmaps or to use individual textures for each character.
 * 2. font_size (int): The size of the font.
 * 3. use_sdf (boolean): Whether to use Signed Distance Fields for the font.
 */
class ResourceFormatImporterFont : public ResourceFormatImporter {
public:
	virtual Ref<Resource> _import(const String &p_file, int p_argc, Variant *p_args) override;

	ResourceFormatImporterFont();
	~ResourceFormatImporterFont();
};
