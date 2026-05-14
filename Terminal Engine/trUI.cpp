#include "trUI.h"

// UI

using namespace std;
using namespace UITools;
using namespace UIToolsCore;

// fnc

trUserInterface* CreateUserInterface()
{
	// return new trUserInterface(0);
	std::cerr << "NE PAS UTILISER";
	MessageBox(
		NULL,                           // Pas de fenêtre parente
		L"Aucun RenderType ne correspond",  // Message
		L"Erreur",                      // Titre de la boîte
		MB_ICONERROR | MB_OK           // Icône d'erreur + bouton OK
	);
	std::exit(1);
}

// INI default

trUserInterface::trUserInterface() : trUserInterface(BUFFER_SYSTEM, 6, L"\033[0m")
{

}

// INI

trUserInterface::trUserInterface(uint8_t RenderType_, int BordW_, wstring RstClr) : Mutex(new std::mutex), KB(new trKeyBoardManagement()), ThrUI(new std::thread()), ThrKB(new std::thread()), Render(new trRender(RenderType_, BordW_, RstClr)), World(new trWorld()), Time(new trData<TimePoint>()), DeltaTime(new trData<double>())
{
	BorderWidth = BordW_;
}

// INI deep copy

trUserInterface::trUserInterface(const trUserInterface& other) : Mutex(new std::mutex), KB(new trKeyBoardManagement()), ThrUI(new std::thread()), ThrKB(new std::thread()), Render(new trRender(*other.Render)), World(new trWorld(*other.World)), Time(new trData<TimePoint>(*other.Time)), DeltaTime(new trData<double>(*other.DeltaTime))
{

}

// Copy

trUserInterface& trUserInterface::operator=(const trUserInterface& other)
{
	if (this == &other) { return *this; }

	if (Mutex == nullptr) {
		Mutex = new std::mutex();
	} else {
		// Mutex ne peut pas être copié, donc on le laisse tel quel
	}

	if (KB == nullptr) {
		KB = new trKeyBoardManagement();
	} else {
		*KB = *other.KB;
	}

	if (ThrUI == nullptr) {
		ThrUI = new std::thread();
	} else {
		// Le thread ne peut pas être copié, donc on le laisse tel quel
	}

	if (ThrKB == nullptr) {
		ThrKB = new std::thread();
	} else {
		// Le thread ne peut pas être copié, donc on le laisse tel quel
	}

	if (Render == nullptr) {
		Render = new trRender();
	} else {
		*Render = *other.Render;
	}

	if (World == nullptr) {
		World = new trWorld();
	} else {
		*World = *other.World;
	}

	if (Time == nullptr) {
		Time = new trData<TimePoint>();
	}
	else {
		*Time = *other.Time;
	}

	if (DeltaTime == nullptr) {
		DeltaTime = new trData<double>();
	}
	else {
		*DeltaTime = *other.DeltaTime;
	}

	return *this;
}

// FNC

void trUserInterface::Start(int argc, char* argv[])
{
	SetConsoleOutputCP(CP_UTF8);

	SetupConsole();

	std::wstring file = stringToWstring(getArgFileWithExt(argc, argv, ".widg"));
	std::wstring page_ = stringToWstring(getArgFileWithExt(argc, argv, ".page"));

	if (!file.empty()) {
		std::cout << "Ouverture du fichier : ";

		std::wcout << file << std::endl;

		LPCTSTR file_c = file.c_str();

		LOAD(trActor, Widg, file_c);

		trPawn* widgL = dynamic_cast<trPawn*>(Widg);

		if (widgL)
		{
			widgL->SetPosition(0, 0);
			widgL->SetTypeRelativePosition(MiddleCenter);
		}

		// ThrUI = new std::thread([this, Widg]() { this->Load(Widg); });
		ThrKB = new std::thread([this]() { this->KB->Start(); });

		bool Lock = false;

		KB->CreateBTN(new trBTN_Key(KEY_F5, OnRelease, PressToTrigger,
			[Widg, file_c, &Lock]() { LOAD(trWidget, T_Widg, file_c); T_Widg->SetPosition(0, 0);  *Widg = *T_Widg; delete T_Widg; Lock = false; }, this));

		trText* T_Text = dynamic_cast<trText*>(Widg);
		trSelector* T_Selector = dynamic_cast<trSelector*>(Widg);

		/// DANS LE FUTUR ///
		/*
		Ce serait cool d'éviter de faire des lock et plustot enregistrer l'état des modifications et reload à chaque foix
		*/
		/// DANS LE FUTUR ///

		if (T_Text)
		{
			KB->CreateBTN(new trBTN_Key(KEY_F6, OnRelease, PressToTrigger,
				[T_Text, &Lock]() { Lock = true; T_Text->DoCharToCharAnimation(20); T_Text->ResetIndexAnimation(); }, this)); 

			KB->CreateBTN(new trBTN_Key(KEY_F8, OnRelease, PressToTrigger,
				[T_Text, &Lock]() { Lock = true; T_Text->DoAnimation(); }, this)); 

			KB->CreateBTN(new trBTN_Key(KEY_F10, OnRelease, PressToTrigger,
				[T_Text, &Lock]() { Lock = true; T_Text->DoNextFrameAnimation(); }, this));

			KB->CreateBTN(new trBTN_Key(KEY_F9, OnRelease, PressToTrigger,
				[T_Text, &Lock]() { Lock = true; T_Text->DoPreviousFrameAnimation(); }, this));
		}

		if (T_Selector)
		{
			KB->CreateBTN(new trBTN_Key(KEY_F9, OnRelease, PressToTrigger,
				[T_Selector, &Lock]() { Lock = true; T_Selector->SetSelected(true); }, this));

			KB->CreateBTN(new trBTN_Key(KEY_F10, OnRelease, PressToTrigger,
				[T_Selector, &Lock]() { Lock = true; T_Selector->SetSelected(false); }, this));
		}

		World->CreateActor(Widg);

		Load(Widg, file_c, Lock);
	}

	else if (!page_.empty()) {
		// std::cout << "Ouverture du fichier : " << file << std::endl;
		// → Chargement automatique du fichier
	}

	else {
		cout << WstringToUtf8(L"Clique sur F11, pour une meilleur experience (sans bug) 😊🎉") << endl << endl << endl;

		cout << WstringToUtf8(L"Ensuite clique sur n'importe quelle touche (entrée)") << endl;

		cin.ignore(); // A voir pour enlever ou pas

		KB->CreateBTN(new trBTN_Key(VK_LEFT, OnRelease, PressToTrigger, bind(&trUserInterface::SelectPrevious, this), this));
		KB->CreateBTN(new trBTN_Key(VK_RIGHT, OnRelease, PressToTrigger, bind(&trUserInterface::SelectNext, this), this));

		ThrUI = new std::thread([this]() { this->Loop(); });
		ThrKB = new std::thread([this]() { this->KB->Start(); });

		Time->SetData(Clock::now());
	}
}

void trUserInterface::Load(trObject* LoadObj, LPCTSTR file_c, bool &Lock)
{
	trWidget* LoadObjWidg = dynamic_cast<trWidget*>(LoadObj);

	int i = 1;

	while (true)
	{
		LoadObjWidg->SetPosition(0, 0);

		std::lock_guard<std::mutex> lock(*Mutex); // <<-- A voir pourquoi quand on l'enlève ça crée des prblms

		Render->UpdateSizeWindow();

		KB->Update();

		if (RefreshVerification())
		{
			Refresh();
		}

		World->UpdateActors(Render->GetSizeWindowBorder(), DeltaTime->GetDataActual());

		const std::unordered_map<std::string, trActor*>& Actors = World->GetActors();

		if (LoadObjWidg)
		{
			Render->DisplayWidget(LoadObjWidg);
		}

		if (Render->GetRenderType() == BUFFER_SYSTEM || Render->GetRenderType() == RENDER_SYSTEM)
		{
			Render->Render(Actors);
		}

		if (i >= 500 && !Lock)
		{
			RELOAD(trActor, LoadObjWidg, file_c);

			Refresh();

			i = 0;
		}

		Refreshed = false;

		UpdateTime();

		i += static_cast<float>(DeltaTime->GetDataActual());
	}
}

// GET

const trData<double>& trUserInterface::GetDeltaTime()
{
	return *DeltaTime;
}

// UPDATE

void trUserInterface::UpdateTime()
{
	Time->SetData(Clock::now());
	Time->Update();
	Nanoseconds delta = std::chrono::duration_cast<Nanoseconds>(Time->GetDataOld() - Time->GetDataActual());

	long long deltaTime_ns = delta.count(); // delta time en nanosecondes

	DeltaTime->SetData(-(static_cast<double>(deltaTime_ns) / (double)1'000'000.0));
	DeltaTime->Update();

	//	cout << "Delta Time: " << DeltaTime->GetDataActual() << " ms" << endl;
}

void trUserInterface::Update()
{
	std::lock_guard<std::mutex> lock(*Mutex); // <<-- A voir pourquoi quand on l'enlève ça crée des prblms

	Render->UpdateSizeWindow();

	if (RefreshVerification()) 
	{
		Refresh();
	}

	KB->Update();

	World->Update();
	
	World->UpdateActors(Render->GetSizeWindowBorder(), DeltaTime->GetDataActual());

	const std::unordered_map<std::string, trActor*> &Actors = World->GetActors();

	for (auto& widg : Actors)
	{
		trWidget* widgetPtr = dynamic_cast<trWidget*>(widg.second);

		if (!widgetPtr) continue; // Passe au suivant si ce n'est pas un trWidget

		if (!widg.second->GetActivate().GetDataActual())
		{
			Render->HideWidget(widgetPtr);
		}

		else if (widg.second->GetActivate().GetDataActual() && widg.second->GetChange().GetDataActual())
		{
			Render->DisplayWidget(widgetPtr);
		}
	}

	if (Render->GetRenderType() == BUFFER_SYSTEM || Render->GetRenderType() == RENDER_SYSTEM)
	{
		Render->Render(Actors);
	}

	World->Destroy();

	Refreshed = false;

	UpdateTime();
}

void trUserInterface::Refresh()
{
	Render->Clear();

	Refreshed = true;
	ForceRefresh = false;

	Render->Border();

	for (auto& widg : World->GetActors())
	{
		widg.second->SetChange(true);
	}

	hideCursor();
}

bool trUserInterface::RefreshVerification()
{
	return (
		Render->GetSizeWindow().GetSizeX().GetDataActual() != Render->GetSizeWindow().GetSizeX().GetDataOld() || 
		Render->GetSizeWindow().GetSizeY().GetDataActual() != Render->GetSizeWindow().GetSizeY().GetDataOld() || 
		ForceRefresh
		) ? true : false;
}

void trUserInterface::SetupConsole()
{
	// Récupérer le handle du processus actuel
	HANDLE hProcess = GetCurrentProcess();

	// Mettre la priorité à HIGH PRIORITY CLASS
	if (SetPriorityClass(hProcess, HIGH_PRIORITY_CLASS)) {
		std::cout << "Priorité élevée appliquée avec succès ! 🚀\n";
	}
	else {
		std::cerr << "Erreur lors du changement de priorité : " << GetLastError() << "\n";
	}

	SetConsoleOutputCP(CP_UTF8);

	SetConsoleCP(CP_UTF8);

	Render->SetCursorSelector(0);

	timeBeginPeriod(2);  // Demande timer à résolution 2 ms

	hideCursor();

	Render->UpdateSizeWindow();
}

/// SELECTION ///

void trUserInterface::Select(const string& name)
{
	for (auto& widg : World->GetActors())
	{
		trSelector* sltr = dynamic_cast<trSelector*>(widg.second);
		if (sltr)
		{
			sltr->SetSelected(sltr->GetName().GetDataActual() == name);
		}

		else
		{
			MessageBox(
				NULL,
				L"Ce n'est pas un selector",
				L"Message",
				MB_ICONERROR | MB_OK
			);
		}
	}
}

void trUserInterface::SelectNext()
{
	// Récupérer la liste des selectors uniquement
	std::vector<trSelector*> selectors;
	for (auto& widg : World->GetActors())
	{
		if (trSelector* sltr = dynamic_cast<trSelector*>(widg.second))
		{
			selectors.push_back(sltr);
		}
	}

	if (selectors.empty())
		return; // Rien à faire s'il n'y a pas de selectors

	// Trouver l'index du selector sélectionné actuellement
	int currentIndex = -1;
	for (size_t i = 0; i < selectors.size(); ++i)
	{
		if (selectors[i]->IsSelected().GetDataActual())
		{
			currentIndex = static_cast<int>(i);
			break;
		}
	}

	// Désélectionner l'actuel si trouvé
	if (currentIndex != -1)
	{
		selectors[currentIndex]->SetSelected(false);
		selectors[currentIndex]->SetChange(true);
	}

	// Calculer l'index du suivant (boucle)
	int nextIndex = (currentIndex + 1) % selectors.size();

	// Sélectionner le suivant
	selectors[nextIndex]->SetSelected(true);
}

void trUserInterface::SelectPrevious()
{
	// Récupérer la liste des selectors uniquement
	std::vector<trSelector*> selectors;
	for (auto& widg : World->GetActors())
	{
		if (trSelector* sltr = dynamic_cast<trSelector*>(widg.second))
		{
			selectors.push_back(sltr);
		}
	}

	if (selectors.empty())
		return; // Rien à faire s'il n'y a pas de selectors

	// Trouver l'index du selector sélectionné actuellement
	int currentIndex = -1;
	for (size_t i = 0; i < selectors.size(); ++i)
	{
		if (selectors[i]->IsSelected().GetDataActual())
		{
			currentIndex = static_cast<int>(i);
			break;
		}
	}

	// Désélectionner l'actuel si trouvé
	if (currentIndex != -1)
	{
		selectors[currentIndex]->SetSelected(false);
		selectors[currentIndex]->SetChange(true);
	}

	// Calculer l'index du précédent (boucle en arrière)
	int prevIndex;
	if (currentIndex == -1) // Si aucun sélectionné, prendre le dernier
	{
		prevIndex = static_cast<int>(selectors.size()) - 1;
	}
	else
	{
		prevIndex = (currentIndex - 1 + static_cast<int>(selectors.size())) % selectors.size();
	}

	// Sélectionner le précédent
	selectors[prevIndex]->SetSelected(true);
}


// private FNC

void trUserInterface::Loop()
{
	while (true)
	{
		Update();
	}
}

// DESTRUCTEUR

trUserInterface::~trUserInterface()
{
	delete KB;

	delete Mutex;

	delete ThrUI;

	delete ThrKB;

	delete Render;

	delete World;

	delete Time;

	delete DeltaTime;

	timeEndPeriod(1);    // Libère la résolution demandée
}
