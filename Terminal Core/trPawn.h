#pragma once

#include "include.h"

#include "trSize.h"
#include "trCoordinate.h"
#include "trData.h"
#include "trPair.h"
#include "trMap.h"
#include "trActor.h"

#ifndef __TR_PAWN__
#define __TR_PAWN__

#ifdef TERMINAL_CORE_EXPORTS
#define TERMINAL_CORE_API __declspec(dllexport)
#else
#define TERMINAL_CORE_API __declspec(dllimport)
#endif

class TERMINAL_CORE_API trPawn : public trActor
{
public:

	// INI default

	trPawn();

	virtual void Init() override;

	// INI

	trPawn(int x_, int y_, int RelativePosition_, std::string name_);

	// INI deep copy

	trPawn(const trPawn& other);

	// Copy

	trPawn& operator=(const trActor& other);

	virtual trPawn& Clone(const trActor& other_) override;

	// SET

	void SetPosition(int x_, int y_);

	bool SetTypeRelativePosition(int rp);

	void SetPawnCollision(trActor* actor, bool collision);

	// ADD

	void AddToPosition(int x_, int y_);

	// GET

	const trCoordinate<int>& GetPosition() const;

	const trCoordinate<int>& GetAbsolutePosition() const;

	const trData<uint8_t>& GetRelativePositionType() const;

	const std::vector<trActor*>& GetPawnCollision() const;

	virtual void SetProprety(const std::string& name, const std::string& data, const std::string& type) override;

	// UPDATE APPLY (a mettre en private ?)

	void UpdateRelativePosition();

	// APPLY

protected:

	virtual void APPLY_(const trSize<uint16_t>& SizeWindow) override;

public:

	virtual void Tick() {};

	// FONCTION

	virtual bool VerificationProprety() override;

	virtual void UpdateRelativePositionPoint(const trSize<uint16_t>& SizeWindow);

	// DESTRUCTEUR

	virtual ~trPawn();

protected:

	trCoordinate<int>* PositionRelative;

	trCoordinate<int>* RelativePositionPoint;

	trCoordinate<int>* PositionAbsolue;

	std::vector<trActor*>* PawnCollision;

	trData<uint8_t>* RpType;
};

#endif