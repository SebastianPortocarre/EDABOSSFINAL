#ifndef SUGERIDOR_H
#define SUGERIDOR_H

#include <string>
#include <vector>

class Sugeridor {
public:
    std::string sugerir(const std::string& termino, const std::vector<std::string>& lista);

private:
    int calcularDistanciaLevenshtein(const std::string& s1, const std::string& s2);
};

#endif