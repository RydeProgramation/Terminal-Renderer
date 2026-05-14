#pragma once

#include "trObject.h"

#ifndef __TR_DATA__
#define __TR_DATA__

#ifdef TERMINAL_CORE_EXPORTS
#define TERMINAL_CORE_API __declspec(dllexport)
#else
#define TERMINAL_CORE_API __declspec(dllimport)
#endif

template <class DATA_T>
struct trData : trObject
{
public:

	// INI default
	trData() : Old(new DATA_T()), Actual(new DATA_T()), New(new DATA_T())
	{

	}

	// INI
	trData(const DATA_T& Actual_) : Old(new DATA_T(Actual_)), Actual(new DATA_T(Actual_)), New(new DATA_T(Actual_))
	{

	}

	// INI deep copy
	trData(const trData& other) : Old(new DATA_T(*other.Old)), Actual(new DATA_T(*other.Actual)), New(new DATA_T(*other.New))
	{

	}

	// Copy
	trData& operator=(const trData& other)
	{
		if (this == &other) {
			return *this;
		}

		if (New == nullptr) {
			New = new DATA_T(*other.New);
		}
		else {
			*New = *other.New;
		}

		if (Actual == nullptr) {
			Actual = new DATA_T(*other.Actual);
		}
		else {
			*Actual = *other.Actual;
		}

		if (Old == nullptr) {
			Old = new DATA_T(*other.Old);
		}
		else {
			*Old = *other.Old;
		}

		return *this;
	}

	// Copy VAR
	trData& operator=(const DATA_T& other)
	{
		if (Old == nullptr) {
			Old = new DATA_T(other);
		}
		else {
			*Old = other;
		}

		if (Actual == nullptr) {
			Actual = new DATA_T(other);
		}
		else {
			*Actual = other;
		}

		if (New == nullptr) {
			New = new DATA_T(other);
		}
		else {
			*New = other;
		}

		return *this;
	}

	// SET
	void SetData(const DATA_T& newValue)
	{
		*New = newValue;
	}

	// GET
	const DATA_T& GetDataNew() const
	{
		return *New;
	}

	const DATA_T& GetDataOld() const
	{
		return *Old;
	}

	const DATA_T& GetDataActual() const
	{
		return *Actual;
	}

	// Update
	void Update()
	{
		*Old = *Actual;
		*Actual = *New;
	}

	// Destructuer
	virtual ~trData()
	{
		delete New;
		delete Old;
		delete Actual;
	}

private:

	DATA_T* New;
	DATA_T* Actual;
	DATA_T* Old;

};

#endif