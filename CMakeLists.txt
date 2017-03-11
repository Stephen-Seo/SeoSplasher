cmake_minimum_required(VERSION 2.6)
project(Game_Project)

set(Game_Project_SOURCES
    src/animatedSprite.cpp
    src/connection.cpp
    src/context.cpp
    src/game.cpp
    src/gui.cpp
    src/musicPlayer.cpp
    src/networkIdentifiers.cpp
    src/resourceIdentifiers.cpp
    src/resourceManager.cpp
    src/sceneNode.cpp
    src/soundPlayer.cpp
    src/state.cpp
    src/stateStack.cpp
    src/ec/engine.cpp
    src/ec/entity.cpp
    src/ec/system.cpp
    src/seoSplasher/cPathFinderRef.cpp
    src/seoSplasher/cPlayerControl.cpp
    src/seoSplasher/nAIControl.cpp
    src/seoSplasher/nAnimated.cpp
    src/seoSplasher/nBalloon.cpp
    src/seoSplasher/nBreakable.cpp
    src/seoSplasher/nCControl.cpp
    src/seoSplasher/nDeath.cpp
    src/seoSplasher/nDraw.cpp
    src/seoSplasher/nMove.cpp
    src/seoSplasher/nPControl.cpp
    src/seoSplasher/nPFUpdater.cpp
    src/seoSplasher/nPickupHit.cpp
    src/seoSplasher/nSplosion.cpp
    src/seoSplasher/soundContext.cpp
    src/seoSplasher/soundEventManager.cpp
    src/seoSplasher/splashClient.cpp
    src/seoSplasher/splashMenu.cpp
    src/seoSplasher/splashServer.cpp
    src/seoSplasher/splashState.cpp
    src/seoSplasher/utility.cpp
    src/seoSplasher/AI/pathFinder.cpp
    src/seoSplasher/AI/uai.cpp
    src/main.cpp)

if(WIN32)
add_executable(Game_Project WIN32
    ${Game_Project_SOURCES})
else()
add_executable(Game_Project
    ${Game_Project_SOURCES})
endif()

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})

find_package(SFML 2 REQUIRED audio network graphics window system)

if(SFML_FOUND)
    include_directories(${SFML_INCLUDE_DIR})
    target_link_libraries(Game_Project ${SFML_LIBRARIES})
endif()

if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/ResourcePacker/src")
message(FATAL_ERROR "ResourcePacker not found! Please do `git submodule update --init`")
endif()

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/ResourcePacker/src")

target_include_directories(Game_Project
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/ResourcePacker/src")
target_link_libraries(Game_Project ResourcePacker)

target_compile_definitions(Game_Project
    PUBLIC ResourcePacker_FOUND)

if(WIN32)
    add_custom_command(TARGET Game_Project POST_BUILD
        COMMAND copy "${CMAKE_CURRENT_SOURCE_DIR}/seoSplasherResources.dat" "${CMAKE_CURRENT_BINARY_DIR}/seoSplasherResources.dat"
        BYPRODUCTS "${CMAKE_CURRENT_BINARY_DIR}/seoSplasherResources.dat")
else()
    add_custom_command(TARGET Game_Project POST_BUILD
        COMMAND cp "${CMAKE_CURRENT_SOURCE_DIR}/seoSplasherResources.dat" "${CMAKE_CURRENT_BINARY_DIR}/seoSplasherResources.dat"
        BYPRODUCTS "${CMAKE_CURRENT_BINARY_DIR}/seoSplasherResources.dat")
endif()

set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -D NDEBUG")

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to 'Debug', none was specified.")
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
endif()

install(TARGETS Game_Project DESTINATION bin)
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/seoSplasherResources.dat" DESTINATION bin)

if(WIN32)
    install(FILES "${SFML_ROOT}/bin/libsndfile-1.dll" DESTINATION bin)
    install(FILES "${SFML_ROOT}/bin/openal32.dll" DESTINATION bin)
    if(ResourcePacker_FOUND AND BUILD_SHARED_LIBS)
        install(FILES "${ResourcePacker_LIBRARY}" DESTINATION bin)
    endif()
    if(CMAKE_BUILD_TYPE MATCHES "Debug" AND BUILD_SHARED_LIBS)
        install(FILES "${SFML_ROOT}/bin/sfml-audio-d-2.dll" DESTINATION bin)
        install(FILES "${SFML_ROOT}/bin/sfml-graphics-d-2.dll" DESTINATION bin)
        install(FILES "${SFML_ROOT}/bin/sfml-network-d-2.dll" DESTINATION bin)
        install(FILES "${SFML_ROOT}/bin/sfml-system-d-2.dll" DESTINATION bin)
        install(FILES "${SFML_ROOT}/bin/sfml-window-d-2.dll" DESTINATION bin)
    elseif(BUILD_SHARED_LIBS)
        install(FILES "${SFML_ROOT}/bin/sfml-audio-2.dll" DESTINATION bin)
        install(FILES "${SFML_ROOT}/bin/sfml-graphics-2.dll" DESTINATION bin)
        install(FILES "${SFML_ROOT}/bin/sfml-network-2.dll" DESTINATION bin)
        install(FILES "${SFML_ROOT}/bin/sfml-system-2.dll" DESTINATION bin)
        install(FILES "${SFML_ROOT}/bin/sfml-window-2.dll" DESTINATION bin)
    endif()
endif()
