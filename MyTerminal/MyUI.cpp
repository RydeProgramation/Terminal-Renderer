#include "MyUI.h"

REGISTER_TYPE(Munition, int, int, int, std::wstring, std::string)

using namespace std;
using namespace UITools;

// INI

MyUI::MyUI() : trUserInterface(RENDER_SYSTEM, 6, L"\033[0m")
{

}

MyUI::~MyUI()
{

}

// test CHAT GPT

void hsvToRgb(float h, float s, float v, int& r, int& g, int& b) {
	float c = v * s;
	float x = static_cast<float>(c * (1 - fabs(fmod(h / 60.0, 2) - 1)));
	float m = v - c;

	float rPrime, gPrime, bPrime;
	if (h < 60) {
		rPrime = c; gPrime = x; bPrime = 0;
	}
	else if (h < 120) {
		rPrime = x; gPrime = c; bPrime = 0;
	}
	else if (h < 180) {
		rPrime = 0; gPrime = c; bPrime = x;
	}
	else if (h < 240) {
		rPrime = 0; gPrime = x; bPrime = c;
	}
	else if (h < 300) {
		rPrime = x; gPrime = 0; bPrime = c;
	}
	else {
		rPrime = c; gPrime = 0; bPrime = x;
	}

	r = static_cast<int>((rPrime + m) * 255);
	g = static_cast<int>((gPrime + m) * 255);
	b = static_cast<int>((bPrime + m) * 255);
}

// Fonction pour appliquer un dégradé arc-en-ciel avec décalage
wstring applyColorToText(const wstring& text, float offset) {
	wstring coloredText;
	float hueStep = 360.0f / text.size(); // Espace de teinte pour chaque caractère
	float hue = offset; // Démarrage de la teinte avec un décalage

	for (wchar_t c : text) {
		int r, g, b;
		hsvToRgb(fmod(hue, 360.0f), 1.0f, 1.0f, r, g, b); // Saturation et luminosité maximales

		// Construire la séquence de couleur ANSI
		wstring colorRgb = L"\033[38;2;" + to_wstring(r) + L";" + to_wstring(g) + L";" + to_wstring(b) + L"m";

		// Construire la séquence de couleur ANSI
		wstring colorRgb2 = L"\033[48;2;" + to_wstring(b / 10) + L";" + to_wstring(r / 10) + L";" + to_wstring(g / 10) + L"m";

		// Ajouter le caractère coloré
		coloredText += colorRgb + colorRgb2 + c;

		// Passer à la teinte suivante
		hue += hueStep;
	}

	// Réinitialiser les couleurs à la fin
	coloredText += L"\033[0m";

	return coloredText;
}

// CODE

void MyUI::Debut()
{
	Menu1();

	LOAD(trWidget, Hello, L"Hello.widg")
	LOAD(trWidget, Hello2, L"HelloWorld.widg")
	LOAD(trWidget, Hello3, L"HelloWorldComplete.widg")

	vector<wstring> hello3;
	wstring hello3_;

	CreateWidgetWait(Hello);

	for (int i = 0; i < 115; i++)
	{
		Hello->AddToPosition(-1, 0);

		Sleep(10);
	}

	Sleep(1500);

	CreateWidgetWait(Hello2);

	Hello->Destroy();

	Sleep(2000); 

	CreateWidgetWait(Hello3);

	Hello2->Destroy();

	int i_ = 0;

	for (int i = 0; i <= 500; i += 5)
	{
		i += 5;
		hello3.clear();

		hello3.push_back(applyColorToText(L" _   _      _ _        __        __         _     _ _ ", float(i))); // 54
		hello3.push_back(applyColorToText(L"| | | | ___| | | ___   \\ \\      / /__  _ __| | __| | |", float(i))); // 54
		hello3.push_back(applyColorToText(L"| |_| |/ _ \\ | |/ _ \\   \\ \\ /\\ / / _ \\| '__| |/ _` | |", float(i))); // 54
		hello3.push_back(applyColorToText(L"|  _  |  __/ | | (_) |   \\ V  V / (_) | |  | | (_| |_|", float(i))); // 54
		hello3.push_back(applyColorToText(L"|_| |_|\\___|_|_|\\___/     \\_/\\_/ \\___/|_|  |_|\\__,_(_)", float(i))); // 54

		hello3_ = hello3[0] + hello3[1] + hello3[2] + hello3[3] + hello3[4];

		Hello3->SetContent(hello3_);

		Sleep(35);

		i_ = i;
	}

	for (int i = 0; i < 10; i++)
	{
		Hello3->AddToPosition(0, 1);

		hello3.clear();

		hello3.push_back(applyColorToText(L" _   _      _ _        __        __         _     _ _ ", float(i_))); // 54
		hello3.push_back(applyColorToText(L"| | | | ___| | | ___   \\ \\      / /__  _ __| | __| | |", float(i_))); // 54
		hello3.push_back(applyColorToText(L"| |_| |/ _ \\ | |/ _ \\   \\ \\ /\\ / / _ \\| '__| |/ _` | |", float(i_))); // 54
		hello3.push_back(applyColorToText(L"|  _  |  __/ | | (_) |   \\ V  V / (_) | |  | | (_| |_|", float(i_))); // 54
		hello3.push_back(applyColorToText(L"|_| |_|\\___|_|_|\\___/     \\_/\\_/ \\___/|_|  |_|\\__,_(_)", float(i_))); // 54

		hello3_ = hello3[0] + hello3[1] + hello3[2] + hello3[3] + hello3[4];

		i_ += 20;

		Hello3->SetContent(hello3_);

		Sleep(40);
	}

	for (int i = 0; i < 5; i++)
	{
		Hello3->AddToSize(0, -1);

		hello3.clear();

		hello3.push_back(applyColorToText(L" _   _      _ _        __        __         _     _ _ ", float(i_))); // 54
		hello3.push_back(applyColorToText(L"| | | | ___| | | ___   \\ \\      / /__  _ __| | __| | |", float(i_))); // 54
		hello3.push_back(applyColorToText(L"| |_| |/ _ \\ | |/ _ \\   \\ \\ /\\ / / _ \\| '__| |/ _` | |", float(i_))); // 54
		hello3.push_back(applyColorToText(L"|  _  |  __/ | | (_) |   \\ V  V / (_) | |  | | (_| |_|", float(i_))); // 54
		hello3.push_back(applyColorToText(L"|_| |_|\\___|_|_|\\___/     \\_/\\_/ \\___/|_|  |_|\\__,_(_)", float(i_))); // 54

		hello3_ = hello3[0] + hello3[1] + hello3[2] + hello3[3] + hello3[4];

		i_ += 20;

		Hello3->SetContent(hello3_);

		Sleep(40);
	}

	Hello3->Destroy();

	LOAD(trText, Paragraphe, L"parag.widg");

	CreateWidgetWait(Paragraphe);

	bool IsFinished = false;

	Paragraphe->DoCharToCharAnimation(10); // 50 de base

	Paragraphe->DoAnimation(IsFinished);

	wait(!IsFinished);

	Sleep(max(0, 1200 - Paragraphe->GetDeltaTime().GetDataActual()));

	for (int i = 0; i < 6; i++)
	{
		World->SetActor<trWidget>("Paragraphe", &trWidget::AddToSize, 0, -1);

		Sleep(250);
	}

	World->DestroyActor("Paragraphe");

	Menu1();
}

void MyUI::Menu1()
{
	LOAD(trWidget, SimulationTitle, L"Simulation.widg");
	CreateWidgetWait(SimulationTitle);

	/*vector<wstring> Diff;

	wstring Diff_ = Diff[0] + Diff[1] + Diff[2] + Diff[3] + Diff[4];*/

	LOAD(trSelector, SimCarlo, L"MonteCarlo.widg");

	CreateSelectorWait(SimCarlo);

	World->SetActor<trSelector>("SimCarlo", &trSelector::SetSelected, bool(true));

	// CreateSelectorWait(new trSelector(40, 5, static_cast<int>(Diff[0].size()), 5, MiddleCenter, Diff_, "SimCarlo2"));

	// RENDRE ÇA PLUS SIMPLE A CODER ET PLUS OPTIMISER

	double x(0), y(0), e(0), o(0);

	KB->CreateBTN(new trBTN_Key(
		KEY_W,
		OnPress,
		HoldToTrigger,
		[ptr = dynamic_cast<trWidget*>(World->GetPtrActor("Simulation")), &x]() {
			if (ptr) {
				x += ptr->GetDeltaTime().GetDataActual() * 0.2;
				if (x > 1)
				{
					ptr->AddToPosition(0, -(int(x)));
					x = 0;
				}
			}
		},
		World->GetPtrActor("Simulation")
	));

	KB->CreateBTN(new trBTN_Key(
		0x41,
		OnPress,
		HoldToTrigger,
		[ptr = dynamic_cast<trWidget*>(World->GetPtrActor("Simulation")), &y]() {
			
			y += ptr->GetDeltaTime().GetDataActual() * 0.4;
			if (y > 1)
			{
				if (ptr) ptr->AddToPosition(-(int(y)), 0);
				y = 0;
			}
		},
		World->GetPtrActor("Simulation")
	));

	KB->CreateBTN(new trBTN_Key(
		0x53,
		OnPress,
		HoldToTrigger,
		[ptr = dynamic_cast<trWidget*>(World->GetPtrActor("Simulation")), &e]() {
			
			e += ptr->GetDeltaTime().GetDataActual() * 0.2;
			if (e > 1)
			{
				if (ptr) ptr->AddToPosition(0, (int(e)));
				e = 0;
			}
		},
		World->GetPtrActor("Simulation")
	));

	KB->CreateBTN(new trBTN_Key(
		0x44,
		OnPress,
		HoldToTrigger,
		[ptr = dynamic_cast<trWidget*>(World->GetPtrActor("Simulation")), &o]() {
			
			o += ptr->GetDeltaTime().GetDataActual() * 0.4;
			if (o > 1)
			{
				if (ptr) ptr->AddToPosition((int(o)), 0);
				o = 0;
			}
		},
		World->GetPtrActor("Simulation")
	));

	// ancienne version que je veux retravailler pour l'optimiser
	// KB->CreateBTN(trBTN_Key(0x41, OnPress, HoldToTrigger, bind(&trWidget::AddToPosition, dynamic_cast<trWidget*>GetPtrActor("Simulation"), -1, 0), GetPtrActor("Simulation")));
	/*KB->CreateBTN(trBTN_Key(0x53, OnPress, HoldToTrigger, bind(&trWidget::AddToPosition, GetPtrActor("Simulation"), 0, 1), GetPtrActor("Simulation")));
	KB->CreateBTN(trBTN_Key(0x44, OnPress, HoldToTrigger, bind(&trWidget::AddToPosition, GetPtrActor("Simulation"), 1, 0), GetPtrActor("Simulation")));
	KB->CreateBTN(trBTN_Key(KEY_J, OnPress, HoldToTrigger, bind(&trUserInterface::CreateActor, GetPtrActor("Simulation"), new Munition(0, 0, TopLeft, L"*", "Bullet_")), GetPtrActor("Simulation")));
	*/

	trWidget* Ptr = dynamic_cast<trWidget*>(World->GetPtrActor("Simulation"));

	KB->CreateBTN(new trBTN_Key(
		KEY_J,            // La touche J
		OnPress,          // Appui
		HoldToTrigger,    // Mode d'utilisation
		[Ptr, this]() {        // Lambda sans argument, qui capture this
			World->CreateActor(trCREATE(Munition, int(Ptr->GetPosition().GetX().GetDataActual()), int(Ptr->GetPosition().GetY().GetDataActual()), int(MiddleCenter), wstring(L"**"), string("Bullet_")));
		},
		Ptr  // Pointeur vers widget parentda
	));

	trObject* test = trCREATE(Munition, 1, 1, 1, wstring(L"TEST ahahahahaha"), string("TEST"));

	Munition* test2 = dynamic_cast<Munition*>(test);

	string testname = test2->GetName().GetDataActual();

	const trWidget* widget = dynamic_cast<const trWidget*>(&World->GetActor("Simulation"));

	while (true)
	{
		/*a += 15;
		World->SetActor("Simulation", &trWidget::SetContent, applyColorToText(widget->GetContent().GetDataActual(), float(a)));
		Sleep(1);*/

		Sleep(10000);
	}
}

void MyUI::CreateWidgetWait(trWidget* WIDG)
{
	World->CreateActor(WIDG);

	while (!WIDG->IsCreated()) {} // faire attention ici
}

void MyUI::CreateSelectorWait(trSelector* WIDG)
{
	World->CreateActor(WIDG);

	while (!WIDG->IsCreated()) {} // faire attention ici
}

int Munition::bulletCount;
