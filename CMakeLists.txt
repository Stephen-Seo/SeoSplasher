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

if("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
    set(IS_32_BIT False)
else()
    set(IS_32_BIT true)
endif()

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})

find_package(SFML 2 QUIET COMPONENTS audio network graphics window system)

if(SFML_FOUND)
    message(STATUS "SFML found, using it")
    include_directories(${SFML_INCLUDE_DIR})
    target_link_libraries(Game_Project ${SFML_LIBRARIES})
else()
    message(STATUS "SFML not found, building it")

    # use extlibs
    if(WIN32)
        if(MINGW)
            set(EXTLIBS_LIBSDIR "libs-mingw")
        elseif(MSVC)
            set(EXTLIBS_LIBSDIR "libs-msvc")
        endif()
        if(IS_32_BIT)
            set(EXTLIBS_LIBSSUBDIR "x86")
        else()
            set(EXTLIBS_LIBSSUBDIR "x64")
        endif()
        set(JPEG_LIBRARY "${CMAKE_CURRENT_SOURCE_DIR}/SFML/extlibs/${EXTLIBS_LIBSDIR}/${EXTLIBS_LIBSSUBDIR}/libjpeg.a")
        set(JPEG_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/SFML/extlibs/headers/jpeg")
        set(OPENAL_LIBRARY "${CMAKE_CURRENT_SOURCE_DIR}/SFML/extlibs/${EXTLIBS_LIBSDIR}/${EXTLIBS_LIBSSUBDIR}/libopenal32.a")
        set(OPENAL_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/SFML/extlibs/headers/AL")
        set(VORBIS_LIBRARY "${CMAKE_CURRENT_SOURCE_DIR}/SFML/extlibs/${EXTLIBS_LIBSDIR}/${EXTLIBS_LIBSSUBDIR}/libvorbis.a")
        set(VORBIS_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/SFML/extlibs/headers/vorbis")
        set(VORBISFILE_LIBRARY "${CMAKE_CURRENT_SOURCE_DIR}/SFML/extlibs/${EXTLIBS_LIBSDIR}/${EXTLIBS_LIBSSUBDIR}/libvorbisfile.a")
        set(VORBISENC_LIBRARY "${CMAKE_CURRENT_SOURCE_DIR}/SFML/extlibs/${EXTLIBS_LIBSDIR}/${EXTLIBS_LIBSSUBDIR}/libvorbisenc.a")
        set(OGG_LIBRARY "${CMAKE_CURRENT_SOURCE_DIR}/SFML/extlibs/${EXTLIBS_LIBSDIR}/${EXTLIBS_LIBSSUBDIR}/libogg.a")
        set(OGG_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/SFML/extlibs/headers/ogg")
        set(FLAC_LIBRARY "${CMAKE_CURRENT_SOURCE_DIR}/SFML/extlibs/${EXTLIBS_LIBSDIR}/${EXTLIBS_LIBSSUBDIR}/libFLAC.a")
        set(FLAC_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/SFML/extlibs/headers")
        set(FREETYPE_LIBRARY "${CMAKE_CURRENT_SOURCE_DIR}/SFML/extlibs/${EXTLIBS_LIBSDIR}/${EXTLIBS_LIBSSUBDIR}/libfreetype.a")
        include_directories("${CMAKE_CURRENT_SOURCE_DIR}/SFML/extlibs/headers/freetype2")
    endif()

    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/SFML")
    target_include_directories(Game_Project PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/SFML/include")
    target_link_libraries(Game_Project sfml-system sfml-window sfml-graphics sfml-audio sfml-network)
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

