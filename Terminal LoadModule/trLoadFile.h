#pragma once

#include "include.h"

#include "trSize.h"
#include "trCoordinate.h"
#include "trText.h"
#include "trSelector.h"
#include "trWidget.h"
#include "trData.h"
#include "trUIToolsCore.h"
#include <msxml6.h>

#pragma comment(lib, "msxml6.lib")

#ifndef __TR_LOADFILE__
#define __TR_LOADFILE__

#ifdef TERMINAL_LOAD_MODULE_EXPORTS
#define TERMINAL_LOAD_MODULE_API __declspec(dllexport)
#else
#define TERMINAL_LOAD_MODULE_API __declspec(dllimport)
#endif

namespace trLoad
{
	TERMINAL_LOAD_MODULE_API bool FileExists(LPCWSTR filename);
	TERMINAL_LOAD_MODULE_API trObject* LoadObject(LPCWSTR NameFile_);
};

#endif

#define LOAD(Type, Name, path) \
	Type* Name = dynamic_cast<Type*>(trLoad::LoadObject(path)); 

#define RELOAD(Type, Name, path) \
	Type* Temp = dynamic_cast<Type*>(trLoad::LoadObject(path)); \
	*Name = *Temp; \
	delete Temp; 