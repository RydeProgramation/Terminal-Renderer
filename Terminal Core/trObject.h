#pragma once

#include "include.h"

#ifndef __TR_OBJECT__
#define __TR_OBJECT__

#ifdef TERMINAL_CORE_EXPORTS
#define TERMINAL_CORE_API __declspec(dllexport)
#else
#define TERMINAL_CORE_API __declspec(dllimport)
#endif

class TERMINAL_CORE_API trObject
{
private:

	// VAR

	bool* Validate;

	std::uint32_t* TakeNumber;

	bool IsTaked = false;

public:
	
	// FNC
	
	void Take(bool *&Validate_, std::uint32_t *&TakeNumber_)
	{
		if (Validate && !Validate_ && TakeNumber && !TakeNumber_)
		{
			IsTaked = true;
			Validate_ = Validate;
			TakeNumber_ = TakeNumber;

			(*TakeNumber)++;
		}
		
		else
		{
			MessageBox(
				NULL,
				L"c'est fait suprr",
				L"ERROR",
				MB_ICONERROR | MB_OK
			);
		}
	}

	void Give(bool *&Validate_, std::uint32_t *&TakeNumber_)
	{
		if (Validate_ && TakeNumber_)
		{
			if (*TakeNumber == 0)
			{
				IsTaked = false;
			}

			(*TakeNumber)--;

			Validate_ = nullptr;
			TakeNumber_ = nullptr;
		}

		else
		{
			MessageBox(
				NULL,
				L"c'est fait suprr",
				L"ERROR",
				MB_ICONERROR | MB_OK
			);
		}
	}

public:

	trObject() : Validate(new bool(true)), TakeNumber(new std::uint32_t(0))
	{

	}

	virtual ~trObject()
	{
		*Validate = false;

		if (!IsTaked)
		{
			delete Validate;
			delete TakeNumber;
		}
	}
};

#endif