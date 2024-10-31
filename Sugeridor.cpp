#include "Sugeridor.h"
#include <algorithm>
#include <limits>

int Sugeridor::calcularDistanciaLevenshtein(const std::string& s1, const std::string& s2) {
    size_t len1 = s1.size(), len2 = s2.size();
    std::vector<std::vector<int>> d(len1 + 1, std::vector<int>(len2 + 1));

    for (size_t i = 0; i <= len1; ++i) d[i][0] = i;
    for (size_t j = 0; j <= len2; ++j) d[0][j] = j;

    for (size_t i = 1; i <= len1; ++i) {
        for (size_t j = 1; j <= len2; ++j) {
            int cost = (s1[i - 1] == s2[j - 1]) ? 0 : 1;
            d[i][j] = std::min({ d[i - 1][j] + 1, d[i][j - 1] + 1, d[i - 1][j - 1] + cost });
        }
    }
    return d[len1][len2];
}

// Método para encontrar el término más cercano en la lista
std::string Sugeridor::sugerir(const std::string& termino, const std::vector<std::string>& lista) {
    std::string mejorCoincidencia;
    int menorDistancia = std::numeric_limits<int>::max();

    for (const auto& opcion : lista) {
        int distancia = calcularDistanciaLevenshtein(termino, opcion);
        if (distancia < menorDistancia) {
            menorDistancia = distancia;
            mejorCoincidencia = opcion;
        }
    }

    return mejorCoincidencia;
}
