#pragma once

#include "include.h"
#include "trObject.h"
#include "trActor.h"

#ifndef __TR_OBJECT_FACTORY__
#define __TR_OBJECT_FACTORY__

#ifdef TERMINAL_CORE_EXPORTS
#define TERMINAL_CORE_API __declspec(dllexport)
#else
#define TERMINAL_CORE_API __declspec(dllimport)
#endif

struct trAnObjectBase {
	virtual ~trAnObjectBase() = default;
};

template<typename... Args>
struct trAnObject_ : public trAnObjectBase // Avec argument
{
	using CreateFunc = trObject * (*)(Args&&...);

	CreateFunc createFunc;
	std::type_index typeSignature;

	trAnObject_(CreateFunc func) : createFunc(func), typeSignature(typeid(trAnObject_<Args...>)) {}

	bool matchesSignature(std::type_index otherType) const {
		return otherType == typeSignature;
	}
};

struct trAnObjectDefault_ : public trAnObjectBase // Sans argument
{
	using CreateFunc = trObject * (*)();

	CreateFunc createFunc;
	std::type_index typeSignature;

	trAnObjectDefault_(CreateFunc func) : createFunc(func), typeSignature(typeid(trAnObjectDefault_)) {}

	bool matchesSignature(std::type_index otherType) const {
		return otherType == typeSignature;
	}
};

template<typename... T>
std::string GetTypeNames()
{
	std::vector<std::string> names = { typeid(T).name()... };
	std::string result;
	for (size_t i = 0; i < names.size(); ++i)
	{
		result += names[i];
		if (i != names.size() - 1)
			result += ", ";
	}
	return result;
}

class TERMINAL_CORE_API trObjectFactory
{
public:

	template<typename TypeObject, typename... Args>
	static void RegisterType(const std::string& typeName)
	{
		std::map<std::string, trAnObjectBase*>& objectsType = getobjectsType();
		std::map<std::string, trAnObjectBase*>& objectsTypeDefault = getobjectsTypeDefault();

		objectsType[typeName] = new trAnObject_<Args...>(
			[](Args&&... args) -> trObject* {
				return new TypeObject(std::forward<Args>(args)...);
			}
		);

		objectsTypeDefault[typeName] = new trAnObjectDefault_(
			[]() -> trObject* {
				return new TypeObject();
			}
		);
	}

	template<typename... Args>
	static trObject* Create(const std::string& typeName, Args&&... args)
	{
		std::map<std::string, trAnObjectBase*>& objectsType = getobjectsType();

		auto it = objectsType.find(typeName);

		if (it == objectsType.end())
		{
			std::wstring errTypeName = std::wstring(typeName.begin(), typeName.end());
			MessageBoxW(NULL, (L"TYPE NON ENREGISTRÉ : " + errTypeName).c_str(), L"Erreur", MB_ICONERROR | MB_OK);
			return nullptr;
		}

		trAnObjectBase* base = it->second;

		auto typed = dynamic_cast<trAnObject_<Args...>*>(base);

		if (!typed) {
			std::wstring errTypeName = std::wstring(typeName.begin(), typeName.end());

			// Obtenir les types donnés
			std::string givenTypes = GetTypeNames<Args...>();

			// Obtenir les types attendus
			// std::string expectedTypes = typed->typeSignature.name(); // manglé...

			std::wstring message = L"LES PARAMÈTRES NE CORRESPONDENT PAS POUR " + errTypeName +
				L"\n\nTypes donnés    : " + std::wstring(givenTypes.begin(), givenTypes.end()); /*+
				L"\nTypes attendus : " + std::wstring(expectedTypes.begin(), expectedTypes.end());*/

			MessageBoxW(NULL, message.c_str(), L"Erreur de création", MB_ICONERROR | MB_OK);
			return nullptr;
		}

		return typed->createFunc(std::forward<Args>(args)...);
	}

	static trObject* Create(const std::string& typeName)
	{
		std::map<std::string, trAnObjectBase*>& objectsTypeDefault = getobjectsTypeDefault();

		auto it = objectsTypeDefault.find(typeName);

		if (it == objectsTypeDefault.end())
		{
			std::wstring errTypeName = std::wstring(typeName.begin(), typeName.end());
			MessageBoxW(NULL, (L"TYPE NON ENREGISTRÉ : " + errTypeName).c_str(), L"Erreur", MB_ICONERROR | MB_OK);
			return nullptr;
		}

		trAnObjectBase* base = it->second;

		auto typed = dynamic_cast<trAnObjectDefault_*>(base);

		return typed->createFunc();
	}

	/*template<typename... Args>
	static trObject* Create(const std::string& typeName, Args&&... args)
	{
		std::map<std::string, trAnObjectBase*>& objectsType = getobjectsType();

		auto it = objectsType.find(typeName);
		if (it == objectsType.end()) return nullptr;

		trAnObjectBase* base = it->second;

		auto typed = dynamic_cast<trAnObject_<Args...>*>(base);
		if (!typed) {
			// tentative pour afficher une erreur claire
			// tu peux essayer de caster vers trAnObject_<> générique ou juste imprimer un message d'erreur

			std::wstring errTypeName = std::wstring(typeName.begin(), typeName.end());

			MessageBoxW(
				NULL,
				(L"LES PARAMÈTRES NE CORRESPONDENT PAS : " + errTypeName).c_str(),
				L"Message",
				MB_ICONERROR | MB_OK
			);
			return nullptr;
		}

		return typed->createFunc(std::forward<Args>(args)...);
	}*/

	static std::map<std::string, trAnObjectBase*>& getobjectsType()
	{
		static std::map<std::string, trAnObjectBase*> *objectsType;

		if (!objectsType) {
			objectsType = new std::map<std::string, trAnObjectBase*>;
		}

		return *objectsType;
	}

	static std::map<std::string, trAnObjectBase*>& getobjectsTypeDefault()
	{
		static std::map<std::string, trAnObjectBase*>* objectsType;

		if (!objectsType) {
			objectsType = new std::map<std::string, trAnObjectBase*>;
		}

		return *objectsType;
	}

private:
	// static std::map<std::string, trAnObjectBase*> *objectsType;
};

#endif

#define REGISTER_TYPE(Type, ...)                  \
struct AutoRegister##Type {                        \
    AutoRegister##Type() {                         \
        trObjectFactory::RegisterType<Type, __VA_ARGS__>(#Type); \
    }                                              \
};                                                 \
static AutoRegister##Type globalAutoRegister##Type;

#define trCREATE(Type, ...) \
    dynamic_cast<Type*>(trObjectFactory::Create(#Type, __VA_ARGS__))

#define trCREATEDefault(Type) \
    dynamic_cast<Type*>(trObjectFactory::Create(#Type))