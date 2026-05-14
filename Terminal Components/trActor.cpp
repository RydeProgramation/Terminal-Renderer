#include "trUIToolsCore.h"
#include "trActor.h"

using namespace std;
using namespace UIToolsCore;

REGISTER_TYPE(trActor, std::string)

trActor::trActor() : trActor("None")
{

}

// INI

trActor::trActor(string name_) : Name(new trData<string>(name_)), Activate(new trData<bool>(true)), Protected(new trData<bool>(false)), ToChange(new trData<bool>(true)), ToDestroy(new trData<bool>(false)), DeltaTime(new trData<double>())
{
	
}

// INI deep copy

trActor::trActor(const trActor& other) : Name(new trData<string>(*other.Name)), Activate(new trData<bool>(*other.Activate)), ToChange(new trData<bool>(true)), ToDestroy(new trData<bool>(*other.ToDestroy)), Protected(new trData<bool>(*other.Protected)), DeltaTime(new trData<double>(*other.DeltaTime))
{

}

// Copy

trActor& trActor::operator=(const trActor& other)
{
	return Clone(other);
}

trActor& trActor::Clone(const trActor& other)
{
	if (this == &other) { return *this; }

	if (Name == nullptr) {
		Name = new trData<string>(*other.Name);
	}
	else {
		*Name = *other.Name;
	}

	if (Activate == nullptr) {
		Activate = new trData<bool>(*other.Activate);
	}
	else {
		*Activate = *other.Activate;
	}

	if (Protected == nullptr) {
		Protected = new trData<bool>(*other.Protected);
	}
	else {
		*Protected = *other.Protected;
	}

	if (ToChange == nullptr) {
		ToChange = new trData<bool>(true);
	}
	else {
		*ToChange = *other.ToChange;
	}

	if (ToDestroy == nullptr) {
		ToDestroy = new trData<bool>(*other.ToDestroy);
	}
	else {
		*ToDestroy = *other.ToDestroy;
	}

	if (DeltaTime == nullptr) {
		DeltaTime = new trData<double>();
	}
	else {
		*DeltaTime = *other.DeltaTime;
	}

	return *this;
}

// SET

void trActor::SetName(const string& name_)
{
	Name->SetData(name_);
}

void trActor::SetActivate(bool Activate_)
{
	Activate->SetData(Activate_);
}

void trActor::SetProtecte(bool Protected_)
{
	Protected->SetData(Protected_);
}

void trActor::SetChange(bool Change_)
{
	ToChange->SetData(Change_);
}

void trActor::SetDestroy(bool Destroy_)
{
	ToDestroy->SetData(Destroy_);
}

void trActor::SetDeltaTime(const double& DeltaTime_)
{
	DeltaTime->SetData(DeltaTime_);
}

void trActor::SetProprety(const std::string& name, const std::string& data, const std::string& type)
{
	if (name == "Activate")
	{
		if (type == "bool")
		{
			if (data == "true" || data == "1" || data == "True" || data == "TRUE")
			{
				Activate->SetData(true);
			}
			else if (data == "false" || data == "0" || data == "False" || data == "FALSE")
			{
				Activate->SetData(false);
			}
			else
			{
				MessageBox(
					NULL,
					L"Invalid value for Activate property",
					L"Error",
					MB_ICONERROR | MB_OK
				);
			}
		}
	}

	if (name == "Protected")
	{
		if (type == "bool")
		{
			if (data == "true" || data == "1" || data == "True" || data == "TRUE")
			{
				Protected->SetData(true);
			}
			else if (data == "false" || data == "0" || data == "False" || data == "FALSE")
			{
				Protected->SetData(false);
			}
			else
			{
				MessageBox(
					NULL,
					L"Invalid value for Activate property",
					L"Error",
					MB_ICONERROR | MB_OK
				);
			}
		}
	}

	if (name == "ToChange")
	{
		if (type == "bool")
		{
			if (data == "true" || data == "1" || data == "True" || data == "TRUE")
			{
				ToChange->SetData(true);
			}
			else if (data == "false" || data == "0" || data == "False" || data == "FALSE")
			{
				ToChange->SetData(false);
			}
			else
			{
				MessageBox(
					NULL,
					L"Invalid value for Activate property",
					L"Error",
					MB_ICONERROR | MB_OK
				);
			}
		}
	}

	if (name == "Name")
	{
		if (type == "string")
		{
			Name->SetData(data);
		}
	}

	if (name == "ToDestroy")
	{
		if (type == "bool")
		{
			if (data == "true" || data == "1" || data == "True" || data == "TRUE")
			{
				ToDestroy->SetData(true);
			}
			else if (data == "false" || data == "0" || data == "False" || data == "FALSE")
			{
				ToDestroy->SetData(false);
			}
			else
			{
				MessageBox(
					NULL,
					L"Invalid value for Activate property",
					L"Error",
					MB_ICONERROR | MB_OK
				);
			}
		}
	}
}

void trActor::Destroy()
{
	ToDestroy->SetData(true);
}

// GET

const trData<string>& trActor::GetName() const
{
	return *Name;
}

const trData<bool>& trActor::GetActivate() const
{
	return *Activate;
}

const trData<bool>& trActor::GetProtecte() const
{
	return *Protected;
}

const trData<bool>& trActor::GetChange() const
{
	return *ToChange;
}

const trData<bool>& trActor::GetDestroy() const
{
	return *ToDestroy;
}

const trData<double>& trActor::GetDeltaTime() const 
{
	return *DeltaTime;
}

const bool trActor::IsCreated() const
{
	return Created;
}

// APPLY

void trActor::APPLY(const trSize<uint16_t>& SizeWindow)
{	
	DeltaTime->Update();

	Tick();

	APPLY_(SizeWindow);

	Activate->Update();
	Protected->Update();
	Name->Update();
	ToDestroy->Update();

	ToChange->SetData(VerificationProprety() ? true : ToChange->GetDataNew());
	ToChange->Update();

	if (!Created)
	{
		Created = true;
	}
}

// FNC

bool trActor::VerificationProprety()
{
	return (
		Activate->GetDataOld() != Activate->GetDataActual()
		);
}

// EMPTY ACTOR STATIC

trActor trActor::EmptyActor = trActor();

// DESTRUCTEUR

trActor::~trActor()
{
	delete Activate;

	delete Protected;

	delete ToChange;

	delete Name;

	delete ToDestroy;

	delete DeltaTime;
}
