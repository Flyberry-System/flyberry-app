cmake_minimum_required(VERSION 3.16)
project(QtButtonsApp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Qt finden (Qt6 bevorzugt, sonst Qt5)
find_package(Qt6 6.2 REQUIRED COMPONENTS Core Gui Widgets  Svg LinguistTools)



# --- Übersetzungen (.ts -> .qm) ---
set(TS_FILES
    languages/flyberry_de.ts
    languages/flyberry_en.ts
)

qt_add_translation(QM_FILES ${TS_FILES})


# Executable zuerst definieren
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
)

# Linken
target_link_libraries(
	FlyberryApp PRIVATE 
	
	Qt${QT_VERSION_MAJOR}::Widgets 
	Qt${QT_VERSION_MAJOR}::Svg 
	Qt${QT_VERSION_MAJOR}::Core
	Qt${QT_VERSION_MAJOR}::Gui 
)

# Pfad zu den Qt6-Translations auf dem System
set(QT_TRANSLATIONS_DIR "/home/markus/flyberry-app/languages")

# Definiere den Pfad als Preprocessor-Makro für C++
add_definitions(-DQT_TRANSLATIONS_DIR="${QT_TRANSLATIONS_DIR}")

# --- Installation (optional) ---
install(TARGETS FlyberryApp DESTINATION bin)
# QM_FILES enthält die erzeugten .qm-Dateien (liegen im Build-Verzeichnis!)
# Beim Installieren richtig ablegen:
install(FILES ${QM_FILES} DESTINATION ${CMAKE_INSTALL_DATADIR}/flyberry/translations)


# QM-Dateien in Build-Verzeichnis
foreach(qm ${QM_FILES})
    add_custom_command(
        TARGET FlyberryApp POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${qm}
                ${CMAKE_BINARY_DIR}
    )
endforeach()









