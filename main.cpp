#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFPhysics.h>
#include <vector>

using namespace std;
using namespace sf;
using namespace sfp;

void LoadTex(Texture& tex, string filename) {
    if (!tex.loadFromFile(filename)) {
        cout << "Could not load " << filename << endl;
    }
}

Vector2f GetTextSize(Text text) {
    FloatRect r = text.getGlobalBounds();
    return Vector2f(r.width, r.height);
}

int main() {
    RenderWindow window(VideoMode(800, 600), "Balloon Buster");
    World world(Vector2f(0, 0));
    
    int arrows(5);
    int score(0);

    //  Create crossbow sprite
    PhysicsSprite& crossbow = *new PhysicsSprite();
    Texture crossTex;
    LoadTex(crossTex, "images/crossbow.png");
    crossbow.setTexture(crossTex);
    Vector2f sz = crossbow.getSize();
    crossbow.setCenter(Vector2f(400, 600 - (sz.y / 2)));

    //  Create arrow prefab sprite
    PhysicsSprite arrow;
    Texture arrowTex;
    LoadTex(arrowTex, "images/arrow.png");
    arrow.setTexture(arrowTex);
    bool drawing(false);
    
    //  Create bounding box
    PhysicsRectangle top;
    top.setSize(Vector2f(800, 10));
    top.setCenter(Vector2f(400, 5));
    top.setStatic(true);
    world.AddPhysicsBody(top);

    PhysicsRectangle right;
    right.setSize(Vector2f(10, 600));
    right.setCenter(Vector2f(795, 300));
    right.setStatic(true);
    world.AddPhysicsBody(right);

    //  Create duck prefab
    Texture duckTex;
    LoadTex(duckTex, "images/duck.png");
    PhysicsShapeList<PhysicsSprite> ducks;

    bool drawingArrow(false);
    float t(1);

    arrow.onCollision = [&arrow, &world, &top, &drawingArrow](PhysicsBodyCollisionResult result) {
        if (result.object2 == top) {
            drawingArrow = false;
            world.RemovePhysicsBody(arrow);
        }
    };

    Font fnt;
    if (!fnt.loadFromFile("arial.ttf")) {
        cout << "Could not load font." << endl;
        exit(3);
    }

    //  Track deltaTime
    Clock clock;
    Time lastTime(clock.getElapsedTime());
    Time currentTime(lastTime);

    while (drawingArrow || arrows > 0) {
        //  update physics
        currentTime = clock.getElapsedTime();
        Time deltaTime = currentTime - lastTime;
        long deltaMS = deltaTime.asMilliseconds();

        if (deltaMS > 9) {
            lastTime = currentTime;
            world.UpdatePhysics(deltaMS);

            if (t < 0) {
                PhysicsSprite& duck = ducks.Create();
                duck.setTexture(duckTex);
                Vector2f sz = duck.getSize();
                duck.setSize(Vector2f(sz.x / 4, sz.y / 4));
                duck.setCenter(Vector2f(50, 20 + (sz.y / 8)));
                duck.setVelocity(Vector2f(0.25, 0));
                world.AddPhysicsBody(duck);

                duck.onCollision =
                    [&drawingArrow, &arrow, &world, &duck, &ducks, &score]
                    (PhysicsBodyCollisionResult result) {
                    if (result.object2 == arrow) {
                        drawingArrow = false;
                        world.RemovePhysicsBody(duck);
                        world.RemovePhysicsBody(arrow);
                        ducks.QueueRemove(duck);
                        score += 10;
                    }
                    else {
                        world.RemovePhysicsBody(duck);
                        ducks.QueueRemove(duck);
                    }
                };
        
                t = 1;

            }

            ducks.DoRemovals();
            
            if (!drawingArrow && Keyboard::isKeyPressed(Keyboard::Space)) {
                arrows--;
                drawingArrow = true;
                arrow.setCenter(crossbow.getCenter());
                arrow.setVelocity(Vector2f(0, -1));
                world.AddPhysicsBody(arrow);
            }

            window.clear();
            
            for (PhysicsShape& duck : ducks) {
                window.draw((PhysicsSprite&) duck);
            }

            window.draw(crossbow);
            if (drawingArrow)
                window.draw(arrow);
        
            Text scoreText;
            scoreText.setString(to_string(score));
            scoreText.setFont(fnt);
            scoreText.setPosition(Vector2f(790 - GetTextSize(scoreText).x, 600 - 2 * GetTextSize(scoreText).y));


            Text arrowText;
            arrowText.setString(to_string(arrows));
            arrowText.setFont(fnt);
            arrowText.setPosition(Vector2f(GetTextSize(arrowText).x, 600 - 2 * GetTextSize(arrowText).y));

            window.draw(scoreText);
            window.draw(arrowText);

            t -= deltaTime.asSeconds();

            window.display();
        }

    }
    //  show last frame
    window.display();

    cout << "Game over!";

    Text gameOverText;
    gameOverText.setString("GAME OVER");
    gameOverText.setFont(fnt);
    sz = GetTextSize(gameOverText);
    gameOverText.setPosition(400 - (sz.x / 2), 300 - (sz.y / 2));
    window.draw(gameOverText);

    window.display();

    while (true);
}

