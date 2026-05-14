#include "trWorld.h"

using namespace UITools;
using namespace UIToolsCore;
using namespace std;

// INI default

trWorld::trWorld() : Actors(new std::unordered_map<std::string, trActor*>())
{

}

// INI deep copy

trWorld::trWorld(const trWorld& other) : Actors(new std::unordered_map<std::string, trActor*>(*other.Actors))
{

}

// Copy

trWorld& trWorld::operator=(const trWorld& other)
{
	if (this == &other) { return *this; }

	if (Actors == nullptr) {
		Actors = new std::unordered_map<std::string, trActor*>(*other.Actors);
	} else {
		*Actors = *other.Actors;
	}

	return *this;
}

/// ACTORS MANG ///

bool trWorld::CreateActor(trActor* WIDG)
{
	// std::lock_guard<std::mutex> lock(*Mutex); // des fois abort()

	for (auto& pair : *Actors) {
		if (pair.second == WIDG) {
			MessageBox(
				NULL,                           // Pas de fenêtre parente
				L"Widget already created",      // Message
				L"Erreur",                      // Titre de la boîte
				MB_ICONERROR | MB_OK            // Icône d'erreur + bouton OK
			);
			return false;
		}
	}

	WIDG->GetName().GetDataActual() == "None" ? WIDG->APPLY(GetConsoleSize(BorderWidth)) : (void)0;
	(*Actors)[WIDG->GetName().GetDataActual()] = WIDG;
	WIDG->Init();
	WIDG->APPLY(GetConsoleSize());
	
	return true;
}

bool trWorld::DestroyActor(trActor* WIDG)
{
	WIDG->SetDestroy(true);

	return true;
}

bool trWorld::DestroyActor(const string& name) // VERIF SI BUG
{
	// Ne pas mettre de mutex ici

	if ((*Actors)[name])
	{
		return DestroyActor((*Actors)[name]);
	}

	MessageBox(
		NULL,
		L"Widget Not found for destroying",
		L"Erreur",
		MB_ICONERROR | MB_OK
	);

	return false;
}

bool trWorld::Destroy()
{
	for (auto it = Actors->begin(); it != Actors->end();)
	{
		if (it->second->GetDestroy().GetDataActual())
		{
			delete it->second;        // supprime l'objet pointé
			it = Actors->erase(it);   // efface et récupère l'itérateur valide suivant
			ForceRefresh = true;
		}
		else
		{
			++it;
		}
	}

	return true; // Retourne true si tous les widgets ont été détruits avec succès
}

const trActor& trWorld::GetActor(const string& Name) const
{
	auto it = Actors->find(Name);

	if (it != Actors->end())
	{
		return *(it->second);
	}

	else
	{
		MessageBox(
			NULL,
			L"Widget Not found",
			L"Warning",
			MB_ICONERROR | MB_OK
		);

		return trActor::EmptyActor; // Retourne un actor vide
	}
}

const std::unordered_map<std::string, trActor*>& trWorld::GetActors() const
{
	return *Actors; // Retourne un pointeur vers la map contenant tous les widgets
}

// Protected 

trActor* trWorld::GetPtrActor(const std::string& Name) const
{
	auto it = Actors->find(Name);

	if (it != Actors->end())
	{
		return (it->second);
	}

	else
	{
		MessageBox(
			NULL,
			L"Widget Not found",
			L"Warning",
			MB_ICONERROR | MB_OK
		);

		return nullptr; // Retourne le widget vide
	}
}

void trWorld::UpdateActors(const trSize<uint16_t>& ConsoleSize_Border, const double& DeltaTime_)
{
	for (auto& widg : *Actors)
	{
		widg.second->APPLY(ConsoleSize_Border);
		widg.second->SetDeltaTime(DeltaTime_);
	}
}

void trWorld::Update()
{
	std::map<trActor*, trRect<int>> Rects;

	for (auto& widg : *Actors)
	{
		trPawn* pawPtr = dynamic_cast<trPawn*>(widg.second);
		if (!pawPtr) continue; 

		trWidget* widgetPtr = dynamic_cast<trWidget*>(pawPtr);

		if (widgetPtr == nullptr)
		{
			// C'est un trPawn MAIS PAS un trWidget
			int PosX = pawPtr->GetAbsolutePosition().GetX().GetDataActual();
			int PosY = pawPtr->GetAbsolutePosition().GetY().GetDataActual();
			Rects[pawPtr] = trRect<int>(PosX, PosY, PosX, PosY);
		}

		else
		{
			// C'est un trPawn ET aussi un trWidget (widgetPtr valide)
			int Left_ = widgetPtr->GetAbsolutePosition().GetX().GetDataActual();
			int Right_ = widgetPtr->GetAbsolutePosition().GetX().GetDataActual() + widgetPtr->GetSize().GetSizeX().GetDataActual();
			int Top_ = widgetPtr->GetAbsolutePosition().GetY().GetDataActual();
			int Bottom_ = widgetPtr->GetAbsolutePosition().GetY().GetDataActual() + widgetPtr->GetSize().GetSizeY().GetDataActual();

			Rects[widgetPtr] = trRect<int>(Left_, Top_, Right_, Bottom_);
		}
	}

	DetectCollisionsOptimized(Rects);
}

// FNC

void trWorld::DetectCollisionsOptimized(const std::map<trActor*, trRect<int>>& rectMap)
{
	// Extraire dans un vector pour trier
	std::vector<std::pair<trActor*, trRect<int>>> rects(rectMap.begin(), rectMap.end());

	// Tri selon left()
	std::sort(rects.begin(), rects.end(), [](auto& a, auto& b) {
		return a.second.left() < b.second.left();
		});

	for (size_t i = 0; i < rects.size(); ++i)
	{
		const auto& A = rects[i].second;
		trActor* actorA = rects[i].first;

		for (size_t j = i + 1; j < rects.size(); ++j)
		{
			const auto& B = rects[j].second;
			trActor* actorB = rects[j].first;

			if (B.left() > A.right())
				break;

			// Collision si chevauchement vertical
			if (!(A.bottom() < B.top() || A.top() > B.bottom()))
			{
				trPawn* pawn = dynamic_cast<trPawn*>(actorA);

				if (pawn)
				{
					pawn->SetPawnCollision(actorB, true);
				}

				pawn = dynamic_cast<trPawn*>(actorB);

				if (pawn)
				{
					pawn->SetPawnCollision(actorA, true);
				}
			}
		}
	}
}

// DESTRUCTEUR

trWorld::~trWorld()
{
	delete Actors;
}