#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
int main()
{
    auto window = sf::RenderWindow{ { 512u, 128u }, "Fighter" };

    sf::View veiw = window.getDefaultView();
    window.setFramerateLimit(144);
    sf::RectangleShape background(sf::Vector2f(512, 128));
    background.setFillColor(sf::Color::White);
    while (window.isOpen())
    {
        for (auto event = sf::Event{}; window.pollEvent(event);)
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
            if (event.type == sf::Event::Resized)
            {
                veiw.setSize(sf::Vector2f(512,512* event.size.height / event.size.width));
                window.setView(veiw);
            }
        }
        window.clear();
        window.draw(background);
        window.display();
    }
}
