#include "importers/resource_importer_image.h"

#include "rendering/rendering_server.h"
#include "scene/resources/texture.h"

#include <core/io/filesystem.h>
#include <core/variant/array.h>
#include <core/variant/variant.h>

#include <stb/stb_image.h>

Ref<Resource> ResourceFormatImporterImage::_import(const String &p_file, int p_argc, Variant *p_args) {
	Ref<FileSystem> fs = FileSystem::open(p_file, FileSystem::FILE_ACCESS_READ);

	int len = fs->get_length();

	Vector<uint8_t> buf;
	buf.resize(len);
	fs->get_buffer(buf.ptrw(), len);

	Ref<Texture> t;
	t.instantiate();
	int width, height, channel_count;
	uint8_t *data = stbi_load_from_memory(buf.ptr(), buf.size(), &width, &height, &channel_count, 0);

	RS::TextureFormat format;
	switch (channel_count) {
		case 3: {
			format = RS::FORMAT_RGB;
		} break;
		case 4: {
			format = RS::FORMAT_RGBA;
		} break;
		default:
			format = RS::FORMAT_RGB;
	}
	t->set_data(width, height, channel_count, (Texture::TextureFormat)format);

	RS::get_singleton()->texture_set_from_data(t->get_texture(), data, width, height, RS::FORMAT_RGB);

	stbi_image_free(data);

	return t;
}

ResourceFormatImporterImage::ResourceFormatImporterImage() {
	supported_formats.push_back("png");
	supported_formats.push_back("jpeg");
	supported_formats.push_back("jpg");
}