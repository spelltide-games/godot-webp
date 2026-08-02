#pragma once

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>
#include <godot_cpp/variant/dictionary.hpp>

#include "webp/demux.h"

namespace godot {

// Decodes animated (and static) WebP images frame by frame.
class WebPAnimDecoder : public RefCounted {
	GDCLASS(WebPAnimDecoder, RefCounted)

	Ref<Image> current_image;
	Ref<ImageTexture> current_texture;
public:
	WebPAnimDecoder();
	~WebPAnimDecoder();

	// Load WebP data. Must be called before any other method.
	bool load_bytes(const PackedByteArray &p_data);

	int get_canvas_width() const;
	int get_canvas_height() const;
	int get_loop_count() const;
	int get_frame_count() const;
	Color get_background_color() const;

	bool has_more_frames() const;
	int get_next_frame();
	Ref<Image> get_current_image() const { return current_image; }
	Ref<ImageTexture> get_current_texture() const { return current_texture; }
	void reset();

protected:
	static void _bind_methods();

private:
	void _release();

	PackedByteArray _data; // keep data alive for the decoder lifetime
	WebPData _webp_data{};
	::WebPAnimDecoder *_decoder = nullptr;
	WebPAnimInfo _anim_info{};
};

} // namespace godot
