#include "trUIToolsCore.h"
#include "trPawn.h"

using namespace std;
using namespace UIToolsCore;

REGISTER_TYPE(trPawn, int, int, int, string)

// INI default

trPawn::trPawn() : trPawn(0, 0, TopLeft, "None")
{

}

// INI

trPawn::trPawn(int x_, int y_, int RelativePositionType_, string name_) : trActor(name_), PositionRelative(new trCoordinate<int>(x_, y_)), RpType(new trData<uint8_t>(RelativePositionType_)), RelativePositionPoint(new trCoordinate<int>(0, 0)), PawnCollision(new std::vector<trActor*>()), /*TEST DE COPILOT*/ PositionAbsolue(new trCoordinate<int>(max(x_ + RelativePositionPoint->GetX().GetDataActual(), 0), max(y_ + RelativePositionPoint->GetY().GetDataActual(), 0)))
{

}

void trPawn::Init()
{
	UpdateRelativePositionPoint(GetConsoleSize());

	RelativePositionPoint->Update();

	PositionAbsolue = new trCoordinate<int>(max(PositionRelative->GetX().GetDataNew() + RelativePositionPoint->GetX().GetDataActual(), 0), max(PositionRelative->GetY().GetDataNew() + RelativePositionPoint->GetY().GetDataActual(), 0));
}

// INI deep copy

trPawn::trPawn(const trPawn& other) : trActor(other), PositionRelative(new trCoordinate<int>(*other.PositionRelative)), RpType(new trData<uint8_t>(*other.RpType)), RelativePositionPoint(new trCoordinate<int>(*other.RelativePositionPoint)), PositionAbsolue(new trCoordinate<int>(*other.PositionAbsolue)), PawnCollision(new vector<trActor*>(*other.PawnCollision))
{

}

// Copy

trPawn& trPawn::operator=(const trActor& other_)
{
	return Clone(other_);
}

trPawn& trPawn::Clone(const trActor& other_)
{
	try
	{
		const trPawn& other = dynamic_cast<const trPawn&>(other_);

		if (this == &other) { return *this; }

		trActor::Clone(other_);

		if (RpType == nullptr) {
			RpType = new trData<uint8_t>(*other.RpType);
		}
		else {
			*RpType = *other.RpType;
		}

		if (PositionRelative == nullptr) {
			PositionRelative = new trCoordinate<int>(*other.PositionRelative);
		}
		else {
			*PositionRelative = *other.PositionRelative;
		}

		if (RelativePositionPoint == nullptr) {
			RelativePositionPoint = new trCoordinate<int>(*other.RelativePositionPoint);
		}
		else {
			*RelativePositionPoint = *other.RelativePositionPoint;
		}

		if (PositionAbsolue == nullptr) {
			PositionAbsolue = new trCoordinate<int>(*other.PositionAbsolue);
		}
		else {
			*PositionAbsolue = *other.PositionAbsolue;
		}

		if (PawnCollision == nullptr) {
			PawnCollision = new vector<trActor*>(*other.PawnCollision);
		}
		else {
			*PawnCollision = *other.PawnCollision;
		}

		return *this;
	}

	catch (const std::bad_cast&)
	{
		trActor& Me = dynamic_cast<trActor&>(*this);

		Me.trActor::Clone(other_);

		return *this;
	}
}

// SET

void trPawn::SetPosition(int x_, int y_)
{
	PositionRelative->SetCoord(x_, y_);
}

bool trPawn::SetTypeRelativePosition(int rp)
{
	if (rp < 0 || rp > 9)
	{
		RpType->SetData(0);
		return false;
	}

	RpType->SetData(rp);
	return true;
}

void trPawn::SetPawnCollision(trActor* actor, bool collision)
{
	if (actor != nullptr)
	{
		if (collision)
		{
			PawnCollision->push_back(actor); // faire belek ici
		}

		else
		{
			// bah je sais pas ce que tu fait mdr
		}
	}
}

// ADD

void trPawn::AddToPosition(int x_, int y_)
{
	PositionRelative->SetCoord(PositionRelative->GetX().GetDataNew() + x_, PositionRelative->GetY().GetDataNew() + y_);
}

// GET

const trCoordinate<int>& trPawn::GetPosition() const
{
	return *PositionRelative;
}

const trCoordinate<int>& trPawn::GetAbsolutePosition() const
{
	return *PositionAbsolue;
}

const trData<uint8_t>& trPawn::GetRelativePositionType() const
{
	return *RpType;
}

const vector<trActor*>& trPawn::GetPawnCollision() const
{
	return *PawnCollision;
}

void trPawn::SetProprety(const std::string& name, const std::string& data, const std::string& type)
{
	trActor::SetProprety(name, data, type);

	if (name == "PositionRelative")
	{
		MessageBox(
			NULL,
			L"Invalid type for PositionRelative property | a voir plus tard pour changer ça",
			L"Error",
			MB_ICONERROR | MB_OK
		);

		if (type == "int")
		{
			/*int x, y;
			sscanf(data.c_str(), "%d,%d", &x, &y);
			SetPosition(x, y);*/
		}

		else
		{
			MessageBox(
				NULL,
				L"Invalid type for PositionRelative property",
				L"Error",
				MB_ICONERROR | MB_OK
			);
		}
	}

	if (name == "RelativePositionType")
	{
		MessageBox(
			NULL,
			L"Invalid type for PositionRelative property",
			L"Error",
			MB_ICONERROR | MB_OK
		);

		if (type == "int")
		{
			/*int rp;

			// sscanf(data.c_str(), "%d", &rp);

			SetTypeRelativePosition(rp);*/
		}

		else
		{
			MessageBox(
				NULL,
				L"Invalid type for RelativePositionType property",
				L"Error",
				MB_ICONERROR | MB_OK
			);
		}
	}

	if (name == "PositionAbsolue")
	{
		MessageBox(
			NULL,
			L"Invalid type for PositionRelative property",
			L"Error",
			MB_ICONERROR | MB_OK
		);

		if (type == "int")
		{
			/*int x, y;
			// sscanf(data.c_str(), "%d,%d", &x, &y);
			SetPosition(x, y);*/
		}

		else
		{
			MessageBox(
				NULL,
				L"Invalid type for PositionAbsolue property",
				L"Error",
				MB_ICONERROR | MB_OK
			);
		}
	}

	if (name == "RpType" || name == "RelativePositionType")
	{
		MessageBox(
			NULL,
			L"Invalid type for PositionRelative property",
			L"Error",
			MB_ICONERROR | MB_OK
		);

		if (type == "int")
		{
			/*int rp;
			// sscanf(data.c_str(), "%d", &rp);
			SetTypeRelativePosition(rp);*/
		}

		else
		{
			MessageBox(
				NULL,
				L"Invalid type for RpType property",
				L"Error",
				MB_ICONERROR | MB_OK
			);
		}
	}
}

// APPLY

void trPawn::UpdateRelativePosition()
{
	PositionAbsolue->SetCoord(max(PositionRelative->GetX().GetDataNew() + RelativePositionPoint->GetX().GetDataActual(), 0), max(PositionRelative->GetY().GetDataNew() + RelativePositionPoint->GetY().GetDataActual(), 0));
}

void trPawn::APPLY_(const trSize<uint16_t>& SizeWindow)
{
	RpType->Update();

	UpdateRelativePositionPoint(SizeWindow);

	RelativePositionPoint->Update();

	UpdateRelativePosition();

	PositionAbsolue->Update();
	PositionRelative->Update();

	if (PawnCollision->size() > 0)
	{
		/*for (auto& it : *PawnCollision) // a utiliser si jamais mais bon pas besoin pour l'instant
		{
			
		}*/

		SetChange(true); // on met a jour les collisions

		PawnCollision->clear();
	}
}

// FNC

bool trPawn::VerificationProprety()
{
	return (
		trActor::VerificationProprety() ||
		PositionRelative->GetX().GetDataOld() != PositionRelative->GetX().GetDataActual() ||
		PositionRelative->GetY().GetDataOld() != PositionRelative->GetY().GetDataActual() ||
		RpType->GetDataOld() != RpType->GetDataActual()
		);
}

void trPawn::UpdateRelativePositionPoint(const trSize<uint16_t>& SizeWindow)
{
	int ConsoleSize_x = GetConsoleSize().GetSizeX().GetDataActual();
	int ConsoleSize_y = GetConsoleSize().GetSizeY().GetDataActual();

	switch (RpType->GetDataActual())
	{
	case TopLeft:
		RelativePositionPoint->SetCoord(0, 0);
		break;
	case TopCenter:
		RelativePositionPoint->SetCoord(ConsoleSize_x / 2, 0);
		break;
	case TopRight:
		RelativePositionPoint->SetCoord(ConsoleSize_x, 0);
		break;
	case MiddleLeft:
		RelativePositionPoint->SetCoord(0, ConsoleSize_y / 2);
		break;
	case MiddleCenter:
		RelativePositionPoint->SetCoord(ConsoleSize_x / 2, ConsoleSize_y / 2);
		break;
	case MiddleRight:
		RelativePositionPoint->SetCoord(ConsoleSize_x, ConsoleSize_y / 2);
		break;
	case BottomLeft:
		RelativePositionPoint->SetCoord(0, ConsoleSize_y);
		break;
	case BottomCenter:
		RelativePositionPoint->SetCoord(ConsoleSize_x / 2, ConsoleSize_y);
		break;
	case BottomRight:
		RelativePositionPoint->SetCoord(ConsoleSize_x, ConsoleSize_y);
		break;
	default:
		std::cerr << "AUCUN NE CORESPOND";
		std::exit(1);
	}
}

// DESTRUCTEUR

trPawn::~trPawn()
{
	delete PositionRelative;

	delete RelativePositionPoint;

	delete PositionAbsolue;

	delete RpType;

	delete PawnCollision;
}