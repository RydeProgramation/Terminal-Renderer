#pragma once

#include "include.h"

#include "trSize.h"
#include "trCoordinate.h"
#include "trData.h"
#include "trPair.h"
#include "trMap.h"
#include "trPawn.h"

#ifndef __TR_WIDGET__
#define __TR_WIDGET__

#ifdef TERMINAL_COMPONENTS_EXPORTS
#define TERMINAL_COMPONENTS_API __declspec(dllexport)
#else
#define TERMINAL_COMPONENTS_API __declspec(dllimport)
#endif

class TERMINAL_COMPONENTS_API trWidget : public trPawn
{
public:

	// INI default

	trWidget();

	virtual void Init() override;

	// INI

	trWidget(int x_, int y_, int size_x_, int size_y_, uint8_t RelativePositionType_, std::wstring content_, std::string name_);

	// INI deep copy

	trWidget(const trWidget& other);

	// Copy

	trWidget& operator=(const trActor& other);

	virtual trWidget& Clone(const trActor& other_) override;

	// SET

	void SetContent(const std::wstring& content_);

	void SetResetColor(const std::vector<trPair<std::wstring, trCoordinate<int>>>& RstColor);

	void SetColor(uint8_t R, uint8_t G, uint8_t B, bool Backround);

	void SetColor(const std::wstring& CodeCouleurAnsi);

	void ResetColor();

	void SetSize(int x_, int y_);

	virtual void SetProprety(const std::string& name, const std::string& data, const std::string& type) override;

	// ADD

	void AddToContent(const std::wstring& content_);

	void AddToSize(int x_, int y_);

	// GET

	const trSize<int>& GetSize() const;

	const trData<std::wstring>& GetContent() const;

	const trData<std::wstring>& GetColoredContent() const;

	const trData<std::wstring>& GetRawContent() const;

	const trData<trMap<int, trPair<std::wstring, trCoordinate<int>>>>& GetColoredMap() const;

	const std::vector<trPair<std::wstring, trCoordinate<int>>>& GetResetColor() const;

	const trData<std::wstring>& GetColor() const;

	const trData<std::wstring>& GetBackroundColor() const;

protected:

	virtual void APPLY_(const trSize<uint16_t>& SizeWindow) override;

	// FNC

public:

	bool VerificationProprety() override;

	virtual void UpdateRelativePositionPoint(const trSize<uint16_t>& SizeWindow) override;
	
	virtual void Display(std::wostringstream& output_line);

private:

	std::wstring ContentReorganisation(std::wstring _content, const trSize<int>& SizeWidget) const;

	std::wstring ContentReorganisationKeepColor(std::wstring _content, const trSize<int>& SizeWidget) const;

public:

	// Empty Widget (Static)
	static const trWidget& EmptyWidget();

	// DESTRUCTEUR

	virtual ~trWidget();

private:

	static trWidget* EmptyWidgetInstance; // Pointeur pour l'instance unique

	trSize<int> *Size;

	trData<std::wstring> *Color; // A voir pour supprimer

	trData<std::wstring>* BackroundColor; // A voir pour supprimer

	trData<std::wstring>* RawContent;

	/// <summary>
	/// L'index correspond aux coordonnées de l'info en 1 dimensions par rapport aux widget PAS a la fenêtre (merci de bien vouloir lire ;) )
	/// </summary>
	trData<trMap<int, trPair<std::wstring, trCoordinate<int>>>> *ColoredMap;

	trData<std::wstring>* ColoredContent;

	std::vector<trPair<std::wstring, trCoordinate<int>>> *BaseColor;

	trData<std::wstring>* Content;
};

#endif