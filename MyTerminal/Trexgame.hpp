#pragma once

#include "Header.h"

#ifndef __TREX_GAME__
#define __TREX_GAME__

// ============================================================
//  GROUND
// ============================================================
class Ground : public trWidget
{
public:
	Ground(int consoleWidth)
		: trWidget(0, 0, consoleWidth, 1, BottomLeft, L"", "Ground")
	{
		// Ligne de sol : tirets sur toute la largeur
		std::wstring line(consoleWidth, L'_');
		this->SetContent(line);
	}

	~Ground() {}
};

// ============================================================
//  DINO
// ============================================================
class Dino : public trWidget
{
public:
	// 6 colonnes x 4 lignes
	static constexpr int W = 6;
	static constexpr int H = 4;

	// Position sol (Y relatif depuis le bas) — ajuste selon ta console
	static constexpr int GROUND_Y_OFFSET = 5; // lignes depuis le bas

	bool isJumping = false;
	bool isDead = false;
	float jumpVel = 0.0f;
	float posYf = 0.0f; // position Y flottante (en lignes, vers le haut depuis le sol)

	static const std::wstring SPRITE_RUN1;
	static const std::wstring SPRITE_RUN2;
	static const std::wstring SPRITE_DEAD;

	int animTimer = 0;
	int animFrame = 0;

	Dino(int startX, int groundY)
		: trWidget(startX, groundY - H, W, H, TopLeft, L"", "Dino")
	{
		posYf = static_cast<float>(groundY - H);
		this->SetContent(SPRITE_RUN1);
	}

	void Jump()
	{
		if (!isJumping && !isDead)
		{
			isJumping = true;
			jumpVel = -0.55f; // vitesse initiale vers le haut (unités/ms)
		}
	}

	void Die()
	{
		isDead = true;
		this->SetContent(SPRITE_DEAD);
	}

	void Tick() override
	{
		if (isDead) return;

		double dt = this->GetDeltaTime().GetDataActual(); // en ms

		// --- Gravité ---
		if (isJumping)
		{
			float gravity = 0.0030f; // accélération vers le bas (unités/ms²)
			jumpVel += gravity * static_cast<float>(dt);
			posYf += jumpVel * static_cast<float>(dt);

			// Limite : retour au sol
			int groundY = static_cast<int>(this->GetPosition().GetY().GetDataActual())
				- static_cast<int>(posYf - this->GetPosition().GetY().GetDataActual());

			// On récupère la position Y de départ stockée dans posYf0
			// En pratique on gère ça via SetPosition directement
			int newY = static_cast<int>(posYf);
			this->SetPosition(this->GetPosition().GetX().GetDataActual(), newY);
		}

		// --- Animation course ---
		animTimer += static_cast<int>(dt);
		if (animTimer > 180)
		{
			animFrame = 1 - animFrame;
			animTimer = 0;
			this->SetContent(animFrame == 0 ? SPRITE_RUN1 : SPRITE_RUN2);
		}
	}

	~Dino() {}
};

// Sprites du dino (6 cols x 4 lignes, séparés par \n implicitement via SetContent)
// trWidget remplit le contenu ligne par ligne dans la boîte W x H
const std::wstring Dino::SPRITE_RUN1 =
L" ,_. "
L"(o v)"
L" )=( "
L"_/ \\_";

const std::wstring Dino::SPRITE_RUN2 =
L" ,_. "
L"(o v)"
L" )=( "
L"/ \\_  ";

const std::wstring Dino::SPRITE_DEAD =
L" ,_. "
L"(x v)"
L" )=( "
L"_/ \\_";

// ============================================================
//  CACTUS
// ============================================================
class Cactus : public trWidget
{
public:
	static constexpr int W = 4;
	static constexpr int H = 4;

	float posXf;
	float speed; // colonnes / ms
	bool  markedForDelete = false;

	static const std::wstring SPRITE;

	Cactus(int startX, int groundY, float speed_, std::string name_)
		: trWidget(startX, groundY - H, W, H, TopLeft, L"", name_)
		, posXf(static_cast<float>(startX))
		, speed(speed_)
	{
		this->SetContent(SPRITE);
	}

	void Tick() override
	{
		double dt = this->GetDeltaTime().GetDataActual();

		posXf -= speed * static_cast<float>(dt);

		int newX = static_cast<int>(posXf);

		if (newX + W < 0)
		{
			markedForDelete = true;
			this->SetDestroy(true);
			return;
		}

		this->SetPosition(newX, this->GetPosition().GetY().GetDataActual());
	}

	~Cactus() {}
};

const std::wstring Cactus::SPRITE =
L" /\\ "
L"|/\\|"
L" || "
L"_||_";

// ============================================================
//  SCORE WIDGET
// ============================================================
class ScoreWidget : public trWidget
{
public:
	int score = 0;

	ScoreWidget(int consoleWidth)
		: trWidget(consoleWidth - 16, 1, 14, 1, TopLeft, L"SCORE: 0      ", "Score")
	{
	}

	void SetScore(int s)
	{
		score = s;
		std::wstring txt = L"SCORE: " + std::to_wstring(s);
		// Pad to fixed width
		while (static_cast<int>(txt.size()) < 14) txt += L' ';
		this->SetContent(txt);
	}

	~ScoreWidget() {}
};

// ============================================================
//  GAME OVER WIDGET
// ============================================================
class GameOverWidget : public trWidget
{
public:
	static constexpr int W = 30;
	static constexpr int H = 3;

	GameOverWidget()
		: trWidget(0, 0, W, H, MiddleCenter, L"", "GameOver")
	{
		this->SetContent(
			L"   *** GAME  OVER ***   "
			L"                        "
			L"  Appuie sur ESPACE...  "
		);
	}

	~GameOverWidget() {}
};

// ============================================================
//  TREX GAME (UI principale)
// ============================================================
class TRexGame : public trUserInterface
{
public:
	TRexGame();
	virtual ~TRexGame();

	// Point d'entrée appelé après Start()
	void Run();

private:
	// --- helpers ---
	void StartGame();
	void GameOver();
	void SpawnCactus();
	bool CheckCollision();

	// --- état ---
	bool  gameRunning = false;
	bool  gameOver_ = false;
	int   score_ = 0;
	float scoreTimer_ = 0.0f;
	float spawnTimer_ = 0.0f;
	float spawnDelay_ = 2500.0f; // ms entre cactus
	float gameSpeed_ = 0.030f;  // colonnes/ms

	int cactusCount_ = 0;

	// pointeurs pratiques (non owning — World les gère)
	Dino* dino_ = nullptr;
	ScoreWidget* scoreWidg_ = nullptr;
	Ground* ground_ = nullptr;

	int consoleW_ = 80;
	int groundY_ = 0;  // ligne Y du sol dans la console
};

#endif // __TREX_GAME__