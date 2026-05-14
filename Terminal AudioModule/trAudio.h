#pragma once

#include "include.h"

#include "trObject.h"

#ifndef __TR_AUDIO__
#define __TR_AUDIO__

#ifdef TERMINAL_AUDIO_MODULE_EXPORTS
#define TERMINAL_AUDIO_API __declspec(dllexport)
#else
#define TERMINAL_AUDIO_API __declspec(dllimport)
#endif

struct TERMINAL_AUDIO_API trAudio : trObject
{
public:

	// INI default

	trAudio()
	{
	}

	// INI

	

	// INI deep copy

	

	// Copy

	

	// SET

	

	// GET

	

	// APPLY

	

	// DESTRUCTEUR

	~trAudio()
	{
	}

private:

	
};

#endif