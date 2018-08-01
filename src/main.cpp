#include <SFML/Window.hpp>
#include "map_data.hpp"
#include <cstdio>

int main() {

    mdata::MapData mapData;

    sf::Window mainWindow(sf::VideoMode(800, 600), "Victoria III");
    sf::Window auxWindow(sf::VideoMode(200, 500), "Production");

    while (mainWindow.isOpen()) {

        if(!mapData.ready) {
            fprintf(stdout, "> Generating map...\n");
            fflush(stdout);

            if(mapData.generateMapData()) {
                fprintf(stdout, "> READY!\n");
                fflush(stdout);
            } else
                break;

        }

        // check all the window's events that were triggered since the last iteration of the loop
        sf::Event mainEvent;
        while (mainWindow.pollEvent(mainEvent))
        {
            // "close requested" event: we close the window
            if (mainEvent.type == sf::Event::Closed)
                mainWindow.close();
        }

        // check all the window's events that were triggered since the last iteration of the loop
        sf::Event auxEvent;
        while (auxWindow.pollEvent(auxEvent))
        {
            // "close requested" event: we close the window
            if (auxEvent.type == sf::Event::Closed)
                auxWindow.close();
        }

    }

    return 0;
}