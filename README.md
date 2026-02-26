@ -1 +1,65 @@
# eigen_test
# Eigen Test: Modern C++20 3D Renderer

A simple, low-poly car model wireframe rendered manually to an SDL screen using CPU rendering. It's a perfect demonstration of handling the linear algebra and projection mathematics with the **Eigen** library.

## Controls

* **WASD**: Standard first-person movement of forward/backward and strafing.
* **Mouse**: Look around with pitch and yaw
* **Escape**: Quit the application  

## Features

* **Manual 3D Pipeline**: Custom implementation of Model, View, and Projection (MVP) transformations.
* **Optimized Camera**: Using **Quaternions** avoids Gimbal Lock and faster rotation calculation than matrices.
* **Simple Near-Plane Clipping**: Prevents rendering artifacts when vertices pass behind the camera.
* **Frame-Rate Independence**: Delta-timing ensures consistent movement speeds regardless of hardware performance.
* **SIMD-Accelerated Math**: My code is optimized for 16-byte alignment to leverage parallelism in matrix and vector operations done by **Eigen**.
* **Modern C++ Architecture**:
    * **Memory Safety**: Smart management of SDL3 resources using `std::unique_ptr` with custom deleters.
    * **Performance Hints**: `[[nodiscard]]`, `noexcept`, and `alignas(16)` used to maximize compiler optimization.
    * **Design**: Rule of Five adhered to in class definitions.

### Projection & Clipping

For more information on the specifics as to how this math works, please see [Learn OpenGL's "Coordinate Systems" page](https://learnopengl.com/Getting-started/Coordinate-Systems).

1. The camera and the car exist in world space, a virtual 3D space with a non-direct translation of one unit to any real world unit of measurement like feet, meters, etc. 
2. A rotation matrix representing the direction of the camera's frustrum (mimicking a human eye) can be transposed and used to unwrap vertices of objects in world space into view space.
  * Now the vertices of objects exist in the space of what the camera can see.
  * Yes, in every 3D game you've veer played, you are not moving through the world, but the world is moving around you in reverse.
3. A projection matrix is used to project what the camera sees into a normalized clip space in the range of -1 to 1.
  * This is done with normal linear algebra matrix multiplication.
  * M = A transform (affine) matrix of rotation, translation, and scale that allows the object to move and rotate (not featured in this project).
  * V = The view matrix calculated by the camera.
  * P = The projection matrix.
  * To accomplish this in Eigen's order of operations it's P * V * M * vertex.
4. Clip space is in 4D, and can be converted to normalized device coordinates (NDC) by dividing the x,y,z coordinates by the homogenous w coordinate.
5. The projection is complete when the coordinates are "stretched" to the dimensions of the screen (the drawing area of the app window).
6. What can be seen depends no a near and far clip plane to "clip" vertices out that are behind the camera and too far away to see.
  * You can see this in this project by moving close to the car and seeing the edges disappear.
7. By using quaternions instead of Eular angles, the camera avoids loss of degree of freedom (Gimbal Lock).
  * This happens when the pitch is rotated 90 degrees and looks up at the sky.
  * Yaw depends on pitch and roll depends on yaw to calulate the up and forward axes.
  * This means that when trying to manipulate them, they spin around like a top while the camera is stuck looking up.
  * By being a rotation around an arbitrary vector in 4D space, a quaternion does not have this dependency.

## Building and Running

### Prerequisites
* A C++20 compatible compiler (Clang 10+, GCC 10+, or MSVC 19.29+)
* CMake version 3.24 or higher
* git

### Build Instructions
```bash
# Clone the repository
git clone [https://github.com/PandaDad-17/eigen_test.git](https://github.com/PandaDad-17/eigen_test.git)
cd eigen_test

# Configure and Build
cmake -S . -B build
cmake --build build

# Run
./build/test
```

## Time spent

The point of this project was to show what I could do in about full work day (8 hours).

1. **Project Setup** This commit took me about 2 hours. I am used to taking more control over CMake and using dependencies as subdmodules. Whenever looking into how to include Eigen most people reccommended using fetching in CMake, so I got quite a bit involved in learning how to best optimize that and utilizing CMake for VSCode build tasks instead of manually creating them myself. I used AI Gemini to give me a simple project template using SDL that draws a low-poly car wireframe model.
2. **Added Camera** This commit took 4 hours. Besides the camera, I had to do the projection matrix and modify the template main function to utilize my view/projection and Eigen class variables.
3. **Modularization and optimization** This commit took 2 hours. The most time spent was connecting of localized code between the classes. I accomplished most of this using the AppConfig class to help get rid of magic numbers. Converting to using quaternions was about 30 minutes. I admittedly struggle with how the math of wuaternions work but Eigen is incredibly easy to use and made this step significantly faster than it would have been if I was responsible for this math like the projection matrix.
