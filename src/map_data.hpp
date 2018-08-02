#ifndef GRAND_MAPGEN_HPP
#define GRAND_MAPGEN_HPP

#include <set>
#include <vector>
#include <string>
#include <utility>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Image.hpp>

namespace mdata {

struct Province {

    unsigned numeric_id;
    sf::Color color_id;

    sf::Vector2u center;
    std::string terrain;

    Province(unsigned id, sf::Color color, sf::Vector2u cen) {

        numeric_id = id;
        color_id = color;

        center = cen;

    }

};

struct ProvinceGenerationData;

// Class used to store information important to the generation of an province.
struct ProvinceGenerating {

        mdata::ProvinceGenerationData* province_generation_data;

        unsigned numeric_id;

        sf::Color color_in_map;
        sf::Vector2u initial_pixel;

        unsigned min_x;
        unsigned max_x;

        unsigned min_y;
        unsigned max_y;
    
        std::set<unsigned> adjacent_ids;

        ProvinceGenerating(mdata::ProvinceGenerationData* prov_gen_data, unsigned id, sf::Color map_color, sf::Vector2u pixel) {

            province_generation_data = prov_gen_data;

            numeric_id = id;
            color_in_map = map_color;
            pixel = initial_pixel;

            min_x = 0x3f3f3f3f;
            max_x = 0;

            min_y = 0x3f3f3f3f;
            max_y = 0;

        }

    void generate_province ();

    void analysePixel(sf::Vector2u pixel);

};

struct ProvinceGenerationData {

    unsigned* cur_id_number; 

    std::vector<mdata::ProvinceGenerating>* province_generation_queue;
    std::vector<mdata::Province>* province_list;
        
    sf::Vector2u* pixel_queue;
    unsigned max_queue_pos;

    sf::Image province_map;
    sf::Vector2u map_size;

    short** visited_pixels;

    sf::Vector2u initial_pixel;

    ProvinceGenerationData(unsigned* cur_id_num,
                            std::vector<mdata::ProvinceGenerating>* prov_gen_q,
                            std::vector<mdata::Province>* prov_l,     
                            sf::Vector2u* px_q,
                            sf::Image prov_map,
                            short** visited_px) {

        cur_id_number = cur_id_num; 

        province_generation_queue = prov_gen_q;
        province_list = prov_l;
        
        pixel_queue = px_q;
        max_queue_pos = 0;

        province_map = prov_map;
        map_size = prov_map.getSize();

        visited_pixels = visited_px;

    }

};

class MapData {

    public:

        bool ready;

        std::vector<Province> provinces;
        unsigned** adjacecies_matrix;
        
        MapData(void);

        bool generateMapData(void);
    
};
    
}

#endif
