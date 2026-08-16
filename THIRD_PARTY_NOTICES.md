# Third-party software

Radinue's source code is distributed under GPL-3.0-or-later. Building and
running Radinue also requires the following third-party software, which remains
under its respective license:

| Software | Purpose | Upstream | License information |
| --- | --- | --- | --- |
| Qt 6 | Native application and test framework | <https://www.qt.io/> | <https://www.qt.io/licensing/> |
| mpv / libmpv | Audio decoding and playback | <https://mpv.io/> | <https://github.com/mpv-player/mpv/tree/master/Copyright> |

The Windows CI build uses a pinned libmpv SDK produced by
[`zhongfly/mpv-winbuild`](https://github.com/zhongfly/mpv-winbuild). Its exact
release URL and SHA-256 digest are recorded in
[`cmake/DownloadMpvWindows.cmake`](cmake/DownloadMpvWindows.cmake).

No third-party binaries are committed to this repository. Release packaging
must include all license texts and corresponding-source information required by
the exact binaries being distributed.
