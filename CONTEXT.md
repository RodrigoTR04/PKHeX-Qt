# PKHeX Qt

Unofficial independent port of the PKHeX save editor to a Qt GUI, so the same editing behavior can run on Linux, macOS, and Windows.

## Language

**PKHeX Qt**:
The application this repository is building. It is not an upstream contribution to kwsch/PKHeX and not a hard fork of Core. The window title is "PKHeX Qt", with an unofficial disclaimer in About. It has its own version number. About also shows the Oracle date.
_Avoid_: official PKHeX, PKHeX WinForms, calling this product PKHeX Online, using Original PKHeX's version as ours

**PKHeX Online**:
The sibling browser port at `Documents/Projects/PKHeX-online`. It is a different product. We do not share code with it. Its UI catalogs are the Field reference for Host forms.
_Avoid_: the web app, PKHeX Qt (when you mean the browser port)

**Field reference**:
PKHeX Online's `docs/ui/` catalogs, used when choosing Qt control kinds (table, dropdown, checkbox, modal) and which submenu or tab a fact belongs in. Not a source of Byte parity, not a source of `.ui` object names, not code to copy. Pipeline: `docs/ui/online-field-reference.md` in this repo.
_Avoid_: copying Online, web layout, screenshot parity with the browser app

**Original PKHeX**:
The Windows PKHeX application we are matching, currently snapshotted as the read-only tree `PKHeX-26.07.07`.
_Avoid_: upstream (when you mean the files in this repo), vanilla, stock

**Oracle**:
A dated, read-only PKHeX source tree we treat as the behavior and layout source of truth. We swap the tree when we choose to chase a newer Original PKHeX. We do not edit it. A swap replaces `core/` with that Core, copies new artwork, and updates the About Oracle date.
_Avoid_: submodule, live upstream, reference copy (when you mean we will patch it)

**Core**:
`PKHeX.Core`, the portable `net10.0` library of save and Pokémon logic. We copy it into this tree so the app does not link at a path that can be removed when the Oracle is swapped. The copy stays identical to that Oracle's Core. We do not patch legality or formats in the copy.
_Avoid_: the app, WinForms, Drawing, forked Core

**Host**:
The C++ Qt 6 Widgets process and the OS entry point. Forms are hand-built `.ui` files whose object names match Original PKHeX controls so `lang_*.txt` keys work. C# stubs are generated from those names. Qt 6 is dynamically linked and the libraries sit in the publish folder. Command-line paths are forwarded to Core. Splash and error windows exist. About lists GPL, Qt LGPL, and third-party notices. Core and Editor-layer calls run on the GUI thread.
_Avoid_: the Qt GUI (when you mean C# editor code), bindings, generated layouts from Designer pixels, static Qt, system libqt

**Editor layer**:
The C# that replaced WinForms code-behind: Save handlers, unsaved prompts, recently loaded and template-folder startup, slot clicks, drag-and-drop, hover previews, drag cursors, clipboard, shortcuts, QR, cry playback, in-game font, backups, and a SaveFinder wrapper that supplies Linux/macOS roots. It is not Core and not the Host. Slot images are composited in C# and handed to the Host as pixmaps.
_Avoid_: WinForms, Core, the Host, GUI logic (when you mean Core legality)

**Byte parity**:
The same user operations on the same input produce the same exported save bytes as Original PKHeX. A harness exists from the start and runs against golden fixtures on any OS. Live comparison to Original PKHeX runs later on Windows. Core unit tests are not this proof.
_Avoid_: visual match, Core tests, "it calls Core so it matches"

**User config**:
PKHeX Qt's own settings file in the OS per-user config directory. Not Original PKHeX's settings, not next to the executable, not next to the save.
_Avoid_: AppData (when you mean the Linux/macOS path too), ini beside the binary

**Accuracy**:
Same edits, same legality outcomes, same shortcuts, same dialogs. Control names match Original PKHeX. Field kinds and submenu placement follow the Field reference (PKHeX Online catalogs), then Qt layouts. Native Qt widgets, native file dialogs, and platform window chrome are allowed. Slots scale with the form and stay sharp. Clipboard Pokémon formats match Original PKHeX. Species and item images use Artwork sprites with Original PKHeX artwork-builder overlays, including box wallpapers. Nickname fields use the same in-game font Original PKHeX embeds. Cries play from an optional local Sounds directory. Startup language follows Original PKHeX (User config, OS locale default). The app follows the OS color scheme. HaX is out of v1. Windows and macOS Releases are unsigned.
_Avoid_: pixel-perfect, feature parity (when you mean a redesigned UI), lookalike, DarkMode setting

**v1**:
A ship of PKHeX Qt that includes the main window and every subform and save editor Original PKHeX ships. No plugin API. No file associations. Multiple instances allowed. Public incomplete builds are allowed before v1. The first public incomplete build is the main window as a real editor: open/save, boxes, party, PKM tabs, legality report, Showdown import/export, drag-drop, clipboard.
_Avoid_: MVP, complete parity (when that would include plugins)

**Artwork sprites**:
The `a_` / artwork image set already shipped inside Original PKHeX's PokeSprite resources, which PKHeX Online prefers because classic `b_` sprites scale poorly. We copy those assets into a shipped assets tree. This is a deliberate visual exception to matching Original PKHeX's default look.
_Avoid_: regular sprites, official icons, PKHeX Online exclusive art (the files come from Original PKHeX), downloading sprites

**Release**:
A public Linux AppImage built by GitHub Actions on the free tier of a public repository. The workflow is CMake (Host, Qt 6) plus `dotnet publish` (Editor layer and Core, self-contained per RID), then `appimagetool` over that folder. Users download one file and do not compile Qt.
_Avoid_: local zip as the official artifact, distro package as v1

**Plugin**:
A third-party .NET DLL loaded by Original PKHeX against `IPlugin`. Out of scope for v1.
_Avoid_: addon, extension, mod
