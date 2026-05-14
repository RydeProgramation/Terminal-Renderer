#pragma once

#include "include.h"

#include "trSize.h"
#include "trKeyBoardManagement.h"
#include "trCoordinate.h"
#include "trWidget.h"
#include "trData.h"
#include "trPair.h"
#include "trSelector.h"
#include "trUITools.h"
#include "trRender.h"
#include "trWorld.h"
#include "trActor.h"
#include "trText.h"
#include "trLoadFile.h"
#include "trSaveFile.h"

#ifndef __TR_UI__
#define __TR_UI__

#define wait(x) while (x) {Sleep(100);}

#ifdef TERMINAL_ENGINE_EXPORT
#define TERMINAL_ENGINE_API __declspec(dllexport)
#else
#define TERMINAL_ENGINE_API __declspec(dllimport)
#endif

class TERMINAL_ENGINE_API trUserInterface : public trObject 
{
public: 

	// INI default

	trUserInterface();
	
	// INI

	trUserInterface(uint8_t RenderType_, int BordW_, std::wstring RstClr);

	// INI deep copy

	trUserInterface(const trUserInterface& other);

	// Copy

	trUserInterface& operator=(const trUserInterface& other);
	
	// FNC

	/// <summary>
	/// Lance l'interface utilisateur
	/// </summary>
	void Start(int argc, char* argv[]);

	void Load(trObject* LoadObj, LPCTSTR file_c, bool &Lock);

	// GET

	const trData<double>& GetDeltaTime();

	/// <summary>
	/// 
	/// </summary>
	void UpdateTime();

	/// <summary>
	/// Update (cote...);
	/// </summary>
	/// <param Name_="BorderWidth"> BorderWidth des * </param>
	virtual void Update();

	/// <summary>
	/// Raffraichie l'interface utilisateur
	/// </summary>
	virtual void Refresh();

	/// <summary>
	/// Vérifie si l'interface utilisateur doit être rafraichie 
	/// </summary>
	/// <returns> true si l'interface utilisateur doit être rafraichie, false sinon </returns>
	bool RefreshVerification();

	/// <summary>
	/// SetupConsole
	/// </summary>
	virtual void SetupConsole();

	/// SELECTION /// 

	/// <summary>
	/// Sélectionne un widget par son nom
	/// </summary>
	/// <param Name_="Name_"> Le nom du widget à sélectionner </param>
	void Select(const std::string& Name_); // a changer la méthode

	/// <summary>
	/// Séléctionne le widget suivant
	/// </summary>
	void SelectNext();

	/// <summary>
	/// Séléctionne le widget précédent
	/// </summary>
	void SelectPrevious();

private:

	/// <summary>
	/// Boucle principale de l'interface utilisateur
	/// </summary>
	void Loop();


public:

	// DESTRUCTEUR

	virtual ~trUserInterface();

protected:

	trKeyBoardManagement *KB;

	std::mutex* Mutex; // Mutex pour les threads et a voir pour mieux les utiliser de façon plus propore et moins a la râche

	trWorld* World;

private:

	trData<TimePoint> *Time;

	trData<double> *DeltaTime;

	std::thread *ThrUI;

	std::thread *ThrKB;

	trRender* Render;
};

TERMINAL_ENGINE_API trUserInterface* CreateUserInterface();

#endif