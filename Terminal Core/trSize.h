#pragma once

#include "include.h"
#include "trData.h"

#ifndef __TR_SIZE__
#define __TR_SIZE__

#ifdef TERMINAL_CORE_EXPORTS
#define TERMINAL_CORE_API __declspec(dllexport)
#else
#define TERMINAL_CORE_API __declspec(dllimport)
#endif

template <class SIZE_T>
struct trSize : trObject
{
public:

	// INI default

	trSize() : x(new trData<SIZE_T>()), y(new trData<SIZE_T>())
	{

	}

	// INI

	trSize(const SIZE_T& x_, const SIZE_T& y_) : x(new trData<SIZE_T>(x_)), y(new trData<SIZE_T>(y_))
	{
		
	}

	// INI deep copy

	trSize(const trSize& other) : x(new trData<SIZE_T>(*other.x)), y(new trData<SIZE_T>(*other.y))
	{

	}

	// Copy

	trSize& operator=(const trSize& other)
	{
		if (this == &other) 
		{ 
			return *this; 
		}

		if (x == nullptr) {
			x = new trData<SIZE_T>(*other.x);
		}
		else {
			*x = *other.x;
		}

		if (y == nullptr) {
			y = new trData<SIZE_T>(*other.y);
		}
		else {
			*y = *other.y;
		}		

		return *this;
	}

	// SET

	void SetSize(const SIZE_T& x_, const SIZE_T& y_)
	{
		x->SetData(x_);
		y->SetData(y_);
	}

	// GET

	const trData<SIZE_T>& GetSizeX() const 
	{
		return *x;
	}

	const trData<SIZE_T>& GetSizeY() const 
	{
		return *y;
	}

	// Update

	void Update() 
	{
		x->Update();
		y->Update();
	}

	// DESTRUCTEUR

	virtual ~trSize()
	{
		delete x;
		delete y;
	}

private:

	trData<SIZE_T> *x;
	trData<SIZE_T> *y;
};

#endif
