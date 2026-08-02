#include "webp_anim_decoder.h"

#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/color.hpp>

namespace godot {

WebPAnimDecoder::WebPAnimDecoder() {}

WebPAnimDecoder::~WebPAnimDecoder() {
	_release();
}

void WebPAnimDecoder::_release() {
	if (_decoder) {
		WebPAnimDecoderDelete(_decoder);
		_decoder = nullptr;
	}
	_webp_data = {};
	_anim_info = {};
}

bool WebPAnimDecoder::load_bytes(const PackedByteArray &p_data) {
	_release();

	if (p_data.is_empty()) {
		return false;
	}

	_data = p_data;
	_webp_data.bytes = _data.ptr();
	_webp_data.size = static_cast<size_t>(_data.size());

	WebPAnimDecoderOptions options;
	if (!WebPAnimDecoderOptionsInit(&options)) {
		return false;
	}
	options.color_mode = MODE_RGBA;

	_decoder = WebPAnimDecoderNew(&_webp_data, &options);
	if (!_decoder) {
		_release();
		return false;
	}

	if (!WebPAnimDecoderGetInfo(_decoder, &_anim_info)) {
		_release();
		return false;
	}

	current_image = Image::create_empty(get_canvas_width(), get_canvas_height(), false, Image::FORMAT_RGBA8);
	current_texture.instantiate();
	current_texture->set_image(current_image);
	return true;
}

int WebPAnimDecoder::get_canvas_width() const {
	return static_cast<int>(_anim_info.canvas_width);
}

int WebPAnimDecoder::get_canvas_height() const {
	return static_cast<int>(_anim_info.canvas_height);
}

int WebPAnimDecoder::get_loop_count() const {
	return static_cast<int>(_anim_info.loop_count);
}

int WebPAnimDecoder::get_frame_count() const {
	return static_cast<int>(_anim_info.frame_count);
}

Color WebPAnimDecoder::get_background_color() const {
	// bgcolor is stored as 0xAABBGGRR in libwebp
	const uint32_t c = _anim_info.bgcolor;
	return Color(
			((c >> 0) & 0xFF) / 255.0f,
			((c >> 8) & 0xFF) / 255.0f,
			((c >> 16) & 0xFF) / 255.0f,
			((c >> 24) & 0xFF) / 255.0f);
}

bool WebPAnimDecoder::has_more_frames() const {
	if (!_decoder) {
		return false;
	}
	return WebPAnimDecoderHasMoreFrames(_decoder);
}

int WebPAnimDecoder::get_next_frame() {
	if (!_decoder) {
		return -1;
	}

	uint8_t *buf = nullptr;
	int timestamp = 0;
	if (!WebPAnimDecoderGetNext(_decoder, &buf, &timestamp)) {
		return -1;
	}

	const int w = get_canvas_width();
	const int h = get_canvas_height();
	const int byte_count = w * h * 4; // RGBA

	uint8_t* p = current_image->ptrw();
	std::memcpy(p, buf, byte_count);
	current_texture->update(current_image);
	return timestamp;
}

void WebPAnimDecoder::reset() {
	if (_decoder) {
		WebPAnimDecoderReset(_decoder);
	}
}

void WebPAnimDecoder::_bind_methods() {
	ClassDB::bind_method(D_METHOD("load_bytes", "data"), &WebPAnimDecoder::load_bytes);
	ClassDB::bind_method(D_METHOD("get_canvas_width"), &WebPAnimDecoder::get_canvas_width);
	ClassDB::bind_method(D_METHOD("get_canvas_height"), &WebPAnimDecoder::get_canvas_height);
	ClassDB::bind_method(D_METHOD("get_loop_count"), &WebPAnimDecoder::get_loop_count);
	ClassDB::bind_method(D_METHOD("get_frame_count"), &WebPAnimDecoder::get_frame_count);
	ClassDB::bind_method(D_METHOD("get_background_color"), &WebPAnimDecoder::get_background_color);
	ClassDB::bind_method(D_METHOD("has_more_frames"), &WebPAnimDecoder::has_more_frames);
	ClassDB::bind_method(D_METHOD("get_next_frame"), &WebPAnimDecoder::get_next_frame);
	ClassDB::bind_method(D_METHOD("get_current_image"), &WebPAnimDecoder::get_current_image);
	ClassDB::bind_method(D_METHOD("get_current_texture"), &WebPAnimDecoder::get_current_texture);
	ClassDB::bind_method(D_METHOD("reset"), &WebPAnimDecoder::reset);
}

} // namespace godot
