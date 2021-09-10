cmake_minimum_required(VERSION 3.7)
project(Game_Project)

set(Game_Project_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/animatedSprite.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/connection.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/context.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/game.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/gui.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/musicPlayer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/networkIdentifiers.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/resourceIdentifiers.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/resourceManager.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/sceneNode.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/soundPlayer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/state.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/stateStack.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ec/engine.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ec/entity.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ec/system.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/seoSplasher/cPathFinderRef.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/seoSplasher/cPlayerControl.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/seoSplasher/nAIControl.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/seoSplasher/nAnimated.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/seoSplasher/nBalloon.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/seoSplasher/nBreakable.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/seoSplasher/nCControl.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/seoSplasher/nDeath.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/seoSplasher/nDraw.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/seoSplasher/nMove.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/seoSplasher/nPControl.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/seoSplasher/nPFUpdater.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/seoSplasher/nPickupHit.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/seoSplasher/nSplosion.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/seoSplasher/soundContext.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/seoSplasher/soundEventManager.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/seoSplasher/splashClient.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/seoSplasher/splashMenu.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/seoSplasher/splashServer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/seoSplasher/splashState.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/seoSplasher/utility.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/seoSplasher/AI/pathFinder.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/seoSplasher/AI/uai.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ResourcePacker/src/ResourcePacker.cpp"
)

if(WIN32)
add_executable(SeoSplasher WIN32
    ${Game_Project_SOURCES})
else()
add_executable(SeoSplasher
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
    target_link_libraries(SeoSplasher ${SFML_LIBRARIES})
else()
    message(STATUS "SFML not found, building it")

    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/SFML/src")
        message(FATAL_ERROR "SFML submodule not loaded, please do `git submodule update --init`!")
    endif()

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
    target_include_directories(SeoSplasher PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/SFML/include")
    target_link_libraries(SeoSplasher sfml-system sfml-window sfml-graphics sfml-audio sfml-network)
endif()

if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/ResourcePacker/src")
message(FATAL_ERROR "ResourcePacker not found! Please do `git submodule update --init`")
endif()

target_include_directories(SeoSplasher
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/ResourcePacker/src")

target_compile_definitions(SeoSplasher
    PUBLIC ResourcePacker_FOUND)

if(WIN32 AND NOT MINGW)
    add_custom_command(TARGET SeoSplasher POST_BUILD
        COMMAND copy "${CMAKE_CURRENT_SOURCE_DIR}/seoSplasherResources.dat" "${CMAKE_CURRENT_BINARY_DIR}/seoSplasherResources.dat"
        BYPRODUCTS "${CMAKE_CURRENT_BINARY_DIR}/seoSplasherResources.dat")
else()
    add_custom_command(TARGET SeoSplasher POST_BUILD
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

install(TARGETS SeoSplasher DESTINATION bin)
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/seoSplasherResources.dat" DESTINATION opt/SeoSplasher)
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/packaging/SeoSplasher.desktop" DESTINATION share/applications)
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/packaging/icons" DESTINATION share)
