#pragma once

#include "include.h"

#include "trSize.h"
#include "trCoordinate.h"
#include "trWidget.h"
#include "trData.h"
#include <msxml6.h>

#pragma comment(lib, "msxml6.lib")

#ifndef __TR_SAVEFILE__
#define __TR_SAVEFILE__

#ifdef TERMINAL_LOAD_MODULE_EXPORTS
#define TERMINAL_LOAD_MODULE_API __declspec(dllexport)
#else
#define TERMINAL_LOAD_MODULE_API __declspec(dllimport)
#endif

namespace trSave
{
	void TERMINAL_LOAD_MODULE_API SaveObject();
};

#endif