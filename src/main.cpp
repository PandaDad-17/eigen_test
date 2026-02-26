#include "scene.h"

int main(int argc, char* argv[]) 
{
  AppConfig config = {
    .windowTitle = "Eigen Test",
    .screenW = 800, .screenH = 600,
    .fov  = 60.f * (EIGEN_PI / 180.f), // 60 degress to radians
    .cameraNearPlane = 0.001f, .cameraFarPlane = 1000.f,
    .mouseSensitivity = 0.125f,
    .cameraSpeed = 7.5f,
    .cameraStartPosition = Eigen::Vector3f(0, 0, -10)
  };

  Scene scene(config);

  bool result = scene.run();
  return result ? 0 : -1;
}
