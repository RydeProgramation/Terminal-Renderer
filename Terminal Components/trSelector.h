#pragma once

#include "include.h"

#include "trSize.h"
#include "trCoordinate.h"
#include "trWidget.h"
#include "trData.h"

#ifndef __TR_SELECTOR__
#define __TR_SELECTOR__

#ifdef TERMINAL_COMPONENTS_EXPORTS
#define TERMINAL_COMPONENTS_API __declspec(dllexport)
#else
#define TERMINAL_COMPONENTS_API __declspec(dllimport)
#endif

class TERMINAL_COMPONENTS_API trSelector : public trWidget
{
public:

	// INI default

	trSelector();

	virtual void Init() override;

	// INI

	trSelector(int x_, int y_, int size_x_, int size_y_, uint8_t RelativePosition_, std::wstring content_, std::string name_);

	// INI deep copy

	trSelector(const trSelector& other);

	// Copy

	trSelector& operator=(const trActor& other);

	virtual trSelector& Clone(const trActor& other_) override;

	// SET

	void SetSelected(bool Slct);

	void SetColorSelected(uint8_t R, uint8_t G, uint8_t B, bool Backround);

	void SetColorSelected(const std::wstring& CodeCouleurAnsi);

	void SetColorUnSelected(uint8_t R, uint8_t G, uint8_t B, bool Backround);

	void SetColorUnSelected(const std::wstring& CodeCouleurAnsi);

	virtual void SetProprety(const std::string& name, const std::string& data, const std::string& type) override;

	// GET

	const trData<bool> IsSelected() const;

	const trData<std::wstring> GetColorSelected() const;

	// APPLY

	bool VerificationProprety() override;

	void APPLY_(const trSize<uint16_t>& SizeWindow) override;

	void Display(std::wostringstream& OutputLine) override;

	// DESTRUCTEUR

	~trSelector();

private:

	trData<bool> *Selected;

	trData<std::wstring> *ColorSelected;

	trData<std::wstring> *ColorUnSelected;
};

#endif