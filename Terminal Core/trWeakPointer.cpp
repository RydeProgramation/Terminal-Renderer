#include "trWeakPointer.h"

// INI

trWeakPointer::trWeakPointer(trObject* Ptr_) : Ptr(Ptr_), PtrValidate(nullptr), TakeNumber(nullptr)
{
	if (Ptr)
	{
		Ptr->Take(PtrValidate, TakeNumber); // On récupère le bool* valide

		if (!PtrValidate || !TakeNumber)
		{
			MessageBox(
				NULL,
				L"Erreur lors de l'assignation du WeakPointer",
				L"Erreur",
				MB_ICONERROR | MB_OK
			);
		}
	} else {
		MessageBox(
			NULL,
			L"pas de pointeur",
			L"Message",
			MB_ICONERROR | MB_OK
		);
	}
}

// COPY

trWeakPointer& trWeakPointer::operator=(const trWeakPointer& other)
{
	if (this == &other)
	{
		return *this;
	}

	// Libère les références actuelles si valides
	if (Ptr && PtrValidate && TakeNumber)
	{
		if (!(*PtrValidate) && *TakeNumber == 1)
		{
			delete PtrValidate;
			delete TakeNumber;
		}

		else
		{
			Ptr->Give(PtrValidate, TakeNumber);
		}
	}

	// Réinitialisation des pointeurs
	Ptr = other.Ptr;

	// Nouvelle acquisition si le nouveau Ptr n'est pas nul
	if (Ptr)
	{
		Ptr->Take(PtrValidate, TakeNumber);

		if (!PtrValidate || !TakeNumber)
		{
			MessageBox(
				NULL,
				L"Erreur lors de l'assignation du WeakPointer",
				L"Erreur",
				MB_ICONERROR | MB_OK
			);
		}
	}

	return *this;
}

// GET

const bool trWeakPointer::IsValid() const
{
	return *PtrValidate;
}

// DESTRUCTUER

trWeakPointer::~trWeakPointer()
{
	if (!(*PtrValidate) && *TakeNumber == 1)
	{
		delete PtrValidate;
		delete TakeNumber;
	}

	else
	{
		Ptr->Give(PtrValidate, TakeNumber);
	}
}
