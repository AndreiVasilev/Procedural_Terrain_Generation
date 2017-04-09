//
// Created by Andrei Vasilev on 4/3/17.
//

#ifndef TERRAIN_TERRAIN_H
#define TERRAIN_TERRAIN_H

#include <SDL2/SDL.h>
#include <vector>
#include <utility>

class Terrain {

public:
    // Public Methods
    Terrain() = default;
    Terrain(const unsigned, const double);
    std::vector<Uint32> get_map() const;

private:
    using coord_pair = std::pair<int, int>;

    // Private Methods
    void generate_maps();
    void draw_map();
    void divide(const int);
    void set_height(const int, const int, const double);
    void square(const int, const int, const int, const double);
    void diamond(const int, const int, const int, const double);
    void draw_rect(const coord_pair, const coord_pair, const Uint32);
    bool within_map(const int, const int) const;
    double get_height(const int, const int) const;
    double average(const std::vector<double> &) const;
    Uint32 water_brightness(const int, const int) const;
    Uint32 slope_brightness(const int, const int, const double) const;
    Uint32 get_color(const Uint8, const Uint8, const Uint8, const Uint8) const;
    coord_pair project_map(const int, const int, const double) const;

    // Private Members
    unsigned map_size      {0};     // Map dimensions, map is a cube, l = w = h
    unsigned map_area      {0};     // Area of 1 cross section, l * w
    unsigned screen_adjust {0};     // Adjustment value so map is centered in window
    double map_roughness   {0};     // General roughness to map terrain
    std::vector<double> height_map; // Map to hold all height values of terrain
    std::vector<Uint32> color_map;  // Map to hold color values to be loaded on screen
};


#endif //TERRAIN_TERRAIN_H
