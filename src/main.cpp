#include <SFML/Graphics.hpp>
#include <verlet.hpp>
#include <iostream>

using namespace std;

int main()
{
    sf::ContextSettings settings;
    settings.antialiasingLevel = 0;
    sf::RenderWindow window(sf::VideoMode(900, 900), "SFML works!", sf::Style::Default, settings);
    sf::RectangleShape shape(sf::Vector2f(20.f, 20.f));
    shape.setFillColor(sf::Color::White);
    shape.setPosition(250.f, 250.f);

    Solver solver(0);

    float dt = 0.02;

    for (float x = 25.f; x < 800.f; x+=20.f)
    for (float y = 25.f; y < 500.f; y+=20.f)
    {
        solver.add_particle(sf::Vector2f(x, y));
        solver.P.back().update_velocity(sf::Vector2f(1.f, 0.f), dt);
        if (y>100.f) solver.P.back().update_velocity(sf::Vector2f(-1.f, 0.1f), dt);
    }

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        
        
        window.clear();
        
        // window.draw(shape);

        solver.update(dt);

        for (auto &particle: solver.P)
        {
            window.draw(particle.shape);
        }
        window.display();
    }

    return 0;
}