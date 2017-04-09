//
// Created by Andrei Vasilev on 4/3/17.
//

#include "../include/Screen.h"
#include <iostream>

Screen::Screen() : m_window{nullptr}, m_renderer{nullptr}, m_texture{nullptr}, m_main_buffer{nullptr} {
    // Initializes all required SDL functionality and creates SDL objects.
    init_SDL();
    init_window();
    init_renderer();
    init_texture();
    init_buffers();
}

Screen::~Screen() {
    // Destroys all SDL related objects.
    delete [] m_main_buffer;
    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyTexture(m_texture);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}

void Screen::init_SDL() {
    // Initializes SDL library. Returns 0 if successful.
    // Must be called before using any SDL functionality.
    if(SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "SDL_Init Error: \n" << SDL_GetError() << std::endl;
        exit(1);
    }
}


void Screen::init_window() {
    // Creates an SDL window struct. Returns NULL on failure.
    m_window = SDL_CreateWindow(
            "Terrain Generation",        // Window title
            SDL_WINDOWPOS_UNDEFINED,     // Initial x position
            SDL_WINDOWPOS_UNDEFINED,     // Initial y position
            SCREEN_WIDTH,                // Width, in pixels
            SCREEN_HEIGHT,               // Height, in pixels
            SDL_WINDOW_ALLOW_HIGHDPI     // Increases resolution
    );

    // Checks if window creation failed.
    if(!m_window) {
        std::cerr << "SDL_CreateWindow Error: \n" << SDL_GetError() << std::endl;
        exit(2);
    }
}


void Screen::init_renderer() {
    // Creates an SDL renderer struct: a rendering context relative to the state/information
    // of the associated window. Textures to be rendered pass through this renderer and when
    // done rendering are sent to the window to be displayed.
    m_renderer = SDL_CreateRenderer(
            m_window,                      // Window associated with the renderer.
            -1,                            // Index of rendering driver to initialize, -1 to use first available.
            SDL_RENDERER_PRESENTVSYNC |    // Synchronize rendering with window refresh rate.
            SDL_RENDERER_ACCELERATED  |    // Allow renderer to use hardware acceleration.
            SDL_RENDERER_TARGETTEXTURE);   // Renderer supports rendering to textures.

    // Check if renderer creation failed.
    if(!m_renderer) {
        std::cerr << "SDL_CreateRenderer Error: \n" << SDL_GetError() << std::endl;
        SDL_DestroyWindow(m_window);
        exit(3);
    }

    // Sets renderer blend mode for alpha channel
    SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);

    // Sets color that screen will be filled with when renderer is cleared
    SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 0);
}


void Screen::init_texture() {
    // Creates an SDL texture struct: a set of pixel data to be sent to the renderer.
    m_texture = SDL_CreateTexture(
            m_renderer,                    // The rendering context that this texture will pass through.
            SDL_PIXELFORMAT_RGBA8888,      // The format of the pixel color information, RGBA 8 bits per channel.
            SDL_TEXTUREACCESS_STATIC,      // How the texture is accessed when updated.
            SCREEN_WIDTH,                  // Texture map_width in pixels, equals associated window map_width.
            SCREEN_HEIGHT                  // Texture height in pixels, equals associated window height.
    );

    // Checks if texture creation failed.
    if(!m_texture) {
        std::cerr << "SDL_CreateTexture Error: \n" << SDL_GetError() << std::endl;
        SDL_DestroyRenderer(m_renderer);
        SDL_DestroyWindow(m_window);
        exit(4);
    }

    // Sets texture blend mode for alpha channel
    SDL_SetTextureBlendMode(m_texture, SDL_BLENDMODE_BLEND);
}


void Screen::init_buffers() {
    // Initialize main_buffer and blur_buffer memory. During runtime, buffers are loaded with
    // particle objects and then fed to the SDL texture to be rendered within the SDL window.
    m_main_buffer = new Uint32[SCREEN_HEIGHT * SCREEN_WIDTH];
    memset(m_main_buffer, 0, SCREEN_HEIGHT * SCREEN_WIDTH * sizeof(Uint32));
}


void Screen::update(const std::vector<Uint32> &map) {

    // Clears current screen and renderer
    SDL_RenderClear(m_renderer);

    // Loads color values of color map into pixel buffer
    load_map(map);

    // Updates the texture containing the new pixel data.
    SDL_UpdateTexture(
            m_texture,                      // Texture to be updated.
            NULL,                           // Area in texture to update, NULL for entire map_area.
            m_main_buffer,                  // Updated pixel data being sent to the texture.
            SCREEN_WIDTH * sizeof(Uint32)   // Number of bytes in a row of pixel data.
    );

    // Copies the newly updated texture into the renderer.
    SDL_RenderCopy(
            m_renderer,    // The renderer to be updated.
            m_texture,     // The source texture.
            NULL,          // Amount of texture to be copied, NULL for entire texture.
            NULL           // Amount of renderer to be updated, NULL for entire renderer.
    );

    // Loads the renderer to the SDL window.
    SDL_RenderPresent(m_renderer);
}

// Loads color values of color map into main screen buffer
void Screen::load_map(const std::vector<Uint32> &map) {
    for(int x = 0; x < SCREEN_WIDTH; ++x)
        for(int y = 0; y < SCREEN_HEIGHT; ++y)
            m_main_buffer[x + y * SCREEN_WIDTH] = map[x + y * SCREEN_WIDTH];
}

bool Screen::quit_program() {
    // Check for SDL events. If the window is closed, quit the program.
    while(SDL_PollEvent(&m_event)) {
        if(m_event.type == SDL_QUIT)
            return true;
    }
    return false;
}