SeoSplasher
===========

A Bomberman clone with networked multiplayer capabilities.

# Controls

WASD to move

Spacebar to place a balloon.

E to trigger remote controlled balloons.

ESC to leave in-game.

Enter/Return to start/reset the game (cannot start/reset the game as a client).

# Compiling

## Using Gradle

The gradle build scripts build for Linux 32/64 and Windows 32/64 on a Arch Linux 64-bit system.

### Prerequisites

Requires [ResourcePacker](https://github.com/Stephen-Seo/ResourcePacker) built using gradle. This can be found on ResourcePacker's gradle branch.

Requires [SFML2 built using gradle.](https://github.com/Stephen-Seo/SFML2_Gradle_BuildScripts)

Must be built on Arch Linux 64-bit with installed package groups multilib-devel and mingw-w64-toolchain.

SFML2_DIR must be set to the location of the SFML2 gradle build.

EXTLIBS_DIR must be set to the location of the `extlibs` directory inside SFML2 source zip from the SFML website. This is to enable windows builds.

ResourcePacker_DIR must be set to the location of the ResourcePacker gradle build.

These paths must be set with the -P option when using gradle.  
For example, `gradle -PSFML2_DIR=${PATH_TO_SFML2_DIR} tasks`

### Compiling with Gradle

Run gradle with `tasks` to get the available tasks to build. Run gradle with the needed task to build SeoSplasher.

## Using CMake

Builds for the current platform. Mostly untested on Windows and Apple computers.

### Prerequisites

Requires [ResourcePacker](https://github.com/Stephen-Seo/ResourcePacker) built using CMake.

Requires [SFML2 built using CMake](http://sfml-dev.org/tutorials/2.1/compile-with-cmake.php) or the [corresponding pre-built libs of SFML2 on the SFML website.](http://sfml-dev.org/download/sfml/2.1/)

CMake gui recommended.

### Compiling with CMake

Configure and Generate makefiles using cmake (the CMakeLists.txt file is located in the src directory. You can point CMake gui to the src directory as the source directory.)

Then `make` to build the game.

Optionally, `make install` to install the built files.

(Note you can change the install directory by specifying `CMAKE_INSTALL_PREFIX` to another directory with cmake.)
