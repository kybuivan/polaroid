# Polaroid
Polaroid is a simple image viewer built using C++ and the ImGui library. It allows users to open and view images in a variety of formats.

## Getting started
Prerequisites
To build and run Polaroid, you will need the following:

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
```css
git clone https://github.com/kybuivan/polaroid.git
```
2. Initialize the cmake submodule recursively:
```css
git submodule update --init --recursive
```
3. Create a build directory:
```bash
cd polaroid
mkdir build
cd build
```
4. Run CMake:
```bash
cmake ..
```
5. Build the project:
```css
cmake --build .
```

## Running
To run Polaroid, simply execute the polaroid executable that was built in the previous step.

## Usage
Polaroid has a simple menu bar that allows users to open images and perform basic operations. The following options are available:

- File
	- Open File...: Opens a file dialog that allows users to select an image file to open.
	- Open Folder...: Not implemented.
	- Save: Not implemented.
	- Save As...: Not implemented.
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