#pragma once

#include "include.h"
#include "trData.h"

#ifndef __TR_COORDINATE__
#define __TR_COORDINATE__

#ifdef TERMINAL_CORE_EXPORTS
#define TERMINAL_CORE_API __declspec(dllexport)
#else
#define TERMINAL_CORE_API __declspec(dllimport)
#endif

template <class COORD_T>
struct trCoordinate : trObject
{
public:

	// INI default
	trCoordinate() : x(new trData<COORD_T>()), y(new trData<COORD_T>())
	{

	}

	// INI
	trCoordinate(const COORD_T& x_, const COORD_T& y_) : x(new trData<COORD_T>(x_)), y(new trData<COORD_T>(y_))
	{

	}

	// INI deep copy
	trCoordinate(const trCoordinate& other) : x(new trData<COORD_T>(*other.x)), y(new trData<COORD_T>(*other.y))
	{

	}

	// Copy
	trCoordinate& operator=(const trCoordinate& other)
	{
		if (this == &other)
		{ 
			return *this; 
		}

		if (x == nullptr) {
			x = new trData<COORD_T>(*other.x);
		}
		else {
			*x = *other.x;
		}

		if (y == nullptr) {
			y = new trData<COORD_T>(*other.y);
		}
		else {
			*y = *other.y;
		}

		return *this;
	}

	// SET
	void SetCoord(const COORD_T& x_, const COORD_T& y_) 
	{
		x->SetData(x_);
		y->SetData(y_);
	}

	// GET
	const trData<COORD_T>& GetX() const 
	{
		return *x;
	}
	 
	const trData<COORD_T>& GetY() const 
	{
		return *y;
	}

	// UPDATE
	void Update() 
	{
		x->Update();
		y->Update();
	}

	// DESTRUCTEUR

	virtual ~trCoordinate()
	{
		delete x;
		delete y;
	}

private:

	trData<COORD_T> *x;
	trData<COORD_T> *y;
};

#endif
