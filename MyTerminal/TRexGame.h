#pragma once
#include "Header.h"

#ifndef __TREX_GAME__
#define __TREX_GAME__

// ─────────────────────────────────────────────────────────────────────────────
//  État global du jeu  (partagé entre les actors via pointeur)
// ─────────────────────────────────────────────────────────────────────────────

struct TRexState
{
    bool  gameOver    = false;
    bool  started     = false;
    float score       = 0.f;
    float speed       = 22.f;   // colonnes / seconde
    bool  jumpSignal  = false;
    bool  resetSignal = false;
};

// ─────────────────────────────────────────────────────────────────────────────
//  Constantes de layout
// ─────────────────────────────────────────────────────────────────────────────

static constexpr int GND_Y    = 22;   // ligne du sol (adapter selon ton terminal)
static constexpr int SCREEN_W = 110;  // largeur utilisable
static constexpr int DINO_X   = 6;    // colonne fixe du dino
static constexpr int DINO_W   = 5;    // largeur widget dino
static constexpr int DINO_H   = 4;    // hauteur widget dino
static constexpr int CACTUS_W = 5;    // largeur widget cactus
static constexpr int CACTUS_H = 5;    // hauteur widget cactus

// ─────────────────────────────────────────────────────────────────────────────
//  Dino
// ─────────────────────────────────────────────────────────────────────────────

class Dino : public trWidget
{
public:
    TRexState* S;

    float posYf   = float(GND_Y - DINO_H);
    float velY    = 0.f;
    bool  jumping = false;

    // Physique (unités : rows/s  et  rows/s²)
    static constexpr float JUMP_VEL = -16.f;
    static constexpr float GRAVITY  =  32.f;

    Dino(TRexState* s);

    void Tick() override;
};

// ─────────────────────────────────────────────────────────────────────────────
//  Cactus
// ─────────────────────────────────────────────────────────────────────────────

class Cactus : public trWidget
{
public:
    TRexState* S;
    Dino*      DinoRef;
    float      posXf;
    static int nextId;

    Cactus(TRexState* s, Dino* dino, int startX, const std::string& name);

    void Tick() override;

private:
    bool CheckCollision() const;
};

// ─────────────────────────────────────────────────────────────────────────────
//  Sol
// ─────────────────────────────────────────────────────────────────────────────

class Ground : public trWidget
{
public:
    Ground() : trWidget(0, GND_Y, SCREEN_W, 1, TopLeft, std::wstring(SCREEN_W, L'_'), "Ground")
    {
    }
    void Tick() override {}
};

// ─────────────────────────────────────────────────────────────────────────────
//  Scoreboard  (coin haut droit)
// ─────────────────────────────────────────────────────────────────────────────

class ScoreBoard : public trWidget
{
public:
    TRexState* S;

    ScoreBoard(TRexState* s);
    void Tick() override;
};

// ─────────────────────────────────────────────────────────────────────────────
//  Overlay  (message centré)
// ─────────────────────────────────────────────────────────────────────────────

class InfoOverlay : public trWidget
{
public:
    InfoOverlay(const std::wstring& line1, const std::wstring& line2, const std::string& name);
    void Tick() override {}
};

// ─────────────────────────────────────────────────────────────────────────────
//  TRexGame  –  classe principale
// ─────────────────────────────────────────────────────────────────────────────

class TRexGame : public trUserInterface
{
public:
    TRexGame() : trUserInterface(RENDER_SYSTEM, 2, L"\033[0m") {}
    virtual ~TRexGame() {}

    // À appeler depuis main() après Start()
    void Run();

private:
    TRexState S;

    // Pointeurs vers les actors principaux (non-owning, World possède)
    Dino*       DinoPtr        = nullptr;
    Ground*     GroundPtr      = nullptr;
    ScoreBoard* ScoreBoardPtr  = nullptr;

    float spawnTimer    = 0.f;
    float spawnCooldown = 2200.f;    // ms entre deux cactus (diminue avec le score)
    int   cactusCount   = 0;

    // Helpers
    void WaitCreated(trWidget* w);
    void SpawnCactus();
    void ShowOverlay(const std::wstring& l1, const std::wstring& l2);
    void DestroyOverlay();
    void InitScene();
    void ResetScene();
};

#endif // __TREX_GAME__
