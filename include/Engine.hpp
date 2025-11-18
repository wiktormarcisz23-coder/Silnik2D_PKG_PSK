#pragma once
#include <string>
#include <functional>
#include <optional>
#include <SFML/Graphics.hpp>

struct EngineConfig {
    unsigned width = 1280;
    unsigned height = 720;
    std::string title = "Tiny2DEngine";

    bool fullscreen = false;
    bool vsync = true;
    unsigned targetFPS = 60;

    bool enableKeyboard = true;
    bool enableMouse = true;

    sf::Color clearColor = sf::Color(30, 30, 30);
    std::optional<std::string> clearTexturePath = std::nullopt;
    std::string logFile = "engine.log";
};

struct InputState {
    bool keyW = false, keyA = false, keyS = false, keyD = false;
    sf::Vector2i mousePos{ 0,0 };
    bool mouseLeft = false, mouseRight = false;
};

class Engine {
public:
    explicit Engine(const EngineConfig& cfg);

    bool init();
    void run(const std::function<void(float, const InputState&)>& onUpdate = {},
        const std::function<void(sf::RenderWindow&)>& onRender = {});
    void stop();

    void setClearColor(const sf::Color& c);
    bool setClearTexture(const std::string& path);
    void shutdown();

private:
    EngineConfig     cfg_;
    sf::RenderWindow window_;
    bool             running_ = false;

    sf::Clock frameClock_;

    sf::Texture               clearTexture_;
    std::optional<sf::Sprite> clearSprite_;
    bool                      hasClearTexture_ = false;

    InputState input_{};

    sf::VideoMode pickVideoMode_() const;

    void processEvents_();
    void renderFrame_(const std::function<void(sf::RenderWindow&)>& onRender);
};
