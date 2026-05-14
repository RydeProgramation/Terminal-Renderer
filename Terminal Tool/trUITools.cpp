#include "trUITools.h"

using namespace std;

// UI tool

void UITools::MoveCursorTo(const trCoordinate<int>& Coordinate_, uint8_t BorderWitdh_)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD PositionRelative = { static_cast<SHORT>(Coordinate_.GetX().GetDataActual() + (BorderWitdh_ * 2)), static_cast<SHORT>(Coordinate_.GetY().GetDataActual() + BorderWitdh_) };
	SetConsoleCursorPosition(hConsole, PositionRelative);
}

void UITools::MoveCursorTo(const trCoordinate<int>& Coordinate_)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD PositionRelative = { static_cast<SHORT>(Coordinate_.GetX().GetDataActual()), static_cast<SHORT>(Coordinate_.GetY().GetDataActual()) };
	SetConsoleCursorPosition(hConsole, PositionRelative);
}

void UITools::hideCursor()
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	CONSOLE_CURSOR_INFO cursorInfo;

	GetConsoleCursorInfo(hConsole, &cursorInfo);

	cursorInfo.bVisible = FALSE;

	SetConsoleCursorInfo(hConsole, &cursorInfo);
}

inline bool UITools::IsOutSide(const trCoordinate<int>& xy, uint8_t BorderWidth_)
{
	if (xy.GetY().GetDataActual() >= GetConsoleSize(BorderWidth_).GetSizeY().GetDataActual() || xy.GetX().GetDataActual() >= GetConsoleSize(BorderWidth_).GetSizeX().GetDataActual())
	{
		return true;
	}

	else if (xy.GetY().GetDataActual() <  0 || xy.GetX().GetDataActual() < 0)
	{
		return true;
	}

	return false;
}

bool UITools::IsOutSideFast(uint16_t x, uint16_t y, uint16_t maxX, uint16_t maxY)
{
	return (x >= maxX || y >= maxY || x < 0 || y < 0);
}

inline bool UITools::IsOutSide(const trCoordinate<int>& xy, uint8_t BorderWidth_, const trSize<uint16_t>& WindowSize_Border)
{
	if (xy.GetY().GetDataActual() >= WindowSize_Border.GetSizeY().GetDataActual() || xy.GetX().GetDataActual() >= WindowSize_Border.GetSizeX().GetDataActual())
	{
		return true;
	}

	else if (xy.GetY().GetDataActual() < 0 || xy.GetX().GetDataActual() < 0)
	{
		return true;
	}

	return false;
}

inline bool UITools::IsOutSide(const trCoordinate<int>& xy, uint8_t BorderWidth_, bool CountTouch)
{
	if (CountTouch)
	{
		if (xy.GetY().GetDataActual() >= GetConsoleSize(BorderWidth_).GetSizeY().GetDataActual() || xy.GetX().GetDataActual() >= GetConsoleSize(BorderWidth_).GetSizeX().GetDataActual())
		{
			return true;
		}

		else if (xy.GetY().GetDataActual() < 0 || xy.GetX().GetDataActual() < 0)
		{
			return true;
		}
	}

	else
	{
		if (xy.GetY().GetDataActual() >= GetConsoleSize(BorderWidth_).GetSizeY().GetDataActual() || xy.GetX().GetDataActual() > GetConsoleSize(BorderWidth_).GetSizeX().GetDataActual())
		{
			return true;
		}

		else if (xy.GetY().GetDataActual() < 0 || xy.GetX().GetDataActual() < 0)
		{
			return true;
		}
	}

	return false;
}

inline bool UITools::IsOutSide(int z, uint8_t BorderWidth_, bool CountTouch)
{
	MessageBox(
		NULL,                           // Pas de fenêtre parente
		L"Le problème est que la distance z, ne pourra jamais être bien convertit en x et y car si le x est plus grans que la taille de la fenêtre ça va juste rajouter une ligne en plus et ce n'est pas ce que nous voulons",      // Message
		L"Erreur",                      // Titre de la boîte
		MB_ICONERROR | MB_OK            // Icône d'erreur + bouton OK
	);

	// Convertir l'indice 1D en coordonnées 2D
	int y = z / GetConsoleSize().GetSizeX().GetDataActual() - BorderWidth_;  // Position verticale (y) 
	int x = z % GetConsoleSize().GetSizeX().GetDataActual() - BorderWidth_ * 2;  // Position horizontale (x) 

	if (CountTouch)
	{
		if (y >= GetConsoleSize(BorderWidth_).GetSizeY().GetDataActual() || x >= GetConsoleSize(BorderWidth_).GetSizeX().GetDataActual())
		{
			return true;
		}

		else if (y < 0 || x < 0)
		{
			return true;
		}
	}

	else
	{
		if (y >= GetConsoleSize(BorderWidth_).GetSizeY().GetDataActual() || x > GetConsoleSize(BorderWidth_).GetSizeX().GetDataActual())
		{
			return true;
		}

		else if (y < 0 || x < 0)
		{
			return true;
		}
	}

	return false;
}

inline bool UITools::IsOutSide(int z, uint8_t BorderWidth_)
{
	MessageBox(
		NULL,                           // Pas de fenêtre parente
		L"Le problème est que la distance z, ne pourra jamais être bien convertit en x et y car si le x est plus grans que la taille de la fenêtre ça va juste rajouter une ligne en plus et ce n'est pas ce que nous voulons",      // Message
		L"Erreur",                      // Titre de la boîte
		MB_ICONERROR | MB_OK            // Icône d'erreur + bouton OK
	);

	// Convertir l'indice 1D en coordonnées 2D
	int y = z / GetConsoleSize().GetSizeX().GetDataActual() - BorderWidth_;  // Position verticale (y) 
	int x = z % GetConsoleSize().GetSizeX().GetDataActual() - BorderWidth_ * 2;  // Position horizontale (x) 

	if (y >= GetConsoleSize(BorderWidth_).GetSizeY().GetDataActual() || x >= GetConsoleSize(BorderWidth_).GetSizeX().GetDataActual())
	{
		return true;
	}

	else if (y < 0 || x < 0)
	{
		return true;
	}

	return false;
}

inline bool UITools::IsOutSide(const trCoordinate<int>& xy)
{
	if (xy.GetY().GetDataActual() >= GetConsoleSize().GetSizeY().GetDataActual() || xy.GetX().GetDataActual() >= GetConsoleSize().GetSizeX().GetDataActual())
	{
		return true;
	}

	else if (xy.GetY().GetDataActual() < 0 || xy.GetX().GetDataActual() < 0)
	{
		return true;
	}

	return false;
}

inline bool UITools::IsOutSide(const trCoordinate<int>& xy, bool CountTouch)
{
	if (CountTouch)
	{
		if (xy.GetY().GetDataActual() >= GetConsoleSize().GetSizeY().GetDataActual() || xy.GetX().GetDataActual() >= GetConsoleSize().GetSizeX().GetDataActual())
		{
			return true;
		}

		else if (xy.GetY().GetDataActual() < 0 || xy.GetX().GetDataActual() < 0)
		{
			return true;
		}
	}

	else
	{
		if (xy.GetY().GetDataActual() >= GetConsoleSize().GetSizeY().GetDataActual() || xy.GetX().GetDataActual() > GetConsoleSize().GetSizeX().GetDataActual())
		{
			return true;
		}

		else if (xy.GetY().GetDataActual() < 0 || xy.GetX().GetDataActual() < 0)
		{
			return true;
		}
	}

	return false;
}

inline bool UITools::IsOutSide(int z)
{
	MessageBox(
		NULL,                           // Pas de fenêtre parente
		L"Le problème est que la distance z, ne pourra jamais être bien convertit en x et y car si le x est plus grans que la taille de la fenêtre ça va juste rajouter une ligne en plus et ce n'est pas ce que nous voulons",      // Message
		L"Erreur",                      // Titre de la boîte
		MB_ICONERROR | MB_OK            // Icône d'erreur + bouton OK
	);

	// Convertir l'indice 1D en coordonnées 2D
	int x = z % GetConsoleSize().GetSizeX().GetDataActual();  // Position horizontale (x)
	int y = z / GetConsoleSize().GetSizeX().GetDataActual();  // Position verticale (y)

	if (y >= GetConsoleSize().GetSizeY().GetDataActual() || x >= GetConsoleSize().GetSizeX().GetDataActual())
	{
		return true;
	}

	else if (y < 0 || x < 0)
	{
		return true;
	}

	return false;
}

inline bool UITools::IsOutSide(int z, bool CountTouch)
{
	MessageBox(
		NULL,                           // Pas de fenêtre parente
		L"Le problème est que la distance z, ne pourra jamais être bien convertit en x et y car si le x est plus grans que la taille de la fenêtre ça va juste rajouter une ligne en plus et ce n'est pas ce que nous voulons",      // Message
		L"Erreur",                      // Titre de la boîte
		MB_ICONERROR | MB_OK            // Icône d'erreur + bouton OK
	);

	// Convertir l'indice 1D en coordonnées 2D
	int x = z % GetConsoleSize().GetSizeX().GetDataActual();  // Position horizontale (x)
	int y = z / GetConsoleSize().GetSizeX().GetDataActual();  // Position verticale (y)

	if (CountTouch)
	{
		if (y >= GetConsoleSize().GetSizeY().GetDataActual() || x >= GetConsoleSize().GetSizeX().GetDataActual())
		{
			return true;
		}

		else if (y < 0 || x < 0)
		{
			return true;
		}
	}

	else
	{
		if (y >= GetConsoleSize().GetSizeY().GetDataActual() || x > GetConsoleSize().GetSizeX().GetDataActual())
		{
			return true;
		}

		else if (y < 0 || x < 0)
		{
			return true;
		}
	}

	return false;
}

bool UITools::IsInWidget(trWidget* WIDG, const trCoordinate<int>& crd) // a mettre dans UIToolsCore
{
	if (crd.GetX().GetDataActual() >= WIDG->GetAbsolutePosition().GetX().GetDataActual() && crd.GetX().GetDataActual() <= WIDG->GetAbsolutePosition().GetX().GetDataActual() + WIDG->GetSize().GetSizeX().GetDataActual())
	{
		if (crd.GetY().GetDataActual() >= WIDG->GetAbsolutePosition().GetY().GetDataActual() && crd.GetY().GetDataActual() <= WIDG->GetAbsolutePosition().GetY().GetDataActual() + WIDG->GetSize().GetSizeY().GetDataActual())
		{
			return true;
		}
	}

	return false;
}

void UITools::MoveCursorToOstream(const trCoordinate<int>& Pos, std::wostringstream* output, const trSize<uint16_t>& SizeOutput)
{
	output->seekp(Pos.GetX().GetDataActual() + Pos.GetY().GetDataActual() * SizeOutput.GetSizeX().GetDataActual());
}

void UITools::MoveCursorToOstream(const trCoordinate<int>& Pos, std::wostringstream* output, const trSize<uint16_t>& SizeOutput, uint8_t BorderW)
{
	int adjustedX = Pos.GetX().GetDataActual() + (BorderW * 2);
	int adjustedY = Pos.GetY().GetDataActual() + BorderW;

	int position = adjustedX + adjustedY * SizeOutput.GetSizeX().GetDataActual();

	output->seekp(position);
}

void UITools::CleanOstreamSize(std::wostringstream* output, const trSize<uint16_t>& SizeOutput)
{
	output->str(output->str().substr(0, SizeOutput.GetSizeX().GetDataActual() * SizeOutput.GetSizeY().GetDataActual()));
}

std::string UITools::DifferenceStringOutput(const std::string& ActualFrame_, const std::string& PreviousFrame_)
{
	int sizeX = GetConsoleSize().GetSizeX().GetDataActual();
	int sizeY = GetConsoleSize().GetSizeY().GetDataActual();

	// ✅ Vérification des tailles
	if (ActualFrame_.size() - 1 != PreviousFrame_.size() && ActualFrame_.size() != PreviousFrame_.size()) {
		// Affichage optionnel en debug
		// MessageBox(nullptr, L"Taille incompatible", L"Erreur", MB_ICONERROR);
		return ActualFrame_; // On affiche tout au cas où
	}

	std::string differences;

	for (int index = 0; index < (int)ActualFrame_.size(); index++)
	{
		if (ActualFrame_[index] != PreviousFrame_[index])
		{
			// 📍 Position du curseur (ANSI commence à 1,1)
			int i = index / sizeX + 1; // ligne
			int j = index % sizeX; // colonne

			differences += "\x1b[" + std::to_string(i) + ";" + std::to_string(j) + "H";

			// 🔁 Ajoute tous les caractères modifiés consécutifs
			while (ActualFrame_[index] != PreviousFrame_[index])
			{
				differences += ActualFrame_[index];
				index++;
			}
		}
	}

	return differences;
}

// buffer += "\x1b[4;10H##########";     // ligne 4, colonne 10

uint8_t UITools::BorderWidth = 6; // Largeur de la bordure par défaut

bool UITools::ForceRefresh = true;

bool UITools::Refreshed = false;