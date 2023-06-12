#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <format>
#include <cmath>
#include <fstream>
#include <string>
#include <map>
#include "Objects.h"
#include "Utils.h"

using namespace sf;
using namespace std;

void writeText(RenderWindow& app, Text& text, string string, Vector2f pos)
{
	text.setString(string);
	float originx = (float)string.length() * (float)text.getCharacterSize() / 2.f;
	float originy = (float)text.getCharacterSize() / 2.f;
	text.setOrigin(originx, originy);
	text.setPosition(pos);
	app.draw(text);
}

bool gameloop(Texture& SpriteSheet, RenderWindow& app, View& camera, Sprite& sprBackground, Sprite& sprPlatform, int& p1wins, int& p2wins, Text& text)
{
	text.setCharacterSize(20);
	RectangleShape p1h(Vector2f(CAMERA_WIDTH / 7 * 3, 40));
	p1h.setOrigin(CAMERA_WIDTH / 7 * 3, 0);
	p1h.setFillColor(sf::Color(249, 215, 28));
	RectangleShape p2h(Vector2f(CAMERA_WIDTH / 7 * 3, 40));
	p2h.setOrigin(0, 0);
	p2h.setFillColor(sf::Color(249, 215, 28));

	player* P1 = new swordguy(WINDOW_WIDTH / 2 - round_start, 7.f, -3.f, -12.f, 1000.f, side::left, Keyboard::D, Keyboard::A, Keyboard::W, Keyboard::J, Keyboard::K);
	P1->loadSprites(&SpriteSheet, "Resources/AnimationData.txt");
	P1->LoadFrameData("Resources/FrameData.txt");
	player* P2 = new swordguy(WINDOW_WIDTH / 2 + round_start, 7.f, -3.f, -12.f, 1000.f, side::left, Keyboard::Right, Keyboard::Left, Keyboard::Up, Keyboard::C, Keyboard::V);
	P2->loadSprites(&SpriteSheet, "Resources/AnimationData.txt");
	P2->LoadFrameData("Resources/FrameData.txt");

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

		P1->Input(*P2);                                                              //zaidejo inputs
		P2->Input(*P1);

		P1->changeFrame();                                                        //animation stuff
		P2->changeFrame();
		P1->setSprite();
		P2->setSprite();

		P1->swapInputs(P1->updateFacing(P2->getX()));                               //apvercia zaideja
		P2->swapInputs(P2->updateFacing(P1->getX()));

		float dx0 = P1->getDx();
		P1->pushCollision(*P2->getCollisionBox(), P2->getDx());            //zaidejai "stumia" viens kita"
		P2->pushCollision(*P1->getCollisionBox(), dx0);            //zaidejai "stumia" viens kita"

		P1->setCoordinates(P1->getX() + P1->getDx(), P1->getY() + P1->getDy());       //judina veikejus
		P2->setCoordinates(P2->getX() + P2->getDx(), P2->getY() + P2->getDy());
		P1->groundCollision();
		P2->groundCollision();

		if (cornerLeft.Collision(*P1->getCollisionBox()))                          //neleidzia uzeit uz ekrano krastu
			P1->setCoordinates(10, P1->getY());
		if (cornerRight.Collision(*P1->getCollisionBox()))
			P1->setCoordinates(WINDOW_WIDTH - 10, P1->getY());
		if (cornerLeft.Collision(*P2->getCollisionBox()))
			P2->setCoordinates(10, P2->getY());
		if (cornerRight.Collision(*P2->getCollisionBox()))
			P2->setCoordinates(WINDOW_WIDTH - 10, P2->getY());

		if (abs(P1->getX() == 10.f || P1->getX() == WINDOW_WIDTH - 10.f))
			P2->setCoordinates(P2->getX() + P1->wallCollision(*P2->getCollisionBox()), P2->getY());
		if (abs(P2->getX() == 10.f || P2->getX() == WINDOW_WIDTH - 10.f))
			P1->setCoordinates(P1->getX() + P2->wallCollision(*P1->getCollisionBox()), P1->getY());

		float cameraX = (P1->getX() + P2->getX()) / 2;
		float cameraY = (P1->getY() + P2->getDy()) / 2 + 150;
		if (cameraX < 480)
			cameraX = 480;
		if (cameraX > 720)
			cameraX = 720;
		camera.setCenter(Vector2f(cameraX, cameraY));
		app.setView(camera);

		sprBackground.setPosition(0, 100);
		sprPlatform.setPosition(0, stage);

		if (P1->getHp() > 0)
			p1h.setScale(Vector2f(P1->getHp() / 1000.0f, 1.0f));
		else
		{
			p1h.setScale(Vector2f(0.0f, 1.0f));
			app.draw(p1h);
			return 0;
		}
		p1h.setPosition(cameraX - round_start / 5, cameraY - 230);
		if (P2->getHp() > 0)
			p2h.setScale(Vector2f(P2->getHp() / 1000.0f, 1.0f));
		else
		{
			p2h.setScale(Vector2f(0.0f, 1.0f));
			app.draw(p2h);
			return 1;
		}
		p2h.setPosition(cameraX - round_start / 5 + 40, cameraY - 230);

		app.draw(sprBackground);
		app.draw(sprPlatform);
		app.draw(p1h);
		app.draw(p2h);
		if (p1wins)
		{
			string smt("  WIN ");
			smt[0] = p1wins + 48;
			if (p1wins > 1)
				smt[5] = 'S';
			writeText(app, text, smt, Vector2f(p1h.getPosition().x - 5, p1h.getPosition().y - 15));
		}
		if (p2wins)
		{
			string smt("  WIN ");
			smt[0] = p2wins + 48;
			if (p2wins > 1)
				smt[5] = 'S';
			writeText(app, text, smt, Vector2f(p2h.getPosition().x + 55, p1h.getPosition().y - 15));
		}
		app.draw(P1->getSprite());
		app.draw(P2->getSprite());

		app.display();
	}
}



int main()
{
	srand((unsigned)time(nullptr));
	RenderWindow app(VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "dreaming in neon");
	app.setFramerateLimit(60);

	Texture tBackground, tMenu, tPlatform;
	tBackground.loadFromFile("Resources/background.png");
	tMenu.loadFromFile("Resources/Menu.png");
	tPlatform.loadFromFile("Resources/platform.png");
	Sprite sprBackground(tBackground),
		sprPlatform(tPlatform),
		sprMenu(tMenu);

	Texture SpriteSheet;
	SpriteSheet.loadFromFile("Resources/spritesheet.png");

	View camera(Vector2f(WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f), Vector2f(CAMERA_WIDTH, CAMERA_HEIGHT));
	
	int gamestate = 0,
		p1wins = 0,
		p2wins = 0;
	Texture menutex;
	menutex.loadFromFile("Resources/menubgr.png");
	Sprite menu(menutex);
	int selection = 0;

	sf::Font font;
	font.loadFromFile("Resources/arialbd.ttf");
	sf::Text text;
	text.setFont(font);
	text.setCharacterSize(40);
	text.setFillColor(Color::Red);
	text.setOutlineThickness(1);
	text.setOutlineColor(Color::Black);
	text.setPosition(Vector2f(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2));

	bool clean_menu_input = 0;
	bool gamerez = 0;

	while (app.isOpen())
	{
		Event e;
		while (app.pollEvent(e))
		{
			if (e.type == Event::Closed)
				app.close();
		}
		if (gamestate == 0)
		{
			menu.setPosition(0, 0);
			app.draw(menu);

			if (Keyboard::isKeyPressed(Keyboard::W) || Keyboard::isKeyPressed(Keyboard::S))
			{
				if (!clean_menu_input)
				{
					selection++;
					if (selection > 1)
						selection = 0;
					clean_menu_input = 1;
				}
			}
			else
				clean_menu_input = 0;

			if (selection == 0)
			{
				writeText(app, text, "START<", Vector2f(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 - 50));
				writeText(app, text, "EXIT", Vector2f(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 + 50));
			}
			else
			{
				writeText(app, text, "START", Vector2f(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 - 50));
				writeText(app, text, "EXIT<", Vector2f(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 + 50));
			}

			if (Keyboard::isKeyPressed(Keyboard::Enter))
			{
				if (selection == 0)
				{
					gamerez = gameloop(SpriteSheet, app, camera, sprBackground, sprPlatform, p1wins, p2wins, text);
					if (gamerez)
						p1wins++;
					else
						p2wins++;
					gamestate = 1;
					menu.setPosition(-1351, 0);
				}
				else
					break;
			}
		}
		else
		{
			app.draw(menu);
			if (menu.getPosition().x > 0)
			{
				if (Keyboard::isKeyPressed(Keyboard::W) || Keyboard::isKeyPressed(Keyboard::S))
				{
					if (!clean_menu_input)
					{
						selection++;
						if (selection > 1)
							selection = 0;
						clean_menu_input = 1;
					}
				}
				else
					clean_menu_input = 0;

				if (gamerez)
					writeText(app, text, "P1 WINS", Vector2f(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 - 150));
				if (!gamerez)
					writeText(app, text, "P2 WINS", Vector2f(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 - 150));
				if (selection == 0)
				{
					writeText(app, text, "REMATCH<", Vector2f(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2));
					writeText(app, text, "EXIT", Vector2f(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 + 70));
				}
				else
				{
					writeText(app, text, "REMATCH", Vector2f(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2));
					writeText(app, text, "EXIT<", Vector2f(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 + 70));
				}

				if (Keyboard::isKeyPressed(Keyboard::Enter))
				{
					if (selection == 0)
					{
						gamerez = gameloop(SpriteSheet, app, camera, sprBackground, sprPlatform, p1wins, p2wins, text);
						if (gamerez)
							p1wins++;
						else
							p2wins++;					
						menu.setPosition(-1351, 0);
					}
					else
						break;
				}
			}
			else
				menu.setPosition(menu.getPosition().x + 30.f, 0);
		}
		text.setCharacterSize(40);
		camera.setCenter(Vector2f(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2));
		app.setView(camera);
		app.display();
	}
	app.close();   
	return 0;
}