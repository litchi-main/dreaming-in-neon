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

void MovementLR(Player& player, PSprite FWalk, PSprite BWalk, PSprite Idle)
{
	if (sf::Keyboard::isKeyPressed(player.Back))
	{
		player.dx = player.backward_v;
		if (abs(player.dx) > abs(player.backward_v))
			player.dx = player.backward_v;
		player.sprite = BWalk;
	}
	if (sf::Keyboard::isKeyPressed(player.Forward))
	{
		player.dx = player.forward_v;
		if (abs(player.dx) > abs(player.forward_v))
			player.dx = player.forward_v;
		player.sprite = FWalk;
	}
	if (!Keyboard::isKeyPressed(player.Forward) && !Keyboard::isKeyPressed(player.Back))
	{
		player.dx -= player.dx;
		player.sprite = Idle;
	}
}

void NeutralMovement(Player& player, PSprite FWalk, PSprite BWalk, PSprite Idle, PSprite Jump)
{
	MovementLR(player,FWalk,BWalk,Idle);
	if (Keyboard::isKeyPressed(player.Up) && player.air_action > 0 && player.jump_input == 1)
	{
		player.dy = player.jump_v;
		player.air_action--;
		player.jump_input = 0;
		player.sprite = Jump;
	}
}

void AirMovement(Player& player, PSprite Jump)
{
	
	if (player.y >= ground)
	{
		player.y = ground;
		player.air_action = 2;                //galima n kartu jumpint
	}
	if (Keyboard::isKeyPressed(player.Up) && player.air_action > 0 && player.jump_input == 1)
	{
		MovementLR(player,Jump,Jump,Jump);
		player.dy = player.jump_v;
		player.air_action--;
		player.jump_input = 0;
	}
	if (Keyboard::isKeyPressed(player.Up) != 1)
		player.jump_input = 1;
}

void UpdateFacing(Player& player1, Player& player2, Sprite& sprplayer) //apvercia sprites ir movement controls, bus veliau naudinga nustatyti, ar blokuojama ataka
{
	side old = player1.side;
	if (player1.x < player2.x)
		player1.side = side::left;
	if (player1.x > player2.x)
		player1.side = side::right;
	if (old != player1.side)
	{
		Keyboard::Key m = player1.Forward;
		player1.Forward = player1.Back;
		player1.Back = m;
		player1.forward_v *= -1;
		player1.backward_v *= -1;
		sprplayer.scale(-1.f, 1.f);
		player1.hurtbox[0].first *= -1;
		player1.hurtbox[1].first *= -1;
	}
}

void UpdateNState(Player& player)
{
	if (player.y == ground)
		player.state = Nstate::neutral;
	if (player.y < ground)
		player.state = Nstate::air;
}

void NCollision(Player& player1, Player& player2, bool k)
{
	if (player1.side == side::left && k)
	{
		float d = player1.x + player1.width - (player2.x - player2.width);
		if (d > 0) {
			player2.dx += player1.dx;
		}
	}
	if (player1.side == side::right && k)
	{
		float d = player2.x + player2.width - (player1.x - player1.width);
		if (d > 0) {
			player2.dx += player1.dx;
		}
	}
}

void AirCollision(Player& player1, Player& player2)
{
	if (player1.y < player2.y && player1.y >= player2.y - player2.height)
	{
		if (player1.side == side::left)
		{
			float d = player1.x + player1.width - (player2.x - player2.width);
			if (d > 0) {
				player2.x += d;
			}
		}
		if (player1.side == side::right)
		{
			float d = player2.x + player2.width - (player1.x - player1.width);
			if (d > 0) {
				player2.x -= d;
			}
		}
	}
}

void CornerCollision(Player& player, side player2)
{
	if (player.x + player.width > WINDOW_WIDTH - 20)    //sudaro "siena" kiekviename kraste
	{
		player.x = WINDOW_WIDTH - 20 - player.width;
		if(player2 != side::right)
			player.AtCorner = side::right;
	}
	else if (player.x - player.width < 20)
	{
		player.x = 20 + player.width;
		if(player2 != side::left)
			player.AtCorner = side::left;
	}
	else player.AtCorner = side::none;
	if (player.x + player.width == WINDOW_WIDTH - 20 && player2 != side::right && player.y == ground)
	{
		player.AtCorner = side::right;
	}
	if (player.x - player.width == 20 && player2 != side::left && player.y == ground)
	{
		player.AtCorner = side::left;
	}
}

void PlayerCornerCollision(Player& player1, Player& player2)
{
	if (player2.y <= player1.y && player2.y >= player1.y - player1.height || player2.y - player2.height <= player1.y && player2.y - player2.height >= player1.y - player1.height)
	{
		if (player1.AtCorner == side::left)
		{
			if (player2.x - player2.width <= player1.x + player1.width)
			{
				player2.x = player1.x + player1.width + player2.width;
			}
		}
		if (player1.AtCorner == side::right)
		{
			if (player2.x + player2.width >= player1.x - player1.width)
			{
				player2.x = player1.x - player1.width - player2.width;
			}
		}
	}
}

void LoadSprite(PSprite& sprite, float x, float y, string file)
{
	sprite.x = x;
	sprite.y = y;
	sprite.tex.loadFromFile(file);
}

void LoadFrameData(map<string,Attack>& A, string file, string tStart, string tActive, string tRecovery)
{
	ifstream fr(file);
	string key;
	Attack B;
	while (fr >> key)
	{
		fr >> B.startup >> B.active >> B.recovery >> B.hitstun >> B.blockstun
			>> B.hitbox[0].first >> B.hitbox[0].second >> B.hitbox[1].first >> B.hitbox[1].second
			>> B.dmg
			>> B.tStart.x >> B.tStart.y 
			>> B.tActive.x >> B.tActive.y 
			>> B.tRecovery.x >> B.tRecovery.y 
			>> B.pushback >> B.pushblock;
		A[key] = B;
		string str = tStart;
		str.insert(16, key);
		A[key].tStart.tex.loadFromFile(str);
		str = tActive;
		str.insert(16, key);
		A[key].tActive.tex.loadFromFile(str);
		str = tRecovery;
		str.insert(16, key);
		A[key].tRecovery.tex.loadFromFile(str);
	}
	fr.close();
}



void AttackInput(Player& player, map<string,Attack>& N, Attack prev)
{
	if (player.attack_input == 'n')
	{
		if (Keyboard::isKeyPressed(player.B) && prev.recovery != N["5B"].recovery)
		{
			player.attack_input = 'B';
			player.cancel = 0;
			player.move = N["5B"];
			player.attack = Astate::attack;
			if (player.side == side::right)
			{
				player.move.hitbox[0].first *= -1;
				player.move.hitbox[1].first *= -1;
			}
		}
		if (Keyboard::isKeyPressed(player.A))
		{
			player.attack_input = 'A';
			player.cancel = 0;
			player.move = N["5A"];
			player.attack = Astate::attack;
			if (player.side == side::right)
			{
				player.move.hitbox[0].first *= -1;
				player.move.hitbox[1].first *= -1;
			}
		}
	}
}

void Attacking(Player& player)
{
	player.dx = 0;
	if (player.move.startup > 0)
	{
		player.sprite = player.move.tStart;
		player.move.startup--;
		if (player.move.startup == 0)
			player.sprite = player.move.tActive;
	}
	else if (player.move.active > 0)
	{
		player.sprite = player.move.tActive;
		player.move.active--;
		if (player.move.active == 0)
			player.sprite = player.move.tRecovery;
	}
	else if (player.move.recovery > 0)
	{
		player.sprite = player.move.tRecovery;
		player.move.recovery--;
	}
}

bool HitCheck(Player& player1, Player& player2, int& hitstop)
{
	if (player1.side == side::left)
		if (player1.move.hitbox[0].first + player1.x <= player2.x + player2.hurtbox[1].first &&
			player1.move.hitbox[1].first + player1.x >= player2.x + player2.hurtbox[0].first &&
			player1.move.hitbox[0].second + player1.y >= player2.y + player2.hurtbox[1].second &&
			player1.move.hitbox[1].second + player1.y <= player2.y + player2.hurtbox[0].second)
		{
			player2.dx = 0;
			if (Keyboard::isKeyPressed(player2.Back))
			{
				hitstop += player1.move.active;
				player1.move.hitbox[0].first = 0;
				player1.move.hitbox[0].second = 0;
				player1.move.hitbox[1].first = 0;
				player1.move.hitbox[1].second = 0;
				player2.attack = Astate::block;
				player2.stun = player1.move.blockstun;
				player2.health -= player1.move.dmg / 6;
				player2.x += player1.move.pushblock;
				if (player2.AtCorner == side::right)
					player1.x -= player1.move.pushblock;
				return 1;
			}
			else
			{
				hitstop += player1.move.active;
				player1.move.hitbox[0].first = 0;
				player1.move.hitbox[0].second = 0;
				player1.move.hitbox[1].first = 0;
				player1.move.hitbox[1].second = 0;
				player2.attack = Astate::hitstun;
				player2.stun = player1.move.hitstun;
				player2.health -= player1.move.dmg;
				player2.x += player1.move.pushback;
				if (player2.AtCorner == side::right)
					player1.x -= player1.move.pushback;
				return 1;
			}
		}
	if (player1.side == side::right)
		if (player1.move.hitbox[0].first + player1.x >= player2.x + player2.hurtbox[1].first && 
			player1.move.hitbox[1].first + player1.x <= player2.x + player2.hurtbox[0].first &&
			player1.move.hitbox[0].second + player1.y >= player2.y + player2.hurtbox[1].second &&
			player1.move.hitbox[1].second + player1.y <= player2.y + player2.hurtbox[0].second)
		{
			if (Keyboard::isKeyPressed(player2.Back))
			{
				hitstop += player1.move.active;
				player1.move.hitbox[0].first = 0;
				player1.move.hitbox[0].second = 0;
				player1.move.hitbox[1].first = 0;
				player1.move.hitbox[1].second = 0;
				player2.attack = Astate::block;
				player2.stun = player1.move.blockstun;
				player2.health -= player1.move.dmg / 6;
				player2.x -= player1.move.pushblock;
				if (player2.AtCorner == side::left)
					player1.x += player1.move.pushblock;
				return 1;
			}
			else
			{
				hitstop += player1.move.active;
				player1.move.hitbox[0].first = 0;
				player1.move.hitbox[0].second = 0;
				player1.move.hitbox[1].first = 0;
				player1.move.hitbox[1].second = 0;
				player2.attack = Astate::hitstun;
				player2.stun = player1.move.hitstun;
				player2.health -= player1.move.dmg;
				player2.x -= player1.move.pushback;
				if (player2.AtCorner == side::left)
					player1.x += player1.move.pushback;
				return 1;
			}
		}
	return 0;
}

void CleanInputCheck(Player& player)
{
	if (!Keyboard::isKeyPressed(player.A) && player.attack_input == 'A')
	{
		player.attack_input = 'n';
	}
	if (!Keyboard::isKeyPressed(player.B) && player.attack_input == 'B')
	{
		player.attack_input = 'n';
	}
}

int main()
{
	srand((unsigned)time(nullptr));
	RenderWindow app(VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "a kusoge");
	app.setFramerateLimit(60);

	Texture tBackground, tPlatform;
	PSprite tPlayer1Idle, tPlayer2Idle, tPlayer1FWalk, tPlayer2FWalk, tPlayer1BWalk, tPlayer2BWalk, 
		tPlayer1Jump, tPlayer2Jump, tPlayer1JumpStart, tPlayer2JumpStart,
		tPlayer15BStart, tPlayer15BActive, tPlayer15BRecovery, tPlayer25BStart, tPlayer25BActive, tPlayer25BRecovery,
		tPlayer1HitG, tPlayer2HitG, tPlayer1BlockG, tPlayer2BlockG;
	PSprite* P1Idle = &tPlayer1Idle;
	PSprite* P1FWalk = &tPlayer1FWalk;
	PSprite* P1BWalk = &tPlayer1BWalk;
	PSprite* P1Jmp = &tPlayer1Jump;
	PSprite* P1JmpS = &tPlayer1JumpStart;
	PSprite* P1HitG = &tPlayer1HitG;
	PSprite* P1BlockG = &tPlayer1BlockG;
	PSprite* P2Idle = &tPlayer2Idle;
	PSprite* P2FWalk = &tPlayer2FWalk;
	PSprite* P2BWalk = &tPlayer2BWalk;
	PSprite* P2Jmp = &tPlayer2Jump;
	PSprite* P2JmpS = &tPlayer2JumpStart;
	PSprite* P2HitG = &tPlayer2HitG;
	PSprite* P2BlockG = &tPlayer2BlockG;
	LoadSprite(*P1Idle, 173, 150, "Resources/sword idle.png");
	LoadSprite(*P1FWalk, 177, 150, "Resources/sword walk f.png");
	LoadSprite(*P1BWalk, 175, 150, "Resources/sword back walk.png");
	LoadSprite(*P1Jmp, 175, 117, "Resources/sword jump.png");
	LoadSprite(*P1JmpS, 175, 137, "Resources/sword jump start.png");
	LoadSprite(*P1HitG, 145, 147, "Resources/sword hit ground.png");
	LoadSprite(*P1BlockG, 35, 218, "Resources/sword block ground.png");
	LoadSprite(*P2Idle, 173, 150, "Resources/sword idle 2.png");
	LoadSprite(*P2FWalk, 177, 150, "Resources/sword walk f 2.png");
	LoadSprite(*P2BWalk, 175, 150, "Resources/sword back walk 2.png");
	LoadSprite(*P2Jmp, 175, 117, "Resources/sword jump 2.png");
	LoadSprite(*P2JmpS, 175, 137, "Resources/sword jump start 2.png");
	LoadSprite(*P2HitG, 145, 147, "Resources/sword hit ground 2.png");
	LoadSprite(*P2BlockG, 35, 218, "Resources/sword block ground 2.png");
	tBackground.loadFromFile("Resources/background.png");
	tPlatform.loadFromFile("Resources/platform.png");

	map<string, Attack> p1n;
	map<string, Attack> p2n;
	LoadFrameData(p1n, "Resources/FrameData.txt", "Resources/sword  startup.png", 
		"Resources/sword  active.png", "Resources/sword  recovery.png");
	LoadFrameData(p2n, "Resources/FrameData.txt", "Resources/sword  startup 2.png", 
		"Resources/sword  active 2.png", "Resources/sword  recovery 2.png");

	sf::Font font;
	font.loadFromFile("Resources/arialbd.ttf");


	Sprite sprBackground(tBackground);
	Sprite sprPlayer1(tBackground);
	sprPlayer1.setOrigin(tPlayer1FWalk.x, tPlayer1FWalk.y);
	Sprite sprPlayer2(tBackground);
	sprPlayer2.setOrigin(tPlayer2FWalk.x, tPlayer2FWalk.y);
	sprPlayer2.scale(-1.f, 1.f);
	Sprite sprPlatform(tPlatform);

	Player player1;
	player1.x = WINDOW_WIDTH / 2 - round_start;
	player1.y = ground;
	player1.side = side::left;
	player1.Forward = Keyboard::D;
	player1.Back = Keyboard::A;
	player1.Up = Keyboard::W;
	player1.A = Keyboard::H;
	player1.B = Keyboard::J;
	player1.state = neutral;
	player1.attack = rest;
	player1.hurtbox[0].first = 35;
	player1.hurtbox[1].first = -35;
	player1.hurtbox[0].second = 0;
	player1.hurtbox[1].second = -150;
	RectangleShape p1h(Vector2f(WINDOW_WIDTH / 5 * 2, 50));
	p1h.setOrigin(WINDOW_WIDTH / 5 * 2, 0);
	p1h.setFillColor(sf::Color(249, 215, 28));
	

	Player player2;
	player2.x = WINDOW_WIDTH / 2 + round_start;
	player2.y = ground;
	player2.side = side::right;
	player2.forward_v *= -1;
	player2.backward_v *= -1;
	player2.Forward = Keyboard::C;
	player2.Back = Keyboard::B;
	player2.Up = Keyboard::F;
	player2.A = Keyboard::Comma;
	player2.B = Keyboard::Period;
	player2.state = neutral;
	player2.attack = rest;
	player2.hurtbox[0].first = -35;
	player2.hurtbox[1].first = 35;
	player2.hurtbox[0].second = 0;
	player2.hurtbox[1].second = -150;
	RectangleShape p2h(Vector2f(WINDOW_WIDTH / 5 * 2, 50));
	p2h.setOrigin(0, 0);
	p2h.setFillColor(sf::Color(249, 215, 28));

	Attack none;

	View camera(Vector2f(WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f), Vector2f(720.f, 480.f));

	while (app.isOpen())
	{
		Event e;
		while (app.pollEvent(e))
		{
			if (e.type == Event::Closed)
				app.close();
		}

		player1.dy += 0.3f; // gravitacija su pagreiciu
		player2.dy += 0.3f;
		player1.y += player1.dy;
		player2.y += player2.dy;

		if (player1.y >= ground)
		{
			player1.y = ground;
			player1.air_action = 2;                //galima n kartu jumpint
		}
		if (player2.y >= ground)
		{
			player2.y = ground;
			player2.air_action = 2;                //galima n kartu jumpint
		}

		if (player1.attack == Astate::rest)
		{
			UpdateNState(player1);
			UpdateFacing(player1, player2, sprPlayer1);

			if (player1.state == Nstate::neutral && player1.y == ground)
			{
				AttackInput(player1, p1n, none);
				player1.dx = 0;
				NeutralMovement(player1, *P1FWalk, *P1BWalk, *P1Idle, *P1Jmp);
			}
			if (player1.state == Nstate::air)
			{
				AirMovement(player1, *P1Jmp);
			}
		}
		if (player2.attack == Astate::rest)
		{
			UpdateNState(player2);
			UpdateFacing(player2, player1, sprPlayer2);

			if (player2.state == Nstate::neutral && player2.y == ground)
			{
				AttackInput(player2, p2n, none);
				player2.dx = 0;
				NeutralMovement(player2, *P2FWalk, *P2BWalk, *P2Idle, *P2Jmp);
			}
			if (player2.state == Nstate::air)
			{
				AirMovement(player2, *P2Jmp);
			}
		}

		if (player1.attack == Astate::attack || player1.attack == Astate::recovery)
		{
			Attacking(player1);
			if (player1.move.startup > 0)
				player1.attack = Astate::attack;
			else if (player1.move.active > 0)
			{
				if (player1.cancel)
					AttackInput(player1, p1n, player1.move);
				player1.attack = Astate::attack;
			}
			else if (player1.move.recovery > 0)
				player1.attack = Astate::recovery;
			else player1.attack = Astate::rest;
		}
		if (player2.attack == Astate::attack || player2.attack == Astate::recovery)
		{
			Attacking(player2);
			if (player2.move.startup > 0)
				player2.attack = Astate::attack;
			else if (player2.move.active > 0)
			{
				if (player2.cancel)
					AttackInput(player2, p2n, player2.move);
				player2.attack = Astate::attack;
			}
			else if (player2.move.recovery > 0)
				player2.attack = Astate::recovery;
			else player2.attack = Astate::rest;
		}

		if (player1.move.startup == 0 && player1.move.active > 0)
		{
			if (HitCheck(player1, player2, player1.move.active))
			{
				player1.cancel = 1;
			}
		}
		if (player2.move.startup == 0 && player2.move.active > 0)
		{
			if (HitCheck(player2, player1, player2.move.active))
			{
				player2.cancel = 1;
			}
		}

		CleanInputCheck(player1);
		CleanInputCheck(player2);

		if (player1.stun > 0)
		{
			player1.dx = 0;
			if (player1.attack == Astate::hitstun)
				player1.sprite = *P1HitG;
			if (player1.attack == Astate::block)
				player1.sprite = *P1BlockG;
			player1.stun--;
			if (player1.stun == 0)
				player1.attack = Astate::rest;
		}
		if (player2.stun > 0)
		{
			player2.dx = 0;
			if (player2.attack == Astate::hitstun)
				player2.sprite = *P2HitG;
			if (player2.attack == Astate::block)
				player2.sprite = *P2BlockG;
			player2.stun--;
			if (player2.stun == 0)
				player2.attack = Astate::rest;
		}

		if (player1.state != Nstate::air && player2.state != Nstate::air)
		{
			float odx = player2.dx;
			NCollision(player1, player2, player1.side == side::left && player1.dx > 0 || player1.side == side::right && player1.dx < 0);
			float ndx = player2.dx;
			player2.dx = odx;
			NCollision(player2, player1, player2.side == side::left && player2.dx > 0 || player2.side == side::right && player2.dx < 0);
			player2.dx = ndx;
		}

		if (player1.AtCorner == side::none && player2.AtCorner == side::none)
		{
			AirCollision(player1, player2);
			AirCollision(player2, player1);
		}

		player1.x += player1.dx;
		player2.x += player2.dx;

		PlayerCornerCollision(player1, player2);
		PlayerCornerCollision(player2, player1);

		CornerCollision(player1, player2.AtCorner);
		CornerCollision(player2, player1.AtCorner);

		camera.setCenter(Vector2f((player1.x + player2.x) / 2.f, min(min(player1.y, player2.y), WINDOW_HEIGHT - 240.f)));
		app.setView(camera);

		sprPlayer1.setOrigin(player1.sprite.x, player1.sprite.y);
		sprPlayer2.setOrigin(player2.sprite.x, player2.sprite.y);
		sprPlayer1.setTexture(player1.sprite.tex);
		sprPlayer2.setTexture(player2.sprite.tex);


		app.draw(sprBackground);
		sprPlatform.setPosition(0, ground);
		app.draw(sprPlatform);

		if(player1.health > 0)
			p1h.setScale(Vector2f(player1.health / 4500.0f, 1.0f));
		else
			p1h.setScale(Vector2f(0.0f, 1.0f));
		p1h.setPosition((WINDOW_WIDTH - round_start) / 2, 40);
		app.draw(p1h);

		if (player2.health > 0)
			p2h.setScale(Vector2f(player2.health / 4500.0f, 1.0f));
		else
			p2h.setScale(Vector2f(0.0f, 1.0f));
		p2h.setPosition((WINDOW_WIDTH + round_start) / 2, 40);
		app.draw(p2h);


		sprPlayer1.setPosition(player1.x, player1.y);
		app.draw(sprPlayer1);
		sprPlayer2.setPosition(player2.x, player2.y);
		app.draw(sprPlayer2);

		app.display();

		if (player1.health <= 0 || player2.health <= 0)
			break;
	}

	sf::Text text;
	text.setFont(font);
	text.setString("0");
	text.setCharacterSize(20);
	text.setFillColor(Color::Red);
	text.setOutlineThickness(1);
	text.setOutlineColor(Color::Black);
	text.setPosition(Vector2f(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2));

	app.draw(sprBackground);

	if (player1.health > 0)
	{
		text.setString("P1 WINS");
	}
	if (player2.health > 0)
	{
		text.setString("P2 WINS");
	}	
	if (player1.health <= 0 && player2.health <= 0)
	{
		text.setString("DRAW");
	}
	app.draw(text);
	return 0;
}