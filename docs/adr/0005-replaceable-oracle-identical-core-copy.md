# Replaceable Oracle, identical Core copy

`PKHeX-26.07.07` is a read-only Oracle. We copy Core into `core/` so the app does not project-reference a folder that disappears on a swap. The copy stays identical to that Oracle's Core. No legality patches. When we chase a newer Original PKHeX we replace the Oracle tree, replace `core/`, copy new artwork, and bump the About Oracle date. NuGet and a live submodule would split the source of truth.
