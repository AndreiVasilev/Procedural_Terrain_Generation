//
// Created by Andrei Vasilev on 4/3/17.
//

#ifndef PARTICLE_MOTION_SCREEN_H
#define PARTICLE_MOTION_SCREEN_H

#include <SDL2/SDL.h>
#include <vector>

class Screen {
public:
    // Public Methods
    Screen();
    virtual ~Screen();
    void update(const std::vector<Uint32> &);
    bool quit_program();

    // Public Members
    const static int SCREEN_WIDTH  {1400};
    const static int SCREEN_HEIGHT {800};

private:
    // Private Methods
    void init_SDL();
    void init_window();
    void init_renderer();
    void init_texture();
    void init_buffers();
    void load_map(const std::vector<Uint32> &);

    // Private Members
    SDL_Window *m_window     { nullptr };
    SDL_Renderer *m_renderer { nullptr };
    SDL_Texture *m_texture   { nullptr };
    Uint32 *m_main_buffer    { nullptr };
    SDL_Event m_event;

};


#endif //PARTICLE_MOTION_SCREEN_H
