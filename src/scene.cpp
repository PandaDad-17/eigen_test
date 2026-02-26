#include "scene.h"

#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_timer.h>

Scene::Scene(AppConfig config)
  : m_config(std::move(config))
  , m_screenDimensions(static_cast<float>(config.screenW), static_cast<float>(config.screenH))
{
  m_camera.set(config.cameraStartPosition, config.cameraNearPlane, config.cameraFarPlane, config.fov);
  m_camera.lookAt(Eigen::Vector3f::Zero());
  updateProjection();
}

void Scene::updateProjection() noexcept
{
  // https://www.songho.ca/opengl/gl_projectionmatrix.html
  float aspect = m_screenDimensions.x() / m_screenDimensions.y();
  float near = m_camera.nearPlane();
  float far = m_camera.farPlane();
  
  float fov = m_camera.fov();
  float tangent = std::tan(fov * 0.5f);

  float right = near * tangent;
  float top = right / aspect;

  m_projection = Eigen::Matrix4f::Zero();
  m_projection(0, 0) = near / right;
  m_projection(1, 1) = near / top;
  m_projection(2, 2) = -(far + near) / (far - near);
  
  // Note: Eigen matrices are collumn major.
  m_projection(2, 3) = -(2.f * far * near) / (far - near);
  m_projection(3, 2) = -1;
}

bool Scene::run()
{
  if (!m_sdl.init(m_config.windowTitle, m_config.screenW, m_config.screenH))
    return false;

  const float perfFreqRatio = 1.f / SDL_GetPerformanceFrequency();
  float deltaTime = 0.f;
  uint64_t prevFrameTime = SDL_GetPerformanceCounter();

  bool running = true;
  while(running)
  {
    uint64_t curFrameTime = SDL_GetPerformanceCounter();
    deltaTime = (curFrameTime - prevFrameTime) * perfFreqRatio;

    if (running = m_sdl.update(); !running)
      break;

    handle_mouse(deltaTime);
    if (running = handle_keyboard(deltaTime); !running)
      break;

    m_sdl.preDraw();
    draw();
    m_sdl.postDraw();

    prevFrameTime = curFrameTime;
  }

  return true;
}

void Scene::handle_mouse(float deltaTime) noexcept
{
  float xrel, yrel;
  SDL_GetRelativeMouseState(&xrel, &yrel);

  // x-movement = rotate around y-axis
  // Inverse to emulate head turning in that direction by rotating the camera the opposite way. 
  float yaw = -xrel * m_config.mouseSensitivity * deltaTime;
  
  // y-movement = rotate around x-axis
  // INverse because SDL y increases downward.
  float pitch = -yrel * m_config.mouseSensitivity * deltaTime;
    
  m_camera.rotate(pitch, yaw, 0.f);
}

bool Scene::handle_keyboard(float deltaTime) noexcept
{
  const bool* keys = SDL_GetKeyboardState(nullptr);
  if (keys[SDL_SCANCODE_ESCAPE])
    return false;

  Eigen::Vector3f axis = Eigen::Vector3f::Zero();
  if (keys[SDL_SCANCODE_W])
    axis.z() += 1.f;
  if (keys[SDL_SCANCODE_S])
    axis.z() -= 1.f;
  if (keys[SDL_SCANCODE_A])
    axis.x() -= 1.f;
  if (keys[SDL_SCANCODE_D])
    axis.x() += 1.f;

  if (!axis.isZero())
    m_camera.move(axis * m_config.cameraSpeed * deltaTime);

  return true;
}

void Scene::draw() const noexcept
{
  SDL_SetRenderDrawColor(m_sdl.renderer(), 0, 255, 0, 255);

  Eigen::Matrix4f view = m_camera.viewMatrix();
  Eigen::Matrix4f VP = m_projection * view;

  // TODO: Have the model continue to spin using this.
  Eigen::Matrix4f model = Eigen::Matrix4f::Identity();
  Eigen::Matrix4f MVP = VP * model;

  float screenHalfW = m_screenDimensions.x() * 0.5f;
  float screenHalfH = m_screenDimensions.y() * 0.5f;

  auto project = [&](Eigen::Vector3f v) 
  {
    Eigen::Vector4f clipSpace = MVP * v.homogeneous();

    // Cull vertices off camera.
    if (clipSpace.w() < m_camera.nearPlane())
      return SDL_FPoint{ -1.f, -1.f };

    // Normalized device coordinates (-1 to 1)
    Eigen::Vector3f ndc = clipSpace.head<3>() / clipSpace.w();

    Eigen::Vector2f screen_space(
      (ndc.x() + 1.f) * screenHalfW,
      (1.f - ndc.y()) * screenHalfH);

    return SDL_FPoint{ screen_space.x(), screen_space.y() };
  };

  const auto& verts = m_car.vertices();
  for (const auto& edge : m_car.edges())
  {
    SDL_FPoint p1 = project(verts[edge[0]]);
    SDL_FPoint p2 = project(verts[edge[1]]);

    // Check for culled.
    if (p1.x >= 0 && p2.x >= 0)
      SDL_RenderLine(m_sdl.renderer(), p1.x, p1.y, p2.x, p2.y);
  }
}
