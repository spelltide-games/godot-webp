extends TextureRect

@export var path: String

var dec := WebPAnimDecoder.new()

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
    var b := FileAccess.get_file_as_bytes(path)
    assert(!b.is_empty())
    play(b, true)

func play(b: PackedByteArray, loop: bool):
    while loop:
        var st = Time.get_ticks_msec()
        var decoding_ms = 0.0
        assert(dec.load(b))
        var w := dec.get_canvas_width()
        var h := dec.get_canvas_height()
        print('width: %d, height: %d' % [w, h])
        size = Vector2(w, h)
        decoding_ms += Time.get_ticks_msec() - st

        while dec.has_more_frames():
            st = Time.get_ticks_msec()
            var res := dec.get_next_frame()
            var img := res['image'] as Image
            var tex := ImageTexture.create_from_image(img)
            decoding_ms += Time.get_ticks_msec() - st
            self.texture = tex

            print(decoding_ms)
            var delay: float = 1/24.0 - decoding_ms/1000.0
            await get_tree().create_timer(delay).timeout
            decoding_ms = 0
