#include <SDL3/SDL.h>
#include <Eigen/Dense>
#include <vector>
#include <cmath>
#include "camera.h"

// Simple non-descript car model
const std::vector<Eigen::Vector3f> vertices = 
{
  // Body bottom
  {-2.0f, -0.5f,  1.0f}, { 2.0f, -0.5f,  1.0f}, { 2.0f, -0.5f, -1.0f}, {-2.0f, -0.5f, -1.0f},
  // Body top (beltline)
  {-2.0f,  0.2f,  1.0f}, { 2.0f,  0.2f,  1.0f}, { 2.0f,  0.2f, -1.0f}, {-2.0f,  0.2f, -1.0f},
  // Roof
  {-0.5f,  0.8f,  0.8f}, { 1.0f,  0.8f,  0.8f}, { 1.0f,  0.8f, -0.8f}, {-0.5f,  0.8f, -0.8f},
  // Wheels (represented by squares for simplicity)
  {-1.2f, -0.8f,  1.0f}, {-0.8f, -0.8f,  1.0f}, {-1.2f, -0.5f,  1.0f}, {-0.8f, -0.5f,  1.0f}, // FL
  { 0.8f, -0.8f,  1.0f}, { 1.2f, -0.8f,  1.0f}, { 0.8f, -0.5f,  1.0f}, { 1.2f, -0.5f,  1.0f}, // FR
  {-1.2f, -0.8f, -1.0f}, {-0.8f, -0.8f, -1.0f}, {-1.2f, -0.5f, -1.0f}, {-0.8f, -0.5f, -1.0f}, // RL
  { 0.8f, -0.8f, -1.0f}, { 1.2f, -0.8f, -1.0f}, { 0.8f, -0.5f, -1.0f}, { 1.2f, -0.5f, -1.0f}  // RR
};

const std::vector<Eigen::Vector2i> edges = 
{
  // Bottom rectangle
  {0, 1}, {1, 2}, {2, 3}, {3, 0},
  // Beltline rectangle
  {4, 5}, {5, 6}, {6, 7}, {7, 4},
  // Verticals body
  {0, 4}, {1, 5}, {2, 6}, {3, 7},
  // Roof rectangle
  {8, 9}, {9, 10}, {10, 11}, {11, 8},
  // Roof pillars
  {4, 8}, {5, 9}, {6, 10}, {7, 11},
  // Wheels (Front Left)
  {12, 13}, {13, 15}, {15, 14}, {14, 12},
  // Wheels (Front Right)
  {16, 17}, {17, 19}, {19, 18}, {18, 16},
  // Wheels (Rear Left)
  {20, 21}, {21, 23}, {23, 22}, {22, 20},
  // Wheels (Rear Right)
  {24, 25}, {25, 27}, {27, 26}, {26, 24}  
};  

// https://www.songho.ca/opengl/gl_projectionmatrix.html
Eigen::Matrix4f get_projection_matrix(float width, float height, const camera& cam)
{
  float aspect = width / height;
  float near = cam.near_plane();
  float far = cam.far_plane();
  
  float fov = 60.f * (EIGEN_PI / 180.f); // 60 degress to radians
  float tangent = std::tan(fov * 0.5f);

  float right = near * tangent;
  float top = right / aspect;

  Eigen::Matrix4f m = Eigen::Matrix4f::Zero();
  m(0, 0) = near / right;
  m(1, 1) = near / top;
  m(2, 2) = -(far + near) / (far - near);
  
  // Note: Eigen matrices are collumn major.
  m(2, 3) = -(2.f * far * near) / (far - near);
  m(3, 2) = -1;

  return m;
}

int main(int argc, char* argv[]) 
{
  if (SDL_Init(SDL_INIT_VIDEO) == false) 
  {
    return 1;
  }

  SDL_Window* window = SDL_CreateWindow("Eigen Test", 800, 600, 0);
  if (!window) 
  {
    SDL_Quit();
    return 1;
  }

  SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);
  if (!renderer) 
  {
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  SDL_SetWindowRelativeMouseMode(window, true);

  camera cam(Eigen::Vector3f(0, 0, -10), 0.001f, 1000.f);
  cam.look_at(Eigen::Vector3f::Zero());

  bool running = true;
  const float mouse_sensitivity = 0.002f;
  const float move_speed = 0.1f;

  while (running) 
  {
    SDL_Event event;
    while (SDL_PollEvent(&event)) 
    {
      switch (event.type)
      {
      case SDL_EVENT_QUIT:
        running = false;
        break;
      case SDL_EVENT_MOUSE_MOTION:
      {
        // x-movement = rotate around y-axis
        float yaw = -event.motion.xrel * mouse_sensitivity;
        
        // y-movement = rotate around x-axis
        // SDL y increases downward.
        float pitch = -event.motion.yrel * mouse_sensitivity;
         
        cam.rotate(pitch, yaw, 0.f);
        break;
      }
      default: 
        break;
      }
    }

    const bool* keys = SDL_GetKeyboardState(nullptr);
    if (keys[SDL_SCANCODE_ESCAPE]) running = false;

    if (keys[SDL_SCANCODE_W]) cam.move(0, 0, move_speed);
    if (keys[SDL_SCANCODE_S]) cam.move(0, 0, -move_speed);
    if (keys[SDL_SCANCODE_A]) cam.move(-move_speed, 0, 0);
    if (keys[SDL_SCANCODE_D]) cam.move(move_speed, 0, 0);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);

    Eigen::Matrix4f view = cam.get_view_matrix();
    Eigen::Matrix4f projection = get_projection_matrix(800, 600, cam);
    Eigen::Matrix4f VP = projection * view;

    // TODO: Have the model continue to spin using this.
    Eigen::Matrix4f model = Eigen::Matrix4f::Identity();
    Eigen::Matrix4f MVP = VP * model;

    auto project = [&](Eigen::Vector3f v) 
    {
      Eigen::Vector4f clip_space = MVP * v.homogeneous();

      // Cull vertices off camera.
      if (clip_space.w() < cam.near_plane())
        return SDL_FPoint{ -1.f, -1.f };

      // Normalized device coordinates (-1 to 1)
      Eigen::Vector3f ndc = clip_space.head<3>() / clip_space.w();

      Eigen::Vector2f screen_space(
        (ndc.x() + 1.f) * 0.5f * 800.f,
        (1.f - ndc.y()) * 0.5f * 600.f);

      return SDL_FPoint{ screen_space.x(), screen_space.y() };
    };

    for (const auto& edge : edges)
    {
      SDL_FPoint p1 = project(vertices[edge[0]]);
      SDL_FPoint p2 = project(vertices[edge[1]]);

      // Check for culled.
      if (p1.x >= 0 && p2.x >= 0)
        SDL_RenderLine(renderer, p1.x, p1.y, p2.x, p2.y);
    }

    SDL_RenderPresent(renderer);
    SDL_Delay(16);
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
