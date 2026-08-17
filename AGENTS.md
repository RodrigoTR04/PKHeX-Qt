# PKHeX Qt

This is the repository for PKHeX Qt, a repository for using PKHeX with Qt GUI.

The goal is to recreate the GUI of the program with Qt so it can be multiplatform used on not only Windows, but Linux and MacOS as well. Accuracy is of utmost importance and the functionality must be identical to the original application. Only the Windows only aspects must be replaced.

## Current status

The project is in very early stages, and still has to figure out every detail necessary to reach its end goal.

## Some thoughts from the author

The following is a letter from me to you, the agent. We are building this together, but to be frank, I'm building this for other users. I want you to be able to come up to build whatever we come up with together. PKHeX Qt is meant to help users.

This is meant to be a bold project. Going with the flow and existing solutions will not get us where we want to be.
Quick glossary of relevant parties in this document:

* *you* - the agent reading this document and working on PKHeX Qt.
* *me/we/us* - the humans contributing to PKHeX Qt. This is the party talking to you as we build.
* *user* - the humans using the application.

## GUI field kinds and submenus

When placing tables, dropdowns, and other fields, follow `docs/ui/online-field-reference.md`. That pipeline reads PKHeX Online's UI catalogs as a baseline for control kind and submenu membership. Do not copy Online code. Byte parity and WinForms control names still come from the Oracle.

## Use of subagents

Subagents are only to be used if I ask for it, unless it's explicitly called for do not use subagents. DO NOT USE SUBAGENTS.

## Running tests

Because this program is being developed on multiple machines sometimes some things won't line up perfectly. Linux machines won't have access to .NET for example. Poking to see if .NET is available is fine, but if it is not found immediately assume it's not available.

## Original application

`/mnt/557377b4-8a41-467d-a6ab-9205ddc5becc/Documents/Projects/PKHeX-Qt/PKHeX-26.07.07` is where the source code of the original application lives, it's meant to be read-only, chaning that would only detract from the accuracy of the project and is not desired.
