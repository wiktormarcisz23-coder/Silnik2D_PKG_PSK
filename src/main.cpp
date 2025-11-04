#include "Engine.hpp"
#include "Logger.hpp"
#include <SFML/Graphics.hpp>

int main() {
    EngineConfig cfg;
    cfg.width = 1280;
    cfg.height = 720;
    cfg.title = "Tiny2DEngine – wersja SFML 3";
    cfg.vsync = true;
    cfg.targetFPS = 120;
    cfg.clearColor = sf::Color(25, 28, 35);

    Engine engine(cfg);
    if (!engine.init()) {
        Logger::instance().error("Nie udało się zainicjalizować silnika.");
        return 1;
    }

    // ✨ onUpdate – logika gry
    auto onUpdate = [&](float dt, const InputState& in) {
        (void)dt;
        (void)in;
        // Brak logiki w tym przykładzie
        };

    // 🎨 onRender – rysowanie obiektów
    auto onRender = [&](sf::RenderWindow& win) {
        // okrąg
        sf::CircleShape circle(50.f);
        circle.setFillColor(sf::Color::Cyan);
        circle.setOrigin(sf::Vector2f(50.f, 50.f));
        circle.setPosition(sf::Vector2f(cfg.width / 2.f, cfg.height / 2.f));
        win.draw(circle);

        // prostokąt
        sf::RectangleShape rect(sf::Vector2f(120.f, 60.f));
        rect.setFillColor(sf::Color(255, 128, 0));
        rect.setOrigin(sf::Vector2f(60.f, 30.f));
        rect.setPosition(sf::Vector2f(cfg.width / 2.f + 200.f, cfg.height / 2.f));
        win.draw(rect);

        // tekst (jeśli chcesz)
        // sf::Font font;
        // if (font.loadFromFile("arial.ttf")) {
        //     sf::Text text("SFML 3 działa!", font, 24);
        //     text.setPosition(sf::Vector2f(20.f, 20.f));
        //     win.draw(text);
        // }
        };

    try {
        engine.run(onUpdate, onRender);
    }
    catch (const std::exception& e) {
        Logger::instance().error(std::string("Błąd krytyczny: ") + e.what());
    }

    engine.shutdown();
    return 0;
}
