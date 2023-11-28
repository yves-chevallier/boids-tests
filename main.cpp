// main.cpp
#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <GL/glew.h>
#include <random>
#include <iostream>
#include <sstream>
#include <cmath>
#include <array>

#define MAX_BOIDS 1000000
#define FRAMERATE 120
#define WIDTH 1000
#define HEIGHT 1000

using namespace std;

#ifndef M_PI
constexpr float M_PI = 3.141592653589793;
constexpr float M_PI_2 = M_PI / 2;
constexpr float M_PI_4 = M_PI / 4;
#endif

float FastArcTan(float x)
{
    return M_PI_4 * x - x * (fabs(x) - 1) * (0.2447 + 0.0663 * fabs(x));
}

float FastArcTan2(float y, float x)
{
    if (x >= 0)     // -pi/2 .. pi/2
        if (y >= 0) // 0 .. pi/4 or pi/4 .. pi/2
            return y < x ? FastArcTan(y / x) : M_PI_2 - FastArcTan(x / y);
        else // -pi/4 .. 0 or -pi/2 .. -pi/4
            return (-y < x) ? FastArcTan(y / x) : -M_PI_2 - FastArcTan(x / y);
    else            // -pi..-pi/2, pi/2..pi
        if (y >= 0) // pi*3/4 .. pi or pi/2 .. pi*3/4
            return y < -x ? FastArcTan(y / x) + M_PI : M_PI_2 - FastArcTan(x / y);
        else // -pi .. -pi*3/4 or -pi*3/4 .. -pi/2
            return -y < -x ? FastArcTan(y / x) - M_PI : -M_PI_2 - FastArcTan(x / y);
}

template <typename T>
std::string to_string_with_precision(const T a_value, const int n = 6)
{
    std::ostringstream out;
    out.precision(n);
    out << std::fixed << a_value;
    return out.str();
}

struct Boid
{
    sf::Vector2f position;
    sf::Vector2f velocity;

    void update(float dt)
    {
        position += velocity * dt;
    }

    void bounce(sf::Vector2f size)
    {
        if (position.x < -size.x)
        {
            position.x = -size.x;
            velocity.x *= -1;
        }
        if (position.x > size.x)
        {
            position.x = size.x;
            velocity.x *= -1;
        }
        if (position.y < -size.y)
        {
            position.y = -size.y;
            velocity.y *= -1;
        }
        if (position.y > size.y)
        {
            position.y = size.y;
            velocity.y *= -1;
        }
    }

    auto getX() const {
        return position.x;
    }
    auto getY() const {
        return position.y;
    }
};

array<GLfloat[3], MAX_BOIDS> vertices;
array<Boid, MAX_BOIDS> boids;

int main()
{
    size_t nBoids = 300;
    float radius = 0.1f;
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Boids Simulation");

    sf::Transform matrix = sf::Transform::Identity;
    matrix.scale(2.f / WIDTH, 2.f / HEIGHT);
    sf::Glsl::Mat4 projectionViewMatrix = matrix;

    sf::Texture texture;
    texture.create(WIDTH, HEIGHT);
    sf::Sprite sprite(texture);

    sf::RenderTexture renderTexture;
    renderTexture.create(WIDTH, HEIGHT);

    sf::Shader shader;
    sf::Shader circleShader;
    sf::Shader fastShader;
    sf::Shader textureShader;

    shader.loadFromFile("assets/boid.vert", "assets/boid.geom", "assets/boid.frag");
    circleShader.loadFromFile("assets/circles.vert", "assets/circles.geom", "assets/circles.frag");
    fastShader.loadFromFile("assets/fast.vert", "assets/fast.frag");
    textureShader.loadFromFile("assets/fast.vert", "assets/texture.frag");

    sf::Shader *shaders[] = {&shader, &circleShader, &fastShader, &textureShader};
    for (auto *s : shaders)
    {
        s->setUniform("boidColor", sf::Glsl::Vec3(85.f / 255, 140.f / 255, 244.f / 255));
        s->setUniform("radiusColor", sf::Glsl::Vec3(42.f / 255, 120.f / 255, 200.f / 255));
        s->setUniform("boidSize", 0.01f);
        s->setUniform("projectionViewMatrix", projectionViewMatrix);
        s->setUniform("screenWidth", (float)WIDTH);
        s->setUniform("iChannel0", texture);
        s->setUniform("iResolution", sf::Glsl::Vec2(WIDTH, HEIGHT));
    }

    // Time management
    sf::Clock clock;
    sf::Clock seconds;
    sf::Time secondsCount = sf::Time::Zero;
    sf::Time timeSinceLastUpdate = sf::Time::Zero;
    sf::Time timePerFrame = sf::seconds(1.f / 60);
    sf::Time timeSinceLastDraw = sf::Time::Zero;
    window.setFramerateLimit(FRAMERATE);

    // Fps text
    sf::Font font;
    if (!font.loadFromFile("assets/collegiate.ttf"))
        std::cout << "Error loading font" << std::endl;
    sf::Text fps;
    fps.setFont(font);
    fps.setPosition(10, 10);
    fps.setCharacterSize(16);
    fps.setFillColor(sf::Color::White);

    for (int i = 0; i < nBoids; i++)
    {
        boids[i].position = sf::Vector2f(rand() % WIDTH - WIDTH / 2, rand() % HEIGHT - HEIGHT / 2);
        boids[i].velocity = sf::Vector2f(rand() % 500 - 250, rand() % 500 - 250);
    }

    for (int i = 0; i < nBoids; i++)
    {
        auto &p = boids[i];
        vertices[i][0] = p.position.x;
        vertices[i][1] = p.position.y;
        vertices[i][2] = atan2(p.velocity.y, p.velocity.x);
    }

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float fpsAvg = 0;
    float fpsCount = 0;
    float avg = 0;
    bool showCircles = false;
    bool ctrlDown = false;
    bool isFullscreen = false;

    sf::Shader *boidShader = &shader;
    while (window.isOpen())
    {
        sf::Event currEvent;
        while (window.pollEvent(currEvent))
        {
            switch (currEvent.type)
            {
            case (sf::Event::Closed):
                window.close();
                break;
            case (sf::Event::MouseWheelScrolled):
                if (ctrlDown)
                {
                    const size_t increment = nBoids / 10;
                    if (currEvent.mouseWheelScroll.delta > 0)
                    {
                        nBoids += increment;
                        nBoids = nBoids > MAX_BOIDS ? MAX_BOIDS : nBoids;

                        for (size_t i = nBoids - increment; i < nBoids; i++)
                        {
                            boids[i].position = sf::Vector2f(rand() % WIDTH - WIDTH / 2, rand() % HEIGHT - HEIGHT / 2);
                            boids[i].velocity = sf::Vector2f(rand() % 500 - 250, rand() % 500 - 250);
                        }
                    }
                    else
                    {
                        nBoids -= increment;
                        nBoids = nBoids < 10 ? 10 : nBoids;
                    }
                }

                if (currEvent.mouseWheelScroll.delta > 0)
                {
                    radius += 0.01f;
                    showCircles = true;
                }
                else
                {
                    if (radius < 0.01f)
                    {
                        radius = 0.01f;
                        showCircles = false;
                    }
                    else
                        radius -= 0.01f;
                }
                break;

            case (sf::Event::KeyReleased):
                if (currEvent.key.code == sf::Keyboard::LControl)
                    ctrlDown = false;
                break;
            case (sf::Event::KeyPressed):

                switch (currEvent.key.code)
                {
                case sf::Keyboard::F12:

                    window.create(sf::VideoMode(WIDTH, HEIGHT), "Boids", isFullscreen ? sf::Style::Default : sf::Style::Fullscreen);
                    isFullscreen = !isFullscreen;
                    break;

                case sf::Keyboard::LControl:
                    ctrlDown = true;
                    break;

                case sf::Keyboard::P:
                {
                    size_t increment = nBoids / 10;

                    nBoids += increment;
                    if (nBoids > MAX_BOIDS)
                        nBoids = MAX_BOIDS;

                    for (size_t i = nBoids - increment; i < nBoids; i++)
                    {
                        boids[i].position = sf::Vector2f(rand() % WIDTH - WIDTH / 2, rand() % HEIGHT - HEIGHT / 2);
                        boids[i].velocity = sf::Vector2f(rand() % 500 - 250, rand() % 500 - 250);
                    }
                    break;
                }
                case sf::Keyboard::M:
                    nBoids -= nBoids / 10;
                    if (nBoids < 10)
                        nBoids = 10;

                    break;
                case sf::Keyboard::Space:
                    showCircles = !showCircles;
                    break;
                case sf::Keyboard::F:
                    boidShader = boidShader == &shader ? &fastShader : &shader;
                    break;
                case sf::Keyboard::Escape:
                    window.close();
                    break;
                }
            }
        }

        circleShader.setUniform("radius", radius);
        timeSinceLastDraw = clock.restart();
        timeSinceLastUpdate += timeSinceLastDraw;
        float timeFps = 1.f / timeSinceLastDraw.asSeconds();

        while (timeSinceLastUpdate > timePerFrame)
        {
            timeSinceLastUpdate -= timePerFrame;

            auto dt = timePerFrame.asSeconds();

            for (auto *s : shaders)
            {
                if (nBoids < 1000)
                    s->setUniform("boidSize", 0.02f);
                else if (nBoids < 10000)
                    s->setUniform("boidSize", 0.01f);
                else if (nBoids < 100000)
                    s->setUniform("boidSize", 0.005f);
                else if (nBoids >= 100000)
                    s->setUniform("boidSize", 0.003f);
                else
                    s->setUniform("boidSize", 0.03f);
            }

            for (int i = 0; i < nBoids; i++)
            {
                auto &p = boids[i];
                p.update(dt);
                p.bounce(sf::Vector2f(WIDTH / 2.0, HEIGHT / 2.0));
                vertices[i][0] = p.position.x;
                vertices[i][1] = p.position.y;
                vertices[i][2] = boidShader == &fastShader ? 0 : FastArcTan2(p.velocity.y, p.velocity.x);
            }

            fpsCount++;
            fpsAvg += timeFps;
            if (seconds.getElapsedTime() >= sf::seconds(1))
            {
                avg = fpsAvg / fpsCount;
                fpsAvg = 0;
                fpsCount = 0;
                seconds.restart();
            }
        }

        fps.setString("boids: " + to_string(nBoids) + " fps: " + to_string_with_precision(avg, 0));

        window.clear(sf::Color(40, 44, 52));

        glVertexPointer(3, GL_FLOAT, 0, vertices.data());

        if (timeFps > 30 && showCircles)
        {
            sf::Shader::bind(&circleShader);
            glDrawArrays(GL_POINTS, 0, nBoids / 3);
        }

        sf::Shader::bind(boidShader);
        glDrawArrays(GL_POINTS, 0, nBoids / 3);

        sf::Shader::bind(nullptr);
        window.draw(fps);

        window.display();
        texture.update(window);
    }
}
