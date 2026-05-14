#pragma once

#include "trObject.h"

#ifndef __TR_WEAK_POINTER__
#define __TR_WEAK_POINTER__

#ifdef TERMINAL_CORE_EXPORTS
#define TERMINAL_CORE_API __declspec(dllexport)
#else
#define TERMINAL_CORE_API __declspec(dllimport)
#endif

class TERMINAL_CORE_API trWeakPointer
{
public:

	// INI
	trWeakPointer(trObject* Ptr_);

	// INI deep copy : C'est pas possible DU COUP sii
	/*trWeakPointer(const trData& other) : Ptr(new DATA_T(*other.Old))
	{

	}*/

	// COPY 
	trWeakPointer& operator=(const trWeakPointer& other);

public:

	// GET
	const bool IsValid() const;

	// Destructuer
	virtual ~trWeakPointer();

private:

	trObject* Ptr;
	bool* PtrValidate;
	std::uint32_t* TakeNumber;
};

#endif