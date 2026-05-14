#pragma once

#include "include.h"

#include "trData.h"

#ifndef __TR_PAIR__
#define __TR_PAIR__

#ifdef TERMINAL_CORE_EXPORTS
#define TERMINAL_CORE_API __declspec(dllexport)
#else
#define TERMINAL_CORE_API __declspec(dllimport)
#endif

template <class PAIR_T_1, class PAIR_T_2>
struct trPair : trObject
{
public:

	// INI default

	trPair() : first(new PAIR_T_1()), second(new PAIR_T_2())
	{

	}

	// INI

	trPair(const PAIR_T_1& x_1, const PAIR_T_2& x_2) : first(new PAIR_T_1(x_1)), second(new PAIR_T_2(x_2))
	{

	}

	// INI deep

	trPair(const trPair& other) : first(new PAIR_T_1(*other.first)), second(new PAIR_T_2(*other.second))
	{

	}

	// copy

	trPair& operator=(const trPair& other)
	{
		if (this == &other) 
		{ 
			return *this; 
		}

		if (first == nullptr) {
			first = new PAIR_T_1(*other.first);
		}
		else {
			*first = *other.first;
		}

		if (second == nullptr) {
			second = new PAIR_T_2(*other.second);
		}
		else {
			*second = *other.second;
		}

		return *this;
	}

	bool operator==(const trPair<PAIR_T_1, PAIR_T_2>& other) const {
		return *first == *other.first && *second == *other.second;
	}

	// SET

	void SetPair(const PAIR_T_1& x_1, const PAIR_T_2& x_2)
	{
		*first = x_1;
		*second = x_2;
	}

	void SetFirst(const PAIR_T_1& x)
	{
		*first = x;
	}

	void SetSecond(const PAIR_T_2& x)
	{
		*second = x;
	}

	// GET

	const PAIR_T_1& GetFirst() const
	{
		return *first;
	}

	const PAIR_T_2& GetSecond() const
	{
		return *second;
	}

	// DESTRUCTEUR

	virtual ~trPair()
	{
		delete first;
		delete second;
	}

public:

	PAIR_T_1* first;
	PAIR_T_2* second;
};

// C'est pour l'indexion de MAP

namespace std {
	template<>
	struct hash<trPair<int, int>> {
		std::size_t operator()(const trPair<int, int>& p) const noexcept {
			return std::hash<int>()(*p.first) ^ (std::hash<int>()(*p.second) << 1);
		}
	};
}

#endif