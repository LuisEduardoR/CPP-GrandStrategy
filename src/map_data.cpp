#include "map_data.hpp"

#include <cstdio>
#include <cstdlib>
#include <iostream>

mdata::MapData::MapData(void) {

    mDataAvaliable = false;

    // In future will check if there is up to date MapData and load it if that is the case. For now just generates data from scratch.
    /*if(MapData has already been built and was not modified)
    Just load
    else*/
    fprintf(stdout, "> No map data has been found or it is not up to date! New map data will now be generated...\n");
    fprintf(stdout, "> Generating map...\n");
    fflush(stdout);

    // Creates an instance of the MapGenerator and attempts to generate the map data.
    MapGenerator generator;
    if(generator.generateMapData()) { 
        fprintf(stdout, "> Map data generated successfully!\n");
        fflush(stdout);
    } else {
        fprintf(stderr, "> Failed to generate map data!\n"); 
        return;
    }

    if(loadMapData()) { 
        fprintf(stdout, ">> Done loading %lu provinces with %lu adjacencies!\n", provinces.size(), adjacencies.size());
        fprintf(stdout, "> Map data loaded successfully!\n");
        fflush(stdout);
    } else {
        fprintf(stderr, "> Failed to load map data!\n"); 
        return;
    }

}

// Load map data from previously generated files.
bool mdata::MapData::loadMapData() {

    // Load provinces from file.

    unsigned temp_prov;
    unsigned temp_center_x;
    unsigned temp_center_y;

    fprintf(stdout, ">> Loading files...\n");
    fflush(stdout);

    // Tries to open the provinces file and gives an error if it's not possible.

    fprintf(stdout, ">>> Loading %s...\n", PROVINCES_TXT_PATH);
    fflush(stdout);

    FILE* province_file = NULL;
    province_file = fopen(PROVINCES_TXT_PATH,"r");

    if(province_file == NULL) {
        fprintf(stderr, ">>> Failed to load %s!\n", PROVINCES_TXT_PATH);
        return false;
    }

    // Reads the provinces file.
    while (fscanf(province_file, " %u %u %u", &temp_prov, &temp_center_x, &temp_center_y) != EOF) {

        Province new_province;
        new_province.id = temp_prov;
        new_province.generateCenter(temp_center_x, temp_center_y);

        provinces.insert(std::make_pair(temp_prov, new_province));

    }

    fclose(province_file);

    // Load adjacencies from file.

    unsigned temp_adj_u;
    unsigned temp_adj_v;

    // Tries to open the adjacencies file and gives an error if it's not possible.
    fprintf(stdout, ">>> Loading %s...\n", ADJACENCIES_TXT_PATH);
    fflush(stdout);

    FILE* adjacencies_file = NULL;
    adjacencies_file = fopen(ADJACENCIES_TXT_PATH,"r");

    if(adjacencies_file == NULL) {
        fprintf(stderr, ">>> Failed to load %s!\n", ADJACENCIES_TXT_PATH);
        return false;
    }

    // Reads the adjacencies file.
    while (fscanf(adjacencies_file, " %u %u", &temp_adj_u, &temp_adj_v) != EOF) {

        std::pair<unsigned, unsigned> adjacency_entry;

        /* In order of not creating duplicate adjacencies the smaller province id is always added
        first. */
        if(temp_adj_u < temp_adj_v) {
            adjacency_entry.first = temp_adj_u;
            adjacency_entry.second = temp_adj_v;
        } else {
            adjacency_entry.first = temp_adj_v;
            adjacency_entry.second = temp_adj_u;
        }
        
        adjacencies.insert(adjacency_entry);

    }

    fclose(adjacencies_file);

    return true;

}

// Generates map data from image and text files in the ./map directory
bool mdata::MapGenerator::generateMapData() {

    if(loadNecessaryFiles()) { 
        fprintf(stdout, ">> Necessary files loaded successfully!\n");
        fflush(stdout);
    } else {
        fprintf(stderr, ">> Failed to load all necessary files!\n"); 
        return false;
    }

    fprintf(stdout, ">> Generating provinces...!\n");
    fflush(stdout);

    sf::Vector2u map_size = provinceMap.getSize();

    std::set<std::pair<unsigned, unsigned>> adjacencies;

    std::map<unsigned,mdata::Province>::iterator prov_it;

    sf::Color cur_color;
    sf::Color next_color;

    /* NOTE: The color black is used as a "NULL" color. Any pixel using the color black will be
    ignored during the province generation. */

    // Generates provinces that are in the first line of the map.
    for(unsigned x = 0; x < map_size.x; x++) {

        cur_color = provinceMap.getPixel(x,0);

        // Verifies if a province with the color of the current pixel has already been generated and gets an iterator to it.
        prov_it = provincesInGeneration.find(generateIdFromColor(cur_color));

        if(cur_color != sf::Color::Black) {
            // If an iterator is found just update province edges.
            if (prov_it != provincesInGeneration.end())
                updateEdgeCoordinates(x,0,prov_it);
            else // If an iterator is not found a province with this color doesn't exist and therefore needs to be generated.
                createProvince(x,0);
        }

    }

    std::map<unsigned,mdata::Province>::iterator next_prov_it;

    // Verifies all pixels of the image.
    for(unsigned y = 0; y < map_size.y; y++) {
        for(unsigned x = 0; x < map_size.x; x++) {

            cur_color = provinceMap.getPixel(x,y);

            // Check the pixel to the right of the current pixel, 
            if(x + 1 < map_size.x) {

                next_color = provinceMap.getPixel(x + 1, y);

                if(cur_color != sf::Color::Black) {

                    // Finds an iterator to the current province.
                    prov_it = provincesInGeneration.find(generateIdFromColor(cur_color));

                    /* If the color of both pixels is te same theres is nothing that needs to be done.
                    Also don't do anything if the color of the other pixel is black, the color black is reserved if for some reason you need to create a
                    "void" spot in the map. */
                    if(next_color != cur_color) {                        

                        // Update the edge coordinates for the current province.
                        updateEdgeCoordinates(x, y, prov_it);

                        // Finds a iterator to the other province.
                        // OBS: provinces on the same line will always already have being generated by the line above (SEE BELOW).
                        // That is also the reason the first line is generated separately.
                        next_prov_it = provincesInGeneration.find(generateIdFromColor(next_color));

                        if(next_color != sf::Color::Black) {

                            // Add an adjacency between those two provinces. Because it is a std::set duplicates will be ignored.
                            // Always add the province with the smallest id first to guarantee there can't be a duplicate with just the values reversed.
                            if((*prov_it).second.id < (*next_prov_it).second.id)
                                adjacencies.insert(std::make_pair((*prov_it).second.id,(*next_prov_it).second.id));
                            else
                                adjacencies.insert(std::make_pair((*next_prov_it).second.id,(*prov_it).second.id));
                        
                            // Update the edge coordinates for the other province.
                            updateEdgeCoordinates(x, y + 1, next_prov_it);

                        }
                    }
                }
            }

            // Check the pixel below the current pixel.
            if(y + 1 < map_size.y) {

                next_color = provinceMap.getPixel(x, y + 1);

                if(cur_color != sf::Color::Black)
                    prov_it = provincesInGeneration.find(generateIdFromColor(cur_color));

                if(next_color != cur_color) { 
                        
                    if(cur_color != sf::Color::Black)
                        updateEdgeCoordinates(x, y, prov_it);

                    // Tries to find an iterator to the other province.
                    next_prov_it = provincesInGeneration.find(generateIdFromColor(next_color));

                    // If none is found creates a new province and gets the iterator.
                    if(next_color != sf::Color::Black) {
                        if (next_prov_it == provincesInGeneration.end())
                            next_prov_it = createProvince(x,y + 1);
                    }

                    if(cur_color != sf::Color::Black) {
                        if((*prov_it).second.id < (*next_prov_it).second.id)
                            adjacencies.insert(std::make_pair((*prov_it).second.id,(*next_prov_it).second.id));
                        else
                            adjacencies.insert(std::make_pair((*next_prov_it).second.id,(*prov_it).second.id));
                    }

                    if(next_color != sf::Color::Black)
                        updateEdgeCoordinates(x, y + 1, next_prov_it);
                    
                }       
            }
        }
    }

    fprintf(stdout, ">> Done analysing %u pixels and generating %lu provinces!\n", map_size.x * map_size.y, provincesInGeneration.size());
    fprintf(stdout, ">> Saving data to files...\n");
    fflush(stdout);

    // Saves province data to files.
    fprintf(stdout, ">>> Saving %s...\n", PROVINCES_TXT_PATH);
    FILE* province_file = fopen(PROVINCES_TXT_PATH,"w+");
    for(std::map <unsigned, mdata::Province>::iterator i = provincesInGeneration.begin(); i != provincesInGeneration.end(); i++) {

        (*i).second.generateCenter();
        fprintf(province_file, "%u %u %u\n", 
                (*i).second.id, (*i).second.center.x, (*i).second.center.y);
        
    }

    fclose(province_file);

    fprintf(stdout, ">>> Saving %s...\n", ADJACENCIES_TXT_PATH);
    FILE* adjacencies_file = fopen(ADJACENCIES_TXT_PATH,"w+");

    for(std::set<std::pair<unsigned, unsigned>>::iterator i = adjacencies.begin(); i != adjacencies.end(); i++) {

        fprintf(adjacencies_file, "%u %u\n", (*i).first, (*i).second);
        
    }

    fclose(adjacencies_file);

    return true;

}

bool mdata::MapGenerator::loadNecessaryFiles() {

    // Attempts to load the provinces.png image;
    fprintf(stdout, ">> Loading files...\n");
    fprintf(stdout, ">>> Loading %s...\n", PROVINCES_BMP_PATH);
    fflush(stdout);

    if(!provinceMap.loadFromFile(PROVINCES_BMP_PATH)) { 
        fprintf(stderr, ">>> Failed to load %s!\n", PROVINCES_BMP_PATH);
        return false; 
    }

    return true;

}

// Create a new province.
std::map <unsigned, mdata::Province>::iterator mdata::MapGenerator::createProvince(unsigned x, unsigned y) {

    Province new_province;
    new_province.id = generateIdFromColor(provinceMap.getPixel(x,y));

    // Return an iterator to the province.
    return provincesInGeneration.insert(std::make_pair(new_province.id, new_province)).first;

}

// Update edge coordinates of a province that will later be used to find it's center.
void mdata::MapGenerator::updateEdgeCoordinates(unsigned x, unsigned y, std::map<unsigned,mdata::Province>::iterator prov_it) {

    if((*prov_it).second.min_x > x) { (*prov_it).second.min_x = x; }
    if((*prov_it).second.max_x < x) { (*prov_it).second.max_x = x; }
    if((*prov_it).second.min_y > y) { (*prov_it).second.min_y = y; }
    if((*prov_it).second.max_y < y) { (*prov_it).second.max_y = y; }

}

// Generates an unique number from a color RGB value that can be used as a key for a map.
unsigned mdata::generateIdFromColor(sf::Color col) { 
    return 1000000 * col.r + 1000 * col.g + col.b; 
}

// Generates a color from an id.
sf::Color mdata::generateColorFromId(unsigned id) { 

    sf::Color color;

    color.r = id / 1000000;
    id = id % 1000000;
        
    color.g = id / 1000;
    id = id % 1000;

    color.b = id;

    color.a = 255;

    return color;

}