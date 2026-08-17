# Self-contained .NET 10, Core stays C#

.NET 10 runs on Linux and macOS. A self-contained publish puts the runtime in the app folder so the user never installs a SDK. Replacing .NET means replacing Core. NativeAOT is a second compiler mode we do not need on day one. Official Releases are folders built by GitHub Actions on a public repo: CMake for the Host, `dotnet publish` per RID for Editor layer and Core.
