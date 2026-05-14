#include "trText.h"
#include "trUIToolsCore.h"

REGISTER_TYPE(trText, int, int, int, int, uint8_t, std::wstring, std::string)

using namespace std;
using namespace UIToolsCore;

// INI default

trText::trText() : trWidget(), AnimationVector(new std::vector<trPair<uint16_t, std::wstring>>()), indexAnimation(0), isAnimation(false), AccumulatedDelayAnimation(0), isCharToCharAnimation(false), AccumulatedDelayCharToCharAnimation(0), Content_(new wstring(L""))
{

}

// INI

trText::trText(int x_, int y_, int size_x_, int size_y_, uint8_t RelativePosition_, wstring content_, string name_) : trWidget(x_, y_, size_x_, size_y_, RelativePosition_, content_, name_), AnimationVector(new std::vector<trPair<uint16_t, std::wstring>>()), indexAnimation(0), isAnimation(false), AccumulatedDelayAnimation(0), isCharToCharAnimation(false), AccumulatedDelayCharToCharAnimation(0), Content_(new wstring(content_))
{

}

void trText::Init()
{
	
}

// INI deep copy

trText::trText(const trText& other) : trWidget(other), AnimationVector(other.AnimationVector), indexAnimation(other.indexAnimation), isAnimation(other.isAnimation), AccumulatedDelayAnimation(other.AccumulatedDelayAnimation), isCharToCharAnimation(other.isCharToCharAnimation), AccumulatedDelayCharToCharAnimation(other.AccumulatedDelayCharToCharAnimation), Content_(other.Content_)
{

}

// Copy

trText& trText::operator=(const trActor& other_)
{
	return Clone(other_);
}

trText& trText::Clone(const trActor& other_)
{
	try
	{
		const trText& other = dynamic_cast<const trText&>(other_);

		// Si on est en train de se copier soi-même, il n'y a rien à faire
		if (this == &other) { return *this; }

		trWidget::Clone(other_);

		if (AnimationVector == nullptr) {
			AnimationVector = new std::vector<trPair<uint16_t, std::wstring>>(*other.AnimationVector);
		}
		else {
			*AnimationVector = *other.AnimationVector;
		}

		if (Content_ == nullptr) {
			Content_ = new wstring(*other.Content_);
		}
		else {
			*Content_ = *other.Content_;
		}

		return *this;
	}

	catch (const std::bad_cast&)
	{
		trWidget& Me = dynamic_cast<trWidget&>(*this);

		Me.trWidget::Clone(other_);

		return *this;
	}
}

// SET

void trText::SetAnimationVector(const std::vector<trPair<uint16_t, std::wstring>>& AnimationVector_)
{
	delete AnimationVector;
	AnimationVector = new std::vector<trPair<uint16_t, std::wstring>>(AnimationVector_);
}

void trText::SetProprety(const std::string& name, const std::string& data, const std::string& type)
{
	trWidget::SetProprety(name, data, type);


}

// GET

const std::vector<trPair<uint16_t, std::wstring>>& trText::GetAnimationVector() const
{
	return *AnimationVector;
}

// ADD

void trText::AddAnimation(const trPair<uint16_t, std::wstring>& Animation_)
{
	// faudrait utiliser un mutex ici je pense
	AnimationVector->push_back(Animation_);
}

void trText::AddAnimation(uint16_t Time_, const std::wstring& AnimationText_)
{
	// faudrait utiliser un mutex ici je pense
	trPair<uint16_t, std::wstring> Animation_(Time_, AnimationText_);

	AnimationVector->push_back(Animation_);
}

// RESET

void trText::ResetIndexAnimation()
{
	indexAnimation = 0;
}

// FNC

void trText::DoCharToCharAnimation(uint16_t SleepTime_, const std::wstring Content)
{
	if (!isCharToCharAnimation)
	{
		*Content_ = Content;

		SetContent(L"");

		isCharToCharAnimation = true;

		IsAnimationPrevalent == true ? IsCharToCharAnimationPrevalent = false : IsCharToCharAnimationPrevalent = true;

		DelayCharToCharAnimation = max(1, SleepTime_);

		SetActivate(true);
	}
}

void trText::DoCharToCharAnimation(uint16_t SleepTime_)
{
	if (!isCharToCharAnimation)
	{
		*Content_ = GetRawContent().GetDataActual().empty() ? GetRawContent().GetDataNew() : GetRawContent().GetDataActual();

		SetContent(L"");

		isCharToCharAnimation = true;

		IsAnimationPrevalent == true ? IsCharToCharAnimationPrevalent = false : IsCharToCharAnimationPrevalent = true;

		DelayCharToCharAnimation = max(1, SleepTime_);

		SetActivate(true);
	}
}

void trText::DoCharToCharAnimation(uint16_t SleepTime_, const std::wstring Content, bool& Finished)
{
	if (!isCharToCharAnimation)
	{
		*Content_ = Content;

		SetContent(L"");

		isCharToCharAnimation = true;

		IsAnimationPrevalent == true ? IsCharToCharAnimationPrevalent = false : IsCharToCharAnimationPrevalent = true;

		DelayCharToCharAnimation = max(1, SleepTime_);

		SetActivate(true);

		if (!isFinishedCharToCharAnimation)
		{
			Finished = false;
			isFinishedCharToCharAnimation = &Finished;
		}
		else
		{
			Finished = false;
			*isFinishedCharToCharAnimation = true;
			isFinishedCharToCharAnimation = &Finished;
		}
	}
}

void trText::DoCharToCharAnimation(uint16_t SleepTime_, bool& Finished)
{
	if (!isCharToCharAnimation)
	{
		*Content_ = GetRawContent().GetDataActual().empty() ? GetRawContent().GetDataNew() : GetRawContent().GetDataActual();

		SetContent(L"");

		isCharToCharAnimation = true;

		IsAnimationPrevalent == true ? IsCharToCharAnimationPrevalent = false : IsCharToCharAnimationPrevalent = true;

		DelayCharToCharAnimation = max(1, SleepTime_);

		SetActivate(true);

		if (!isFinishedCharToCharAnimation)
		{
			Finished = false;
			isFinishedCharToCharAnimation = &Finished;
		}
		else
		{
			Finished = false;
			*isFinishedCharToCharAnimation = true;
			isFinishedCharToCharAnimation = &Finished;
		}
	}
}

void trText::StopCharToCharAnimation()
{
	isCharToCharAnimation = false;
	indexChar = 0;
	AccumulatedDelayCharToCharAnimation = 0;
	IsCharToCharAnimationPrevalent = false;
	if (isFinishedCharToCharAnimation)
	{
		*isFinishedCharToCharAnimation = true;
		isFinishedCharToCharAnimation = nullptr;
	}
	SetContent((*Content_));
}

void trText::DoNextFrameAnimation()
{
	indexAnimation >= AnimationVector->size() - 1 ? indexAnimation = AnimationVector->size() - 1 : indexAnimation++;

	SetContent((*AnimationVector)[indexAnimation].GetSecond());
}

void trText::DoPreviousFrameAnimation()
{
	indexAnimation <= 0 ? 0 : indexAnimation--;

	SetContent((*AnimationVector)[indexAnimation].GetSecond());
}

void trText::DoAnimation()
{
	indexAnimation = 0;

	isAnimation = true;

	IsCharToCharAnimationPrevalent == true ? IsAnimationPrevalent = false : IsAnimationPrevalent = true;
}

void trText::DoAnimation(bool& Finished)
{
	indexAnimation = 0;

	isAnimation = true;

	IsCharToCharAnimationPrevalent == true ? IsAnimationPrevalent = false : IsAnimationPrevalent = true;

	if (!isFinishedAnimation)
	{
		Finished = false;
		isFinishedAnimation = &Finished;
	}
	else
	{
		Finished = false;
		*isFinishedAnimation = true;
		isFinishedAnimation = &Finished;
	}
}

void trText::StopAnimation()
{
	isAnimation = false;
	IsAnimationPrevalent = false;
	if (isFinishedAnimation)
	{
		*isFinishedAnimation = true;
		isFinishedAnimation = nullptr;
	}
}

// APPLY

bool trText::VerificationProprety()
{
	return (
		trWidget::VerificationProprety()
		);
}

void trText::Tick()
{
	trWidget::Tick();

	if (isAnimation && (!isCharToCharAnimation || IsAnimationPrevalent))
	{
		if (AnimationVector->empty())
		{
			return void();
		}

		if (indexAnimation >= AnimationVector->size())
		{
			StopAnimation();
			return void();
		}

		AccumulatedDelayAnimation += static_cast<float>(GetDeltaTime().GetDataActual());

		DelayAnimation = max(1, (*AnimationVector)[indexAnimation].GetFirst());

		if (AccumulatedDelayAnimation >= DelayAnimation)
		{
			int frameToRender = 0;

			for (int i = indexAnimation; indexAnimation <= AnimationVector->size() - 1 && AccumulatedDelayAnimation > (*AnimationVector)[i].GetFirst(); i = min(int(i + 1), AnimationVector->size() - 1))
			{
				AccumulatedDelayAnimation -= (*AnimationVector)[i].GetFirst();
				frameToRender++;
			}

			int BeginIndex = indexAnimation;

			for (indexAnimation; indexAnimation < min(int(BeginIndex + frameToRender), int((*AnimationVector).size())); indexAnimation++)
			{
				SetContent((*AnimationVector)[indexAnimation].GetSecond());

				AccumulatedDelayAnimation = 0;
			}
		}
	}

	if (isCharToCharAnimation && (!isAnimation || IsCharToCharAnimationPrevalent))
	{
		AccumulatedDelayCharToCharAnimation += static_cast<float>(GetDeltaTime().GetDataActual());

		if (indexChar >= Content_->size())
		{
			StopCharToCharAnimation();
			return void();
		}

		else if (AccumulatedDelayCharToCharAnimation >= DelayCharToCharAnimation)
		{
			int CharNumber = AccumulatedDelayCharToCharAnimation / DelayCharToCharAnimation;

			int i = indexChar;

			for (i = indexChar; i < min((indexChar + CharNumber), Content_->size()); i++)
			{
				if ((*Content_)[i] == L'\033') // ça serait cool que ça fonctionne tout le temps
				{
					// Trouve la fin de la séquence ANSI (caractère 'm' à la fin)
					size_t endSeq = Content_->find(L'm', i);

					if (endSeq != std::wstring::npos) {
						// On prend toute la séquence ANSI, en incluant le 'm'
						std::wstring ansiSeq = Content_->substr(i, endSeq - i + 1);  // +1 pour inclure 'm'

						// On ajoute la séquence ANSI à l'interface
						AddToContent(ansiSeq);
						// World->SetActor<trWidget>("Paragraphe", &trWidget::AddToContent, ansiSeq);

						// On saute l'indice de la séquence ANSI pour ne pas la traiter à nouveau
						i += static_cast<int>(ansiSeq.size() - 1); // -1 car on a déjà ajouté un caractère
						CharNumber += static_cast<int>(ansiSeq.size() - 1);
					}
				}

				else
				{
					// Si ce n'est pas une séquence ANSI, on ajoute un seul caractère
					AddToContent(wstring(1, (*Content_)[i]));
					//World->SetActor<trWidget>("Paragraphe", &trWidget::AddToContent, wstring(1, paragColor[i]));
				}

				// Sleep(max(0, SleepTime_ - GetDeltaTime().GetDataActual()));
			}

			indexChar = i;
			AccumulatedDelayCharToCharAnimation = 0;
		}
	}
}

void trText::APPLY_(const trSize<uint16_t>& SizeWindow)
{
	trWidget::APPLY_(SizeWindow);
}

trText::~trText()
{
	delete AnimationVector;
	delete Content_;
}
