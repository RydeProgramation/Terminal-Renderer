#include "Header.h"

#include "MyUI.h"
#include "TRexGame.h" // EXAMPLE

#pragma comment(lib, "Terminal Engine.lib") // je crois inutile par ce queje le fait pas pour les autres dll

/*

SI JAMAIS IL Y A UNE ERREUR LNK 2019, c'est qu'il faut initialiser au moins une fois pour que se soit compiler dans Terminal Engine.lib
SI C'EST UN TEMPLATE IL NE FAUT PAS METTRE EN IMPORT/EXPORT CAR IL FAUT INITIAILISER POUR CHAQUE TYPE

*/

using namespace std;
using namespace UITools;

int main(int argc, char* argv[])
{
	///////// EXEMPLE GAME T-REX
	TRexGame Game;
	Game.Start(0, nullptr);
	///////// EXEMPLE GAME T-REX

// 	MyUI UI;
// 
// 	UI.Start(argc, argv);

	while (true) {
		Sleep(10000); // Laisse le CPU tranquille sans bloquer l'UI
	}
}