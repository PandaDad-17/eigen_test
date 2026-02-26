#ifndef __SDL_MGR_H__
#define __SDL_MGR_H__

#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>

#include <memory>
#include <string_view>

using SDL_WindowPtr = std::unique_ptr<SDL_Window, void(*)(SDL_Window*)>;
using SDL_RendererPtr = std::unique_ptr<SDL_Renderer, void(*)(SDL_Renderer*)>;

class SDL_Manager
{
  SDL_WindowPtr m_window{ nullptr, SDL_DestroyWindow };
  SDL_RendererPtr m_renderer{ nullptr, SDL_DestroyRenderer };

public:
  SDL_Manager() = default;
  ~SDL_Manager();

  SDL_Manager(const SDL_Manager& other) = delete;
  SDL_Manager& operator=(const SDL_Manager& other) = delete;
  
  SDL_Manager(SDL_Manager&& other) noexcept = default;
  SDL_Manager& operator=(SDL_Manager&& other) noexcept = default;

  [[nodiscard]] bool init(const std::string_view& windowTitle, int screenW, int screenH);
  void quit();

  [[nodiscard]] bool update();

  void preDraw();
  void postDraw();

  SDL_Renderer* renderer() const { return m_renderer.get(); }
};

#endif // __SDL_MGR_H__
