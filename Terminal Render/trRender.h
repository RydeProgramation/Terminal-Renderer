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
#include "trActor.h"
#include "trPrint.h"

#ifndef __TR_RENDER__
#define __TR_RENDER__

#ifdef TERMINAL_RENDER_EXPORTS
#define TERMINAL_RENDER_API __declspec(dllexport)
#else
#define TERMINAL_RENDER_API __declspec(dllimport)
#endif

#define DIRECT_SYSTEM 0 // chaque actor est rendu séparément
#define BUFFER_SYSTEM 1 // tous les actors sont rendus dans un buffer et affichés en une fois
#define RENDER_SYSTEM 2 // Que ce qui est différent est rendu

class TERMINAL_RENDER_API trRender : public trObject
{
public:

	// INI default

	trRender();

	// INI

	trRender(uint8_t RenderType_, int BordW_, std::wstring RstClr);

	// INI deep copy

	trRender(const trRender& other);

	// Copy

	trRender& operator=(const trRender& other);

	// SET

	void SetBaseColor(const std::wstring& BaseColor_);

	void SetCursorSelector(const trData<int>& CursorSelector_);

	// GET

	const std::wstring& GetBaseColor() const;

	const trSize<uint16_t>& GetSizeWindow() const;

	const trSize<uint16_t>& GetSizeWindowBorder() const;

	const trData<int>& GetCursorSelector() const;

	const std::wostringstream& GetRender() const;

	const std::wostringstream& GetRenderColor() const;

	const uint8_t GetRenderType() const;

	// FNC

	/// <summary>
	/// Crée une bordure
	/// </summary>
	/// <param Name_="BorderWidth">Nombre de pixel sur le coté</param>
	void Border();

	/// <summary>
	/// Permet de vérifier si les coordonnées données sont en dehors de la fenêtre
	/// </summary>
	/// <param name="SizeWindow_"> Taille de la fenêtre </param>
	void UpdateSizeWindow();

	/// <summary>
	/// Affiche un widget
	/// </summary>
	/// <param name="WIDG"> Le widget à afficher </param>
	/// <returns> true si le widget a été affiché avec succès, false sinon </returns>
	void DisplayWidget(trWidget* WIDG); // à vérifier

	/// <summary>
	/// Aplique les couleurs
	/// </summary>
	void DisplayColor(const std::unordered_map<std::string, trActor*>& Actors_); // à verifier

	/// <summary>
	/// Cache un widget (le rend invisible)
	/// </summary>
	/// <param name="WIDG"> Le widget à cacher </param>
	void HideWidget(trWidget* WIDG);

	/// <summary>
	/// Nettoie un widget (le rend vide)
	/// </summary>
	/// <param name="WIDG"> Le widget à nettoyer </param>
	void CleanWidget(trWidget* WIDG);

	/// <summary>
	/// Nettoie le terminal (vide le buffer de rendu)
	/// </summary>
	void Clear();

	//Render

	/// <summary>
	/// Raffraichie le terminal en affichant les "pixel"
	/// </summary>
	void Render(const std::unordered_map<std::string, trActor*>& Actors_);

public:

	// DESTRUCTEUR

	virtual ~trRender();

protected:

	std::wstring* BaseColor;

	trSize<uint16_t>* SizeWindow;

	trSize<uint16_t>* SizeWindow_Border;

	trData<int>* CursorSelector;

	std::wostringstream* Render_;

	std::string* PreviousRender_;

	std::wostringstream* RenderColor_;

	uint8_t RenderType;
};

#endif