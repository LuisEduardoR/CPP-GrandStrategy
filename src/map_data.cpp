#include "map_data.hpp"
#include <cstdio>
#include <cstdlib>
#include <set>
#include <vector>
#include <utility>
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
    short** visited_pixels = NULL;
    visited_pixels = (short**)malloc(sizeof(short*) * map_px_size.x);
    for (int i = 0; i < map_px_size.x; i++)
        visited_pixels[i] = (short*)malloc(sizeof(short) * map_px_size.y);

    // Initiate the allocated space with zeroses.
    for(int a = 0; a < map_px_size.x; a++)
        for(int b = 0; b < map_px_size.y; b++)
            visited_pixels[a][b] = 0;
    
    fprintf(stdout, ">> Generating provinces...\n");
    fflush(stdout);

    // Works as a list of provinces to be generated.
    std::vector<ProvinceGenerating> provinces_to_gen;

    unsigned current_id = 0;

    // Adds an inital province to be generated at the upper left corner of the image.
    sf::Vector2u* pixel_queue = (sf::Vector2u*)calloc(16*1024*1024, sizeof(sf::Vector2u));

    ProvinceGenerationData province_generation_data(&current_id, &provinces_to_gen, &provinces, pixel_queue, province_map, visited_pixels);

    ProvinceGenerating initial_prov(&province_generation_data, current_id, province_map.getPixel(0,0), sf::Vector2u(0,0));
    provinces_to_gen.push_back(initial_prov);

    // While there are provinces to be verified verifies then.
    while(!provinces_to_gen.empty()) {

        /* Creates a variable to store the GenerateProvince data and removes from the list (this is done to not have to remove from the middle
           of the vector later and having to shift following elements in the vector).*/
        ProvinceGenerating cur_prov = provinces_to_gen.back();
        provinces_to_gen.pop_back();

        cur_prov.generate_province();

    }

    fprintf(stdout, ">> Done generating %lu provinces...\n", provinces.size());

    // Frees the memory allocated for the visited matrix.
    for (int j = 0; j < map_px_size.y; j++)
        free(visited_pixels[j]);
    free(visited_pixels);

    ready = true;
    return ready;  

}

void mdata::ProvinceGenerating::generate_province () { 

    unsigned cur_pos_in_list = 0;

    (*province_generation_data).pixel_queue[0] = initial_pixel;

    while (cur_pos_in_list <= (*province_generation_data).max_queue_pos) {    
        
        analysePixel((*province_generation_data).pixel_queue[cur_pos_in_list]);
        cur_pos_in_list++;

    }

    unsigned center_x = (max_x + min_x);
    unsigned center_y = (max_x + min_x);
    mdata::Province new_province(numeric_id, color_in_map, sf::Vector2u(center_x, center_y));

    fprintf(stderr,"new prov %u\n", new_province.numeric_id);

    (*((*province_generation_data).province_list)).push_back(new_province);

    fprintf(stderr,"after new prov %u\n", new_province.numeric_id);

}

void mdata::ProvinceGenerating::analysePixel(sf::Vector2u pixel) {

    if((*province_generation_data).visited_pixels[pixel.x][pixel.y] == 1)
        return;

    sf::Color px_color = (*province_generation_data).province_map.getPixel(pixel.x, pixel.y);

    if(px_color == color_in_map) {

        (*province_generation_data).visited_pixels[pixel.x][pixel.y] = 1;

        // Update the max and min for x and y, this will be used later to determine province center.
        if(pixel.x < min_x) { min_x = pixel.x; }
        if(pixel.x > max_x) { max_x = pixel.x; }
        if(pixel.y < min_y) { min_y = pixel.y; }
        if(pixel.y > max_y) { max_y = pixel.y; }

        if (pixel.x > 0 && (*province_generation_data).visited_pixels[pixel.x - 1][pixel.y] == 0) {
            ((*province_generation_data).max_queue_pos)++;
            (*province_generation_data).pixel_queue[(*province_generation_data).max_queue_pos] == sf::Vector2u(pixel.x - 1,pixel.y);
            (*province_generation_data).visited_pixels[pixel.x - 1][pixel.y] = -1;
        }

        if (pixel.x < ((*province_generation_data).map_size.x - 1) && (*province_generation_data).visited_pixels[pixel.x + 1][pixel.y]  == 0) {
            ((*province_generation_data).max_queue_pos)++;
            (*province_generation_data).pixel_queue[(*province_generation_data).max_queue_pos] = sf::Vector2u(pixel.x + 1,pixel.y);
            (*province_generation_data).visited_pixels[pixel.x + 1][pixel.y] = -1;
        }

        if (pixel.y > 0 && (*province_generation_data).visited_pixels[pixel.x][pixel.y - 1] == 0) {
            ((*province_generation_data).max_queue_pos)++;
            (*province_generation_data).pixel_queue[(*province_generation_data).max_queue_pos] = sf::Vector2u(pixel.x,pixel.y - 1);
            (*province_generation_data).visited_pixels[pixel.x][pixel.y - 1] = -1;
        }

        if (pixel.y < ((*province_generation_data).map_size.y - 1) && (*province_generation_data).visited_pixels[pixel.y][pixel.y + 1] == 0) {
            ((*province_generation_data).max_queue_pos)++;
            (*province_generation_data).pixel_queue[(*province_generation_data).max_queue_pos] = sf::Vector2u(pixel.x,pixel.y + 1);
            (*province_generation_data).visited_pixels[pixel.x][pixel.y + 1] = -1;
        }

    } else if (px_color != sf::Color::Black){

        bool is_new = true;

        // Verifies if a province with this color is already waiting to be generated.
        for(unsigned u = 0; u < (*(*province_generation_data).province_generation_queue).size(); u++) {
            if ((*((*province_generation_data).province_generation_queue))[u].color_in_map == px_color) {

                is_new = false;
                (*((*province_generation_data).province_generation_queue))[u].adjacent_ids.insert(numeric_id);
                break;
            }
        }

        if(is_new) {
            (*(*province_generation_data).cur_id_number)++;
            mdata::ProvinceGenerating new_province_gen(province_generation_data, (*(*province_generation_data).cur_id_number), px_color, pixel);
            (*((*province_generation_data).province_generation_queue)).push_back(new_province_gen);
        }

    }

}