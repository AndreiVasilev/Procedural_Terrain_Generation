#include "../include/Screen.h"
#include "../include/Terrain.h"

int main() {

    // Generate SDL Window
    Screen screen;

    while(!screen.quit_program()) {
        Terrain terrain(513, 0.3);          // Generate new terrain map, size must be power of 2 plus 1.
        screen.update(terrain.get_map());   // Update SDL window with new map
    }

    return 0;
}
