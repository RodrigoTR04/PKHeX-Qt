# PKHeX Online as field-kind reference

WinForms Designer is a pixel grid. Qt layouts need a non-pixel answer for "is this a table, a dropdown, a checkbox, or a submenu?" PKHeX Online already catalogued that mapping from Original PKHeX into a non-WinForms UI. We read those catalogs when building Host forms. We do not copy Online code, CSS, or React. Byte parity and control names still come from the Oracle. The lookup order lives in `docs/ui/online-field-reference.md`.
