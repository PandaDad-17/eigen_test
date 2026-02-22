#include <SDL3/SDL.h>
#include <vector>
#include <cmath>

struct Vec3 
{
  float x, y, z;
};

struct Edge
{
  int v1, v2;
};

// Simple non-descript car model
const std::vector<Vec3> vertices = 
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

const std::vector<Edge> edges = 
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

int main(int argc, char* argv[]) 
{
  if (SDL_Init(SDL_INIT_VIDEO) == false) 
  {
    return 1;
  }

  SDL_Window* window = SDL_CreateWindow("SDL3 Wireframe Car", 800, 600, 0);
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

  bool running = true;
  float angle = 0.0f;

  while (running) 
  {
    SDL_Event event;
    while (SDL_PollEvent(&event)) 
    {
      if (event.type == SDL_EVENT_QUIT) 
      {
        running = false;
      }
      else if (event.type == SDL_EVENT_KEY_DOWN) 
      {
        if (event.key.key == SDLK_ESCAPE) 
        {
          running = false;
        }
      }
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);

    angle += 0.01f;
    float cosA = cos(angle);
    float sinA = sin(angle);

    auto project = [&](Vec3 v) 
    {
      // Rotate around Y axis
      float x = v.x * cosA - v.z * sinA;
      float z = v.x * sinA + v.z * cosA;
      float y = v.y;

      // Simple perspective projection
      z += 5.0f; // Move back
      float fov = 400.0f;
      float px = (x * fov) / z + 400.0f;
      float py = (-y * fov) / z + 300.0f;
      return SDL_FPoint{px, py};
    };

    for (const auto& edge : edges)
    {
      SDL_FPoint p1 = project(vertices[edge.v1]);
      SDL_FPoint p2 = project(vertices[edge.v2]);
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
