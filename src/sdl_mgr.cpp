#include "sdl_mgr.h"

#include <SDL3/SDL.h>

SDL_Manager::~SDL_Manager()
{
  quit();
}

bool SDL_Manager::init(const std::string_view& windowTitle, int screenW, int screenH)
{
  if (!SDL_Init(SDL_INIT_VIDEO)) 
    return false;

  m_window.reset(SDL_CreateWindow(windowTitle.data(), screenW, screenH, 0));
  if (m_window == nullptr) 
    return false;

  m_renderer.reset(SDL_CreateRenderer(m_window.get(), nullptr));
  if (m_renderer == nullptr) 
    return false;

  SDL_SetWindowRelativeMouseMode(m_window.get(), true);
  SDL_SetRenderVSync(m_renderer.get(), 1);
  return true;
}

void SDL_Manager::quit()
{
  SDL_Quit();
}

bool SDL_Manager::update()
{
  SDL_Event event;
  while (SDL_PollEvent(&event))
  {
    if (event.type == SDL_EventType::SDL_EVENT_QUIT) [[unlikely]]
      return false;
  }
  return true;
}

void SDL_Manager::preDraw()
{
  SDL_SetRenderDrawColor(m_renderer.get(), 0, 0, 0, 0);
  SDL_RenderClear(m_renderer.get());
}

void SDL_Manager::postDraw()
{
  SDL_RenderPresent(m_renderer.get());
}
