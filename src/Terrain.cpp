//
// Created by Andrei Vasilev on 4/3/17.
//

#include "../include/Terrain.h"
#include "../include/Screen.h"
#include <random>

// Terrain Constructor
Terrain::Terrain(const unsigned size, const double roughness) :
        map_size       {size},
        map_area       {size * size},
        map_roughness  {roughness},
        screen_adjust  {(Screen::SCREEN_WIDTH - size)/2}
{
    // Reserves memory & initializes height and color maps
    height_map.reserve(map_area);
    color_map.reserve(Screen::SCREEN_WIDTH * Screen::SCREEN_HEIGHT);
    for(int i = 0; i < map_area; i++) {
        height_map.push_back(-10);
    }
    for(int i = 0; i < Screen::SCREEN_WIDTH * Screen::SCREEN_HEIGHT; i++) {
        color_map.push_back(0);
    }

    // Generates height and color maps
    generate_maps();
}

// Returns color map to be rendered on screen
std::vector<Uint32> Terrain::get_map() const { return color_map; }

// Sets height at given coordinates in height map
void Terrain::set_height(const int x_coord, const int z_coord, const double y_coord) {
    if(within_map(x_coord, z_coord))                           // Check if coordinates are within map
        height_map[x_coord + (z_coord * map_size)] = y_coord;  // Set height at x,z coordinate to height y
}

// Gets height at given coordinates in height map, return -1 if outside map
double Terrain::get_height(const int x_coord, const int z_coord) const {
    if(!within_map(x_coord, z_coord)) return -1;        // Check if coordinates are within map
    return height_map[x_coord + (z_coord * map_size)];  // Get height at x,z coordinates of map
}

// Checks to see if a set of given coordinates are within the map boundaries
bool Terrain::within_map(const int x, const int z) const {
    return (x >= 0 && x < map_size && z >= 0 && z < map_size);
}

// Generates a height map using Diamond Square algorithm, then
// transposes the height map into the color map
void Terrain::generate_maps() {
    std::random_device rd;
    std::uniform_real_distribution<double> dist(0, 1);

    // Set initial height of corners to random value between 0 and max height
    set_height(0, 0, dist(rd) * map_size);
    set_height(map_size, 0, dist(rd) * map_size);
    set_height(map_size, map_size, dist(rd) * map_size);
    set_height(0, map_size, dist(rd) * map_size);

    // Run Diamond Square algorithm and draw height map into color map
    divide(map_size);
    draw_map();
}

// Recursively divides the map into smaller and smaller square and diamond sections
// and sets the height at the center of each section to the average of the corners
// of the section plus a random offset. Known as the "Diamond-Square Algorithm".
void Terrain::divide(const int size) {
    std::random_device rd;
    std::uniform_real_distribution<double> rand_dist(0, 1);
    int half {size/2};
    double scale {size * map_roughness};

    if(half < 1) return;

    // Sets height at center of successively smaller and smaller squares
    // using an average of the heights of the corners of the squares.
    for(int z = half; z < map_size; z += size) {
        for(int x = half; x < map_size; x += size) {
            square(x, z, half, rand_dist(rd) * scale * 2 - scale);
        }
    }

    // Sets height at center of successively smaller and smaller diamonds
    // using an average of the heights of the corners of the diamonds.
    for(int z = 0; z <= map_size; z += half) {
        for(int x = (z + half) % size; x <= map_size; x += size) {
            diamond(x, z, half, rand_dist(rd) * scale * 2 - scale);
        }
    }

    divide(size / 2);
}

// Sets the height of the center of a square section
void Terrain::square(const int x_coord, const int z_coord, const int size, const double offset) {
    std::vector<double> heights(4, 0.0);

    // Gets heights at 4 corners of the square
    heights[0] = get_height(x_coord - size, z_coord - size);
    heights[1] = get_height(x_coord + size, z_coord - size);
    heights[2] = get_height(x_coord + size, z_coord + size);
    heights[3] = get_height(x_coord - size, z_coord + size);

    double avg = average(heights);               // Gets average height of 4 corners of square
    set_height(x_coord, z_coord, avg + offset);  // Sets middle height to average + random offset
}

// Sets the height of the center of a diamond section
void Terrain::diamond(const int x_coord, const int z_coord, const int size, const double offset) {
    std::vector<double> heights(4, 0.0);

    // Gets heights at 4 corners of the diamond
    heights[0] = get_height(x_coord, z_coord - size);
    heights[1] = get_height(x_coord + size, z_coord);
    heights[2] = get_height(x_coord, z_coord + size);
    heights[3] = get_height(x_coord - size, z_coord);

    double avg = average(heights);               // Gets average height of 4 corners of diamond
    set_height(x_coord, z_coord, avg + offset);  // Sets middle height to average + random offset
}

// Calculates the average heights of the 4 corners of a square or diamond section of the map
double Terrain::average(const std::vector<double> &heights) const {
    double total_height{0.0};
    int valid_heights{0};

    // Checks to see which heights are valid (aka actually on the map) and only select those
    for(auto height : heights) {
        if(height != -1) {
            total_height += height;
            ++valid_heights;
        }
    }
    return valid_heights == 0 ? 0 : total_height / valid_heights;
}

// Draws the height map for terrain and water into the color map
void Terrain::draw_map() {
    coord_pair top, bottom, water;
    double y_coord{0.0}, slope{0.0};
    Uint32 t_color{0}, w_color{0};

    for(int z_coord = 0; z_coord < map_size; ++z_coord) {
        for(int x_coord = 0; x_coord < map_size; ++x_coord){
            y_coord = get_height(x_coord, z_coord);                     // Gets the height at current coordinates
            top = project_map(x_coord, z_coord, y_coord);               // Isometrically projects coordinate into 3D plane
            bottom = project_map(x_coord + 1, z_coord, 0);              // Gets coordinate at absolute bottom of map
            water = project_map(x_coord, z_coord, 0.05 * map_size);     // Isometrically projects water into flat plane
            slope = get_height(x_coord + 1, z_coord) - y_coord;         // Gets slope between adjacent coordinates
            t_color = slope_brightness(x_coord, z_coord, slope);        // Sets color at coordinate relative to slope
            w_color = water_brightness(x_coord, z_coord);               // Sets color at coordinate with random alpha values
            draw_rect(water, bottom, w_color);                          // Draws rectangle into color map between coordinates
            draw_rect(top, bottom, t_color);                            // Draws rectangle into color map between coordinates
        }
    }
}

// Draws a rectangle into the color map with the given color and pair of coordinates.
// All coordinates are adjusted to the right to fit in middle of SDL window.
void Terrain::draw_rect(const coord_pair top, const coord_pair bottom, Uint32 color) {
    for(int i = 0; i < bottom.first - top.first; i++)
        for (int j = 0; j < bottom.second - top.second; j++)
            color_map[top.first + screen_adjust + i + Screen::SCREEN_WIDTH * (top.second + j)] = color;
}


// Sets the brightness at any given coordinate based on the slope between it and its adjacent coordinates
Uint32 Terrain::slope_brightness(const int x_coord, const int z_coord, const double slope) const {
    if(x_coord == map_size - 1 || z_coord == map_size - 1)         // If at edge of map, sets standard border color
        return get_color(192, 154, 98, 20);

    Uint8 alpha = static_cast<Uint8>(std::abs(slope * 50 + 128));  // Sets brightness relative to slope at coordinate
    return get_color(192, 154, 98, alpha);
}

// Sets the brightness with randomly generated alpha values
Uint32 Terrain::water_brightness(const int x_coord, const int z_coord) const {
    if(x_coord == map_size - 1 || z_coord == map_size - 1) // If at edge of map, sets standard border color
        return get_color(50, 150, 200, 20);

    std::random_device rd;
    std::uniform_int_distribution<Uint8> alpha_dist{145, 160};
    return get_color(50, 150, 200, alpha_dist(rd));
}

// Generates a 32 bit color (RGBA) out of 8 bit color values
Uint32 Terrain::get_color(const Uint8 red, const Uint8 green, const Uint8 blue, const Uint8 alpha) const {
    return alpha | (blue << 8) | (green << 16) | (red << 24);
}

// Projects the 2D height map into a seemingly 3D isometric perspective by using coordinate manipulation
std::pair<int, int> Terrain::project_map(const int flat_x, const int flat_z, const double flat_y) const {
    double x_init{0.0}, z_init{0.0}, x_coord{0.0}, z_coord{0.0}, y_coord{0.0};

    // Manipulates coordinates using isometric projection, rotates the map by
    // moving the top left and bottom right corners into the middle of view
    x_coord = 0.5 * (map_size + flat_x - flat_z);
    z_coord = 0.5 * (flat_x + flat_z);

    x_init = 0.5 * map_size;  // Sets width of new perspective
    z_init = 0.2 * map_size;  // Sets depth of new perspective

    // Seemingly tilts the map into the screen so that coordinates higher on
    // the screen now appear further away and lower coordinates appear closer
    y_coord = map_size * 0.5 - flat_y + z_coord * 0.75;
    x_coord = (x_coord - map_size * 0.5) * 6;
    z_coord = (map_size - z_coord) * 0.005 + 1;

    x_coord = x_init + x_coord / z_coord;  // Sets new projected x_coord position
    z_coord = z_init + y_coord / z_coord;  // Sets new projected z_coord position

    return std::pair<int, int>(x_coord, z_coord);
}