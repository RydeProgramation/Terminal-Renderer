#pragma once

#include "include.h"
#include "trPair.h"

#ifndef __TR_MAP__
#define __TR_MAP__

#ifdef TERMINAL_CORE_EXPORTS
#define TERMINAL_CORE_API __declspec(dllexport)
#else
#define TERMINAL_CORE_API __declspec(dllimport)
#endif

template <class INDEX, class MAP_T>
struct trMap : public trObject
{
public:

	// INI default

	trMap() : Map(new std::vector<trPair<INDEX, MAP_T>>()) 
	{

	}

	// INI deep

	trMap(const trMap& other) : Map(new std::vector<trPair<INDEX, MAP_T>>(*other.Map)) 
	{

	}

	// copy

	trMap& operator=(const trMap& other)
	{
		if (this == &other) {
			return *this;
		}

		if (Map == nullptr) {
			Map = new std::vector<trPair<INDEX, MAP_T>>(*other.Map);
		}
		else {
			*Map = *other.Map;
		}

		return *this;
	}

	// index

	MAP_T& operator[](const INDEX& idx)
	{
		for (auto& pair : *Map)
		{
			if (*pair.first == idx)
			{
				return *pair.second;
			}
		}

		Map->push_back(trPair<INDEX, MAP_T>(idx, MAP_T()));

		return *Map->back().second;
	}

	// GET

	size_t GetSize() const 
	{
		return Map->size();
	}

	// Itérateur début

	auto begin() const
	{
		return Map->begin();
	}

	// Itérateur fin

	auto end() const
	{
		return Map->end();
	}

	// Itérateur début (non-const pour modification)

	auto begin()
	{
		return Map->begin();
	}

	// Itérateur fin (non-const pour modification)

	auto end()
	{
		return Map->end();
	}

	// DESTR

	virtual ~trMap()
	{
		delete Map;
	}

private:

	std::vector<trPair<INDEX, MAP_T>>* Map; // je sais que c'est de la triche unpeu mais bon
};

#endif
