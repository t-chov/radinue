# AGENTS.md

## Project

Radinue is a small, stable desktop audio player for Windows and macOS. It is
designed for listening to radio recordings stored in a directory and resuming
exactly where the listener stopped.

Stability, predictable behavior, and a small maintenance surface take priority
over feature count, visual novelty, or broad configurability.

The project is open source under `GPL-3.0-or-later`.

## Product scope

Radinue treats one user-selected directory as one playlist.

Required behavior:

- Only regular audio files directly inside the selected directory belong to
  the playlist. Do not recurse into subdirectories.
- Sort by filename only. The user can choose ascending or descending order.
- Playback automatically advances to the next track in the selected order.
- Do not wrap from the last track to the first, or vice versa, unless the
  product specification is explicitly changed.
- Store the current track and playback position in a human-readable plain-text
  file inside the playlist directory.
- Restore the saved track and position when that directory is opened again.
- Playback speed ranges from 50% through 200%, in exact 10% increments.
- `s` decreases speed by 10%, `d` increases it by 10%, and `g` resets it to
  100%.
- Volume ranges from 0% through 200% and is controlled with a slider. Values
  above 100% are software amplification and may clip.
- The main UI contains buttons for previous track, seek backward 10 seconds,
  play/pause, seek forward 10 seconds, and next track.
- `z` seeks backward 10 seconds and `x` seeks forward 10 seconds.
- Keyboard shortcuts are active only while Radinue has focus. Do not register
  global system shortcuts.
- The center transport control is play/pause. Pausing must not reset the
  position.

Do not add library management, metadata editing, accounts, network streaming,
podcast feeds, shuffle, repeat, equalizers, themes, cloud sync, telemetry,
automatic updates, or directory recursion without an explicit request.

## Technical direction

Use the following stack unless a documented project decision changes it:

- C++20
- Qt 6 Widgets for the UI, filesystem integration, shortcuts, and application
  lifecycle
- libmpv for decoding and playback
- CMake for builds
- Qt Test or Catch2 for automated tests

Do not replace libmpv with Qt Multimedia. Qt Multimedia does not meet the
project's 200% amplification requirement without additional audio processing,
and playback-rate behavior can vary by platform backend.

Avoid QML, Qt WebEngine, Electron, Tauri, embedded browsers, and JavaScript
runtimes. This application does not need a web UI.

Prefer direct, documented libmpv C API usage. A third-party C++ wrapper may be
introduced only when it clearly reduces risk and is actively maintained.

## Architecture

Keep the design small and explicit. Prefer these responsibilities:

- `MainWindow`: widgets, presentation state, and user input only
- `DirectoryPlaylist`: directory scanning, filtering, sorting, and track
  navigation
- `PlayerController`: all libmpv ownership and playback commands
- `PlaybackStateStore`: validation, restoration, and atomic persistence

Business rules must not be embedded in widget callbacks. Keep playlist,
playback, and persistence behavior testable without constructing the complete
UI.

Do not introduce dependency injection frameworks, service locators, event
buses, plugin systems, or generalized media abstractions. Simple constructor
injection and Qt signals/slots are sufficient.

## libmpv and threading

- There must be one clear owner of the `mpv_handle`.
- Do not manipulate Qt widgets from a libmpv callback or worker thread.
- Convert mpv events into queued Qt signals and update widgets on the Qt main
  thread.
- Keep event callbacks short and non-blocking.
- Observe playback position, duration, pause state, end-of-file, current file,
  speed, volume, and errors through documented mpv properties/events.
- Clamp seeks to the valid range. Seeking before zero goes to zero; seeking
  beyond a known duration goes to the duration/end-of-file behavior.
- Configure pitch correction for non-100% playback speed.
- Configure mpv to permit a UI volume value through 200%.
- Surface playback failures in the UI and remain usable. A malformed or
  unsupported track must not crash the application.

## Playlist rules

- Maintain one explicit, documented, case-insensitive extension allowlist for
  supported audio files. Keep the list in one place and cover it with tests.
- Exclude Radinue's state file and temporary state files unconditionally.
- Ignore directories, sockets, devices, and other non-regular entries.
- Symlink handling must be explicit and tested. Default to ignoring symlinks
  to avoid duplicate entries and paths escaping the selected directory.
- Use a deterministic, locale-independent filename comparison. Ascending and
  descending must be exact reversals of one another.
- Do not silently switch to natural-number sorting or platform-specific Finder
  or Explorer ordering.
- Rescan when a directory is opened. Do not add a continuously running
  filesystem watcher until required.
- If the saved file no longer exists, start with the first file in the current
  order and position zero.
- If there are no playable files, show an empty state rather than treating it
  as an application error.

## Playback-state file

Use `.radinue-state.json` in the selected playlist directory. JSON satisfies
the plain-text and human-readable requirement while avoiding a custom parser.

Required schema:

```json
{
  "version": 1,
  "file": "recording.mp3",
  "position_ms": 1234567
}
```

Rules:

- Save the track as a relative filename, never as an absolute path.
- Store playback position as a non-negative integer number of milliseconds.
- Validate all loaded values. Treat missing, malformed, unsupported-version,
  negative, or out-of-range data as recoverable input, never as a crash.
- Unknown JSON fields must be ignored to preserve forward compatibility.
- Write atomically using `QSaveFile` or an equivalent temporary-file-and-rename
  operation. Never overwrite the state file in place.
- Persist on pause, explicit seek, track change, directory change, and normal
  application shutdown.
- Also checkpoint during active playback at a modest interval, approximately
  every 10 seconds, so an abnormal termination loses little progress.
- Avoid redundant writes when neither track nor meaningful position changed.
- If the directory is not writable, explain this clearly to the user and keep
  playback functional. Do not silently fall back to a different state-file
  location because the per-directory state is part of the product contract.

Additional preferences such as speed, volume, and sort direction may be added
to the same versioned JSON object only after their persistence behavior is
intentionally specified. Do not make undocumented fields required.

## UI behavior

- Keep the main window compact and usable at Windows and macOS display scaling
  settings.
- Use standard Qt controls and platform conventions where possible.
- Controls must have accessible names, keyboard focus behavior, and tooltips
  that include their shortcuts.
- Disable actions that cannot currently succeed, such as previous track at the
  beginning of the playlist or seek when nothing is loaded.
- Display at least the current filename, elapsed time, duration when known,
  playback speed, and volume.
- Keep filename display safe for long names; elide visually without changing
  the underlying value.
- Never block the UI thread for directory scans, decoder work, or waiting on
  shutdown. A simple direct directory scan is acceptable while opening a
  normally sized local directory; move it off-thread only if measurement shows
  a real responsiveness problem.

## Error handling and logging

- Errors caused by user files or filesystem permissions are recoverable.
- Show concise user-facing messages and log enough technical detail for issue
  reports without exposing unrelated paths or personal data.
- Do not collect or transmit telemetry, crash reports, filenames, paths, or
  listening history.
- Avoid exceptions crossing Qt signal/slot, C callback, or libmpv boundaries.
- Check return values from filesystem and libmpv operations.
- Prefer an explicit safe fallback over undefined or platform-dependent
  behavior.

## Code conventions

- Follow the repository's formatter and linter configuration when present.
- Use `clang-format` for C++ and CMake formatting conventions consistently.
- Use `PascalCase` for types, `camelCase` for functions and local variables,
  and `m_` prefixes for private data members.
- Prefer RAII, value types, `std::unique_ptr`, and narrowly scoped ownership.
- Avoid raw owning pointers and manual lifetime coupling.
- Use Qt types at Qt API boundaries and standard-library types in independent
  domain logic where that improves testability. Do not churn between types
  without a concrete benefit.
- Keep platform-specific code behind small interfaces and compile guards.
- Do not suppress compiler warnings globally. Fix warnings or scope any
  necessary suppression to the smallest affected third-party boundary.
- Comments should explain invariants and non-obvious decisions, not restate the
  code.

## Dependencies

- Minimize both runtime and build dependencies.
- Pin reproducible dependency versions or artifacts and record their source and
  hashes where practical.
- Do not fetch mutable branches during release builds.
- Any new dependency requires a clear stability, maintenance, packaging, and
  GPL-compatibility justification.
- Keep third-party notices and corresponding-source information current when a
  dependency or build configuration changes.

## Build and packaging

- Build Windows artifacts natively with MSVC and macOS artifacts natively with
  Apple Clang.
- Use CMake presets once they exist; keep developer and CI commands aligned.
- Package all required Qt, libmpv, FFmpeg, and related runtime libraries.
- Test a clean installation on a machine without developer tools or a system
  mpv installation.
- Windows releases should be code-signed when release infrastructure permits.
- macOS releases should be signed, hardened, and notarized for direct
  distribution.
- Do not assume one binary can serve macOS Intel and Apple Silicon unless a
  universal build is intentionally produced and tested.

## Testing

At minimum, add automated coverage for:

- ascending and descending filename order
- deterministic ordering across case and non-ASCII filenames
- extension filtering and ignored entries
- empty and one-track directories
- missing currently saved track
- valid, missing, corrupt, truncated, and future-version state files
- atomic-save failure handling
- position clamping
- speed boundaries and exact 10% steps
- volume boundaries
- previous/next behavior at playlist boundaries
- end-of-track advancement
- shortcut mapping

Use temporary directories for filesystem tests. Tests must never write into a
developer's actual media directory or home directory.

Before considering a change complete:

1. Build the affected Windows/macOS configuration when available.
2. Run unit and integration tests with failure output enabled.
3. Manually verify playback, pause/resume restoration, both sort directions,
   all five transport buttons, all five shortcuts, speed limits, and 200%
   volume for changes touching those areas.
4. Verify that killing and restarting the application loses no more than the
   checkpoint interval during active playback.
5. Check that no unrelated feature, dependency, or persistent data was added.

If only one host platform is available, test it fully and state clearly that
the other platform was not verified. Do not claim cross-platform success based
only on compilation.

## Change discipline

- Make the smallest change that fully solves the requested problem.
- Preserve user-owned changes and avoid unrelated refactors.
- Update tests and user documentation with behavior changes.
- Do not change the state schema, filename ordering, keyboard bindings,
  supported platforms, license, or core stack incidentally.
- Record a short architectural decision when deliberately changing a decision
  in this file.
- When requirements are ambiguous, choose the behavior that preserves playback
  position and user data, minimizes surprise, and adds the least machinery.

