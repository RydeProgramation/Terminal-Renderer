#include "trUIToolsCore.h"

using namespace std;

// UI tool CORE

trSize<uint16_t> UIToolsCore::GetConsoleSize(uint8_t BorderWitdh)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	GetConsoleScreenBufferInfo(hConsole, &csbi);

	uint16_t Conlumns = csbi.srWindow.Right - csbi.srWindow.Left + 1 - (BorderWitdh * 2) * 2;
	uint16_t Rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1 - (BorderWitdh * 2);

	return trSize<uint16_t>(Conlumns, Rows);
}

trSize<uint16_t> UIToolsCore::GetConsoleSize()
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	GetConsoleScreenBufferInfo(hConsole, &csbi);

	uint16_t Conlumns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	uint16_t Rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

	return trSize<uint16_t>(Conlumns, Rows);
}

bool UIToolsCore::IsMyWindowInFocus() // ne fonctionne pas si la console n'est pas au premier plan
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	// CONSOLE_SCREEN_BUFFER_INFO csbi; bzr

	// return GetConsoleWindow() == GetForegroundWindow();

	// boite de dialogue d'erreur pour arreter l'appli si cette fonction est utilisé

	MessageBox(NULL, L"La fonction ne fonctionne jamais", L"Erreur", MB_OK | MB_ICONERROR);
	return false;
}

std::string UIToolsCore::WstringToUtf8(const std::wstring& wstr)
{
	// Calculer la taille nécessaire pour la chaîne UTF-8
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);

	// Convertir la chaîne wchar_t en UTF-8
	std::string utf8_str(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &utf8_str[0], size_needed, NULL, NULL);

	return utf8_str;
}

void UIToolsCore::SetColorConsole(int color)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	SetConsoleTextAttribute(hConsole, color);
}

bool UIToolsCore::IsPureColor(const std::wstring& input) // code chatGPT
{
	size_t firstPos = input.find(L"\x1b"); // ou \033 c'est la meme

	if (firstPos == std::wstring::npos)
	{
		// Pas de séquence \x1b dans la chaîne
		return false;
	}

	// Vérifie s'il y a une autre occurrence de \x1b après la première
	size_t secondPos = input.find(L"\x1b", firstPos + 1);

	if (secondPos != std::wstring::npos)
	{
		// Plus d'une séquence \x1b trouvée
		return false;
	}

	// Vérifie que la séquence ANSI commence par \x1b[ et se termine par 'm'
	return bool(input.find(L"\x1b[") == firstPos && input.find(L"m") == input.size() - 1);
}

std::wstring UIToolsCore::RemoveColor(const std::wstring& input)
{
	std::wstring result = input;
	size_t pos = 0;

	while ((pos = result.find(L"\x1B[", pos)) != std::wstring::npos)
	{
		// Trouver la fin de la séquence ansi, qui termine par 'm'
		size_t endPos = result.find(L'm', pos + 2);
		if (endPos == std::wstring::npos)
			break;  // Séquence incomplète, on stop

		// Effacer la séquence de pos à endPos inclus
		result.erase(pos, endPos - pos + 1);
		// Ne pas avancer pos, car on vient de modifier la string
	}

	return result;
}

inline void UIToolsCore::FastToWString(int value, std::wstring& out)
{
	out.clear();

	if (value < 0) {
		out += L'-';
		value = -value;
	}

	wchar_t temp[20];
	size_t i = 0;

	do {
		temp[i++] = L'0' + (value % 10);
		value /= 10;
	} while (value != 0);

	while (i-- > 0) {
		out += temp[i];
	}
}

void UIToolsCore::substrAnsiSafeUltraFast(
	const std::wstring& input,
	size_t startVisible,
	size_t countVisible,
	std::wstring& result
) {
	// Large estimation pour ne jamais dépasser
	const size_t estimatedSize = input.size() - startVisible + 128;

	result.clear();
	result.resize(estimatedSize, L'\0');

	size_t writeIndex = 0;
	bool inAnsi = false;
	size_t visibleCount = 0;
	wchar_t c;


	for (size_t i = 0; i < input.size(); ++i) {
		c = input[i];

		if (c == L'\033') {
			inAnsi = true;
		}

		if (inAnsi) {
			if (writeIndex < result.size())
				result[writeIndex++] = c;

			if ((c >= L'A' && c <= L'Z') || (c >= L'a' && c <= L'z')) {
				inAnsi = false;
			}
		}
		else {
			if (visibleCount >= startVisible && visibleCount < startVisible + countVisible) {
				if (writeIndex < result.size())
					result[writeIndex++] = c;
			}
			++visibleCount;

			if (visibleCount >= startVisible + countVisible)
				break;
		}
	}

	result.resize(writeIndex);
}

bool UIToolsCore::IsConsoleFocused() /*Ne fonctionne pas*/ {
	HWND consoleWindow = GetConsoleWindow();
	HWND foregroundWindow = GetForegroundWindow();

	return consoleWindow != nullptr && consoleWindow == foregroundWindow;
}

std::string UIToolsCore::getArgFileWithExt(int argc, char* argv[], const std::string& ext) {
	if (argc > 1) {
		std::string filePath = argv[1];
		std::filesystem::path path(filePath);

		if (path.has_extension()) {
			std::string fileExt = path.extension().string();
			// Gère ext = "ia" ou ".ia"
			if (fileExt == ext || fileExt == "." + ext) {
				return filePath;
			}
		}
	}
	return "";
}

TERMINAL_CORE_API std::wstring UIToolsCore::stringToWstring(const std::string& str)
{
	if (str.empty()) return std::wstring();

	// Utiliser CP_ACP (Windows ANSI Code Page)
	int size_needed = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, nullptr, 0);
	if (size_needed == 0) return std::wstring(); // erreur

	std::wstring wstr(size_needed, 0);
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, &wstr[0], size_needed);

	wstr.resize(size_needed - 1); // enlever '\0'
	return wstr;
}
