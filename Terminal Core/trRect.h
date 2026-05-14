#pragma once

#include "include.h"

#include "trData.h"

#ifndef __TR_RECT__
#define __TR_RECT__

#ifdef TERMINAL_CORE_EXPORTS
#define TERMINAL_CORE_API __declspec(dllexport)
#else
#define TERMINAL_CORE_API __declspec(dllimport)
#endif

template <class DATA_>
struct trRect : trObject
{
public:

	// INI default

	trRect() : Top(new DATA_()), Bottom(new DATA_()), Left(new DATA_()), Right(new DATA_())
	{

	}

	// INI

	trRect(const DATA_& Left_, const DATA_& Top_, const DATA_& Right_, const DATA_& Bottom_) : Top(new DATA_(Top_)), Bottom(new DATA_(Bottom_)), Left(new DATA_(Left_)), Right(new DATA_(Right_))
	{

	}

	// INI deep

	trRect(const trRect& other) : Top(new DATA_(*other.Top)), Bottom(new DATA_(*other.Bottom)), Left(new DATA_(*other.Left)), Right(new DATA_(*other.Right))
	{

	}

	// copy

	trRect& operator=(const trRect& other)
	{
		if (this == &other)
		{
			return *this;
		}

		if (Top == nullptr) {
			Top = new DATA_(*other.Top);
		}
		else {
			*Top = *other.Top;
		}

		if (Bottom == nullptr) {
			Bottom = new DATA_(*other.Bottom);
		}
		else {
			*Bottom = *other.Bottom;
		}

		if (Left == nullptr) {
			Left = new DATA_(*other.Left);
		}
		else {
			*Left = *other.Left;
		}

		if (Right == nullptr) {
			Right = new DATA_(*other.Right);
		}
		else {
			*Right = *other.Right;
		}

		return *this;
	}

	// SET

	void SetBottom(const DATA_& x)
	{
		*Top = x;
	}

	void SetTop(const DATA_& x)
	{
		*Bottom = x;
	}

	void SetLeft(const DATA_& x)
	{
		*Left = x;
	}

	void SetRight(const DATA_& x)
	{
		*Right = x;
	}

	// GET

	const DATA_& top() const
	{
		return *Top;
	}

	const DATA_& bottom() const
	{
		return *Bottom;
	}

	const DATA_& left() const
	{
		return *Left;
	}

	const DATA_& right() const
	{
		return *Right;
	}

	// DESTRUCTEUR

	virtual ~trRect()
	{
		delete Top;
		delete Bottom;
		delete Left;
		delete Right;
	}

private:

	DATA_* Top;
	DATA_* Bottom;
	DATA_* Left;
	DATA_* Right;

};

#endif