#pragma once

#include "include.h"

#include "trData.h"
#include "trSize.h"
#include "trCoordinate.h"
#include "trWeakPointer.h"

#ifndef __TR_BTN_KEY__
#define __TR_BTN_KEY__

#ifdef TERMINAL_INPUT_MODULE_EXPORTS
#define TERMINAL_INPUT_API __declspec(dllexport)
#else
#define TERMINAL_INPUT_API __declspec(dllimport)
#endif

// PresseOrRelease

#define OnPress 0x001
#define OnRelease 0x002

// Utilisation mode

// En mode l'action se fait que une fois ça répète pas à l'infini
#define HoldToTrigger 0x003
// Sur l'appuie l'action se fait
#define PressToTrigger 0x004
// Exemple joystick
#define Axis 0x005

struct TERMINAL_INPUT_API trBTN_Key : public trObject
{
public:

	// INI default

	trBTN_Key();

	// INI

	trBTN_Key(int Key_, int PressOrRelease_, int UsingMode_, const std::function<void()>& Action_, trObject* Ptr_);

	// INI deep copy

	trBTN_Key(const trBTN_Key& other);

	// Copy

	trBTN_Key& operator=(const trBTN_Key& other);

	// SET

	/// <summary>
	/// Permet de réinitialiser l'état de la touche et de l'action.
	/// </summary>
	void Reset();

	/// <summary>
	/// Permet de définir la touche à utiliser pour l'action.
	/// </summary>
	/// <param name="Action_"> Touche à utiliser </param>
	void SetAction(std::function<void()> Action_);

	/// <summary>
	/// Permet de définir un pointeur vers un objet qui sera utilisé pour l'action.
	/// </summary>
	/// <param name="Ptr_"> Pointeur vers l'objet à utiliser </param>
	void SetPointer(trObject* Ptr_);

	// GET

	/// <summary>
	/// Permet de récupérer l'état de l'action. 
	/// </summary>
	/// <returns> Vrai si l'action est active, faux sinon </returns>
	const bool GetActionState() const;

	/// <summary>
	/// Permet de récupérer l'état de la touche.
	/// </summary>
	/// <returns> Vrai si la touche est enfoncée, faux sinon </returns>
	const int GetKeyState() const;

	/// <summary>
	/// Permet de récupérer la touche utilisée pour l'action. 
	/// </summary>
	/// <returns> La touche utilisée pour l'action </returns>
	const int GetKey() const;

	trWeakPointer* GetPtr() const;

	// UPDATE

	/// <summary>
	/// Met à jour l'état de la touche et de l'action en fonction de l'état de la touche. 
	/// </summary>
	/// <param name="state_"> État de la touche (vrai pour enfoncée, faux pour relâchée) </param>
	void Update(bool state_);

	// FNC
	
	/// <summary>
	/// Permet d'exécuter l'action associée à la touche si elle est enfoncée ou relâchée, selon le mode d'utilisation.
	/// </summary>
	void DoAction();

	// DESTRUCTEUR

	~trBTN_Key();

private:

	/// <summary>
	/// Indique si la touche est protégée, c'est-à-dire si elle doit être utilisée avec précaution pour éviter les actions non désirées.
	/// </summary>
	bool Protect; // a voir si utile mdmdr

	/// <summary>
	/// Représente la touche utilisée pour l'action.
	/// </summary>
	int Key;

	/// <summary>
	/// Indique l'état de la touche : enfoncée ou relâchée.
	/// </summary>
	bool KeyState;

	/// <summary>
	/// Indique le mode d'utilisation de la touche.
	/// </summary>
	int Mode;

	/// <summary>
	/// Indique si l'action doit être déclenchée à chaque pression ou relâchement de la touche.
	/// </summary>
	int UsingMode;

	/// <summary>
	/// Indique si l'action associée à la touche est actuellement active.
	/// </summary>
	bool ActionState = false;

	/// <summary>
	/// Représente l'action à exécuter lorsque la touche est enfoncée ou relâchée.
	/// </summary>
	std::function<void()> *Action;

	// J'ai oublié pourquoi j'ai fait ça mais je suis sûr que c'est important
	trWeakPointer *Ptr;
};

#endif
