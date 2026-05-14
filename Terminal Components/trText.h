#pragma once

#include "include.h"

#include "trSize.h"
#include "trCoordinate.h"
#include "trWidget.h"
#include "trData.h"

#ifndef __TR_TEXT__
#define __TR_TEXT__

#ifdef TERMINAL_COMPONENTS_EXPORTS
#define TERMINAL_COMPONENTS_API __declspec(dllexport)
#else
#define TERMINAL_COMPONENTS_API __declspec(dllimport)
#endif

class TERMINAL_COMPONENTS_API trText : public trWidget
{
public:

	// INI default

	trText();

	virtual void Init() override;

	// INI

	trText(int x_, int y_, int size_x_, int size_y_, uint8_t RelativePosition_, std::wstring content_, std::string name_);

	// INI deep copy

	trText(const trText& other);

	// Copy

	trText& operator=(const trActor& other);

	virtual trText& Clone(const trActor& other_) override;

	// SET

	void SetAnimationVector(const std::vector<trPair<uint16_t, std::wstring>>& AnimationVector_);

	virtual void SetProprety(const std::string& name, const std::string& data, const std::string& type) override;

	// GET

	const std::vector<trPair<uint16_t, std::wstring>>& GetAnimationVector() const;

	// ADD
	
	void AddAnimation(const trPair<uint16_t, std::wstring>& Animation_);

	void AddAnimation(uint16_t Time_, const std::wstring& Animation_);

	// FNC

	void DoCharToCharAnimation(uint16_t SleepTime_, const std::wstring Content_);

	void DoCharToCharAnimation(uint16_t SleepTime_);

	void DoCharToCharAnimation(uint16_t SleepTime_, const std::wstring Content_, bool & Finished);

	void DoCharToCharAnimation(uint16_t SleepTime_, bool& Finished);

	void StopCharToCharAnimation();

	void DoNextFrameAnimation();

	void DoPreviousFrameAnimation();

	void DoAnimation();

	void DoAnimation(bool& Finished);

	void StopAnimation();

	// APPLY

	bool VerificationProprety() override;

protected:

	virtual void Tick() override;

	void APPLY_(const trSize<uint16_t>& SizeWindow) override;

public:

	// DESTRUCTEUR

	~trText();

private:

	std::vector<trPair<uint16_t, std::wstring>> *AnimationVector;

	uint16_t indexAnimation;

	bool isAnimation = false;
	bool IsAnimationPrevalent = false;
	bool* isFinishedAnimation = nullptr;
	uint16_t DelayAnimation = 500;
	float AccumulatedDelayAnimation = 0;
	int indexChar = 0;

	bool isCharToCharAnimation = false;
	uint16_t DelayCharToCharAnimation = 50;
	bool IsCharToCharAnimationPrevalent = false;
	bool* isFinishedCharToCharAnimation = nullptr;
	float AccumulatedDelayCharToCharAnimation = 0;

	std::wstring* Content_;

public:
	void ResetIndexAnimation();
};


#endif