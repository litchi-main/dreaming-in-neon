#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <format>
#include <cmath>
#include <fstream>
#include <map>

#include "Objects.h"
#include "Utils.h"

using namespace sf;
using namespace std;

int main()
{
	srand((unsigned)time(nullptr));
	RenderWindow app(VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "dreaming in neon");
	app.setFramerateLimit(60);
	
	Texture tBackground, tPlatform;
	tBackground.loadFromFile("Resources/background.png");
	tPlatform.loadFromFile("Resources/platform.png");
	Sprite sprBackground(tBackground),
		sprPlatform(tPlatform);
	
	player P1;
	P1.loadSprites("Resources/spritesheet.png", "Resources/AnimationData.txt");
	View camera(Vector2f(WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f), Vector2f(CAMERA_WIDTH, CAMERA_HEIGHT));
	
	while (app.isOpen())
	{
		Event e;
		while (app.pollEvent(e))
		{
			if (e.type == Event::Closed)
				app.close();
		}

		P1.Input();
		P1.setCoordinates(P1.getX() + P1.getDx(), P1.getY() + P1.getDy());
		
		camera.setCenter(Vector2f(P1.getX(), P1.getY() - 150));
		app.setView(camera);
		
		sprBackground.setPosition(0, 100);
		sprPlatform.setPosition(0, ground);
		P1.setSprite("block");

		app.draw(sprBackground);
		app.draw(sprPlatform);
		app.draw(P1.getSprite());

		app.display();
	}
}