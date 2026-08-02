extends TextureRect

@export var path: String

var player := WebPAnimationPlayer.new()
var anim: PackedByteArray

func _ready() -> void:
    anim = FileAccess.get_file_as_bytes(path)
    assert(!anim.is_empty())

# Called when the node enters the scene tree for the first time.
func _process(_delta: float):
    if Input.is_action_just_pressed("ui_accept"):
        if player.is_playing():
            player.stop()
        else:
            player.play(self, anim, 24, true, true)

func set_webp_size(p_size: Vector2):
    self.size = p_size

func set_webp_texture(tex: ImageTexture):
    self.texture = tex
