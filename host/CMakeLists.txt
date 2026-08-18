cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/ui")

option(PKHEXQT_SELF_CONTAINED "Publish a self-contained .NET runtime next to the Host" OFF)

find_package(Qt6 REQUIRED COMPONENTS Widgets Multimedia)

if(NOT DOTNET_ROOT AND DEFINED ENV{DOTNET_ROOT})
  set(DOTNET_ROOT "$ENV{DOTNET_ROOT}")
endif()
if(NOT DOTNET_ROOT AND EXISTS "$ENV{HOME}/.dotnet")
  set(DOTNET_ROOT "$ENV{HOME}/.dotnet")
endif()

find_program(DOTNET_EXE dotnet HINTS "${DOTNET_ROOT}" ENV PATH)

if(WIN32)
  if(CMAKE_SYSTEM_PROCESSOR MATCHES "ARM64|aarch64")
    set(DOTNET_HOST_RID "win-arm64")
  else()
    set(DOTNET_HOST_RID "win-x64")
  endif()
  set(NETHOST_LIB_NAME "nethost.dll")
elseif(APPLE)
  if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm64|aarch64")
    set(DOTNET_HOST_RID "osx-arm64")
  else()
    set(DOTNET_HOST_RID "osx-x64")
  endif()
  set(NETHOST_LIB_NAME "libnethost.dylib")
else()
  if(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|arm64")
    set(DOTNET_HOST_RID "linux-arm64")
  else()
    set(DOTNET_HOST_RID "linux-x64")
  endif()
  set(NETHOST_LIB_NAME "libnethost.so")
endif()

file(GLOB NETHOST_CANDIDATES
  "${DOTNET_ROOT}/packs/Microsoft.NETCore.App.Host.${DOTNET_HOST_RID}/*/runtimes/${DOTNET_HOST_RID}/native/nethost.h")
list(SORT NETHOST_CANDIDATES)
list(LENGTH NETHOST_CANDIDATES NETHOST_COUNT)
if(NETHOST_COUNT EQUAL 0)
  message(FATAL_ERROR "Could not find nethost.h under DOTNET_ROOT=${DOTNET_ROOT} for ${DOTNET_HOST_RID}.")
endif()
list(GET NETHOST_CANDIDATES -1 NETHOST_HEADER)
get_filename_component(NETHOST_DIR "${NETHOST_HEADER}" DIRECTORY)

add_library(nethost SHARED IMPORTED)
set_target_properties(nethost PROPERTIES
  IMPORTED_LOCATION "${NETHOST_DIR}/${NETHOST_LIB_NAME}"
  INTERFACE_INCLUDE_DIRECTORIES "${NETHOST_DIR}")

set(HOST_SOURCES
  src/MainWindow.cpp
  src/AboutWindow.cpp
  src/ErrorWindow.cpp
  src/SplashScreen.cpp
  src/QrWindow.cpp
  src/InventoryWindow.cpp
  src/PokedexWindow.cpp
    src/AccessoryWindow.cpp
    src/SaveBlockWindow.cpp
  src/LangCatalog.cpp
  src/SavToolChrome.cpp
  src/SlotChrome.cpp
  src/PkmTabChrome.cpp
)
set(HOST_HEADERS
  src/MainWindow.h
  src/AboutWindow.h
  src/ErrorWindow.h
  src/SplashScreen.h
  src/AppInfo.h
  src/QrWindow.h
  src/InventoryWindow.h
  src/PokedexWindow.h
    src/AccessoryWindow.h
    src/SaveBlockWindow.h
  src/EditorBridge.h
  src/LangCatalog.h
  src/SavToolChrome.h
  src/SlotChrome.h
  src/PkmTabChrome.h
)

add_executable(pkhex-qt
  src/main.cpp
  src/DotNetEditorBridge.cpp
  src/DotNetEditorBridge.h
  ${HOST_SOURCES}
  ${HOST_HEADERS}
  ui/MainWindow.ui
  ui/QR.ui
  ui/About.ui
  ui/ErrorWindow.ui
  ui/SplashScreen.ui
  ui/SAV_Inventory.ui
  ui/SAV_Pokedex.ui
  ui/RibbonEditor.ui
  ui/SAV_SimpleTrainer.ui
)
target_include_directories(pkhex-qt PRIVATE src "${NETHOST_DIR}")
target_link_libraries(pkhex-qt PRIVATE Qt6::Widgets Qt6::Multimedia nethost ${CMAKE_DL_LIBS})
target_compile_definitions(pkhex-qt PRIVATE
  PKHEXQT_VERSION="${PROJECT_VERSION}"
  PKHEXQT_ORACLE_DATE="26.07.07")
set_target_properties(pkhex-qt PROPERTIES
  OUTPUT_NAME "pkhex-qt"
  BUILD_RPATH "$ORIGIN"
  INSTALL_RPATH "$ORIGIN")

add_executable(pkhex-qt-host-tests
  tests/host_smoke.cpp
  ${HOST_SOURCES}
  ${HOST_HEADERS}
  ui/MainWindow.ui
  ui/QR.ui
  ui/About.ui
  ui/ErrorWindow.ui
  ui/SplashScreen.ui
  ui/SAV_Inventory.ui
  ui/SAV_Pokedex.ui
  ui/RibbonEditor.ui
  ui/SAV_SimpleTrainer.ui
)
target_include_directories(pkhex-qt-host-tests PRIVATE src)
target_link_libraries(pkhex-qt-host-tests PRIVATE Qt6::Widgets Qt6::Multimedia)
target_compile_definitions(pkhex-qt-host-tests PRIVATE
  PKHEXQT_VERSION="${PROJECT_VERSION}"
  PKHEXQT_ORACLE_DATE="26.07.07")

function(pkhexqt_copy_lang target)
  add_custom_command(TARGET ${target} POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E make_directory "$<TARGET_FILE_DIR:${target}>/lang"
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
      "${CMAKE_SOURCE_DIR}/assets/lang/lang_en.txt"
      "$<TARGET_FILE_DIR:${target}>/lang/lang_en.txt"
    VERBATIM)
endfunction()

pkhexqt_copy_lang(pkhex-qt)
pkhexqt_copy_lang(pkhex-qt-host-tests)

function(pkhexqt_copy_about target)
  add_custom_command(TARGET ${target} POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E make_directory "$<TARGET_FILE_DIR:${target}>/about"
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
      "${CMAKE_SOURCE_DIR}/assets/about/shortcuts.txt"
      "${CMAKE_SOURCE_DIR}/assets/about/changelog.txt"
      "$<TARGET_FILE_DIR:${target}>/about"
    VERBATIM)
endfunction()

pkhexqt_copy_about(pkhex-qt)
pkhexqt_copy_about(pkhex-qt-host-tests)

set(ORACLE_SPRITES "${CMAKE_SOURCE_DIR}/PKHeX-26.07.07/PKHeX.Drawing.PokeSprite/Resources/img")
set(ORACLE_WALLPAPERS "${CMAKE_SOURCE_DIR}/PKHeX-26.07.07/PKHeX.Drawing.Misc/Resources/img/box")
function(pkhexqt_copy_assets target)
  add_custom_command(TARGET ${target} POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E make_directory
      "$<TARGET_FILE_DIR:${target}>/assets/sprites"
      "$<TARGET_FILE_DIR:${target}>/assets/wallpapers"
    COMMAND "${CMAKE_COMMAND}" -E copy_directory
      "${ORACLE_SPRITES}/Artwork Pokemon Sprites"
      "$<TARGET_FILE_DIR:${target}>/assets/sprites"
    COMMAND "${CMAKE_COMMAND}" -E copy_directory
      "${ORACLE_SPRITES}/Artwork Items"
      "$<TARGET_FILE_DIR:${target}>/assets/sprites"
    COMMAND "${CMAKE_COMMAND}" -E copy_directory
      "${ORACLE_SPRITES}/Pokemon Sprite Overlays"
      "$<TARGET_FILE_DIR:${target}>/assets/sprites"
    COMMAND "${CMAKE_COMMAND}" -E copy_directory
      "${ORACLE_SPRITES}/ball"
      "$<TARGET_FILE_DIR:${target}>/assets/sprites"
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
      "${ORACLE_SPRITES}/Big Pokemon Sprites/b_0.png"
      "${ORACLE_SPRITES}/Big Pokemon Sprites/b_unknown.png"
      "${ORACLE_SPRITES}/Big Pokemon Sprites/b_249x.png"
      "${ORACLE_SPRITES}/Big Items/bitem_unk.png"
      "${ORACLE_SPRITES}/Big Items/bitem_tr.png"
      "$<TARGET_FILE_DIR:${target}>/assets/sprites"
    COMMAND "${CMAKE_COMMAND}" -E copy_directory
      "${ORACLE_WALLPAPERS}"
      "$<TARGET_FILE_DIR:${target}>/assets/wallpapers"
    VERBATIM)
endfunction()

pkhexqt_copy_assets(pkhex-qt)

function(pkhexqt_copy_fonts target)
  add_custom_command(TARGET ${target} POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E make_directory
      "$<TARGET_FILE_DIR:${target}>/fonts"
      "$<TARGET_FILE_DIR:${target}>/assets/sprites"
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
      "${CMAKE_SOURCE_DIR}/PKHeX-26.07.07/PKHeX.WinForms/Resources/byte/PGLDings-NormalRegular.ttf"
      "$<TARGET_FILE_DIR:${target}>/fonts/PGLDings-NormalRegular.ttf"
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
      "${CMAKE_SOURCE_DIR}/PKHeX-26.07.07/PKHeX.WinForms/Resources/img/Accents/valid.png"
      "${CMAKE_SOURCE_DIR}/PKHeX-26.07.07/PKHeX.WinForms/Resources/img/Accents/warn.png"
      "$<TARGET_FILE_DIR:${target}>/assets/sprites"
    VERBATIM)
endfunction()

pkhexqt_copy_fonts(pkhex-qt)
pkhexqt_copy_fonts(pkhex-qt-host-tests)

add_test(NAME host_title_cli_open_save COMMAND pkhex-qt-host-tests)
set_tests_properties(host_title_cli_open_save PROPERTIES
  ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

if(DOTNET_EXE)
  if(PKHEXQT_SELF_CONTAINED)
    set(PKHEXQT_PUBLISH_SELF_CONTAINED 1)
  else()
    set(PKHEXQT_PUBLISH_SELF_CONTAINED 0)
  endif()
  add_custom_command(TARGET pkhex-qt POST_BUILD
    COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/scripts/publish-editor.sh"
      "${DOTNET_EXE}"
      "$<IF:$<CONFIG:Debug>,Debug,Release>"
      "$<TARGET_FILE_DIR:pkhex-qt>/dotnet"
      "${CMAKE_SOURCE_DIR}/editor/PKHeX.Editor.csproj"
      "${DOTNET_HOST_RID}"
      "${PKHEXQT_PUBLISH_SELF_CONTAINED}"
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
      "${NETHOST_DIR}/${NETHOST_LIB_NAME}"
      "$<TARGET_FILE_DIR:pkhex-qt>/${NETHOST_LIB_NAME}"
    COMMENT "Publishing Editor layer next to the Host"
    VERBATIM)
endif()
