# GUI field reference (PKHeX Online)

When building or changing a Host form, look up **field kind** and **submenu placement** in PKHeX Online before inventing a Qt control. Do not copy Online source. Do not treat Online as Byte parity.

Sibling repo (read-only for this purpose):

`/mnt/557377b4-8a41-467d-a6ab-9205ddc5becc/Documents/Projects/PKHeX-online`

## What to take

- Which facts are a dropdown, a table, a checkbox, a number, a modal, or a button
- Which submenu, tab, or dialog a fact lives in
- When a generation hides or shows a page

## What not to take

- React, CSS, WASM, or adapter code
- Web-only presentation (dark theme freeze, always-non-shiny sprites, browser chrome)
- Any mutation that would change exported bytes. Bytes follow the Oracle and the Editor layer.

Control **names** on `.ui` files still match Original PKHeX so `lang_*.txt` keys work.

## Lookup order

1. `docs/ui/entity-editor-field-catalog.md` — main PKM tabs and SAV tab: field kinds and tab membership.
2. Generation notes in `docs/ui/` (`gen5-tabbed-dialogs.md`, `gen5-misc-tabs.md`, `gen6-desktop-parity.md`, `gen7-desktop-parity.md`, `gen7b-desktop-parity.md`, `gen8-desktop-parity.md`, `gen9-desktop-parity.md`) — where a gen's extra tools sit.
3. `docs/ui/data-table-chrome.md` — when Original PKHeX is a long indexed list, Online treated it as a table. Qt should too (`QTableWidget` / `QTreeView`), not a stack of line edits.
4. `docs/ui/wire-list-to-tabbed-editor.md` — when Core's storage list is flat but Original PKHeX uses tabs, regroup in the Host the same way, not as one scrolling column.
5. Tool-specific notes in that same `docs/ui/` folder (encounter grid, mystery gift, Pokéathlon, mail, and so on) when the ticket is that tool.
6. Oracle WinForms only after that, and only for operation names, Save-handler Byte parity, and translation keys.

If Online and WinForms disagree on **kind or submenu**, prefer Online and keep WinForms names plus Editor-layer operations. If they disagree on **what Save writes**, prefer the Oracle.
