#include <cstdio>

#include "main.hpp"
#include "map_data.hpp"

#include <SFML/Window.hpp>
int main() {

    // Creates the SFML main window.
    sf::Window main_window(sf::VideoMode(800, 600), "Victoria III But No...");

    // Create an object to store all the map data.
    // The data will automatically be loaded by the MapData constructor.
    mdata::MapData map_data;

    while (main_window.isOpen()) {

        // CLose the program if for some reason the map data failed to be created/loaded.
        if(!map_data.isAvaliable()) {
            close(&main_window);
            return -1;
        }

        sf::Event main_event;
        while (main_window.pollEvent(main_event))
        {
            if (main_event.type == sf::Event::Closed)
                main_window.close();
        }

    }

    return 0;
}

void close(sf::Window* rMainWindow) {

    (*rMainWindow).close();

}