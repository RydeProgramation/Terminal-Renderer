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

#ifndef __TR_WORLD__
#define __TR_WORLD__

#ifdef TERMINAL_WORLD_EXPORTS
#define TERMINAL_WORLD_API __declspec(dllexport)
#else
#define TERMINAL_WORLD_API __declspec(dllimport)
#endif

class TERMINAL_WORLD_API trWorld : public trObject
{
public:

	// INI default

	trWorld();

	// INI deep copy

	trWorld(const trWorld& other);

	// Copy

	trWorld& operator=(const trWorld& other);

	/// ACTOR MANG /// a mettre probablement en static

	/// <summary>
	/// Crée un widget
	/// </summary>
	/// <typeparam name="Widget_T"> trWidget, trSelector... </typeparam>
	/// <param name="WIDG"> Le widget à créer </param>
	/// <returns> true si le widget a été créé avec succès, false sinon </returns>
	bool CreateActor(trActor* WIDG);

	/// <summary>
	/// Met à jour un widget (rafraichit son contenu, sa position, etc.)
	/// </summary>
	void UpdateActors(const trSize<uint16_t>& ConsoleSize_Border, const double& DeltaTime_);

	/// <summary>
	/// Pour mettre à jour les interactions entre les actors
	/// </summary>
	void Update();

	/// <summary>
	/// Détruit un widget
	/// </summary>
	/// <typeparam name="Widget_T"> trWidget, trSelector... </typeparam>
	/// <param name="WIDG"> Le widget à détruire </param>
	/// <returns> true si le widget a été détruit avec succès, false sinon </returns>
	bool DestroyActor(trActor* WIDG);

	/// <summary>
	/// Détruit un widget par son nom
	/// </summary>
	/// <typeparam name="Widget_T"> trWidget, trSelector... </typeparam>
	/// <param name="name"> Le nom du widget à détruire </param>
	/// <returns> true si le widget a été détruit avec succès, false sinon </returns>
	bool DestroyActor(const std::string& Name);

	/// <summary>
	/// Détruit tous les widgets
	/// </summary>
	/// <returns> true si tous les widgets ont été détruits avec succès, false sinon </returns>
	bool Destroy();

public:

	// GET

	/// <summary>
	/// Obtient un widget par son nom
	/// </summary>
	/// <param name="Name"> Le nom du widget à obtenir </param>
	/// <returns> Un object de type trWidget si le widget existe, nullptr sinon </returns>
	const trActor& GetActor(const std::string& Name) const;

	/// <summary>
	/// Obtient un pointeur vers tous les widgets
	/// </summary>
	/// <returns> Un pointeur vers une map contenant tous les widgets </returns>
	const std::unordered_map<std::string, trActor*>& GetActors() const;

	/// <summary>
	/// Obtient un pointeur vers un widget par son nom
	/// </summary>
	/// <param name="Name"> Le nom du widget à obtenir </param>
	/// <returns> Un pointeur vers un object de type trWidget si le widget existe, nullptr sinon </returns>
	trActor* GetPtrActor(const std::string& Name) const;

public: 

	/// <summary>
	/// Pour éxécuter une fonciton sur un Widget
	/// </summary>
	/// <param Name="Name_">Le nom du Widget</param>
	/// <param Name="func">L'adresse de la fonction par exemple (&trWidget::AddToContent)</param>
	/// <param Name="...args">Les argument</param>
	template <typename T, typename Func, typename... Args>
	void SetActor(const std::string& Name, Func&& func, Args&&... args)
	{
		auto it = Actors->find(Name);
		if (it != Actors->end())
		{
			if (auto* casted = dynamic_cast<T*>(it->second))
			{
				std::invoke(std::forward<Func>(func), casted, std::forward<Args>(args)...); // a voir du coup c'est inutile ?
			}
		}
	}

protected:

	// FNC

	void DetectCollisionsOptimized(const std::map<trActor*, trRect<int>>& rectMap);

public:

	// DESTRUCTEUR

	virtual ~trWorld();

private:

	std::unordered_map<std::string, trActor*> *Actors;
};

#endif