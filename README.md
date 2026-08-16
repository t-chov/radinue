# Radinue

[日本語](README-ja.md)

Radinue is a small desktop audio player for Windows and macOS, designed for
listening to radio recordings stored in a directory. It remembers the current
track and playback position so that you can resume exactly where you stopped.

> [!NOTE]
> Radinue is in the early stages of development. There is no usable release or
> supported build procedure yet.

## Planned features

- Treat one selected directory as a playlist, without scanning subdirectories
- Sort tracks deterministically by filename in ascending or descending order
- Automatically advance to the next track without wrapping at playlist ends
- Save the current track and position in a human-readable file inside the
  playlist directory
- Restore playback from the saved position when the directory is opened again
- Change playback speed from 50% to 200% in exact 10% steps, with pitch
  correction
- Adjust volume from 0% to 200%
- Use compact controls for previous, rewind 10 seconds, play/pause, forward 10
  seconds, and next
- Continue playback gracefully when individual files are malformed or
  unsupported

Radinue deliberately focuses on reliable local playback. It does not aim to
provide library management, streaming, podcast feeds, shuffle, repeat,
equalizers, cloud sync, telemetry, or automatic updates.

## Keyboard shortcuts

Shortcuts are active only while Radinue has focus.

| Key | Action |
| --- | --- |
| `z` | Seek backward 10 seconds |
| `x` | Seek forward 10 seconds |
| `s` | Decrease speed by 10% |
| `d` | Increase speed by 10% |
| `g` | Reset speed to 100% |

## Technology

Radinue is planned around a deliberately small native stack:

- C++20
- Qt 6 Widgets
- libmpv
- CMake

## Building

The build system has not been added yet. Build and development instructions
will be documented here once the initial application structure is available.

The intended supported platforms are:

- Windows, built natively with MSVC
- macOS, built natively with Apple Clang

## Contributing

Radinue prioritizes stability, predictable behavior, and a small maintenance
surface. Before proposing a change, please read [AGENTS.md](AGENTS.md) for the
project requirements, architecture, and testing expectations.

## License

Radinue is free software licensed under the
[GNU General Public License v3.0 or later](LICENSE).
