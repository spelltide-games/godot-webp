extends RefCounted

class_name WebPAnimationPlayer

var _dec = WebPAnimDecoder.new()
var _is_playing: bool = false

func is_playing() -> bool:
    return _is_playing

func play(node: Node, b: PackedByteArray, frame_rate: float, loop: bool, debug: bool):
    while loop:
        var st = Time.get_ticks_msec()
        var decoding_ms = 0.0
        if not _dec.load_bytes(b):
            push_error('WebPAnimDecoder::load_bytes failed!')
            return
        var w := _dec.get_canvas_width()
        var h := _dec.get_canvas_height()
        if debug:
            print('width: %d, height: %d' % [w, h])

        node.call('set_webp_size', Vector2(w, h))
        decoding_ms += Time.get_ticks_msec() - st

        _is_playing = true
        while _is_playing and _dec.has_more_frames():
            st = Time.get_ticks_msec()
            _dec.get_next_frame()
            decoding_ms += Time.get_ticks_msec() - st
            node.call('set_webp_texture', _dec.get_current_texture())

            var delay: float = 1/frame_rate - decoding_ms/1000.0
            if debug:
                print('decoding_ms: %d' % decoding_ms)
                if delay < 0:
                    print('delay: %s' % delay)
            await node.get_tree().create_timer(delay).timeout
            decoding_ms = 0

        if !_is_playing:
            break
    _is_playing = false

func stop():
    _is_playing = false
