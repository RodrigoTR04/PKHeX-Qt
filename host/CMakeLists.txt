cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/ui")

find_package(Qt6 REQUIRED COMPONENTS Widgets)

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
  src/LangCatalog.cpp
  src/SavToolChrome.cpp
)
set(HOST_HEADERS
  src/MainWindow.h
  src/EditorBridge.h
  src/LangCatalog.h
  src/SavToolChrome.h
)

add_executable(pkhex-qt
  src/main.cpp
  src/DotNetEditorBridge.cpp
  src/DotNetEditorBridge.h
  ${HOST_SOURCES}
  ${HOST_HEADERS}
  ui/MainWindow.ui
)
target_include_directories(pkhex-qt PRIVATE src "${NETHOST_DIR}")
target_link_libraries(pkhex-qt PRIVATE Qt6::Widgets nethost ${CMAKE_DL_LIBS})
set_target_properties(pkhex-qt PROPERTIES
  OUTPUT_NAME "pkhex-qt"
  BUILD_RPATH "$ORIGIN"
  INSTALL_RPATH "$ORIGIN")

add_executable(pkhex-qt-host-tests
  tests/host_smoke.cpp
  ${HOST_SOURCES}
  ${HOST_HEADERS}
  ui/MainWindow.ui
)
target_include_directories(pkhex-qt-host-tests PRIVATE src)
target_link_libraries(pkhex-qt-host-tests PRIVATE Qt6::Widgets)

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

add_test(NAME host_title_cli_open_save COMMAND pkhex-qt-host-tests)
set_tests_properties(host_title_cli_open_save PROPERTIES
  ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

if(DOTNET_EXE)
  add_custom_command(TARGET pkhex-qt POST_BUILD
    COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/scripts/publish-editor.sh"
      "${DOTNET_EXE}"
      "$<IF:$<CONFIG:Debug>,Debug,Release>"
      "$<TARGET_FILE_DIR:pkhex-qt>/dotnet"
      "${CMAKE_SOURCE_DIR}/editor/PKHeX.Editor.csproj"
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
      "${NETHOST_DIR}/${NETHOST_LIB_NAME}"
      "$<TARGET_FILE_DIR:pkhex-qt>/${NETHOST_LIB_NAME}"
    COMMENT "Publishing Editor layer next to the Host"
    VERBATIM)
endif()
