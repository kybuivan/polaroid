# Polaroid

[![Build Status](https://github.com/kybuivan/polaroid/actions/workflows/windows.yml/badge.svg)](https://github.com/kybuivan/polaroid/actions)

Polaroid is a simple image viewer built using C++ and the ImGui library. It allows users to open and view images in a variety of formats.

<p align="center">
  <img alt="GitHub Language Count" src="https://img.shields.io/github/languages/count/kybuivan/polaroid" />
  <img alt="GitHub Top Language" src="https://img.shields.io/github/languages/top/kybuivan/polaroid" />
  <img alt="" src="https://img.shields.io/github/repo-size/kybuivan/polaroid" />
  <img alt="GitHub Issues" src="https://img.shields.io/github/issues/kybuivan/polaroid" />
  <img alt="GitHub Closed Issues" src="https://img.shields.io/github/issues-closed/kybuivan/polaroid" />
  <img alt="GitHub Pull Requests" src="https://img.shields.io/github/issues-pr/kybuivan/polaroid" />
  <img alt="GitHub Closed Pull Requests" src="https://img.shields.io/github/issues-pr-closed/kybuivan/polaroid" />
  <img alt="GitHub Contributors" src="https://img.shields.io/github/contributors/kybuivan/polaroid" />
  <img alt="GitHub Last Commit" src="https://img.shields.io/github/last-commit/kybuivan/polaroid" />
  <img alt="GitHub Commit Activity (Week)" src="https://img.shields.io/github/commit-activity/w/kybuivan/polaroid" />
  <img alt="GitHub Commit Activity (Month)" src="https://img.shields.io/github/commit-activity/m/kybuivan/polaroid" />
  <img alt="Github License" src="https://img.shields.io/github/license/kybuivan/polaroid" />
</p>

![screenshot](/screenshot/Capture1.PNG "screenshot")

## Getting started
Prerequisites
To build and run Polaroid, you will need the following:

- C++20
- CMake 3.16 or higher
- OpenGL 3.3 or higher
- [GLFW](https://github.com/glfw/glfw)
- [GLAD](https://github.com/Dav1dde/glad)
- [ImGui](https://github.com/ocornut/imgui)
- [nfd](https://github.com/mlabbe/nativefiledialog)
- [OpenCV](https://github.com/opencv/opencv)

## Building
To build Polaroid:

1. Clone the repository:
```bash
git clone https://github.com/kybuivan/polaroid.git
```
2. Initialize the cmake submodule recursively:
```bash
cd polaroid
git submodule update --init --recursive
```
3. Create a build directory:
```bash
mkdir build
cd build
```
4. Run CMake:
```bash
cmake ..
```
5. Build the project:
```bash
cmake --build .
```

## Running
To run Polaroid, simply execute the polaroid executable that was built in the previous step.

## Usage
Polaroid has a simple menu bar that allows users to open images and perform basic operations. The following options are available:

- File
	- New: Creates a new image window.
	- Open File...: Opens a file dialog that allows users to select an image file to open.
	- Open Folder...: Opens a file dialog that allows users to select a folder containing images to open.
	- Save As...: Saves the current image in the active window.
	- Save All: Saves all the images in the image list.
	- Exit: Exits the application.
- Edit
	- Undo: Not implemented.
	- Redo: Not implemented.
- Help
	- About: Not implemented.

## License
This project is licensed under the Apache-2.0 license - see the [LICENSE](https://github.com/kybuivan/polaroid/blob/main/LICENSE) file for details.

## Acknowledgments
This project was built using the following libraries: GLFW, GLAD, ImGui, nfd, and OpenCV.