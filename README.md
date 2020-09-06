GiGi
====

GiGi is a C++ OpenGL GUI library.

Design Goals
------------

GG is designed to achieve several goals:

* Platform-independence: GG should be buildable on any platform that supports
  OpenGL.
* Driver-independence: GG should be a standalone GUI framework that does not
  require any particular application framework.
* Easy extensibility: new controls and GUI behavior should be easy to
  incorporate.
* Complete graphical control for the user/designer: the user should not be
  limited by the author's lack of artistic skill!
* Independence of UI elements from the source code: GG UI elements should be
  configurable as text files, so that UI design and alteration should not
  require a recompilation.
* Overall time efficiency: an application with a reasonable number of GG UI
  elements should not slow down significantly because of the rendering or
  handling of those elements.
* Overall space efficiency: each GG UI element should have a reasonably small
  data size.
* Efficient mixture of 2D and 3D graphical elements: it should be appropriate
  to use GG in any frame-based 3D application, even a realtime 3D video game.
* Simplicity of use: GG UI elements should be able to send arbitrary messages
  when manipulated by the user; there should be no message passing hierarchy
  imposed on the user.

Features
--------

GG has the following features and services:

* Communication of UI controls via signals and slots
* Support for 2D-, 3D-, and mixed-mode rendering
* Managment of textures and fonts
* Serialization of UI elements

Non-Features
------------

* GG is not fully threadsafe, due to its use of signals and slots
* No sound support is provided

Further Readings
----------------

For information on building GG from sources, see [Installing GG](INSTALLING.md).
For information on packaging GG for Linux distributions, see [Packaging GG](PACKAGING.md).
For the license used by GG, see [GNU LESSER GENERAL PUBLIC LICENSE](COPYING).
