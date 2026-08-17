# Qt 6 Widgets, dynamically linked LGPL

The GUI toolkit is Qt 6 Widgets, not QML, not Qt 5, not a C# widget library. Forms are hand-built `.ui` files whose object names match Original PKHeX so `lang_*.txt` keys still work. Qt is dynamically linked and the libraries ship in the publish folder. Static Qt is a license trap. Distro `libqt6` will not match the version we build against. PKHeX Qt itself is GPL-3.0-or-later because it links Core.
