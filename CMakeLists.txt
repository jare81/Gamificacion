cmake_minimum_required(VERSION 3.19)
project(Gamificacion LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)
find_package(Qt6 REQUIRED COMPONENTS Core)

qt_standard_project_setup()

qt_add_executable(Gamificacion
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui


    prueba.cpp
    prueba.h
    sprite.h sprite.cpp
    pictures.qrc
    recursos.qrc
    ruleta.h ruleta.cpp
    escenainterior.h escenainterior.cpp escenainterior.ui
    mapa.h mapa.cpp mapa.ui

        recursos.qrc
        escenahistoria.h escenahistoria.cpp escenahistoria.ui
        colapreguntas.h colapreguntas.cpp
        pregunta.h
        spritevillanahistoria.h spritevillanahistoria.cpp
        escenaciencia.h escenaciencia.cpp escenaciencia.ui
        escenapolitica.h escenapolitica.cpp escenapolitica.ui
        escenaarte.h escenaarte.cpp escenaarte.ui
        batalla.h batalla.cpp batalla.ui
        sprite2.cpp
        spritev.h spritev.cpp
        gamestate.h gamestate.cpp
        spriter.h spriter.cpp

        chooseapectdialog.h chooseapectdialog.cpp chooseapectdialog.ui
        isSprite.h



)

target_link_libraries(Gamificacion
    PRIVATE
        Qt6::Core
        Qt6::Widgets
)
target_link_libraries(Gamificacion PRIVATE Qt6::Core)

include(GNUInstallDirs)

install(TARGETS Gamificacion
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET Gamificacion
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})

find_package(Qt6 REQUIRED COMPONENTS Widgets Multimedia)

target_link_libraries(Gamificacion
    PRIVATE Qt6::Widgets
    Qt6::Multimedia
)


