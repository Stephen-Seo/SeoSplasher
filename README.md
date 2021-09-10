[SeoSplasher](https://github.com/Stephen-Seo/SeoSplasher/wiki/Downloads)
===========

![SeoSplasher](screenshots/00.png?raw=true)

A Bomberman clone with networked multiplayer capabilities.

[Downloads](https://github.com/Stephen-Seo/SeoSplasher/wiki/Downloads)

# Networked Multiplayer

If you want to try out multiplayer, the host must have port 12084 open.

# Controls

WASD to move

Spacebar to place a balloon.

E to trigger remote controlled balloons.

ESC to leave in-game.

Enter/Return to start/reset the game (cannot start/reset the game as a client).

# Compiling

Note: Requires `git submodule update --init` to get the ResourcePacker dependency (and also SFML).

Builds for the current platform. Mostly untested on Windows and Apple computers.

### Prerequisites

Requires [SFML2 built using CMake](http://sfml-dev.org/tutorials/2.1/compile-with-cmake.php) or the [corresponding pre-built libs of SFML2 on the SFML website.](http://sfml-dev.org/download/sfml/2.1/)

Note that if SFML2 is not installed on the system, then this project will attempt to compile SFML from source.

CMake gui recommended.

### Compiling with CMake

Configure and Generate makefiles using cmake (the CMakeLists.txt file is located in the root directory of this project. You can point CMake gui to this directory as the source directory.)

Then `make` to build the game.

Optionally, `make install` to install the built files.

(Note you can change the install directory by specifying `CMAKE_INSTALL_PREFIX` to another directory with cmake.)
