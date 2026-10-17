cmake_minimum_required(VERSION 3.16)

project(FlyberryApp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Svg
    Network
    LinguistTools
)

find_package(spdlog REQUIRED)

add_executable(FlyberryApp
    main.cpp
    networkMenu.cpp
    menuButton.cpp
    settingsMenu.cpp
    systemMenu.cpp
    EnterEventFilter.cpp
    statusBar.cpp
    mainMenu.cpp
    languageHelper.cpp
    updateManager.cpp
    networkManager.cpp
    updateMenu.cpp
    statusDialog.cpp
    confirmDialog.cpp
    StartupDialog.cpp
)

target_link_libraries(FlyberryApp PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Svg
    Qt6::Network
    spdlog::spdlog
)

#
# Übersetzungen
#
set(TS_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/languages/flyberry_de.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/languages/flyberry_en.ts
)

qt_add_translations(FlyberryApp
    TS_FILES ${TS_FILES}
)


target_compile_definitions(FlyberryApp PRIVATE
    QT_TRANSLATIONS_DIR="/usr/translations"
)

#
# Installation
#
install(TARGETS FlyberryApp
    RUNTIME DESTINATION bin
)

install(
    FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/languages/flyberry_de.qm
        ${CMAKE_CURRENT_SOURCE_DIR}/languages/flyberry_en.qm
    DESTINATION translations
)
