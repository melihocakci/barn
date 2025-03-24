#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

int main()
{
    // Create the main window
    sf::RenderWindow window(sf::VideoMode({ 600, 600 }), "SFML window");

    // Load a sprite to display
    const sf::Texture texture("res/teto_pear.jpg");
    sf::Sprite sprite(texture);

    // Create a graphical text to display
    const sf::Font font("res/Consolas.ttf");
    sf::Text text(font, "I'm Tetobating so hard", 50);
    text.setFillColor(sf::Color::Black);

    // Load a music to play
    sf::Music music("res/Kasane Teto - Teto territory.mp3");

    // Play the music
    music.play();

    // Start the game loop
    while (window.isOpen())
    {
        // Process events
        while (const std::optional event = window.pollEvent())
        {
            // Close window: exit
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        // Clear screen
        window.clear();

        // Draw the sprite
        window.draw(sprite);

        // Draw the string
        window.draw(text);

        // Update the window
        window.display();
    }
}
