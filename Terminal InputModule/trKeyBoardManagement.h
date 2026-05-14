#pragma once

#include "include.h"
#include "trBTN_Key.h"
#include "trSize.h"
#include "trCoordinate.h"
#include "trPair.h"
#include "trUIToolsCore.h"

#ifndef __TR_KEYBOARD_M__
#define __TR_KEYBOARD_M__

#ifdef TERMINAL_INPUT_MODULE_EXPORTS
#define TERMINAL_INPUT_API __declspec(dllexport)
#else
#define TERMINAL_INPUT_API __declspec(dllimport)
#endif

class TERMINAL_INPUT_API trKeyBoardManagement : trObject
{
public:

	// INI && default
	trKeyBoardManagement();

	// INI deep copy
	trKeyBoardManagement(const trKeyBoardManagement& other);

	// copy
	trKeyBoardManagement& operator=(const trKeyBoardManagement& other);

	// Create

	/// <summary>
	/// Cette fonction permet de créer un bouton avec les touches associées
	/// </summary>
	bool CreateBTN(trBTN_Key* Btn);

	// SET

	/// <summary>
	/// Cette fonction permet de définir un bouton associé à une touche
	/// </summary>
	/// <param name="Key"> La touche à associer au bouton </param>
	/// <param name="Action"> La fonction à appeler lorsque la touche est appuyée </param>
	void SetActionBtnKey(int Key, void (*Action)());

	// GET
	/// <summary>
	/// Cette fonction permet de récupérer le bouton associé à une touche
	/// </summary>
	/// <returns> Un pointeur vers le bouton associé à la touche </returns>
	const std::unordered_map<int, trBTN_Key*>& GetBTNS() const; // PROBLEME JE PENSE

	/// <summary>
	/// Cette fonction permet de récupérer l'état d'une touche
	/// </summary>
	/// <param name="actkeys"> Un vector qui contient les touches actives </param>
	/// <param name="Key"> La touche dont on veut connaître l'état </param>
	/// <returns> L'état de la touche (true si appuyée, false sinon) </returns>
	const int GetLastKeyState(const std::vector<trPair<int, bool>>& actkeys, int Key);

	// Update
	/// <summary>
	/// Cette fonction permet de mettre à jour l'état des touches
	/// </summary>
	virtual void Update();

	// START
	void Start();

	// DESTRUCTEUR
	~trKeyBoardManagement();

private:

	/// <summary>
	/// Loop qui permet de garder en mémoire les touches appuyées
	/// </summary>
	void Loop();

protected:

	std::unordered_map<int, trBTN_Key*> *BTNS;

	/// <summary>
	/// Un vector qui contient les touches actives
	/// </summary>
	std::vector<trPair<int, bool>> *ActiveKeysBufferWrite;

	/// <summary>
	/// Un vector qui contient les touches actives
	/// </summary>
	std::vector<trPair<int, bool>> *ActiveKeysBufferRead;

	/// <summary>
	/// Un mutex qui permet de gérer les accès concurrents à la variable ActiveKeys
	/// </summary>
	std::mutex *MutexKB;

private:

	bool Start_ = false;
};

#endif
