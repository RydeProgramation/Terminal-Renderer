#include "trPrint.h"

using namespace UITools;

// FLOP

void trPrintModule::trPrint(const std::string& ToPrint_)
{
	short x = GetConsoleSize().GetSizeX().GetDataActual();
	short y = GetConsoleSize().GetSizeY().GetDataActual();

	CHAR_INFO* buffer = new CHAR_INFO[x * y];

	for (size_t i = 0; i < x * y; ++i)
	{
		buffer[i].Char.UnicodeChar = ToPrint_[min(i, ToPrint_.size())];
		buffer[i].Attributes = 1; // il faut ecrire sur les bits différents
	}

	SMALL_RECT RegionEcriture = { 0, 0, x - 1, y - 1 };

	WriteConsoleOutputW(
		GetStdHandle(STD_OUTPUT_HANDLE),
		buffer,
		{ x, y },
		{0, 0},
		&RegionEcriture
	);

	delete[] buffer; // putain ne vraiment pas oublier de delete le buffer en 1 seconde j'étais a 2 go de ram
}
