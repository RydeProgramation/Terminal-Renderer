#include "trSelector.h"
#include "trUIToolsCore.h"

REGISTER_TYPE(trSelector, int, int, int, int, uint8_t, std::wstring, std::string)

using namespace std;
using namespace UIToolsCore;

// INI default

trSelector::trSelector() : trWidget(), Selected(new trData<bool>(false)), ColorSelected(new trData<wstring>(L"\033[48;2;255;255;255m\033[38;2;0;0;0m")), ColorUnSelected(new trData<wstring>(L"\x1b[0m"))
{

}

// INI.

trSelector::trSelector(int x_, int y_, int size_x_, int size_y_, uint8_t RelativePosition_, wstring content_, string name_) : trWidget(x_, y_, size_x_, size_y_, RelativePosition_, content_, name_), Selected(new trData<bool>(false)), ColorSelected(new trData<wstring>(L"\033[38;2;255;255;255m\033[48;2;50;100;255m")), ColorUnSelected(new trData<wstring>(L"\x1b[0m"))
{
	
}

void trSelector::Init()
{
	if (Selected->GetDataActual())
	{
		SetColor(ColorSelected->GetDataActual());
	}
	else
	{
		SetColor(ColorUnSelected->GetDataActual());
	}
}

// INI deep copy

trSelector::trSelector(const trSelector& other) : trWidget(other), Selected(new trData<bool>(*other.Selected)), ColorSelected(new trData<wstring>(*other.ColorSelected)), ColorUnSelected(new trData<wstring>(*other.ColorUnSelected))
{

}

// Copy

trSelector& trSelector::operator=(const trActor& other_)
{
	return Clone(other_);
}

trSelector& trSelector::Clone(const trActor& other_)
{
	try
	{
		const trSelector& other = dynamic_cast<const trSelector&>(other_);

		// Si on est en train de se copier soi-même, il n'y a rien à faire
		if (this == &other) { return *this; }

		trWidget::Clone(other_);

		if (Selected == nullptr) {
			Selected = new trData<bool>(*other.Selected);
		}
		else {
			*Selected = *other.Selected;
		}

		if (ColorSelected == nullptr) {
			ColorSelected = new trData<wstring>(*other.ColorSelected);
		}
		else {
			*ColorSelected = *other.ColorSelected;
		}

		if (ColorUnSelected == nullptr) {
			ColorUnSelected = new trData<wstring>(*other.ColorUnSelected);
		}
		else {
			*ColorUnSelected = *other.ColorUnSelected;
		}

		return *this;
	}

	catch (const std::bad_cast&)
	{
		trWidget& Me = dynamic_cast<trWidget&>(*this);

		Me.trWidget::Clone(other_);

		return *this;
	}
}

// SET

void trSelector::SetSelected(bool Slct)
{
	Selected->SetData(Slct);
}

void trSelector::SetColorSelected(uint8_t R, uint8_t G, uint8_t B, bool Backround)
{
	wstring BackRoundOrNot = Backround ? to_wstring(48) : to_wstring(38);

	ColorSelected->SetData(L"\x1b[" + BackRoundOrNot + L";" + L"2" + L";" + to_wstring(R) + L";" + to_wstring(G) + L";" + to_wstring(B) + L"m");
}

void trSelector::SetColorSelected(const std::wstring& CodeCouleurAnsi)
{
	// Vérifie que ça commence bien par \033[
	if (CodeCouleurAnsi.size() >= 3 && CodeCouleurAnsi[0] == L'\033' && CodeCouleurAnsi[1] == L'[') {
		// Et que ça finit bien par 'm'
		if (CodeCouleurAnsi.back() == L'm') {
			ColorSelected->SetData(CodeCouleurAnsi); // ✅ Code valide
		}
		else {
			MessageBoxW(nullptr, (L"Le code ANSI ne se termine pas par 'm':\n" + CodeCouleurAnsi).c_str(),
				L"❌ Code ANSI invalide", MB_ICONERROR | MB_OK);
		}
	}
	else {
		MessageBoxW(nullptr, (L"Le code ANSI est invalide (doit commencer par '\\033['):\n" + CodeCouleurAnsi).c_str(),
			L"❌ Code ANSI invalide", MB_ICONERROR | MB_OK);
	}
}

void trSelector::SetColorUnSelected(uint8_t R, uint8_t G, uint8_t B, bool Backround)
{
	wstring BackRoundOrNot = Backround ? to_wstring(48) : to_wstring(38);

	ColorUnSelected->SetData(L"\x1b[" + BackRoundOrNot + L";" + L"2" + L";" + to_wstring(R) + L";" + to_wstring(G) + L";" + to_wstring(B) + L"m");
}

void trSelector::SetColorUnSelected(const std::wstring& CodeCouleurAnsi)
{
	// Vérifie que ça commence bien par \033[
	if (CodeCouleurAnsi.size() >= 3 && CodeCouleurAnsi[0] == L'\033' && CodeCouleurAnsi[1] == L'[') {
		// Et que ça finit bien par 'm'
		if (CodeCouleurAnsi.back() == L'm') {
			ColorUnSelected->SetData(CodeCouleurAnsi); // ✅ Code valide
		}
		else {
			MessageBoxW(nullptr, (L"Le code ANSI ne se termine pas par 'm':\n" + CodeCouleurAnsi).c_str(),
				L"❌ Code ANSI invalide", MB_ICONERROR | MB_OK);
		}
	}
	else {
		MessageBoxW(nullptr, (L"Le code ANSI est invalide (doit commencer par '\\033['):\n" + CodeCouleurAnsi).c_str(),
			L"❌ Code ANSI invalide", MB_ICONERROR | MB_OK);
	}
}

void trSelector::SetProprety(const std::string& name, const std::string& data, const std::string& type)
{
	trWidget::SetProprety(name, data, type);

	if (name == "Selected")
	{
		if (type == "bool")
		{
			if (data == "true" || data == "1" || data == "True" || data == "TRUE")
			{
				SetSelected(true);
			}
			else if (data == "false" || data == "0" || data == "False" || data == "FALSE")
			{
				SetSelected(false);
			}
			else
			{
				MessageBox(
					NULL,
					L"Invalid value for Selected property",
					L"Error",
					MB_ICONERROR | MB_OK
				);
			}
		}
		else
		{
			MessageBox(
				NULL,
				L"Invalid type for Selected property",
				L"Error",
				MB_ICONERROR | MB_OK
			);
		}
	}

	else if (name == "ColorSelected")
	{
		SetColorSelected(wstring(data.begin(), data.end()));
	}

	else if (name == "ColorUnSelected")
	{
		SetColorUnSelected(wstring(data.begin(), data.end()));
	}
}

// GET

const trData<bool> trSelector::IsSelected() const
{
	return *Selected;
}

const trData<wstring> trSelector::GetColorSelected() const
{
	return *ColorSelected;
}

// APPLY

bool trSelector::VerificationProprety()
{
	return (
		trWidget::VerificationProprety() ||
		Selected->GetDataOld() != Selected->GetDataActual() ||
		ColorUnSelected->GetDataOld() != ColorUnSelected->GetDataActual() ||
		ColorSelected->GetDataOld() != ColorSelected->GetDataActual() 
		);
}

void trSelector::APPLY_(const trSize<uint16_t>& SizeWindow)
{
	trWidget::APPLY_(SizeWindow);
	Selected->Update();
	ColorSelected->Update();

	bool selectedNow = Selected->GetDataActual();
	bool selectedBefore = Selected->GetDataOld();
	bool colorChanged = ColorSelected->GetDataOld() != ColorSelected->GetDataActual();
	bool colorChanged2 = ColorUnSelected->GetDataOld() != ColorUnSelected->GetDataActual();
	
	if (selectedNow && (colorChanged || !selectedBefore))
	{
		SetColor(ColorSelected->GetDataActual()); // a voir si c ést dans l'ordre
	}

	else if (!selectedNow && (colorChanged2 || selectedBefore))
	{
		SetColor(ColorUnSelected->GetDataActual());
	}
}

void trSelector::Display(wostringstream& output_line) // a ne jamais utiliser mdrr
{
	if (IsSelected().GetDataActual())
	{
		//SetColorConsole(GetColorSelected().GetDataActual());
		cout << WstringToUtf8(output_line.str());
	}

	if (!IsSelected().GetDataActual())
	{
		//SetColorConsole(trWidget::GetColor().GetDataActual());
		cout << WstringToUtf8(output_line.str());
	}
}

trSelector::~trSelector()
{
	delete Selected; 
	delete ColorSelected;
}
