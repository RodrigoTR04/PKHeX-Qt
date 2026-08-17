# Byte parity lives in the Editor layer

WinForms Save handlers contain loops and side effects that Core does not expose as a single command. Inventory Save rebuilds clean items and compresses empty slots before `CopyTo`. Pokédex Save writes extra fields. That C# moves into the Editor layer, not into Core and not into C++. Qt widgets fire it. The GUI bar is Byte parity against Original PKHeX, proven by a harness on golden fixtures, then live Windows comparison later. Calling an existing Core helper with a similar name is not enough.
