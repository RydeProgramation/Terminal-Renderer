#include "trLoadFile.h"

// FNC

using namespace std;

bool trLoad::FileExists(LPCWSTR filename) {
	DWORD attrib = GetFileAttributesW(filename);
	return (attrib != INVALID_FILE_ATTRIBUTES && !(attrib & FILE_ATTRIBUTE_DIRECTORY));
}

std::string WStringToUTF8(const std::wstring& wstr) {
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
	std::string result(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &result[0], size_needed, nullptr, nullptr);
	result.pop_back(); // Supprime le \0 inutile à la fin
	return result;
}

inline void CaseColorProcess(unordered_map<trPair<int, int>, std::wstring>& CaseColor, trWidget& WidgetLoad, wstring& Content_ID)
{
	wstring raw = Content_ID;

	struct Event {
		int pos;
		bool isStart;
		wstring escape;
		int priority; // pour départager les styles imbriqués
	};

	std::vector<Event> events;
	int priority = 0;

	int PosStart = 0;
	int PosEnd = 0;

	// 🔁 Supprime les échappements ANSI existants dans Content_ID (et les ajoute aux events)
	size_t pos = 0;

	while ((pos = Content_ID.find(L'\x1b', pos)) != std::wstring::npos) {
		size_t end = Content_ID.find(L'm', pos);
		if (end == std::wstring::npos) break;

		size_t length = end - pos + 1;
		std::wstring escapeCode = Content_ID.substr(pos, length);

		// ✅ Ajoute un événement de type "start" pour simuler qu’on ferme cette couleur
		events.push_back({ (int)pos, true, escapeCode, priority });

		// 🔧 Supprime l’ANSI du texte
		Content_ID.erase(pos, length);

		priority++;

		// Pas besoin de remettre pos à zéro car on a supprimé du texte à pos,
		// donc le prochain caractère est déjà à la bonne position
	}

	// 🔁 On copie les paires dans un vector pour pouvoir les parcourir à l’envers
	std::vector<std::pair<trPair<int, int>, std::wstring>> reversedCases(CaseColor.begin(), CaseColor.end());

	for (int i = static_cast<int>(reversedCases.size()) - 1; i >= 0; --i)
	{
		const auto& TheCase = reversedCases[i];
		int start = TheCase.first.GetFirst();
		int end = TheCase.first.GetSecond();
		const std::wstring& esc = TheCase.second;

		events.push_back({ start, true,  esc, priority });
		events.push_back({ end,   false, esc, priority });

		++priority;
	}++priority;

	// 🔃 Tri des événements par position puis priorité (inverse pour fin)
	std::sort(events.begin(), events.end(), [](const Event& a, const Event& b) {
		if (a.pos != b.pos) return a.pos < b.pos;
		if (a.isStart != b.isStart) return a.isStart > b.isStart; // commence avant de finir
		return a.priority < b.priority; // plus profond en dernier
		});

	std::map<int, std::wstring> inserts;
	std::vector<std::wstring> styleStack;

	// 🧠 Simulation de la pile
	for (const auto& e : events)
	{
		if (e.isStart)
		{
			// 🔼 Empile la couleur
			styleStack.push_back(e.escape);
			inserts[e.pos] += /*WidgetLoad.GetBackroundColor().GetDataNew() + WidgetLoad.GetColor().GetDataNew() +*/ e.escape;
		}
		else
		{
			// 🔽 Dépile la couleur correspondante
			auto it = std::find(styleStack.rbegin(), styleStack.rend(), e.escape);
			if (it != styleStack.rend())
			{
				styleStack.erase(std::next(it).base());
			}

			// ✨ Insère la couleur précédente si elle existe, sinon reset
			if (!styleStack.empty())
				inserts[e.pos] += WidgetLoad.GetBackroundColor().GetDataNew() + WidgetLoad.GetColor().GetDataNew() + styleStack.back();
			else
				inserts[e.pos] += WidgetLoad.GetBackroundColor().GetDataNew() + WidgetLoad.GetColor().GetDataNew();
		}
	}

	// 📝 Insertion dans le texte
	std::wstring text = Content_ID.empty() ? WidgetLoad.GetRawContent().GetDataNew() : Content_ID;

	for (auto it = inserts.rbegin(); it != inserts.rend(); ++it)
	{
		text.insert(min(it->first, text.size()), it->second);
	}

	Content_ID = text;
}

trObject* trLoad::LoadObject(LPCWSTR NameFile_)
{
	XmlNodeType nodeType;

	IXmlReader* reader = nullptr;
	CreateXmlReader(__uuidof(IXmlReader), (void**)&reader, nullptr);

	IStream* fileStream = nullptr;
	SHCreateStreamOnFile(NameFile_, STGM_READ, &fileStream);
	reader->SetInput(fileStream);

	LPCWSTR ElementName = nullptr;
	LPCWSTR Text = nullptr;
	LPCWSTR Attribute = nullptr;

	trObject* ObjectLoad = nullptr;

	std::string name_;
	std::string type_;
	std::string data_;
	std::wstring wdata_;

	bool isPropreties = false;
	bool isContent = false;
	bool isCaseColor = false;
	bool isAnimation = false;
	bool isRawFrame = false;
	bool isFrameAdd = false;
	bool isOldContent = false;
	bool DoColor = false;
	bool onLastFrame = false;

	wstring OldContent_ = L"";

	trPawn* PawnLoad = nullptr;
	trWidget* WidgetLoad = nullptr;
	trText* TextLoad = nullptr;
	trSelector* SelectorLoad = nullptr;

	vector<trPair<uint16_t, std::wstring>> AnimationVector;

	int NumberFrame = 0;
	int TimeFrame = 0;
	int indexFirst = 0;
	int indexLast = 0;

	wstring Content_ID = L"";

	std::vector<std::wstring> Content_V;

	unordered_map<trPair<int, int>, std::wstring> CaseColor;

	wstring CurrentElementBalise = L"";

	if (!FileExists(NameFile_)) {
		// Construction du message d'erreur
		std::wstring msg = L"Le fichier suivant est introuvable :\n\n";
		msg += NameFile_;

		// Affichage dans une boîte de dialogue
		MessageBoxW(NULL, msg.c_str(), L"Erreur ❌", MB_ICONERROR | MB_OK);

		// Fermeture de l'application avec un code d'erreur
		ExitProcess(1);
	}

	while (reader->Read(&nodeType) == S_OK)
	{
		if (nodeType == XmlNodeType_Element)
		{
			reader->GetLocalName(&ElementName, nullptr);

			CurrentElementBalise = ElementName; // ptete pas utile mais bon

			if (wcscmp(ElementName, L"trObject") == 0)
			{
				// Lire les attributs si présents
				if (reader->MoveToFirstAttribute() == S_OK)
				{
					do
					{
						LPCWSTR attrName = nullptr;
						LPCWSTR attrValue = nullptr;

						reader->GetLocalName(&attrName, nullptr);
						reader->GetValue(&attrValue, nullptr);

						if (wcscmp(attrName, L"type") == 0)
						{
							std::wstring typeStr = attrValue;
							std::string typeStr_ = std::string(typeStr.begin(), typeStr.end());

							ObjectLoad = trObjectFactory::Create(typeStr_);
						}

						if (wcscmp(attrName, L"name") == 0)
						{
							std::wstring typeStr = attrValue;
							std::string typeStr_ = std::string(typeStr.begin(), typeStr.end());

							trActor* pawn = dynamic_cast<trActor*>(ObjectLoad);

							if (pawn)
							{
								pawn->SetName(typeStr_);
							}
						}

					} while (reader->MoveToNextAttribute() == S_OK);
				}
			}

			if (wcscmp(ElementName, L"Properties") == 0)
			{
				isPropreties = true;
			}

			if (wcscmp(ElementName, L"trPawn") == 0)
			{
				PawnLoad = dynamic_cast<trPawn*>(ObjectLoad);

				if (!PawnLoad)
				{
					MessageBox(
						NULL,
						L"trPawn not found",
						L"Error",
						MB_ICONERROR | MB_OK
					);
					return nullptr;
				}
			}

			if (wcscmp(ElementName, L"Position") == 0 && PawnLoad)
			{
				// Lire les attributs si présents
				if (reader->MoveToFirstAttribute() == S_OK)
				{
					int x = 0;
					int y = 0;

					do
					{
						LPCWSTR attrName = nullptr;
						LPCWSTR attrValue = nullptr;

						reader->GetLocalName(&attrName, nullptr);
						reader->GetValue(&attrValue, nullptr);

						if (wcscmp(attrName, L"x") == 0)
						{
							std::wstring typeStr = attrValue;
							x = std::stoi(std::string(typeStr.begin(), typeStr.end()));
						}

						if (wcscmp(attrName, L"y") == 0)
						{
							std::wstring typeStr = attrValue;
							y = std::stoi(std::string(typeStr.begin(), typeStr.end()));
						}

					} while (reader->MoveToNextAttribute() == S_OK);

					PawnLoad->SetPosition(x, y);
				}
			}

			if (wcscmp(ElementName, L"Size") == 0 && WidgetLoad)
			{
				// Lire les attributs si présents
				if (reader->MoveToFirstAttribute() == S_OK)
				{
					int height = 0;
					int width = 0;

					do
					{
						LPCWSTR attrName = nullptr;
						LPCWSTR attrValue = nullptr;

						reader->GetLocalName(&attrName, nullptr);
						reader->GetValue(&attrValue, nullptr);

						if (wcscmp(attrName, L"height") == 0)
						{
							std::wstring typeStr = attrValue;
							height = std::stoi(std::string(typeStr.begin(), typeStr.end()));
						}

						if (wcscmp(attrName, L"width") == 0)
						{
							std::wstring typeStr = attrValue;
							width = std::stoi(std::string(typeStr.begin(), typeStr.end()));
						}

					} while (reader->MoveToNextAttribute() == S_OK);

					WidgetLoad->SetSize(width, height);
				}
			}

			if (wcscmp(ElementName, L"RelativePositionType") == 0 && PawnLoad)
			{
				// Lire les attributs si présents
				if (reader->MoveToFirstAttribute() == S_OK)
				{
					std::string RpType_S = "";

					do
					{
						LPCWSTR attrName = nullptr;
						LPCWSTR attrValue = nullptr;

						reader->GetLocalName(&attrName, nullptr);
						reader->GetValue(&attrValue, nullptr);

						if (wcscmp(attrName, L"RpType") == 0)
						{
							std::wstring typeStr = attrValue;
							RpType_S = std::string(typeStr.begin(), typeStr.end());
						}

					} while (reader->MoveToNextAttribute() == S_OK);

					if (RpType_S == "TopLeft")
					{
						PawnLoad->SetTypeRelativePosition(TopLeft);
					}

					if (RpType_S == "TopCenter")
					{
						PawnLoad->SetTypeRelativePosition(TopCenter);
					}

					if (RpType_S == "TopRight")
					{
						PawnLoad->SetTypeRelativePosition(TopRight);
					}

					if (RpType_S == "MiddleLeft")
					{
						PawnLoad->SetTypeRelativePosition(MiddleLeft);
					}

					if (RpType_S == "MiddleCenter")
					{
						PawnLoad->SetTypeRelativePosition(MiddleCenter);
					}

					if (RpType_S == "MiddleRight")
					{
						PawnLoad->SetTypeRelativePosition(MiddleRight);
					}

					if (RpType_S == "BottomLeft")
					{
						PawnLoad->SetTypeRelativePosition(BottomLeft);
					}

					if (RpType_S == "BottomCenter")
					{
						PawnLoad->SetTypeRelativePosition(BottomCenter);
					}

					if (RpType_S == "BottomRight")
					{
						PawnLoad->SetTypeRelativePosition(BottomRight);
					}
				}
			}

			if (wcscmp(ElementName, L"trWidget") == 0)
			{
				WidgetLoad = dynamic_cast<trWidget*>(ObjectLoad);

				if (!WidgetLoad)
				{
					MessageBox(
						NULL,
						L"trPawn not found",
						L"Error",
						MB_ICONERROR | MB_OK
					);
					return nullptr;
				}
			}

			if (wcscmp(ElementName, L"Content") == 0 && WidgetLoad)
			{
				isContent = true;
				Content_ID = L"";
				Content_V.clear();
			}

			if (wcscmp(ElementName, L"Content") == 0 && isRawFrame)
			{
				isContent = true;
				Content_ID = L"";
				Content_V.clear();
			}

			if (wcscmp(ElementName, L"CaseColor") == 0 && WidgetLoad)
			{
				isCaseColor = true;
				CaseColor.clear();
			}

			if (wcscmp(ElementName, L"CaseColor") == 0 && isRawFrame)
			{
				CaseColor.clear();
				isCaseColor = true;
			}

			if (wcscmp(ElementName, L"CaseColor") == 0 && TextLoad && isOldContent)
			{
				CaseColor.clear();
				isCaseColor = false;
			}

			if (wcscmp(ElementName, L"Color") == 0 && WidgetLoad)
			{
				// Lire les attributs si présents
				if (reader->MoveToFirstAttribute() == S_OK)
				{
					wstring foreground_ = L"";
					wstring background_ = L"";
					uint8_t R = 0, G = 0, B = 0;

					do
					{
						LPCWSTR attrName = nullptr;
						LPCWSTR attrValue = nullptr;

						reader->GetLocalName(&attrName, nullptr);
						reader->GetValue(&attrValue, nullptr);

						if (wcscmp(attrName, L"foreground") == 0)
						{
							foreground_ = attrValue;

							// S'assurer que la chaîne est bien formée : #RRGGBB
							if (wcslen(attrValue) == 7 && attrValue[0] == L'#' || wcslen(attrValue) == 9 && attrValue[0] == L'#')
							{
								// Extraire les composants hexadécimaux
								wchar_t rStr[3] = { attrValue[1], attrValue[2], L'\0' };
								wchar_t gStr[3] = { attrValue[3], attrValue[4], L'\0' };
								wchar_t bStr[3] = { attrValue[5], attrValue[6], L'\0' };

								// Convertir les chaînes hex en entier (base 16)
								int R = wcstol(rStr, nullptr, 16);
								int G = wcstol(gStr, nullptr, 16);
								int B = wcstol(bStr, nullptr, 16);

								// Appliquer la couleur au widget
								WidgetLoad->SetColor(R, G, B, false);
							}

							else
							{
								MessageBox(
									NULL,
									L"Invalid foreground color format. Use #RRGGBB.",
									L"Error",
									MB_ICONERROR | MB_OK
								);
								return nullptr;
							}
						}

						if (wcscmp(attrName, L"background") == 0)
						{
							background_ = attrValue;

							// S'assurer que la chaîne est bien formée : #RRGGBB
							if (wcslen(attrValue) == 7 && attrValue[0] == L'#' || wcslen(attrValue) == 9 && attrValue[0] == L'#')
							{
								// Extraire les composants hexadécimaux
								wchar_t rStr[3] = { attrValue[1], attrValue[2], L'\0' };
								wchar_t gStr[3] = { attrValue[3], attrValue[4], L'\0' };
								wchar_t bStr[3] = { attrValue[5], attrValue[6], L'\0' };

								// Convertir les chaînes hex en entier (base 16)
								int R = wcstol(rStr, nullptr, 16);
								int G = wcstol(gStr, nullptr, 16);
								int B = wcstol(bStr, nullptr, 16);

								// Appliquer la couleur au widget
								WidgetLoad->SetColor(R, G, B, true);
							}

							else
							{
								MessageBox(
									NULL,
									L"Invalid foreground color format. Use #RRGGBB.",
									L"Error",
									MB_ICONERROR | MB_OK
								);
								return nullptr;
							}
						}

					} while (reader->MoveToNextAttribute() == S_OK);
				}
			}

			if (wcscmp(ElementName, L"Line") == 0 && WidgetLoad && isContent)
			{
				// Lire les attributs si présents
				if (reader->MoveToFirstAttribute() == S_OK)
				{
					std::wstring Line_ = L"";

					do
					{
						LPCWSTR attrName = nullptr;
						LPCWSTR attrValue = nullptr;

						reader->GetLocalName(&attrName, nullptr);
						reader->GetValue(&attrValue, nullptr);

						if (wcscmp(attrName, L"Content") == 0)
						{
							Line_ = attrValue;

							if (Line_.size() < WidgetLoad->GetSize().GetSizeX().GetDataNew())
							{
								Line_ += L"\n";
							}
						}

					} while (reader->MoveToNextAttribute() == S_OK);

					Content_V.push_back(Line_);
				}
			}

			if (wcscmp(ElementName, L"LineRaw") == 0 && WidgetLoad && isContent)
			{
				// Lire les attributs si présents
				if (reader->MoveToFirstAttribute() == S_OK)
				{
					std::wstring Line_ = L"";

					do
					{
						LPCWSTR attrName = nullptr;
						LPCWSTR attrValue = nullptr;

						reader->GetLocalName(&attrName, nullptr);
						reader->GetValue(&attrValue, nullptr);

						if (wcscmp(attrName, L"Content") == 0)
						{
							std::wstring raw = attrValue;
							std::wstring parsed = L"";

							for (size_t i = 0; i < raw.size(); ++i)
							{
								if (raw[i] == L'\\' && i + 1 < raw.size())
								{
									switch (raw[i + 1])
									{
									case L'b': parsed += L'\b'; i++; break;
									case L't': parsed += L'\t'; i++; break;
									case L'n': parsed += L'\n'; i++; break;
									case L'f': parsed += L'\f'; i++; break;
									case L'v': parsed += L'\v'; i++; break;
									case L'r': parsed += L'\r'; i++; break;
									default:   parsed += L'\\';      break; // backslash littéral
									}
								}
								else
								{
									parsed += raw[i];
								}
							}

							Line_ = parsed;

							if (Line_.size() < WidgetLoad->GetSize().GetSizeX().GetDataNew())
							{
								Line_ += L"\n";
							}
						}

					} while (reader->MoveToNextAttribute() == S_OK);

					Content_V.push_back(Line_);
				}
			}

			if (wcscmp(ElementName, L"Line") == 0 && isRawFrame)
			{
				// Lire les attributs si présents
				if (reader->MoveToFirstAttribute() == S_OK)
				{
					std::wstring Line_ = L"";

					do
					{
						LPCWSTR attrName = nullptr;
						LPCWSTR attrValue = nullptr;

						reader->GetLocalName(&attrName, nullptr);
						reader->GetValue(&attrValue, nullptr);

						if (wcscmp(attrName, L"Content") == 0)
						{
							Line_ = attrValue;

							if (Line_.size() < TextLoad->GetSize().GetSizeX().GetDataNew())
							{
								Line_ += L"\n";
							}
						}

					} while (reader->MoveToNextAttribute() == S_OK);

					Content_V.push_back(Line_);
				}
			}

			if (wcscmp(ElementName, L"LineRaw") == 0 && isRawFrame)
			{
				// Lire les attributs si présents
				if (reader->MoveToFirstAttribute() == S_OK)
				{
					std::wstring Line_ = L"";

					do
					{
						LPCWSTR attrName = nullptr;
						LPCWSTR attrValue = nullptr;

						reader->GetLocalName(&attrName, nullptr);
						reader->GetValue(&attrValue, nullptr);

						if (wcscmp(attrName, L"Content") == 0)
						{
							std::wstring raw = attrValue;
							std::wstring parsed = L"";

							for (size_t i = 0; i < raw.size(); ++i)
							{
								if (raw[i] == L'\\' && i + 1 < raw.size())
								{
									switch (raw[i + 1])
									{
									case L'b': parsed += L'\b'; i++; break;
									case L't': parsed += L'\t'; i++; break;
									case L'n': parsed += L'\n'; i++; break;
									case L'f': parsed += L'\f'; i++; break;
									case L'v': parsed += L'\v'; i++; break;
									case L'r': parsed += L'\r'; i++; break;
									default:   parsed += L'\\';      break; // backslash littéral
									}
								}
								else
								{
									parsed += raw[i];
								}
							}

							Line_ = parsed;

							if (Line_.size() < WidgetLoad->GetSize().GetSizeX().GetDataNew())
							{
								Line_ += L"\n";
							}
						}

					} while (reader->MoveToNextAttribute() == S_OK);

					Content_V.push_back(Line_);
				}
			}

			if (wcscmp(ElementName, L"Case") == 0 && WidgetLoad && isCaseColor)
			{
				// Lire les attributs si présents
				if (reader->MoveToFirstAttribute() == S_OK)
				{
					wstring foreground_ = L"";
					wstring background_ = L"";
					uint8_t R = 0, G = 0, B = 0;

					int Start, End;
					uint8_t foreground_R, foreground_G, foreground_B;
					uint8_t background_R, background_G, background_B;

					bool IsForeground = false, IsBackground = false;

					do
					{
						LPCWSTR attrName = nullptr;
						LPCWSTR attrValue = nullptr;

						reader->GetLocalName(&attrName, nullptr);
						reader->GetValue(&attrValue, nullptr);

						if (wcscmp(attrName, L"Start") == 0 || wcscmp(attrName, L"start") == 0)
						{
							std::wstring typeStr = attrValue;
							Start = std::stoi(std::string(typeStr.begin(), typeStr.end()));
						}

						if (wcscmp(attrName, L"End") == 0 || wcscmp(attrName, L"end") == 0)
						{
							std::wstring typeStr = attrValue;
							End = std::stoi(std::string(typeStr.begin(), typeStr.end()));
						}

						if (wcscmp(attrName, L"foreground") == 0)
						{
							foreground_ = attrValue;

							IsForeground = true;

							// S'assurer que la chaîne est bien formée : #RRGGBB
							if (wcslen(attrValue) == 7 && attrValue[0] == L'#' || wcslen(attrValue) == 9 && attrValue[0] == L'#')
							{
								// Extraire les composants hexadécimaux
								wchar_t rStr[3] = { attrValue[1], attrValue[2], L'\0' };
								wchar_t gStr[3] = { attrValue[3], attrValue[4], L'\0' };
								wchar_t bStr[3] = { attrValue[5], attrValue[6], L'\0' };

								// Convertir les chaînes hex en entier (base 16)
								foreground_R = static_cast<uint8_t>(wcstol(rStr, nullptr, 16));
								foreground_G = static_cast<uint8_t>(wcstol(gStr, nullptr, 16));
								foreground_B = static_cast<uint8_t>(wcstol(bStr, nullptr, 16));
							}

							else
							{
								MessageBox(
									NULL,
									L"Invalid foreground color format. Use #RRGGBB or #RRGGBBOO.",
									L"Error",
									MB_ICONERROR | MB_OK
								);
								return nullptr;
							}
						}

						if (wcscmp(attrName, L"background") == 0)
						{
							IsBackground = true;

							background_ = attrValue;

							// S'assurer que la chaîne est bien formée : #RRGGBB
							if (wcslen(attrValue) == 7 && attrValue[0] == L'#' || wcslen(attrValue) == 9 && attrValue[0] == L'#')
							{
								// Extraire les composants hexadécimaux
								wchar_t rStr[3] = { attrValue[1], attrValue[2], L'\0' };
								wchar_t gStr[3] = { attrValue[3], attrValue[4], L'\0' };
								wchar_t bStr[3] = { attrValue[5], attrValue[6], L'\0' };

								// Convertir les chaînes hex en entier (base 16)
								background_R = static_cast<uint8_t>(wcstol(rStr, nullptr, 16));
								background_G = static_cast<uint8_t>(wcstol(gStr, nullptr, 16));
								background_B = static_cast<uint8_t>(wcstol(bStr, nullptr, 16));
							}

							else
							{
								MessageBox(
									NULL,
									L"Invalid foreground color format. Use #RRGGBB or #RRGGBBOO.",
									L"Error",
									MB_ICONERROR | MB_OK
								);
								return nullptr;
							}
						}

					} while (reader->MoveToNextAttribute() == S_OK);

					// Appliquer la couleur au widget L"\x1b[48;2;" + to_wstring(R) + L";" + to_wstring(G) + L";" + to_wstring(B) + L"m"
					CaseColor[trPair<int, int>(Start, End)] = (IsForeground ? (L"\x1b[38;2;" + to_wstring(foreground_R) + L";" + to_wstring(foreground_G) + L";" + to_wstring(foreground_B) + L"m") : L"") + (IsBackground ? (L"\x1b[48;2;" + to_wstring(background_R) + L";" + to_wstring(background_G) + L";" + to_wstring(background_B) + L"m") : L"");
				}
			}

			if (wcscmp(ElementName, L"Case") == 0 && isRawFrame)
			{
				// Lire les attributs si présents
				if (reader->MoveToFirstAttribute() == S_OK)
				{
					wstring foreground_ = L"";
					wstring background_ = L"";
					uint8_t R = 0, G = 0, B = 0;

					int Start, End;
					uint8_t foreground_R, foreground_G, foreground_B;
					uint8_t background_R, background_G, background_B;

					bool IsForeground = false, IsBackground = false;

					do
					{
						LPCWSTR attrName = nullptr;
						LPCWSTR attrValue = nullptr;

						reader->GetLocalName(&attrName, nullptr);
						reader->GetValue(&attrValue, nullptr);

						if (wcscmp(attrName, L"Start") == 0 || wcscmp(attrName, L"start") == 0)
						{
							std::wstring typeStr = attrValue;
							Start = std::stoi(std::string(typeStr.begin(), typeStr.end()));
						}

						if (wcscmp(attrName, L"End") == 0 || wcscmp(attrName, L"end") == 0)
						{
							std::wstring typeStr = attrValue;
							End = std::stoi(std::string(typeStr.begin(), typeStr.end()));
						}

						if (wcscmp(attrName, L"foreground") == 0)
						{
							foreground_ = attrValue;

							IsForeground = true;

							// S'assurer que la chaîne est bien formée : #RRGGBB
							if (wcslen(attrValue) == 7 && attrValue[0] == L'#' || wcslen(attrValue) == 9 && attrValue[0] == L'#')
							{
								// Extraire les composants hexadécimaux
								wchar_t rStr[3] = { attrValue[1], attrValue[2], L'\0' };
								wchar_t gStr[3] = { attrValue[3], attrValue[4], L'\0' };
								wchar_t bStr[3] = { attrValue[5], attrValue[6], L'\0' };

								// Convertir les chaînes hex en entier (base 16)
								foreground_R = static_cast<uint8_t>(wcstol(rStr, nullptr, 16));
								foreground_G = static_cast<uint8_t>(wcstol(gStr, nullptr, 16));
								foreground_B = static_cast<uint8_t>(wcstol(bStr, nullptr, 16));
							}

							else
							{
								MessageBox(
									NULL,
									L"Invalid foreground color format. Use #RRGGBB or #RRGGBBOO.",
									L"Error",
									MB_ICONERROR | MB_OK
								);
								return nullptr;
							}
						}

						if (wcscmp(attrName, L"background") == 0)
						{
							IsBackground = true;

							background_ = attrValue;

							// S'assurer que la chaîne est bien formée : #RRGGBB
							if (wcslen(attrValue) == 7 && attrValue[0] == L'#' || wcslen(attrValue) == 9 && attrValue[0] == L'#')
							{
								// Extraire les composants hexadécimaux
								wchar_t rStr[3] = { attrValue[1], attrValue[2], L'\0' };
								wchar_t gStr[3] = { attrValue[3], attrValue[4], L'\0' };
								wchar_t bStr[3] = { attrValue[5], attrValue[6], L'\0' };

								// Convertir les chaînes hex en entier (base 16)
								background_R = static_cast<uint8_t>(wcstol(rStr, nullptr, 16));
								background_G = static_cast<uint8_t>(wcstol(gStr, nullptr, 16));
								background_B = static_cast<uint8_t>(wcstol(bStr, nullptr, 16));
							}

							else
							{
								MessageBox(
									NULL,
									L"Invalid foreground color format. Use #RRGGBB or #RRGGBBOO.",
									L"Error",
									MB_ICONERROR | MB_OK
								);
								return nullptr;
							}
						}

					} while (reader->MoveToNextAttribute() == S_OK);

					// Appliquer la couleur au widget L"\x1b[48;2;" + to_wstring(R) + L";" + to_wstring(G) + L";" + to_wstring(B) + L"m"
					CaseColor[trPair<int, int>(Start, End)] = (IsForeground ? (L"\x1b[38;2;" + to_wstring(foreground_R) + L";" + to_wstring(foreground_G) + L";" + to_wstring(foreground_B) + L"m") : L"") + (IsBackground ? (L"\x1b[48;2;" + to_wstring(background_R) + L";" + to_wstring(background_G) + L";" + to_wstring(background_B) + L"m") : L"");
				}
			}

			if (wcscmp(ElementName, L"Case") == 0 && isOldContent)
			{
				// Lire les attributs si présents
				if (reader->MoveToFirstAttribute() == S_OK)
				{
					wstring foreground_ = L"";
					wstring background_ = L"";
					uint8_t R = 0, G = 0, B = 0;

					int Start, End;
					uint8_t foreground_R, foreground_G, foreground_B;
					uint8_t background_R, background_G, background_B;

					bool IsForeground = false, IsBackground = false;

					do
					{
						LPCWSTR attrName = nullptr;
						LPCWSTR attrValue = nullptr;

						reader->GetLocalName(&attrName, nullptr);
						reader->GetValue(&attrValue, nullptr);

						if (wcscmp(attrName, L"Start") == 0 || wcscmp(attrName, L"start") == 0)
						{
							std::wstring typeStr = attrValue;
							Start = std::stoi(std::string(typeStr.begin(), typeStr.end()));
						}

						if (wcscmp(attrName, L"End") == 0 || wcscmp(attrName, L"end") == 0)
						{
							std::wstring typeStr = attrValue;
							End = std::stoi(std::string(typeStr.begin(), typeStr.end()));
						}

						if (wcscmp(attrName, L"foreground") == 0)
						{
							foreground_ = attrValue;

							IsForeground = true;

							// S'assurer que la chaîne est bien formée : #RRGGBB
							if (wcslen(attrValue) == 7 && attrValue[0] == L'#' || wcslen(attrValue) == 9 && attrValue[0] == L'#')
							{
								// Extraire les composants hexadécimaux
								wchar_t rStr[3] = { attrValue[1], attrValue[2], L'\0' };
								wchar_t gStr[3] = { attrValue[3], attrValue[4], L'\0' };
								wchar_t bStr[3] = { attrValue[5], attrValue[6], L'\0' };

								// Convertir les chaînes hex en entier (base 16)
								foreground_R = static_cast<uint8_t>(wcstol(rStr, nullptr, 16));
								foreground_G = static_cast<uint8_t>(wcstol(gStr, nullptr, 16));
								foreground_B = static_cast<uint8_t>(wcstol(bStr, nullptr, 16));
							}

							else
							{
								MessageBox(
									NULL,
									L"Invalid foreground color format. Use #RRGGBB or #RRGGBBOO.",
									L"Error",
									MB_ICONERROR | MB_OK
								);
								return nullptr;
							}
						}

						if (wcscmp(attrName, L"background") == 0)
						{
							IsBackground = true;

							background_ = attrValue;

							// S'assurer que la chaîne est bien formée : #RRGGBB
							if (wcslen(attrValue) == 7 && attrValue[0] == L'#' || wcslen(attrValue) == 9 && attrValue[0] == L'#')
							{
								// Extraire les composants hexadécimaux
								wchar_t rStr[3] = { attrValue[1], attrValue[2], L'\0' };
								wchar_t gStr[3] = { attrValue[3], attrValue[4], L'\0' };
								wchar_t bStr[3] = { attrValue[5], attrValue[6], L'\0' };

								// Convertir les chaînes hex en entier (base 16)
								background_R = static_cast<uint8_t>(wcstol(rStr, nullptr, 16));
								background_G = static_cast<uint8_t>(wcstol(gStr, nullptr, 16));
								background_B = static_cast<uint8_t>(wcstol(bStr, nullptr, 16));
							}

							else
							{
								MessageBox(
									NULL,
									L"Invalid foreground color format. Use #RRGGBB or #RRGGBBOO.",
									L"Error",
									MB_ICONERROR | MB_OK
								);
								return nullptr;
							}
						}

					} while (reader->MoveToNextAttribute() == S_OK);

					// Appliquer la couleur au widget L"\x1b[48;2;" + to_wstring(R) + L";" + to_wstring(G) + L";" + to_wstring(B) + L"m"
					CaseColor[trPair<int, int>(Start, End)] = (IsForeground ? (L"\x1b[38;2;" + to_wstring(foreground_R) + L";" + to_wstring(foreground_G) + L";" + to_wstring(foreground_B) + L"m") : L"") + (IsBackground ? (L"\x1b[48;2;" + to_wstring(background_R) + L";" + to_wstring(background_G) + L";" + to_wstring(background_B) + L"m") : L"");
				}
			}

			if (wcscmp(ElementName, L"trText") == 0)
			{
				TextLoad = dynamic_cast<trText*>(ObjectLoad);

				if (!TextLoad)
				{
					MessageBox(
						NULL,
						L"trPawn not found",
						L"Error",
						MB_ICONERROR | MB_OK
					);
					return nullptr;
				}
			}

			if (wcscmp(ElementName, L"Animation") == 0 && TextLoad)
			{
				isAnimation = true;

				AnimationVector.clear();
				AnimationVector.emplace_back(trPair<uint16_t, std::wstring>(0, TextLoad->GetRawContent().GetDataNew()));
			}

			if (wcscmp(ElementName, L"RawFrame") == 0 && TextLoad && isAnimation)
			{
				isRawFrame = true;

				// Lire les attributs si présents
				if (reader->MoveToFirstAttribute() == S_OK)
				{
					do
					{
						LPCWSTR attrName = nullptr;
						LPCWSTR attrValue = nullptr;

						reader->GetLocalName(&attrName, nullptr);
						reader->GetValue(&attrValue, nullptr);

						if (wcscmp(attrName, L"number") == 0 || wcscmp(attrName, L"Number") == 0)
						{
							std::wstring typeStr = attrValue;
							NumberFrame = std::stoi(std::string(typeStr.begin(), typeStr.end()));
						}

						if (wcscmp(attrName, L"time") == 0 || wcscmp(attrName, L"Time") == 0)
						{
							std::wstring typeStr = attrValue;
							TimeFrame = std::stoi(std::string(typeStr.begin(), typeStr.end()));
						}

					} while (reader->MoveToNextAttribute() == S_OK);
				}
			}

			if (wcscmp(ElementName, L"FrameAdd") == 0 && TextLoad && isAnimation)
			{
				isFrameAdd = true;

				// Lire les attributs si présents
				if (reader->MoveToFirstAttribute() == S_OK)
				{
					do
					{
						LPCWSTR attrName = nullptr;
						LPCWSTR attrValue = nullptr;

						reader->GetLocalName(&attrName, nullptr);
						reader->GetValue(&attrValue, nullptr);

						if (wcscmp(attrName, L"number") == 0 || wcscmp(attrName, L"Number") == 0)
						{
							std::wstring typeStr = attrValue;
							NumberFrame = std::stoi(std::string(typeStr.begin(), typeStr.end()));
						}

						if (wcscmp(attrName, L"time") == 0 || wcscmp(attrName, L"Time") == 0)
						{
							std::wstring typeStr = attrValue;
							TimeFrame = std::stoi(std::string(typeStr.begin(), typeStr.end()));
						}

						if (wcscmp(attrName, L"onLastFrame") == 0 || wcscmp(attrName, L"OnLastFrame") == 0 || wcscmp(attrName, L"AddLastFrame") == 0 || wcscmp(attrName, L"addLastFrame") == 0)
						{
							std::wstring typeStr = attrValue;
							onLastFrame = typeStr == L"True" || L"true" || L"1" ? true : false;
						}

					} while (reader->MoveToNextAttribute() == S_OK);
				}
			}

			if (wcscmp(ElementName, L"OldContent") == 0 && TextLoad && isFrameAdd)
			{
				isOldContent = true;


				OldContent_ = onLastFrame == false ? TextLoad->GetRawContent().GetDataNew() : AnimationVector.back().GetSecond();
			}

			if (wcscmp(ElementName, L"Add") == 0 && TextLoad && isOldContent)
			{
				// Lire les attributs si présents
				if (reader->MoveToFirstAttribute() == S_OK)
				{
					std::wstring Line_ = L"";

					do
					{
						LPCWSTR attrName = nullptr;
						LPCWSTR attrValue = nullptr;

						reader->GetLocalName(&attrName, nullptr);
						reader->GetValue(&attrValue, nullptr);

						if (wcscmp(attrName, L"position") == 0 || wcscmp(attrName, L"pos") == 0 || wcscmp(attrName, L"Position") == 0 || wcscmp(attrName, L"Pos") == 0)
						{
							indexFirst = std::stoi(attrValue);
						}

					} while (reader->MoveToNextAttribute() == S_OK);
				}
			}

			if (wcscmp(ElementName, L"Erase") == 0 && TextLoad && isOldContent)
			{
				// Lire les attributs si présents
				if (reader->MoveToFirstAttribute() == S_OK)
				{
					std::wstring Line_ = L"";

					do
					{
						LPCWSTR attrName = nullptr;
						LPCWSTR attrValue = nullptr;

						reader->GetLocalName(&attrName, nullptr);
						reader->GetValue(&attrValue, nullptr);

						if (wcscmp(attrName, L"Start") == 0 || wcscmp(attrName, L"start") == 0)
						{
							indexFirst = std::stoi(attrValue);
						}

						if (wcscmp(attrName, L"End") == 0 || wcscmp(attrName, L"end") == 0 || wcscmp(attrName, L"Ending") == 0 || wcscmp(attrName, L"ending") == 0)
						{
							indexLast = std::stoi(attrValue);
						}

						if (wcscmp(attrName, L"doColor") == 0 || wcscmp(attrName, L"DoColor") == 0)
						{
							if (attrValue == L"True" || attrValue == L"TRUE" || attrValue == L"true" || attrValue == L"1" || attrValue == L"yes")
							{
								DoColor = true;
							}

							if (attrValue == L"False" || attrValue == L"FALSE" || attrValue == L"false" || attrValue == L"0" || attrValue == L"no")
							{
								DoColor = false;
							}
						}

					} while (reader->MoveToNextAttribute() == S_OK);
				}

				int j = 0;

				for (int i = 0; i < indexLast; i++)
				{
					if (DoColor)
					{
						MessageBox(
							NULL,
							L"J'ai pas encore implémenté la fonctionnalité du coup je vais gentillement le passer DoColor = false",
							L"Error",
							MB_ICONERROR | MB_OK
						);

						DoColor = false;
					}

					if (OldContent_[i] == L'\033')
					{
						for (j = 0; OldContent_[j + i] != L'm'; j++)
						{
							if (i + j >= OldContent_.size())
							{
								MessageBox(
									NULL,
									L"Invalid escape sequence in Erase.",
									L"Error",
									MB_ICONERROR | MB_OK
								);
								return nullptr;
							}
						}

						indexLast += j + 1;
						indexFirst += j + 1;
						i += j;
					}

					else if (i >= indexFirst)
					{
						OldContent_.erase(i, 1);
						i--;
						indexLast--;
						indexFirst--;
					}
				}

				// pas fini du tout
			}

			if (wcscmp(ElementName, L"Property") == 0 && isPropreties)
			{
				// Lire les attributs si présents
				if (reader->MoveToFirstAttribute() == S_OK)
				{
					do
					{
						LPCWSTR attrName = nullptr;
						LPCWSTR attrValue = nullptr;

						reader->GetLocalName(&attrName, nullptr);
						reader->GetValue(&attrValue, nullptr);

						if (wcscmp(attrName, L"name") == 0)
						{
							std::wstring typeStr = attrValue;
							name_ = std::string(typeStr.begin(), typeStr.end()); \
						}

						if (wcscmp(attrName, L"type") == 0)
						{
							std::wstring typeStr = attrValue;
							type_ = std::string(typeStr.begin(), typeStr.end());
						}

					} while (reader->MoveToNextAttribute() == S_OK);
				}
			}

			if (wcscmp(ElementName, L"trSelector") == 0)
			{
				SelectorLoad = dynamic_cast<trSelector*>(ObjectLoad);

				if (!SelectorLoad)
				{
					MessageBox(
						NULL,
						L"trSelector not found",
						L"Error",
						MB_ICONERROR | MB_OK
					);
					return nullptr;
				}
			}

			if (wcscmp(ElementName, L"SlectedColor") == 0 && SelectorLoad)
			{
				// Lire les attributs si présents
				if (reader->MoveToFirstAttribute() == S_OK)
				{
					wstring ColorSelectedTemp = L"\033[0m";
					wstring foreground_ = L"";
					wstring background_ = L"";
					uint8_t R = 0, G = 0, B = 0;

					do
					{
						LPCWSTR attrName = nullptr;
						LPCWSTR attrValue = nullptr;

						reader->GetLocalName(&attrName, nullptr);
						reader->GetValue(&attrValue, nullptr);

						if (wcscmp(attrName, L"foreground") == 0)
						{
							foreground_ = attrValue;

							// S'assurer que la chaîne est bien formée : #RRGGBB
							if (wcslen(attrValue) == 7 && attrValue[0] == L'#' || wcslen(attrValue) == 9 && attrValue[0] == L'#')
							{
								// Extraire les composants hexadécimaux
								wchar_t rStr[3] = { attrValue[1], attrValue[2], L'\0' };
								wchar_t gStr[3] = { attrValue[3], attrValue[4], L'\0' };
								wchar_t bStr[3] = { attrValue[5], attrValue[6], L'\0' };

								// Convertir les chaînes hex en entier (base 16)
								int R = wcstol(rStr, nullptr, 16);
								int G = wcstol(gStr, nullptr, 16);
								int B = wcstol(bStr, nullptr, 16);

								// Appliquer la couleur au widget
								foreground_ = L"\x1b[38;2;" + to_wstring(R) + L";" + to_wstring(G) + L";" + to_wstring(B) + L"m";
							}

							else
							{
								MessageBox(
									NULL,
									L"Invalid foreground color format. Use #RRGGBB.",
									L"Error",
									MB_ICONERROR | MB_OK
								);
								return nullptr;
							}
						}

						if (wcscmp(attrName, L"background") == 0)
						{
							background_ = attrValue;

							// S'assurer que la chaîne est bien formée : #RRGGBB
							if (wcslen(attrValue) == 7 && attrValue[0] == L'#' || wcslen(attrValue) == 9 && attrValue[0] == L'#')
							{
								// Extraire les composants hexadécimaux
								wchar_t rStr[3] = { attrValue[1], attrValue[2], L'\0' };
								wchar_t gStr[3] = { attrValue[3], attrValue[4], L'\0' };
								wchar_t bStr[3] = { attrValue[5], attrValue[6], L'\0' };

								// Convertir les chaînes hex en entier (base 16)
								int R = wcstol(rStr, nullptr, 16);
								int G = wcstol(gStr, nullptr, 16);
								int B = wcstol(bStr, nullptr, 16);

								// Appliquer la couleur au widget
								background_ = L"\x1b[48;2;" + to_wstring(R) + L";" + to_wstring(G) + L";" + to_wstring(B) + L"m";
							}

							else
							{
								MessageBox(
									NULL,
									L"Invalid foreground color format. Use #RRGGBB.",
									L"Error",
									MB_ICONERROR | MB_OK
								);
								return nullptr;
							}
						}

					} while (reader->MoveToNextAttribute() == S_OK);

					foreground_.empty() ? ColorSelectedTemp += SelectorLoad->GetColorSelected().GetDataActual() : ColorSelectedTemp += (foreground_ + background_);

					SelectorLoad->SetProprety("ColorSelected", WStringToUTF8(ColorSelectedTemp), "string");
				}
			}
		}

		else if (nodeType == XmlNodeType_Text)
		{
			// On lit le texte entre les balises
			reader->GetValue(&Text, nullptr);

			std::wstring wsText = Text;
			wdata_ = wsText;
			data_ = std::string(wsText.begin(), wsText.end());
		}

		else if (nodeType == XmlNodeType_EndElement)
		{
			reader->GetLocalName(&ElementName, nullptr);

			// On arrive à </...>
			if (wcscmp(ElementName, L"trObject") == 0)
			{

			}

			if (wcscmp(ElementName, L"Properties") == 0)
			{
				isPropreties = false;
			}

			if (wcscmp(ElementName, L"trPawn") == 0)
			{
				PawnLoad = nullptr;
			}

			if (wcscmp(ElementName, L"trWidget") == 0)
			{
				wstring raw = Content_ID;

				wstring parsed;

				for (size_t i = 0; i < raw.size(); ++i)
				{
					if (raw[i] == L'\\' && i + 1 < raw.size())
					{
						switch (raw[i + 1])
						{
						case L'b': parsed += L'\b'; i++; break;
						case L't': parsed += L'\t'; i++; break;
						case L'n': parsed += L'\n'; i++; break;
						case L'f': parsed += L'\f'; i++; break;
						case L'v': parsed += L'\v'; i++; break;
						case L'r': parsed += L'\r'; i++; break;
						default:   parsed += L'\\';      break; // backslash littéral
						}
					}
					else
					{
						parsed += raw[i];
					}
				}

				Content_ID = parsed;

				WidgetLoad->SetContent(Content_ID);

				WidgetLoad = nullptr;
			}

			if (wcscmp(ElementName, L"RawFrame") == 0)
			{
				wstring raw = Content_ID;

				wstring parsed;

				for (size_t i = 0; i < raw.size(); ++i)
				{
					if (raw[i] == L'\\' && i + 1 < raw.size())
					{
						switch (raw[i + 1])
						{
						case L'b': parsed += L'\b'; i++; break;
						case L't': parsed += L'\t'; i++; break;
						case L'n': parsed += L'\n'; i++; break;
						case L'f': parsed += L'\f'; i++; break;
						case L'v': parsed += L'\v'; i++; break;
						case L'r': parsed += L'\r'; i++; break;
						default:   parsed += L'\\';      break; // backslash littéral
						}
					}
					else
					{
						parsed += raw[i];
					}
				}

				Content_ID = parsed;

				AnimationVector.emplace_back(trPair<uint16_t, wstring>(TimeFrame, Content_ID));

				isRawFrame = false;
			}

			if (wcscmp(ElementName, L"Content") == 0 && WidgetLoad)
			{
				for (const auto& line : Content_V)
				{
					Content_ID += line;
				}

				isContent = false;
			}

			if (wcscmp(ElementName, L"Content") == 0 && isRawFrame)
			{
				for (const auto& line : Content_V)
				{
					Content_ID += line;
				}

				isContent = false;
			}

			if (wcscmp(ElementName, L"_CaseColor") == 0 && WidgetLoad)
			{
				isCaseColor = false;

				map<int, wstring> Clr;

				int previousFirst = -1;

				int previousSecond = -2;

				wstring previousReset = WidgetLoad->GetBackroundColor().GetDataNew() + WidgetLoad->GetColor().GetDataNew();

				for (const auto& TheCase : CaseColor)
				{
					Clr[TheCase.first.GetFirst()] = TheCase.second;

					if (TheCase.first.GetSecond() >= previousFirst && TheCase.first.GetSecond() <= previousSecond)
					{
						Clr[TheCase.first.GetSecond()] = Clr[previousFirst];
						previousReset = Clr[previousFirst];
					}

					else
					{
						Clr[TheCase.first.GetSecond()] = previousReset;
					}

					previousFirst = TheCase.first.GetFirst();
					previousSecond = TheCase.first.GetSecond();
				}

				wstring TextBase = WidgetLoad->GetRawContent().GetDataNew();

				// Parcours à l'envers 🔁
				for (auto it = Clr.rbegin(); it != Clr.rend(); ++it)
				{
					TextBase.insert(it->first, it->second);
				}

				WidgetLoad->SetContent(TextBase);
			}

			if (wcscmp(ElementName, L"CaseColor") == 0 && WidgetLoad)
			{
				isCaseColor = false;

				CaseColorProcess(CaseColor, *WidgetLoad, Content_ID);

				// WidgetLoad->SetContent(text);
			}

			if (wcscmp(ElementName, L"_CaseColor") == 0 && isRawFrame)
			{
				isCaseColor = false;

				map<int, wstring> Clr;

				int previousFirst = -1;

				int previousSecond = -2;

				wstring previousReset = WidgetLoad->GetBackroundColor().GetDataNew() + WidgetLoad->GetColor().GetDataNew();

				for (const auto& TheCase : CaseColor)
				{
					Clr[TheCase.first.GetFirst()] = TheCase.second;

					if (TheCase.first.GetSecond() >= previousFirst && TheCase.first.GetSecond() <= previousSecond)
					{
						Clr[TheCase.first.GetSecond()] = Clr[previousFirst];
						previousReset = Clr[previousFirst];
					}

					else
					{
						Clr[TheCase.first.GetSecond()] = previousReset;
					}

					previousFirst = TheCase.first.GetFirst();
					previousSecond = TheCase.first.GetSecond();
				}

				wstring TextBase = WidgetLoad->GetRawContent().GetDataNew();

				// Parcours à l'envers 🔁
				for (auto it = Clr.rbegin(); it != Clr.rend(); ++it)
				{
					TextBase.insert(it->first, it->second);
				}

				WidgetLoad->SetContent(TextBase);
			}

			if (wcscmp(ElementName, L"CaseColor") == 0 && isRawFrame)
			{
				isCaseColor = false;

				CaseColorProcess(CaseColor, *TextLoad, Content_ID);
			}

			if (wcscmp(ElementName, L"CaseColor") == 0 && TextLoad && isOldContent)
			{
				isCaseColor = false;

				CaseColorProcess(CaseColor, *TextLoad, OldContent_);
			}

			if (wcscmp(ElementName, L"trText") == 0)
			{


				TextLoad = nullptr;
			}

			if (wcscmp(ElementName, L"OldContent") == 0 && TextLoad && isFrameAdd)
			{
				isOldContent = false;

				wstring raw = OldContent_;

				wstring parsed;

				for (size_t i = 0; i < raw.size(); ++i)
				{
					if (raw[i] == L'\\' && i + 1 < raw.size())
					{
						switch (raw[i + 1])
						{
						case L'b': parsed += L'\b'; i++; break;
						case L't': parsed += L'\t'; i++; break;
						case L'n': parsed += L'\n'; i++; break;
						case L'f': parsed += L'\f'; i++; break;
						case L'v': parsed += L'\v'; i++; break;
						case L'r': parsed += L'\r'; i++; break;
						default:   parsed += L'\\';      break; // backslash littéral
						}
					}
					else
					{
						parsed += raw[i];
					}
				}

				OldContent_ = parsed;
			}

			if (wcscmp(ElementName, L"FrameAdd") == 0 && TextLoad && isAnimation)
			{
				isFrameAdd = false;

				AnimationVector.emplace_back(trPair<uint16_t, wstring>(TimeFrame, OldContent_));
			}

			if (wcscmp(ElementName, L"Add") == 0 && TextLoad && isOldContent)
			{
				// OldContent_.insert(indexFirst, wstring(data_.begin(), data_.end()));

				int j = 0;

				for (int i = 0; i <= indexFirst; i++)
				{
					if (OldContent_[i + j] == L'\033')
					{
						for (j; OldContent_[j + i] != L'm'; j++)
						{
							if (i + j >= OldContent_.size())
							{
								MessageBox(
									NULL,
									L"Invalid escape sequence in Erase.",
									L"Error",
									MB_ICONERROR | MB_OK
								);
								return nullptr;
							}
						}

						indexFirst++;
					}
				}

				OldContent_.insert(indexFirst + j, wdata_);
			}

			if (wcscmp(ElementName, L"Animation") == 0 && TextLoad)
			{
				TextLoad->SetAnimationVector(AnimationVector);

				isAnimation = false;
			}

			if (wcscmp(ElementName, L"Property") == 0 && isPropreties)
			{
				trActor* actorLoad = dynamic_cast<trActor*>(ObjectLoad);

				actorLoad->SetProprety(name_, data_, type_);
			}
		}
	}

	fileStream->Release();
	reader->Release();

	return ObjectLoad;
}

