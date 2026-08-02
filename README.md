# godot-webp

A GDExtension that brings [libwebp](https://github.com/webmproject/libwebp) to Godot 4.5+, enabling decoding of both static and **animated** WebP images at runtime.

## Features

- Decode static and animated WebP images via `WebPAnimDecoder`
- Frame-by-frame playback helper via `WebPAnimationPlayer`
- Supports Windows, macOS, iOS, and Android

## Building

Requires CMake 3.17+ and Python 3.10+. Submodules (`godot-cpp`, `libwebp`) must be initialised first:

```sh
git submodule update --init --recursive
```

Then build with the helper script:

```sh
# Debug (default)
python build.py

# Release
python build.py --config Release

# Cross-compile for Android
python build.py --config Release --platform android

# Cross-compile for iOS
python build.py --config Release --platform ios
```

The compiled library is placed in `demo/addons/godot-webp/bin/<platform>/`.

## Installation

1. Copy the `demo/addons/godot-webp/` folder into your project's `addons/` directory.
2. Enable the plugin in **Project → Project Settings → Plugins**.

The addon exposes two GDScript-accessible classes:

| Class | Type | Description |
|---|---|---|
| `WebPAnimDecoder` | `RefCounted` | Low-level frame decoder backed by libwebp |
| `WebPAnimationPlayer` | `RefCounted` | High-level animated playback helper |

## Usage

### `WebPAnimationPlayer`

`WebPAnimationPlayer` drives animated WebP playback on any node that implements two callable methods:

| Method | Signature | Description |
|---|---|---|
| `set_webp_size` | `(size: Vector2)` | Called once with the canvas dimensions |
| `set_webp_texture` | `(tex: ImageTexture)` | Called each frame with the decoded texture |

#### Example — playing an animated WebP on a `TextureRect`

```gdscript
extends TextureRect

@export var path: String

var player := WebPAnimationPlayer.new()
var anim: PackedByteArray

func _ready() -> void:
    anim = FileAccess.get_file_as_bytes(path)

func _process(_delta: float) -> void:
    if Input.is_action_just_pressed("ui_accept"):
        if player.is_playing():
            player.stop()
        else:
            # play(node, bytes, frame_rate, loop, debug)
            player.play(self, anim, 24, true, false)

func set_webp_size(p_size: Vector2) -> void:
    size = p_size

func set_webp_texture(tex: ImageTexture) -> void:
    texture = tex
```

Set the `path` export variable to a `.webp` file path (e.g. `res://my_animation.webp`), attach the script to a `TextureRect`, and press the configured `ui_accept` action to start or stop playback.

#### `play()` parameters

| Parameter | Type | Description |
|---|---|---|
| `node` | `Node` | The node that receives `set_webp_size` / `set_webp_texture` calls |
| `b` | `PackedByteArray` | Raw WebP file bytes |
| `frame_rate` | `float` | Desired playback frame rate |
| `loop` | `bool` | Whether to loop the animation |
| `debug` | `bool` | Print timing diagnostics to the output |

### `WebPAnimDecoder` (low-level)

Use `WebPAnimDecoder` directly when you need manual frame control:

```gdscript
var dec := WebPAnimDecoder.new()
dec.load_bytes(FileAccess.get_file_as_bytes("res://my_animation.webp"))

print(dec.get_frame_count())   # total frames
print(dec.get_loop_count())    # 0 = infinite loop

while dec.has_more_frames():
    dec.get_next_frame()
    var tex: ImageTexture = dec.get_current_texture()
    # use tex ...
```

## License

This project is licensed under the terms of the [LICENSE](LICENSE) file. libwebp is used under its [BSD license](libwebp/COPYING).
