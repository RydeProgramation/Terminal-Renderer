#pragma once

#include "include.h"

#include "trCoordinate.h"
#include "trSize.h"
#include "trWidget.h"
#include "trData.h"
#include "trUIToolsCore.h"

#ifndef __TR_UI_TOOLS__
#define __TR_UI_TOOLS__

#ifdef TERMINAL_TOOL_EXPORTS
#define TERMINAL_TOOL_API __declspec(dllexport)
#else
#define TERMINAL_TOOL_API __declspec(dllimport)
#endif

namespace UITools
{
	using namespace UIToolsCore;

	/// <summary>
	/// Déplace le curseur vers
	/// </summary>
	/// <param name="trCoordinate"></param>
	/// <param name="WithBorder"> Mettre false si tu veux être à l'interieur des bordures </param>
	TERMINAL_TOOL_API void MoveCursorTo(const trCoordinate<int>& Coordinate_, uint8_t BorderWitdh_);

	/// <summary>
	/// Déplace le curseur vers
	/// </summary>
	/// <param name="trCoordinate"></param>
	/// <param name="WithBorder"> Mettre false si tu veux être à l'interieur des bordures </param>
	TERMINAL_TOOL_API void MoveCursorTo(const trCoordinate<int>& Coordinate_);

	/// <summary>
	/// Cache le curseur
	/// </summary>
	TERMINAL_TOOL_API void hideCursor();

	/// <summary>
	/// Permet de verfier si les coordonnées données sont en dehors de la fenêtre
	/// </summary>
	/// <param name="WIDG"></param>
	/// <param name="trCoordinate"></param>
	/// <returns></returns>
	TERMINAL_TOOL_API inline bool IsOutSide(const trCoordinate<int>& xy, uint8_t BorderWidth_);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="x"></param>
	/// <param name="y"></param>
	/// <param name="maxX"></param>
	/// <param name="maxY"></param>
	/// <returns></returns>
	TERMINAL_TOOL_API inline bool IsOutSideFast(uint16_t x, uint16_t y, uint16_t maxX, uint16_t maxY);

	/// <summary>
	/// Permet de verfier si les coordonnées données sont en dehors de la fenêtre
	/// </summary>
	/// <param name="WIDG"></param>
	/// <param name="trCoordinate"></param>
	/// <param name="CountTouch"></param>
	/// <returns></returns>
	TERMINAL_TOOL_API inline bool IsOutSide(const trCoordinate<int>& xy, uint8_t BorderWidth_, bool CountTouch);

	/// <summary>
	/// Permet de verifier si c'est en dehors juste avec les coord 1 dimensions
	/// </summary>
	/// <param name="z"></param>
	/// <param name="BorderWidth"></param>
	/// <returns></returns>
	TERMINAL_TOOL_API inline bool IsOutSide(int z, uint8_t BorderWidth_, bool CountTouch);

	/// <summary>
	/// Permet de verifier si c'est en dehors juste avec les coord 1 dimensions
	/// </summary>
	/// <param name="z"></param>
	/// <param name="BorderWidth"></param>
	/// <returns></returns>
	TERMINAL_TOOL_API inline bool IsOutSide(int z, uint8_t BorderWidth_);

	/// <summary>
	/// Permet de verfier si les coordonnées données sont en dehors de la fenêtre
	/// </summary>
	/// <param name="WIDG"></param>
	/// <param name="trCoordinate"></param>
	/// <returns></returns>
	TERMINAL_TOOL_API inline bool IsOutSide(const trCoordinate<int>& xy);

	/// <summary>
	/// Permet de verfier si les coordonnées données sont en dehors de la fenêtre
	/// </summary>
	/// <param name="WIDG"></param>
	/// <param name="trCoordinate"></param>
	/// <param name="CountTouch"></param>
	/// <returns></returns>
	TERMINAL_TOOL_API inline bool IsOutSide(const trCoordinate<int>& xy, bool CountTouch);

	/// <summary>
	/// Permet de verifier si c'est en dehors juste avec les coord 1 dimensions
	/// </summary>
	/// <param name="z"></param>
	/// <returns></returns>
	TERMINAL_TOOL_API inline bool IsOutSide(int z);
	
	/// <summary>
	/// Permet de verifier si c'est en dehors juste avec les coord 1 dimensions
	/// </summary>
	/// <param name="z"></param>
	/// <returns></returns>
	TERMINAL_TOOL_API inline bool IsOutSide(int z, bool CountTouch);

	/// <summary>
	/// Permet de verifier si c'est en dehors juste avec les coord 1 dimensions
	/// </summary>
	/// <param name="z"></param>
	/// <returns></returns>
	TERMINAL_TOOL_API inline bool IsOutSide(const trCoordinate<int>& xy, uint8_t BorderWidth_, const trSize<uint16_t>& WindowSize_Border);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="WIDG"></param>
	/// <returns></returns>
	TERMINAL_TOOL_API bool IsInWidget(trWidget* WIDG, const trCoordinate<int>& crd);

	/// <summary>
	/// Permet de déplacer pour un ostrinstream donnée la position du curseur
	/// </summary>
	/// <param name="Pos"> Position du curseur </param>
	/// <param name="Output"> Ostream dans lequel on va écrire </param>
	TERMINAL_TOOL_API void MoveCursorToOstream(const trCoordinate<int>& Pos, std::wostringstream* Output, const trSize<uint16_t>& SizeOutput);

	/// <summary>
	/// Permet de déplacer pour un ostrinstream donnée la position du curseur en tenant compte de la bordure
	/// </summary>
	/// <param name="Pos"> Position du curseur </param>
	/// <param name="Output"> Ostream dans lequel on va écrire </param>
	/// <param name="SizeOutput"> Taille de l'output (pour le terminal) </param>
	/// <param name="BorderW"> Largeur de la bordure </param>
	TERMINAL_TOOL_API void MoveCursorToOstream(const trCoordinate<int>& Pos, std::wostringstream* Output, const trSize<uint16_t>& SizeOutput, uint8_t BorderW);

	/// <summary>
	/// Permet de déplacer pour un ostrinstream donnée la position du curseur
	/// </summary>
	/// <param name="Pos"> Position du curseur </param>
	/// <param name="Output"> Ostream dans lequel on va écrire </param>
	TERMINAL_TOOL_API void CleanOstreamSize(std::wostringstream* Output, const trSize<uint16_t>& SizeOutput);

	/// <summary>
	/// Affiche la différence entre deux frames dans la console
	/// </summary>
	/// <param name="ActualFrame_"> Frame actuelle </param>
	/// <param name="PreviousFrame_"> Frame précédente </param>
	/// <returns> void </returns>
	TERMINAL_TOOL_API std::string DifferenceStringOutput(const std::string& ActualFrame_, const std::string& PreviousFrame_);

	extern TERMINAL_TOOL_API uint8_t BorderWidth;

	extern TERMINAL_TOOL_API bool ForceRefresh;

	extern TERMINAL_TOOL_API bool Refreshed;
}

#endif