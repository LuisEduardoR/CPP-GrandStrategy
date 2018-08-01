#ifndef GRAND_MAPGEN_HPP
#define GRAND_MAPGEN_HPP

#include <vector>
#include <SFML/System/Vector2.hpp>

namespace mdata {

struct Province {

    unsigned short id;
    unsigned center_x, center_y;
    char* terrain;

};

class MapData {

    public:

        bool ready;

        std::vector<Province> provinces;
        unsigned** adjacecies_matrix;
        
        MapData(void);

        bool generateMapData(void);
        void verifyPixel(sf::Vector2u block_start, sf::Vector2u pixel, sf::Vector2u image_size, unsigned** visited, unsigned* verified_count);
    
};
    
}

#endif
