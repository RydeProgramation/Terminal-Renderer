import os
import re
import sys

print("-------------------------- PYTHON PREBUILD LANCEE -------------------------")

SOURCE_DIR = os.getcwd()

CLASS_PATTERN = re.compile(
    r"(class|struct)\s+(?:\w+\s+)*(\w+)(?:\s*:\s*(?:public|private|protected)?\s*(\w+))?"
)

REGISTER_PATTERN = re.compile(r"REGISTER_TYPE\s*\(\s*(\w+)\s*,")

CTOR_PATTERN = lambda cls: re.compile(
    rf"\b{cls}\s*\(\s*([^\)]*?)\s*\)\s*(?:;|:|\{{)", re.MULTILINE | re.DOTALL
)

class_parents = {
    "trActor": "",
    "trPawn": "trActor",
    "trWidget": "trPawn",
    "trText": "trWidget",
    "trSelector": "trWidget"
}

registered_classes = set()

# Etape 1 : remplir class_parents + registered_classes
for root, _, files in os.walk(SOURCE_DIR):
    for file in files:
        if not file.endswith((".h", ".cpp")):
            continue
        path = os.path.join(root, file)
        with open(path, "r", encoding="utf-8", errors="ignore") as f:
            txt = f.read()
        for m in CLASS_PATTERN.finditer(txt):
            cls, parent = m.group(2), m.group(3) or ""
            class_parents.setdefault(cls, parent)
        for m in REGISTER_PATTERN.finditer(txt):
            registered_classes.add(m.group(1))


def inherits_from_trActor(name):
    cur = name
    while cur:
        if cur == "trActor":
            return True
        cur = class_parents.get(cur, "")
    return False


def find_header_for_class(cls):
    """Retourne le .h contenant la declaration de la classe."""
    pattern = re.compile(rf"\b(class|struct)\s+(?:\w+\s+)*{cls}\b")
    for root, _, files in os.walk(SOURCE_DIR):
        for file in files:
            if not file.endswith((".h", ".hpp")):
                continue
            path = os.path.join(root, file)
            with open(path, "r", encoding="utf-8", errors="ignore") as f:
                if pattern.search(f.read()):
                    return path
    return None


def extract_ctor_params(header_path, cls):
    """Extrait la liste des types de parametres du constructeur principal."""
    pattern = CTOR_PATTERN(cls)
    with open(header_path, "r", encoding="utf-8", errors="ignore") as f:
        text = f.read()
    all_ctors = pattern.findall(text)
    valid = []
    for params in all_ctors:
        params = params.strip()
        if not params:
            continue
        if re.match(rf"\s*const\s+{cls}\s*&", params):
            continue
        valid.append(params)
    if not valid:
        return []
    best = max(valid, key=lambda p: p.count(","))
    types = []
    for param in best.split(","):
        parts = param.strip().split()
        if len(parts) >= 2:
            t = " ".join(parts[:-1])
        else:
            t = parts[0]
        types.append(t)
    return types


def insert_after_includes(cpp_path, cls, types):
    """Insere REGISTER_TYPE juste apres les includes, avec ligne vide."""
    with open(cpp_path, "r", encoding="utf-8", errors="ignore") as f:
        lines = f.readlines()

    last_inc = -1
    for i, l in enumerate(lines):
        # On enleve BOM et espaces/tabs invisibles en debut de ligne
        clean = l.lstrip('\ufeff').lstrip()
        if clean.startswith('#include'):
            last_inc = i

    # Position d'insertion : juste apres le dernier include
    insert_pos = last_inc + 1 if last_inc >= 0 else 0

    register_line = f"REGISTER_TYPE({cls}, {', '.join(types)});\n\n"
    blank_line    = "\n"

    # Evite duplication si deja present
    if any(cls in l and "REGISTER_TYPE" in l for l in lines):
        print(f"{cls} deja enregistre dans {cpp_path}")
        return

    # Insere ligne vide + REGISTER_TYPE a la bonne position
    lines.insert(insert_pos, blank_line)
    lines.insert(insert_pos+1, register_line)

    with open(cpp_path, "w", encoding="utf-8") as f:
        f.writelines(lines)

    print(f"Ajoute REGISTER_TYPE({cls}, {', '.join(types)}) dans {cpp_path}")


# Etape 2 : detection des classes manquantes
ignore_classes = {"trWidget", "trText", "trSelector"}

missing = [c for c in class_parents
           if c not in ("trActor", "trPawn")
           and c not in ignore_classes
           and inherits_from_trActor(c)
           and c not in registered_classes]

if not missing:
    print("Tout est deja enregistre !")

for cls in missing:
    header = find_header_for_class(cls)
    if not header:
        print(f"ERROR : Header pour {cls} non trouvee, on passe.")
        continue

    types = extract_ctor_params(header, cls)
    if not types:
        print(f"ERROR : Pas de vrai constructeur trouvee pour {cls}, inserer manuellement.")
        continue  # on N'APPELLE PAS insert_after_includes

    # repere le .cpp associe
    cpp = header[:-2] + ".cpp"
    if not os.path.exists(cpp):
        alt = header[:-2] + ".cc"
        if os.path.exists(alt):
            cpp = alt
        else:
            print(f"ERROR : .cpp introuvable pour {cls}, trouver a la main.")
            continue

    insert_after_includes(cpp, cls, types)

print("Mise a jour terminee")

print("--------------------------- FIN PYTHON PREBUILD ---------------------------")
