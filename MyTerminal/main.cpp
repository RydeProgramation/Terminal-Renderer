#include "Header.h"

#include "MyUI.h"

#pragma comment(lib, "Terminal Engine.lib") // je crois inutile par ce queje le fait pas pour les autres dll

/*

SI JAMAIS IL Y A UNE ERREUR LNK 2019, c'est qu'il faut initialiser au moins une fois pour que se soit compiler dans Terminal Engine.lib
SI C'EST UN TEMPLATE IL NE FAUT PAS METTRE EN IMPORT/EXPORT CAR IL FAUT INITIAILISER POUR CHAQUE TYPE

*/

using namespace std;
using namespace UITools;

std::string WStringToUTF8(const std::wstring& wstr) {
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
	std::string result(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &result[0], size_needed, nullptr, nullptr);
	result.pop_back(); // Supprime le \0 inutile à la fin
	return result;
}

int main(int argc, char* argv[])
{
	MyUI UI;

	// TEST

	// Concaténation de 2 chemins
	std::string chemin1 = "C:\\Users\\Mohamed\\Documents\\Work\\Personnel\\code\\C++\\visual studio ( écrit avec code )\\Terminal-UI\\x64\\Debug\\Terminal UI.exe";
	std::string chemin2 = "C:\\Users\\Mohamed\\Documents\\Work\\Personnel\\Diffusion.widg";

	// Créer des buffers dynamiques
	char* arg1 = new char[chemin1.size() + 1];
	char* arg2 = new char[chemin2.size() + 1];

	// Copier les chaînes dedans
	strcpy_s(arg1, chemin1.size() + 1, chemin1.c_str());
	strcpy_s(arg2, chemin2.size() + 1, chemin2.c_str());

	// Créer un tableau de pointeurs vers char
	char* mychar[] = { arg1, arg2, nullptr };

	int deux = 2;

	cout << deux << endl;

	for (int i = 0; i < deux; i++) {
		cout << mychar[i];
	}

	UI.Start(argc, argv);

	UI.Debut();

	while (true) {
		Sleep(10000); // Laisse le CPU tranquille sans bloquer l'UI
	}
}

/*

  __  __                   _                ____                  _
 |  \/  |   ___    _ __   | |_    ___      / ___|   __ _   _ __  | |   ___
 | |\/| |  / _ \  | '_ \  | __|  / _ \    | |      / _` | | '__| | |  / _ \
 | |  | | | (_) | | | | | | |_  |  __/    | |___  | (_| | | |    | | | (_) |
 |_|  |_|  \___/  |_| |_|  \__|  \___|     \____|  \__,_| |_|    |_|  \___/

  ____    _    __    __                 _                    ____
 |  _ \  (_)  / _|  / _|  _   _   ___  (_)   ___    _ __    / ___|  _   _   _ __  __   __   ___
 | | | | | | | |_  | |_  | | | | / __| | |  / _ \  | '_ \  | |     | | | | | '__| \ \ / /  / _ \
 | |_| | | | |  _| |  _| | |_| | \__ \ | | | (_) | | | | | | |___  | |_| | | |     \ V /  |  __/
 |____/  |_| |_|   |_|    \__,_| |___/ |_|  \___/  |_| |_|  \____|  \__,_| |_|      \_/    \___|

 */
