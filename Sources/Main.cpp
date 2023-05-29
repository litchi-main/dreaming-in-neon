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
	
	sf::Font font;
	font.loadFromFile("Resources/arialbd.ttf");
	sf::Text text1;
	text1.setFont(font);
	text1.setCharacterSize(20);
	text1.setFillColor(Color::Red);
	text1.setOutlineThickness(1);
	text1.setOutlineColor(Color::Black);
	text1.setPosition(Vector2f(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2));
	sf::Text text2;
	text2.setFont(font);
	text2.setCharacterSize(20);
	text2.setFillColor(Color::Red);
	text2.setOutlineThickness(1);
	text2.setOutlineColor(Color::Black);
	text2.setPosition(Vector2f(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2));

	Texture SpriteSheet;
	SpriteSheet.loadFromFile("Resources/spritesheet.png");

	player P1(WINDOW_WIDTH / 2 - round_start, side::left, Keyboard::D, Keyboard::A, Keyboard::W);
	P1.loadSprites(&SpriteSheet, "Resources/AnimationData.txt");	
	player P2(WINDOW_WIDTH / 2 + round_start, side::left, Keyboard::B, Keyboard::C, Keyboard::F);
	P2.loadSprites(&SpriteSheet, "Resources/AnimationData.txt");

	View camera(Vector2f(WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f), Vector2f(CAMERA_WIDTH, CAMERA_HEIGHT));
	
	box cornerLeft(0.f, WINDOW_HEIGHT, 0.f, -3000.f);
	box cornerRight(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_WIDTH, -3000.f);

	while (app.isOpen())
	{
		Event e;
		while (app.pollEvent(e))
		{
			if (e.type == Event::Closed)
				app.close();
		}

		P1.Input();                                                              //zaidejo inputs
		P2.Input();

		P1.changeFrame();                                                        //animation stuff
		P2.changeFrame();
		P1.setSprite();
		P2.setSprite();

		P1.swapInputs(P1.updateFacing(P2.getX()));                               //apvercia zaideja
		P2.swapInputs(P2.updateFacing(P1.getX()));

		P1.setDx(P2.pushCollision(P1.getCollisionBox(), P1.getDx()));

		P1.setCoordinates(P1.getX() + P1.getDx(), P1.getY() + P1.getDy());       //judina veikejus
		P2.setCoordinates(P2.getX() + P2.getDx(), P2.getY() + P2.getDy());
		P1.groundCollision();
		P2.groundCollision();

		if (cornerLeft.Collision(P1.getCollisionBox()))                          //neleidzia uzeit uz ekrano krastu
			P1.setCoordinates(10, P1.getY());
		if (cornerRight.Collision(P1.getCollisionBox()))
			P1.setCoordinates(WINDOW_WIDTH - 10, P1.getY());
		if (cornerLeft.Collision(P2.getCollisionBox()))
			P2.setCoordinates(10, P2.getY());
		if (cornerRight.Collision(P2.getCollisionBox()))
			P2.setCoordinates(WINDOW_WIDTH - 10, P2.getY());

		if (abs(P1.getX() == 10.f || P1.getX() == WINDOW_WIDTH - 10.f))                         
			P2.setCoordinates(P2.getX() + P1.wallCollision(P2.getCollisionBox()), P2.getY());	
		if (abs(P2.getX() == 10.f || P2.getX() == WINDOW_WIDTH - 10.f))
			P1.setCoordinates(P1.getX() + P2.wallCollision(P1.getCollisionBox()), P1.getY());
		
		camera.setCenter(Vector2f((P1.getX() + P2.getX()) / 2, P1.getY() - 150));
		app.setView(camera);
		
		sprBackground.setPosition(0, 100);
		sprPlatform.setPosition(0, ground);

		app.draw(sprBackground);
		app.draw(sprPlatform);
		app.draw(P1.getSprite());
		app.draw(P2.getSprite());

		if (P1.getCollisionBox()->Collision(P2.getCollisionBox()))
			text1.setString("colliding");
		else
			text1.setString("not colliding");

		text1.setPosition(P1.getX(), P1.getY() - 250);
		text2.setString(format("{}", P2.getX()));
		text2.setPosition(P2.getX(), P2.getY() - 400);
		app.draw(text1);
		app.draw(text2);

		app.display();
	}
}