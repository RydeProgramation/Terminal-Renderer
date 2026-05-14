#include "trKeyBoardManagement.h"

using namespace std;
using namespace UIToolsCore;

// INI
trKeyBoardManagement::trKeyBoardManagement() : Start_(false), BTNS(new std::unordered_map<int, trBTN_Key*>()), ActiveKeysBufferWrite(new std::vector<trPair<int, bool>>()), ActiveKeysBufferRead(new std::vector<trPair<int, bool>>()), MutexKB(new std::mutex())
{
	
}

// INI deep copy
trKeyBoardManagement::trKeyBoardManagement(const trKeyBoardManagement& other) : Start_(other.Start_), BTNS(new std::unordered_map<int, trBTN_Key*>(*other.BTNS)), ActiveKeysBufferWrite(new std::vector<trPair<int, bool>>(*other.ActiveKeysBufferWrite)), ActiveKeysBufferRead(new std::vector<trPair<int, bool>>(*other.ActiveKeysBufferRead)), MutexKB(new std::mutex())
{

}

// copy
trKeyBoardManagement& trKeyBoardManagement::operator=(const trKeyBoardManagement& other)
{
	if (this == &other) { return *this; }
	
	if (BTNS == nullptr) {
		BTNS = new std::unordered_map<int, trBTN_Key*>(*other.BTNS);
	}
	else {
		*BTNS = *other.BTNS;
	}

	if (ActiveKeysBufferWrite == nullptr) {
		ActiveKeysBufferWrite = new std::vector<trPair<int, bool>>(*other.ActiveKeysBufferWrite);
	}
	else {
		*ActiveKeysBufferWrite = *other.ActiveKeysBufferWrite;
	}

	if (ActiveKeysBufferRead == nullptr) {
		ActiveKeysBufferRead = new std::vector<trPair<int, bool>>(*other.ActiveKeysBufferRead);
	}
	else {
		*ActiveKeysBufferRead = *other.ActiveKeysBufferRead;
	}

	if (MutexKB == nullptr) {
		MutexKB = new std::mutex();
	}
	else {
		// *MutexKB = *other.MutexKB; on ne devrait pas faire ça normalement
	}

	Start_ = other.Start_;

	return *this;
}

// FNC
bool trKeyBoardManagement::CreateBTN(trBTN_Key* Btn)
{
	// FAUDERAIT MUTEX ?

	(*BTNS)[Btn->GetKey()] = Btn;

	return true;
}

// SET
void trKeyBoardManagement::SetActionBtnKey(int Key, void (*Action)()) 
{
	(*BTNS)[Key]->SetAction(Action);
}

// GET
const std::unordered_map<int, trBTN_Key*>& trKeyBoardManagement::GetBTNS() const
{
	return *BTNS;
}

const int trKeyBoardManagement::GetLastKeyState(const std::vector<trPair<int, bool>>& actkeys, int Key)
{
	for (auto it = actkeys.rbegin(); it != actkeys.rend(); ++it)
	{
		if (*it->first == Key)
		{
			return *it->second;
		}
	}

	return -1; // Non trouvé, donc pas de dernière action pour cette touche
}

// UPDATE
void trKeyBoardManagement::Update() 
{
	{ // Pour éviter que le mutex reste verrouillé trop longtemps, on utilise un bloc pour le verrouiller temporairement
		lock_guard<std::mutex> guard(*MutexKB); 
		std::swap(ActiveKeysBufferWrite, ActiveKeysBufferRead); 
	}

	// On vide le buffer
	for (auto& it : *ActiveKeysBufferRead)
	{
		(*BTNS)[*it.first]->Update(*it.second);
	}

	// Traiter les boutons
	for (auto& it : *BTNS)
	{
		if (it.second->GetActionState())
		{
			it.second->DoAction();
		}
	}

	ActiveKeysBufferRead->clear(); 
}

// START
void trKeyBoardManagement::Start() 
{
	if (!Start_) 
	{
		Start_ = true;
		Loop(); // ici la loop est lancée, JUSTE pour garder en mémoire les touches du clavier
	}
}

// LOOP
void trKeyBoardManagement::Loop() 
{
	std::unordered_map<int, bool> LocalKeyState;

	bool kState = false;

	std::vector<trPair<int, bool>> LocalBuffer;

	int key = 0;
	int lastState = 0;
	bool prevState = 0;   

	while (true) 
	{
		LocalBuffer.clear();

		for (auto it = BTNS->begin(); it != BTNS->end(); )
		{
			key = it->first;

			if (!it->second->GetPtr()->IsValid())
			{
				it = BTNS->erase(it);  // erase renvoie l'itérateur suivant, donc pas besoin de ++it ici
			}

			kState = (GetAsyncKeyState(key) & 0x8000);

			lastState = GetLastKeyState(LocalBuffer, key);
			prevState = LocalKeyState[key];

			if (kState && lastState == -1 && !prevState /*&& IsMyWindowInFocus()*/)
			{
				LocalBuffer.push_back(trPair<int, bool>(key, true));
			}
			else if (!kState && lastState == -1 && prevState /*&& IsMyWindowInFocus()*/)
			{
				LocalBuffer.push_back(trPair<int, bool>(key, false));
			}
			else if (kState && lastState == 0 /*&& IsMyWindowInFocus()*/)
			{
				LocalBuffer.push_back(trPair<int, bool>(key, true));
			}
			else if (!kState && lastState == 1 /*&& IsMyWindowInFocus()*/)
			{
				LocalBuffer.push_back(trPair<int, bool>(key, false));
			}

			LocalKeyState[key] = kState;

			it++;
		}

		if (!LocalBuffer.empty()) 
		{
			lock_guard<std::mutex> guard(*MutexKB); // On verrouille le mutex pour éviter les conflits d'accès
			ActiveKeysBufferWrite->insert(ActiveKeysBufferWrite->end(), LocalBuffer.begin(), LocalBuffer.end()); // On ajoute le vecteur local au buffer principal
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(5)); // test pour le cpu
	}
}

// DESTRUCTEUR
trKeyBoardManagement::~trKeyBoardManagement()
{
	delete BTNS;
}