#pragma once

#include "Header.h"

#ifndef __MYUI__
#define __MYUI__

class MyUI : public trUserInterface
{
public:

	MyUI();

	virtual ~MyUI();

	void Debut();

	void Menu1();

private:

	// attendre qu'il s'affiche au moins une fois pour éviter d'être trop rapide dans l'execution
	void CreateWidgetWait(trWidget* WIDG); 

	// attendre qu'il s'affiche au moins une fois pour éviter d'être trop rapide dans l'execution
	void CreateSelectorWait(trSelector* WIDG);
};

class Munition : public trWidget
{
public:

	Munition()
		: trWidget(0, 0, 3, 5, MiddleCenter, L"", "BulletDefault" + std::to_string(bulletCount))
	{
		this->SetContent(L"****************");
		bulletCount++;

		if (bulletCount >= 20)
		{
			// this->SetDestroy(true);
		}
	}

	Munition(int x_, int y_, int RelativePosition_, std::wstring content_, std::string name_)
		: trWidget(x_, y_, 3, 5, RelativePosition_, content_, name_ + std::to_string(bulletCount))
	{
		this->SetContent(L"****************");
		bulletCount++;

		if (bulletCount >= 20)
		{
			// this->SetDestroy(true);
		}
	}

	void Tick() override
	{
		if (a <= 800)
		{
			this->AddToPosition(0, -static_cast<int>(std::floor(b)));
			b += 0.15f;
			a++;

			if (b >= 1.1)
				b = 0;
		}
		
		else
		{
			this->SetDestroy(true);
		}
	}

	~Munition()
	{
		// bulletCount--;
	}

	int a = 0;
	float b = 0;

	static int bulletCount;
};
#endif