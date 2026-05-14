#include "trRender.h"

using namespace UITools;
using namespace UIToolsCore;
using namespace std;
using namespace trPrintModule;

// INI default

trRender::trRender() : trRender(BUFFER_SYSTEM, 6, L"\033[0m")
{

}

// INI

trRender::trRender(uint8_t RenderType_, int BordW_, std::wstring RstClr) : SizeWindow(new trSize<uint16_t>(0, 0)), SizeWindow_Border(new trSize<uint16_t>(0, 0)), CursorSelector(new trData<int>(0)), Render_(new std::wostringstream()), RenderColor_(new std::wostringstream()), BaseColor(new std::wstring(RstClr)), PreviousRender_(new std::string("")), RenderType(0)
{
	switch (RenderType_)
	{
	case DIRECT_SYSTEM:
		RenderType = RenderType_;
		break;
	case BUFFER_SYSTEM:
		RenderType = RenderType_;
		break;
	case RENDER_SYSTEM:
		RenderType = RenderType_;
		break;
	default:
		MessageBox(
			NULL,
			L"Aucun RenderType ne correspond",
			L"Erreur",
			MB_ICONERROR | MB_OK
		);
		std::exit(1);
	}
}

// INI deep copy

trRender::trRender(const trRender& other) : SizeWindow(new trSize<uint16_t>(*other.SizeWindow)), SizeWindow_Border(new trSize<uint16_t>(*other.SizeWindow_Border)), CursorSelector(new trData<int>(*other.CursorSelector)), Render_(new std::wostringstream()), RenderColor_(new std::wostringstream()), RenderType(other.RenderType), BaseColor(new std::wstring(*other.BaseColor)), PreviousRender_(new std::string())
{

}

// Copy

trRender& trRender::operator=(const trRender& other)
{
	if (this == &other) { return *this; }

	if (SizeWindow == nullptr) {
		SizeWindow = new trSize<uint16_t>(*other.SizeWindow);
	} else {
		*SizeWindow = *other.SizeWindow;
	}

	if (SizeWindow_Border == nullptr) {
		SizeWindow_Border = new trSize<uint16_t>(*other.SizeWindow_Border);
	}
	else {
		*SizeWindow_Border = *other.SizeWindow_Border;
	}

	if (CursorSelector == nullptr) {
		CursorSelector = new trData<int>(*other.CursorSelector);
	} else {
		*CursorSelector = *other.CursorSelector;
	}

	if (Render_ == nullptr) {
		Render_ = new std::wostringstream();
	} else {
		Render_->str(L""); // Vide le contenu de l'ostringstream
	}

	if (PreviousRender_ == nullptr) {
		PreviousRender_ = new std::string(*other.PreviousRender_);
	} else {
		*PreviousRender_ = *other.PreviousRender_;
	}

	if (RenderColor_ == nullptr) {
		RenderColor_ = new std::wostringstream();
	} else {
		RenderColor_->str(L""); // Vide le contenu de l'ostringstream
	}

	if (BaseColor == nullptr) {
		BaseColor = new std::wstring(*other.BaseColor);
	} else {
		*BaseColor = *other.BaseColor;
	}

	RenderType = other.RenderType;

	return *this;
}

// SET

void trRender::SetBaseColor(const std::wstring& BaseColor_)
{
	*BaseColor = BaseColor_;
}

void trRender::SetCursorSelector(const trData<int>& CursorSelector_)
{
	*CursorSelector = CursorSelector_;
}

// GET

const std::wstring& trRender::GetBaseColor() const
{
	return *BaseColor;
}

const trSize<uint16_t>& trRender::GetSizeWindow() const
{
	return *SizeWindow;
}

const trSize<uint16_t>& trRender::GetSizeWindowBorder() const
{
	return *SizeWindow_Border;
}

const trData<int>& trRender::GetCursorSelector() const
{
	return *CursorSelector;
}

const std::wostringstream& trRender::GetRender() const
{
	return *Render_;
}

const std::wostringstream& trRender::GetRenderColor() const
{
	return *RenderColor_;
}

const uint8_t trRender::GetRenderType() const
{
	return RenderType;
}

// FNC

void trRender::UpdateSizeWindow()
{
	SizeWindow->SetSize(GetConsoleSize().GetSizeX().GetDataActual(), GetConsoleSize().GetSizeY().GetDataActual());
	SizeWindow_Border->SetSize(GetConsoleSize(BorderWidth).GetSizeX().GetDataActual(), GetConsoleSize(BorderWidth).GetSizeY().GetDataActual());
	SizeWindow->Update();
	SizeWindow_Border->Update();
}

void trRender::Border()
{
	if (RenderType == DIRECT_SYSTEM)
	{
		MoveCursorTo(trCoordinate<int>(0, 0));

		std::ostringstream output;

		string topBottomBorder(SizeWindow->GetSizeX().GetDataActual() * BorderWidth, '*');
		string sideBorder(BorderWidth * 2, '*');
		string emptyLine(SizeWindow->GetSizeX().GetDataActual() - BorderWidth * 4, ' ');

		output << topBottomBorder;

		string middleLine = sideBorder + emptyLine + sideBorder;
		for (int i = BorderWidth; i < SizeWindow->GetSizeY().GetDataActual() - BorderWidth; ++i)
		{
			output << middleLine;
		}

		output << topBottomBorder;

		SetColorConsole(15);

		cout << output.str();

		MoveCursorTo(trCoordinate<int>(0, 0));
	}

	else if (RenderType == BUFFER_SYSTEM)
	{
		MoveCursorToOstream(trCoordinate<int>(0, 0), Render_, *SizeWindow);

		wstring topBottomBorder(SizeWindow->GetSizeX().GetDataActual() * BorderWidth, '*');
		wstring sideBorder(BorderWidth * 2, '*');
		wstring emptyLine(max(SizeWindow->GetSizeX().GetDataActual() - BorderWidth * 4, 0), ' ');

		(*Render_) << topBottomBorder;

		wstring middleLine = sideBorder + emptyLine + sideBorder;

		for (int i = BorderWidth; i < SizeWindow->GetSizeY().GetDataActual() - BorderWidth; ++i)
		{
			(*Render_) << middleLine;
		}

		(*Render_) << topBottomBorder;

		CleanOstreamSize(Render_, *SizeWindow);
	}

	else if (RenderType == RENDER_SYSTEM)
	{
		int WindowWidth = SizeWindow->GetSizeX().GetDataActual();
		int WindowHeight = SizeWindow->GetSizeY().GetDataActual();

		(*Render_) << L"\x1b[2J\x1b[H";

		if (WindowWidth <= BorderWidth * 4 || WindowHeight <= BorderWidth * 2 /* A enlver c'est pour le test*/ || true)
		{
			wstring topBottomBorder(WindowWidth * BorderWidth, '*');
			wstring sideBorder(BorderWidth * 2, '*');

			(*Render_) << L"\x1b[1;1H" + topBottomBorder;

			wstring l = L"";
			wstring laststart = to_wstring(WindowWidth - BorderWidth * 2 + 1);

			for (int i = 1 + BorderWidth; i < WindowHeight - BorderWidth + 1; ++i)
			{
				l = to_wstring(i);
				(*Render_) << L"\x1b[" + l + L";" + L"1H" + sideBorder;
				(*Render_) << L"\x1b[" + l + L";" + laststart + L"H" + sideBorder;
			}

			(*Render_) << L"\x1b[" + to_wstring(WindowHeight - BorderWidth + 1) + L";1H" + topBottomBorder;
		}

		else
		{
			// ici juste mettre right et bottom 
		}

		CleanOstreamSize(Render_, *SizeWindow);
	}
}

// FNC

void trRender::DisplayWidget(trWidget* WIDG)
{
	///
	/// je pense que je peut deja optimiser par juste crée des appel pour éviter d'appeler 50 000 fois une fonction inutilement
	///

	// améliorer pour direct_terminal a voir quoi faire
	if (RenderType == DIRECT_SYSTEM)
	{
		CleanWidget(WIDG);

		std::wostringstream output;

		int col = 0;

		for (int ln = WIDG->GetAbsolutePosition().GetY().GetDataActual(); ln < WIDG->GetAbsolutePosition().GetY().GetDataActual() + WIDG->GetSize().GetSizeY().GetDataActual(); ln++)
		{
			output << WIDG->GetContent().GetDataActual().substr(min(col, WIDG->GetContent().GetDataActual().size() - 1), WIDG->GetSize().GetSizeX().GetDataActual() - max(WIDG->GetAbsolutePosition().GetX().GetDataActual() + WIDG->GetSize().GetSizeX().GetDataActual() - GetConsoleSize(BorderWidth).GetSizeX().GetDataActual(), 0));

			MoveCursorTo(trCoordinate<int>(WIDG->GetAbsolutePosition().GetX().GetDataActual(), ln), BorderWidth);

			if (!IsOutSide(trCoordinate<int>(WIDG->GetAbsolutePosition().GetX().GetDataActual(), ln), BorderWidth))
			{
				WIDG->Display(output);
			}

			output.str(L"");

			col += WIDG->GetSize().GetSizeX().GetDataActual();
		}

		WIDG->SetChange(false);
	}

	else if (RenderType == BUFFER_SYSTEM)
	{
		if (!Refreshed)
		{
			CleanWidget(WIDG);
		}

		std::wostringstream output;

		int col = 0;

		std::vector<trPair<std::wstring, trCoordinate<int>>> rstColortemp;

		for (int ln = WIDG->GetAbsolutePosition().GetY().GetDataActual(); ln < WIDG->GetAbsolutePosition().GetY().GetDataActual() + WIDG->GetSize().GetSizeY().GetDataActual(); ln++)
		{
			output << WIDG->GetContent().GetDataActual().substr(min(col, WIDG->GetContent().GetDataActual().size()), WIDG->GetSize().GetSizeX().GetDataActual() - max(WIDG->GetAbsolutePosition().GetX().GetDataActual() + WIDG->GetSize().GetSizeX().GetDataActual() - GetConsoleSize(BorderWidth).GetSizeX().GetDataActual(), 0));

			if (!IsOutSide(trCoordinate<int>(WIDG->GetAbsolutePosition().GetX().GetDataActual(), ln), BorderWidth))
			{
				MoveCursorToOstream(trCoordinate<int>(WIDG->GetAbsolutePosition().GetX().GetDataActual(), ln), Render_, *SizeWindow, BorderWidth);

				// PROTOTYPE COULEUR

				rstColortemp.push_back(trPair<wstring, trCoordinate<int>>(L"\033[0m" + *BaseColor, trCoordinate<int>(WIDG->GetAbsolutePosition().GetX().GetDataActual() + static_cast<int>(output.str().size()), ln)));

				// fin PROTOTYPE COULEUR

				*Render_ << output.str();
			}

			output.str(L"");

			col += WIDG->GetSize().GetSizeX().GetDataActual();
		}

		WIDG->SetResetColor(rstColortemp);

		WIDG->SetChange(false);
	}

	else if (RenderType == RENDER_SYSTEM)
	{
		Render_->seekp(0, std::ios::end);

		if (!Refreshed)
		{
			CleanWidget(WIDG);
		}

		std::wstring output = L"";
		output.reserve(WIDG->GetColoredContent().GetDataActual().size());

		int col = 0;

		std::vector<trPair<std::wstring, trCoordinate<int>>> rstColortemp;

		wstring tempAdd = L"";
		wstring temp = L"";
		wstring OutPutTemp = L"";

		uint16_t maxSizeX = SizeWindow_Border->GetSizeX().GetDataActual();
		uint16_t maxSizeY = SizeWindow_Border->GetSizeY().GetDataActual();

		uint16_t widgetSizeX = WIDG->GetSize().GetSizeX().GetDataActual();
		uint16_t widgetSizeY = WIDG->GetSize().GetSizeY().GetDataActual();

		uint16_t widgetPosX = WIDG->GetAbsolutePosition().GetX().GetDataActual();
		uint16_t widgetPosY = WIDG->GetAbsolutePosition().GetY().GetDataActual();

		wstring Ligne = L"";
		wstring Colone = L"";


		for (int ln = widgetPosY; ln < widgetPosY + widgetSizeY; ln++)
		{
			if (!IsOutSideFast(widgetPosX, ln, maxSizeX, maxSizeY))
			{
				FastToWString(ln + BorderWidth + 1, Ligne); 
				FastToWString(widgetPosX + 2 * BorderWidth + 1, Colone);

				output.append(L"\x1b[");
				output.append(Ligne);
				output.append(L";");
				output.append(Colone);
				output.append(L"H");

				output.append(L"\033[0m"); // Reset color + BaseColor
				output.append(*BaseColor);

				tempAdd = WIDG->GetColoredContent().GetDataActual();

				// temp = substrAnsiSafe(tempAdd, min(col, tempAdd.size()), widgetSizeX - max(widgetPosX + widgetSizeX - maxSizeX, 0)); // ça bouffe des perfs de fou
				substrAnsiSafeUltraFast(
					tempAdd,
					min(col, tempAdd.size()), // 🧮 startVisible
					widgetSizeX - max(widgetPosX + widgetSizeX - maxSizeX, 0), // 🧮 countVisible
					temp // 🧠 buffer de sortie
				);

				output.append(temp);
				output.append(L"\033[0m"); 

				OutPutTemp.append(output);
			}

			output = L"";

			col += widgetSizeX;
		}

		*Render_ << OutPutTemp;

		WIDG->SetResetColor(rstColortemp);

		WIDG->SetChange(false);
	}
}

void trRender::DisplayColor(const std::unordered_map<std::string, trActor*>& Actors_)
{
	///
	/// je pense que je peut deja optimiser par juste crée des appel pour éviter d'appeler 50 000 fois une fonction inutilement
	///

	std::map<int, wstring> Color;

	for (auto& it : Actors_)
	{
		trWidget* widgetPtr = dynamic_cast<trWidget*>(it.second);
		if (!widgetPtr) continue; // Passe au suivant si ce n'est pas un trWidget

		for (auto& it_ : widgetPtr->GetColoredMap().GetDataActual())
		{
			// La condition en dessous est juste watafak, pourquoi ça marche (je sais pas)... Mais est-ce ça marche ? (Du tonnerre, enfaite pas de fou)
			if (!IsOutSide(*it_.second->second, BorderWidth, false) && !IsOutSide(trCoordinate<int>(it_.second->second->GetY().GetDataActual() + widgetPtr->GetAbsolutePosition().GetY().GetDataActual(), it_.second->second->GetY().GetDataActual() + widgetPtr->GetAbsolutePosition().GetY().GetDataActual()), BorderWidth, false))
			{
				if (it_.second->second->GetY().GetDataActual() + widgetPtr->GetAbsolutePosition().GetY().GetDataActual() < widgetPtr->GetSize().GetSizeY().GetDataActual() + widgetPtr->GetAbsolutePosition().GetY().GetDataActual()) // verifier si la couelur n'est pas en dehors du widget en taille
				{
					MoveCursorToOstream(trCoordinate<int>(it_.second->second->GetX().GetDataActual() + widgetPtr->GetAbsolutePosition().GetX().GetDataActual(), it_.second->second->GetY().GetDataActual() + widgetPtr->GetAbsolutePosition().GetY().GetDataActual()), Render_, *SizeWindow, BorderWidth);
					Color[static_cast<int>(Render_->tellp())] = *it_.second->first;
				}
			}
		}

		for (auto& it_ : widgetPtr->GetResetColor())
		{
			if (!IsOutSide(*it_.second, BorderWidth, false))
			{
				MoveCursorToOstream(*it_.second, Render_, *SizeWindow, BorderWidth);
				Color[static_cast<int>(Render_->tellp())] = *it_.first;
			}
		}
	}

	for (auto it = Color.rbegin(); it != Color.rend(); ++it)
	{
		if (it->first <= RenderColor_->str().size())
		{
			RenderColor_->str(RenderColor_->str().insert(it->first, it->second));
		}
	}

	// reset les color a la fin de la ligne
	RenderColor_->str(RenderColor_->str().insert(0, L"\033[0m" + *BaseColor));
}

void trRender::HideWidget(trWidget* WIDG) // < ----- BUUUG
{
	/*for (int ln = min(WIDG->GetAbsolutePosition().GetY().GetDataActual(), WIDG->GetAbsolutePosition().GetY().GetDataOld()); ln < max(WIDG->GetAbsolutePosition().GetY().GetDataActual(), WIDG->GetAbsolutePosition().GetY().GetDataOld()) + max(WIDG->GetSize().GetSizeY().GetDataOld(), WIDG->GetSize().GetSizeY().GetDataActual()); ln++)
	{
		for (int i = min(WIDG->GetAbsolutePosition().GetX().GetDataActual(), WIDG->GetAbsolutePosition().GetX().GetDataOld()); i < max(WIDG->GetAbsolutePosition().GetX().GetDataActual(), WIDG->GetAbsolutePosition().GetX().GetDataOld()) + max(WIDG->GetSize().GetSizeX().GetDataOld(), WIDG->GetSize().GetSizeX().GetDataActual()); i++)
		{
			if (IsOutSide(trCoordinate<int>(i, ln), false))
			{
				continue; // <-- pas sur (chatgpt)
			}

			MoveCursorTo(trCoordinate<int>(ln, i), BorderWidth);
			cout << " ";
		}
	}*/

	CleanWidget(WIDG);

	// throw std::out_of_range("code pas fait (utiliser cleanwidget ?)"); // jsp pas pk ln'utilise out of range ;(((((((((((((((((
}

void trRender::CleanWidget(trWidget* WIDG)
{
	if (RenderType == DIRECT_SYSTEM)
	{
		for (int ln = WIDG->GetAbsolutePosition().GetY().GetDataOld(); ln <= WIDG->GetAbsolutePosition().GetY().GetDataOld() + WIDG->GetSize().GetSizeY().GetDataOld(); ln++)
		{
			if (!IsOutSide(trCoordinate<int>(WIDG->GetAbsolutePosition().GetX().GetDataOld(), ln), BorderWidth))
			{
				SetColorConsole(15);
				wstring clean(WIDG->GetSize().GetSizeX().GetDataOld() - max(WIDG->GetAbsolutePosition().GetX().GetDataOld() + WIDG->GetSize().GetSizeX().GetDataOld() - GetConsoleSize(BorderWidth).GetSizeX().GetDataActual(), 0), ' ');
				MoveCursorTo(trCoordinate<int>(WIDG->GetAbsolutePosition().GetX().GetDataOld(), ln), BorderWidth);
				cout << WstringToUtf8(clean);
			}
		}
	}

	else if (RenderType == BUFFER_SYSTEM)
	{
		for (int ln = WIDG->GetAbsolutePosition().GetY().GetDataOld(); ln <= WIDG->GetAbsolutePosition().GetY().GetDataOld() + WIDG->GetSize().GetSizeY().GetDataOld(); ln++)
		{
			if (!IsOutSide(trCoordinate<int>(WIDG->GetAbsolutePosition().GetX().GetDataOld(), ln), BorderWidth))
			{
				wstring clean(WIDG->GetSize().GetSizeX().GetDataOld() - max((WIDG->GetAbsolutePosition().GetX().GetDataOld() + WIDG->GetSize().GetSizeX().GetDataOld() - GetConsoleSize(BorderWidth).GetSizeX().GetDataActual()), 0), ' ');
				MoveCursorToOstream(trCoordinate<int>(WIDG->GetAbsolutePosition().GetX().GetDataOld(), ln), Render_, *SizeWindow, BorderWidth);
				*Render_ << clean;
			}
		}
	}

	else if (RenderType == RENDER_SYSTEM)
	{
		Render_->seekp(0, std::ios::end);

		uint16_t OldPosY = WIDG->GetAbsolutePosition().GetY().GetDataOld();
		uint16_t OldPosX = WIDG->GetAbsolutePosition().GetX().GetDataOld();
		uint16_t OldSizeY = WIDG->GetSize().GetSizeY().GetDataOld();
		uint16_t OldSizeX = WIDG->GetSize().GetSizeX().GetDataOld();

		uint16_t maxSizeX = SizeWindow_Border->GetSizeX().GetDataActual();
		uint16_t maxSizeY = SizeWindow_Border->GetSizeY().GetDataActual();

		wstring outputTemp = L"";
		outputTemp.reserve(WIDG->GetColoredContent().GetDataActual().size());

		wstring Ligne = L"";
		wstring Colone = L"";
		wstring clean = L"";
		clean.reserve(WIDG->GetColoredContent().GetDataActual().size());

		// À faire AVANT la boucle
		uint16_t maxCleanLen = OldSizeX;
		wstring blankLine(maxCleanLen, L' ');

		uint16_t cut;
		uint16_t effectiveCleanLen;

		// ⚠️ En dehors de la boucle (à faire une seule fois)
		const uint16_t MaxSpaceLen = OldSizeX;
		const std::wstring BlankLine(MaxSpaceLen, L' ');

		for (int ln = OldPosY; ln < OldPosY + OldSizeY; ln++)
		{
			if (!IsOutSideFast(OldPosX, ln, maxSizeX, maxSizeY))
			{
				cut = max((OldPosX + OldSizeX - maxSizeX), 0);
				effectiveCleanLen = max(0, OldSizeX - cut);

				// 🔥 Plus besoin de clean = substr(...)
				// 🔄 Écriture directe dans outputTemp

				FastToWString(ln + BorderWidth + 1, Ligne);
				FastToWString(OldPosX + 2 * BorderWidth + 1, Colone);

				outputTemp.append(L"\x1b[");
				outputTemp.append(Ligne);
				outputTemp.append(L";");
				outputTemp.append(Colone);
				outputTemp.append(L"H");

				// ⚡ Ajout direct des blancs, sans créer d'objet clean
				outputTemp.append(BlankLine.data(), effectiveCleanLen);
			}
		}

		*Render_ << outputTemp;
	}
}

void trRender::Clear()
{
	Render_->str(L"");   // Remplace le contenu par une chaîne vide (wstring vide)
	Render_->clear();    // Enlève les flags d'erreur (optionnel, souvent utile après str(""))

	RenderColor_->str(L""); // Remplace le contenu par une chaîne vide (wstring vide)
	RenderColor_->clear();  // Enlève les flags d'erreur (optionnel, souvent utile après str(""))
}

// RENDER

void trRender::Render(const std::unordered_map<std::string, trActor*>& Actors_)
{
	if (RenderType == DIRECT_SYSTEM)
	{
		MessageBox(
			NULL,
			L"Bah du coup non",
			L"Erreur",
			MB_ICONERROR | MB_OK
		);
		std::exit(1);
	}

	else if (RenderType == BUFFER_SYSTEM)
	{
		RenderColor_->str(Render_->str());

		DisplayColor(Actors_); // il faut optimiser

		MoveCursorTo(trCoordinate<int>(0, 0));

		string temp = WstringToUtf8(RenderColor_->str());

		std::this_thread::sleep_for(std::chrono::milliseconds(2));

		// std::print("{}", temp); // ici il faut optimiser
		// trPrint(temp); c'est plus lent... FLOP

		WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), temp.c_str(), static_cast<DWORD>(temp.size()), NULL, nullptr);
	}

	else if (RenderType == RENDER_SYSTEM)
	{
		RenderColor_->str(Render_->str());

		MoveCursorTo(trCoordinate<int>(0, 0));

		string ActualFrame = WstringToUtf8(RenderColor_->str());

		std::this_thread::sleep_for(std::chrono::milliseconds(2));

		WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), ActualFrame.c_str(), static_cast<DWORD>(ActualFrame.size()), NULL, nullptr);

		Render_->str(L""); 
	}
}

// DESTRUCTEUR

trRender::~trRender()
{
	delete SizeWindow;

	delete CursorSelector;

	delete Render_;

	delete RenderColor_;

	delete BaseColor;
}