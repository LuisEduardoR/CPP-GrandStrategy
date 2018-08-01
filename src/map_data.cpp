#include "map_data.hpp"
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Image.hpp>

#define PROVINCE_PNG_PATH "map/provinces.png"
#define FLOOD_IMAGE_BLOCK_SIZE 256

mdata::MapData::MapData(void) {

    ready = false;
    provinces.clear();
    adjacecies_matrix = NULL;

}

bool mdata::MapData::generateMapData(void) {

    adjacecies_matrix = NULL;

    // Attempts to load the provinces.png image;
    sf::Image province_map;
    fprintf(stdout, ">> Loading %s...\n", PROVINCE_PNG_PATH);
    fflush(stdout);
    if(!province_map.loadFromFile(PROVINCE_PNG_PATH)) {
        fprintf(stderr, "> Unable to continue due to previous error!\n");
        return false;
    }

    sf::Vector2u map_px_size = province_map.getSize();


    // Creates a matrix to store wich pixels of the image have been visited.
    unsigned** visited_pixels = NULL;
    visited_pixels = (unsigned**)malloc(sizeof(unsigned*) * map_px_size.x);
    for (int i = 0; i < map_px_size.x; i++)
        visited_pixels[i] = (unsigned*)malloc(sizeof(unsigned) * map_px_size.y);

    // Initiate the allocated space with zeroses.
    for(int a = 0; a < map_px_size.x; a++)
        for(int b = 0; b < map_px_size.y; b++)
            visited_pixels[a][b] = 0;
    
    fprintf(stdout, ">> Analysing pixels...\n");
    fflush(stdout);

    unsigned pxs_verified = 0;

    sf::Vector2u starting_pixel(0,0);
    sf::Vector2u block_start(0,0);

    /* Start the flood-fill algorythm that generates the provinces. TODO: divide the recursion in blocks, as big images could an PROBABLY WILL CAUSE 
       STACK OVERFLOWS. */
    verifyPixel(block_start, starting_pixel, map_px_size, visited_pixels, &pxs_verified);

    // Frees the memory allocated for the visited matrix.
    for (int j = 0; j < map_px_size.y; j++)
        free(visited_pixels[j]);
    free(visited_pixels);

    fprintf(stdout, ">> Done verifying all %u pixels...\n", pxs_verified);
    fflush(stdout);

    ready = true;
    return ready;  

}

void mdata::MapData::verifyPixel(sf::Vector2u block_start, sf::Vector2u pixel, sf::Vector2u image_size, unsigned** visited, unsigned* verified_count) {

    fprintf(stdout, ">>> (DEBUG) Current on %u %u...\n", pixel.x, pixel.y);
    fflush(stdout);

    unsigned x, y;

    // Get an easier to use reference to the current pixel.
    x = pixel.x;
    y = pixel.y;

    if(visited[x][y] == 0) {

        visited[x][y] = 1;
        (*verified_count)++;

        //TODO

    }

    // Try to visit left.
    if(x > 0) {
        if(visited[x - 1][y] == 0) {
            sf::Vector2u next(x-1, y);
            verifyPixel(block_start, next, image_size, visited, verified_count);
        }
    }

    // Try to visit right.
    if(x < (image_size.x - 1)) {
        if(visited[x + 1][y] == 0) {
            sf::Vector2u next(x+1, y);
            verifyPixel(block_start, next, image_size, visited, verified_count);
        }
    }

    // Try to visit up.
    if(y > 0) {
       if(visited[x][y-1] == 0) {
            sf::Vector2u next(x, y-1);
            verifyPixel(block_start, next, image_size, visited, verified_count);
        }
    }

    // Try to visit down.
    if(y < (image_size.y - 1)) {
        if(visited[x][y+1] == 0) {
            sf::Vector2u next(x, y+1);
            verifyPixel(block_start, next, image_size, visited, verified_count);
        }
    }

}