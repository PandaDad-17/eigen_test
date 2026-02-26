#ifndef __SCENE_H__
#define __SCENE_H__

#include <Eigen/Dense>

#include <string>

#include "camera.h"
#include "car.h"
#include "sdl_mgr.h"

struct AppConfig
{
  std::string windowTitle;
  int screenW, screenH;
  float fov;
  float cameraNearPlane, cameraFarPlane;
  float mouseSensitivity;
  float cameraSpeed;
  Eigen::Vector3f cameraStartPosition;
};

class alignas(16) Scene
{
  Eigen::Matrix4f m_projection{Eigen::Matrix4f::Identity()};
  
  AppConfig m_config;
  SDL_Manager m_sdl;

  Car m_car;
  Camera m_camera;
  Eigen::Vector2f m_screenDimensions;

  void updateProjection() noexcept;

  void handle_mouse(float deltaTime) noexcept;
  [[nodiscard]] bool handle_keyboard(float deltaTime) noexcept;

  void draw() const noexcept;

public:
  explicit Scene(AppConfig config);

  [[nodiscard]] bool run();
};

#endif // __SCENE_H__
