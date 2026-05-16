#include "TRexGame.h"

REGISTER_TYPE(InfoOverlay, const std::wstring&, const std::wstring&, const std::string&);


REGISTER_TYPE(ScoreBoard, TRexState*);


REGISTER_TYPE(Cactus, TRexState*, Dino*, int, const std::string&);


REGISTER_TYPE(Dino, TRexState*);


using namespace std;

// ─────────────────────────────────────────────────────────────────────────────
//  Sprites ASCII
// ─────────────────────────────────────────────────────────────────────────────

// Dino vivant  5x4
//  _O_
// (|||)
//  | |
//  J L

static const wstring SPRITE_DINO_RUN1 =
    L" _O_ "
    L"(|||)"
    L" | | "
    L" J L ";

// Dino vivant  (frame 2 : jambes inversées)
//  _O_
// (|||)
//  | |
//  L J

static const wstring SPRITE_DINO_RUN2 =
    L" _O_ "
    L"(|||)"
    L" | | "
    L" L J ";

// Dino mort (croix sur les yeux)
//  _X_
// (|||)
//  | |
//  | |

static const wstring SPRITE_DINO_DEAD =
    L" _X_ "
    L"(|||)"
    L" | | "
    L" | | ";

// Cactus  5x5
//  /|\
// //|\\
//  ||
//  ||
// /||\ 

static const wstring SPRITE_CACTUS =
    L" /|\\ "
    L"//|\\\\"
    L"  |  "
    L"  |  "
    L" /|\\ ";

// ─────────────────────────────────────────────────────────────────────────────
//  Dino
// ─────────────────────────────────────────────────────────────────────────────

int Cactus::nextId = 0;

Dino::Dino(TRexState* s)
    : trWidget(DINO_X, GND_Y - DINO_H, DINO_W, DINO_H, TopLeft, SPRITE_DINO_RUN1, "Dino")
{
    S = s;
    posYf = float(GND_Y - DINO_H);
}

void Dino::Tick()
{
    if (!S->started || S->gameOver)
        return;

    double dt = GetDeltaTime().GetDataActual(); // millisecondes
    float  dtS = float(dt) / 1000.f;            // secondes

    // ── Saut ──────────────────────────────────────────────────
    if (S->jumpSignal && !jumping)
    {
        velY    = JUMP_VEL;
        jumping = true;
        S->jumpSignal = false;
    }
    else
    {
        S->jumpSignal = false; // consommé si déjà en saut
    }

    // ── Physique ───────────────────────────────────────────────
    if (jumping)
    {
        velY  += GRAVITY * dtS;
        posYf += velY * dtS;

        int groundY = int(float(GND_Y - DINO_H));
        if (posYf >= float(groundY))
        {
            posYf   = float(groundY);
            velY    = 0.f;
            jumping = false;
        }
        SetPosition(DINO_X, int(posYf));
        SetChange(true);
    }

    // ── Animation des jambes (alterne toutes les 150 ms) ──────
    static float animTimer = 0.f;
    static bool  frame2    = false;

    if (!jumping)
    {
        animTimer += float(dt);
        if (animTimer >= 150.f)
        {
            animTimer = 0.f;
            frame2    = !frame2;
            SetContent(frame2 ? SPRITE_DINO_RUN2 : SPRITE_DINO_RUN1);
        }
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  Cactus
// ─────────────────────────────────────────────────────────────────────────────

Cactus::Cactus(TRexState* s, Dino* dino, int startX, const string& name)
    : trWidget(startX, GND_Y - CACTUS_H, CACTUS_W, CACTUS_H, TopLeft, SPRITE_CACTUS, name)
{
    S       = s;
    DinoRef = dino;
    posXf   = float(startX);
}

bool Cactus::CheckCollision() const
{
    // AABB simplifiée  (marge de 1 colonne pour être fair)
    int dinoX  = DinoRef->GetPosition().GetX().GetDataActual();
    int dinoY  = DinoRef->GetPosition().GetY().GetDataActual();
    int cactX  = GetPosition().GetX().GetDataActual();
    int cactY  = GetPosition().GetY().GetDataActual();

    bool overlapX = (cactX + 1 < dinoX + DINO_W - 1) && (cactX + CACTUS_W - 1 > dinoX + 1);
    bool overlapY = (cactY     < dinoY + DINO_H)      && (cactY + CACTUS_H > dinoY);

    return overlapX && overlapY;
}

void Cactus::Tick()
{
    if (!S->started || S->gameOver)
        return;

    float dtS = float(GetDeltaTime().GetDataActual()) / 1000.f;

    // Déplacement vers la gauche
    posXf -= S->speed * dtS;
    int newX = int(posXf);
    SetPosition(newX, GND_Y - CACTUS_H);
    SetChange(true);

    // Sorti à gauche → se détruire
    if (newX + CACTUS_W < 0)
    {
        Destroy();
        return;
    }

    // Collision avec le dino
    if (CheckCollision())
    {
        S->gameOver = true;
        DinoRef->SetContent(SPRITE_DINO_DEAD);
        DinoRef->SetChange(true);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  ScoreBoard
// ─────────────────────────────────────────────────────────────────────────────

ScoreBoard::ScoreBoard(TRexState* s)
    : trWidget(SCREEN_W - 18, 1, 17, 1, TopLeft, L"SCORE : 0       ", "ScoreBoard")
{
    S = s;
}

void ScoreBoard::Tick()
{
    if (!S->started || S->gameOver)
        return;

    // Formater le score
    wstring txt = L"SCORE : " + to_wstring(int(S->score));
    while (int(txt.size()) < 17) txt += L' ';
    SetContent(txt);
}

// ─────────────────────────────────────────────────────────────────────────────
//  InfoOverlay
// ─────────────────────────────────────────────────────────────────────────────

InfoOverlay::InfoOverlay(const wstring& line1, const wstring& line2, const string& name)
    : trWidget(0, 0, int(max(line1.size(), line2.size())), 2, MiddleCenter,
               line1 + line2, name)
{}

// ─────────────────────────────────────────────────────────────────────────────
//  TRexGame – helpers
// ─────────────────────────────────────────────────────────────────────────────

void TRexGame::WaitCreated(trWidget* w)
{
    World->CreateActor(w);
    while (!w->IsCreated()) {}
}

void TRexGame::ShowOverlay(const wstring& l1, const wstring& l2)
{
    DestroyOverlay(); // sécurité
    WaitCreated(new InfoOverlay(l1, l2, "Overlay"));
}

void TRexGame::DestroyOverlay()
{
    World->DestroyActor("Overlay");
}

void TRexGame::SpawnCactus()
{
    string name = "Cactus_" + to_string(cactusCount++);
    WaitCreated(new Cactus(&S, DinoPtr, SCREEN_W + 2, name));
}

void TRexGame::InitScene()
{
    // Sol
    GroundPtr = new Ground();
    WaitCreated(GroundPtr);

    // Dino
    DinoPtr = new Dino(&S);
    WaitCreated(DinoPtr);

    // Score
    ScoreBoardPtr = new ScoreBoard(&S);
    WaitCreated(ScoreBoardPtr);

    // Overlay de démarrage
    ShowOverlay(
        L"   T-REX ASCII GAME   ",
        L" [ESPACE] pour démarrer"
    );
}

void TRexGame::ResetScene()
{
    // Détruire tous les cactus encore vivants
    for (int i = 0; i < cactusCount; i++)
    {
        string name = "Cactus_" + to_string(i);
        World->DestroyActor(name);
    }
    cactusCount = 0;

    // Reset état
    S.gameOver    = false;
    S.started     = false;
    S.score       = 0.f;
    S.speed       = 22.f;
    S.jumpSignal  = false;
    S.resetSignal = false;

    spawnTimer    = 0.f;
    spawnCooldown = 2200.f;

    // Remettre le dino en position et sprite normal
    DinoPtr->SetContent(SPRITE_DINO_RUN1);
    DinoPtr->SetPosition(DINO_X, GND_Y - DINO_H);
    DinoPtr->posYf   = float(GND_Y - DINO_H);
    DinoPtr->velY    = 0.f;
    DinoPtr->jumping = false;
    DinoPtr->SetChange(true);

    // Reset score
    ScoreBoardPtr->SetContent(L"SCORE : 0        ");
    ScoreBoardPtr->SetChange(true);

    // Overlay de démarrage
    ShowOverlay(
        L"   T-REX ASCII GAME   ",
        L" [ESPACE] pour démarrer"
    );
}

// ─────────────────────────────────────────────────────────────────────────────
//  TRexGame::Run  –  boucle principale (thread UI)
// ─────────────────────────────────────────────────────────────────────────────

void TRexGame::Run()
{
    // ── Création de la scène ──────────────────────────────────────────────────
    InitScene();

    // ── Binding clavier ───────────────────────────────────────────────────────

    // ESPACE : sauter OU démarrer
    KB->CreateBTN(new trBTN_Key(
        VK_SPACE,
        OnRelease,
        PressToTrigger,
        [this]() {
            if (!S.started && !S.gameOver)
            {
                // Démarrer la partie
                DestroyOverlay();
                S.started = true;
            }
            else if (S.started && !S.gameOver)
            {
                // Sauter
                S.jumpSignal = true;
            }
        },
        DinoPtr
    ));

    // FLECHE HAUT : sauter aussi
    KB->CreateBTN(new trBTN_Key(
        VK_UP,
        OnRelease,
        PressToTrigger,
        [this]() {
            if (S.started && !S.gameOver)
                S.jumpSignal = true;
        },
        DinoPtr
    ));

    // R : rejouer après game over
    KB->CreateBTN(new trBTN_Key(
        0x52,   // touche R
        OnRelease,
        PressToTrigger,
        [this]() {
            if (S.gameOver)
            {
                DestroyOverlay();
                ResetScene();
            }
        },
        DinoPtr
    ));

    // ── Boucle de jeu ─────────────────────────────────────────────────────────

    while (true)
    {
        Sleep(16);  // ~60 fps pour la logique de spawn

        if (!S.started || S.gameOver)
        {
            // Afficher l'overlay game over une seule fois
            if (S.gameOver && !World->GetPtrActor("Overlay"))
            {
                wstring scoreLine = L"  SCORE FINAL : " + to_wstring(int(S.score)) + L"  ";
                ShowOverlay(
                    L"   GAME OVER !   [R] Rejouer   ",
                    scoreLine
                );
            }
            continue;
        }

        // ── Mise à jour du score & vitesse ────────────────────────────────────
        // On incrémente le score à environ 1 pt par frame (~60 fps)
        S.score += 0.3f;

        // La vitesse augmente progressivement
        S.speed = 22.f + S.score * 0.04f;
        if (S.speed > 60.f) S.speed = 60.f;

        // Le cooldown entre les cactus diminue avec le score
        spawnCooldown = max(900.f, 2200.f - S.score * 1.5f);

        // ── Spawn de cactus ───────────────────────────────────────────────────
        spawnTimer += 16.f;
        if (spawnTimer >= spawnCooldown)
        {
            spawnTimer = 0.f;
            SpawnCactus();
        }
    }
}