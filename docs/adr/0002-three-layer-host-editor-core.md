# Three layers: Host, Editor layer, Core

The OS process is a C++ Qt 6 Widgets Host. Editor behavior is C#. Save and Pokémon logic is vendored Core. Community C# Qt bindings are too fragile to bet Accuracy on. A C++-only rewrite of Core is the Accuracy death path. The Host is the entry point, loads Core and the Editor layer in-process with a self-contained .NET 10 runtime, generates C# stubs from `.ui` object names, and keeps Core calls on the GUI thread.

Considered and rejected: C# Qt bindings (no first-party Qt 6 story), C++ Host with Core in another process (IPC for every keystroke), QML (wrong layout model for this editor).
