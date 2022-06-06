// pch.h : Il s'agit d'un fichier d'en-tête précompilé.
// Les fichiers listés ci-dessous sont compilés une seule fois, ce qui améliore les performances de génération des futures builds.
// Cela affecte également les performances d'IntelliSense, notamment la complétion du code et de nombreuses fonctionnalités de navigation du code.
// Toutefois, les fichiers listés ici sont TOUS recompilés si l'un d'entre eux est mis à jour entre les builds.
// N'ajoutez pas de fichiers fréquemment mis à jour ici, car cela annule les gains de performance.

#ifndef PCH_H
#define PCH_H

// ajouter les en-têtes à précompiler ici

#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS

#include <map>
#include <cmath>
#include <array>
#include <atomic>
#include <vector>
#include <string>
#include <memory>
#include <fstream>
#include <typeinfo>
#include <iostream>
#include <typeindex>
#include <filesystem>
#include <functional>
#include <unordered_map>
#include <unordered_set>

#include "maths.hpp"
#include "resource.hpp"

#endif //PCH_H
