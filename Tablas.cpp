#include "Tablas.h"
#include <fstream>
#include <iostream>
#include <sys/stat.h>

namespace fs = std::filesystem;
using namespace std;

uint32_t Tablas::obtenerIndiceNombreApellido(const string& nombre_apellido) {
    auto it = mapa_nombres_apellidos.find(nombre_apellido);
    if (it != mapa_nombres_apellidos.end()) {
        return it->second;
    } else {
        uint32_t indice = nombres_apellidos.size();
        nombres_apellidos.push_back(nombre_apellido);
        mapa_nombres_apellidos[nombre_apellido] = indice;
        return indice;
    }
}

uint32_t Tablas::obtenerIndiceLugarNacimiento(const string& lugar_nacimiento) {
    auto it = mapa_lugares_nacimiento.find(lugar_nacimiento);
    if (it != mapa_lugares_nacimiento.end()) {
        return it->second;
    } else {
        uint32_t indice = lugares_nacimiento.size();
        lugares_nacimiento.push_back(lugar_nacimiento);
        mapa_lugares_nacimiento[lugar_nacimiento] = indice;
        return indice;
    }
}

uint8_t Tablas::obtenerIndiceDepartamento(const string& departamento) {
    auto it = mapa_departamentos.find(departamento);
    if (it != mapa_departamentos.end()) {
        return it->second;
    } else {
        uint8_t indice = departamentos.size();
        departamentos.push_back(departamento);
        mapa_departamentos[departamento] = indice;
        return indice;
    }
}

uint16_t Tablas::obtenerIndiceProvincia(const string& provincia) {
    auto it = mapa_provincias.find(provincia);
    if (it != mapa_provincias.end()) {
        return it->second;
    } else {
        uint16_t indice = provincias.size();
        provincias.push_back(provincia);
        mapa_provincias[provincia] = indice;
        return indice;
    }
}

uint16_t Tablas::obtenerIndiceCiudad(const string& ciudad) {
    auto it = mapa_ciudades.find(ciudad);
    if (it != mapa_ciudades.end()) {
        return it->second;
    } else {
        uint16_t indice = ciudades.size();
        ciudades.push_back(ciudad);
        mapa_ciudades[ciudad] = indice;
        return indice;
    }
}

uint16_t Tablas::obtenerIndiceDistrito(const string& distrito) {
    auto it = mapa_distritos.find(distrito);
    if (it != mapa_distritos.end()) {
        return it->second;
    } else {
        uint16_t indice = distritos.size();
        distritos.push_back(distrito);
        mapa_distritos[distrito] = indice;
        return indice;
    }
}

uint32_t Tablas::obtenerIndiceUbicacion(const string& ubicacion) {
    auto it = mapa_ubicaciones.find(ubicacion);
    if (it != mapa_ubicaciones.end()) {
        return it->second;
    } else {
        uint32_t indice = ubicaciones.size();
        ubicaciones.push_back(ubicacion);
        mapa_ubicaciones[ubicacion] = indice;
        return indice;
    }
}

uint32_t Tablas::obtenerIndiceEmail(const string& email) {
    auto it = mapa_emails.find(email);
    if (it != mapa_emails.end()) {
        return it->second;
    } else {
        uint32_t indice = emails.size();
        emails.push_back(email);
        mapa_emails[email] = indice;
        return indice;
    }
}

void Tablas::guardarTablas(const std::string& directorio) {
    crearDirectorio(directorio);

    guardarVector(nombres_apellidos, directorio + "/nombres_apellidos.bin");
    guardarVector(lugares_nacimiento, directorio + "/lugares_nacimiento.bin");
    guardarVector(departamentos, directorio + "/departamentos.bin");
    guardarVector(provincias, directorio + "/provincias.bin");
    guardarVector(ciudades, directorio + "/ciudades.bin");
    guardarVector(distritos, directorio + "/distritos.bin");
    guardarVector(ubicaciones, directorio + "/ubicaciones.bin");
    guardarVector(emails, directorio + "/emails.bin");

    guardarMapa(mapa_nombres_apellidos, directorio + "/mapa_nombres_apellidos.bin");
    guardarMapa(mapa_lugares_nacimiento, directorio + "/mapa_lugares_nacimiento.bin");
    guardarMapa(mapa_departamentos, directorio + "/mapa_departamentos.bin");
    guardarMapa(mapa_provincias, directorio + "/mapa_provincias.bin");
    guardarMapa(mapa_ciudades, directorio + "/mapa_ciudades.bin");
    guardarMapa(mapa_distritos, directorio + "/mapa_distritos.bin");
    guardarMapa(mapa_ubicaciones, directorio + "/mapa_ubicaciones.bin");
    guardarMapa(mapa_emails, directorio + "/mapa_emails.bin");
}

bool Tablas::cargarTablas(const std::string& directorio) {
    if (!existeDirectorio(directorio)) {
        std::cerr << "El directorio " << directorio << " no existe.\n";
        return false;
    }
    if (!cargarVector(nombres_apellidos, directorio + "/nombres_apellidos.bin")) return false;

    if (!cargarVector(nombres_apellidos, directorio + "/nombres_apellidos.bin")) return false;
    if (!cargarVector(lugares_nacimiento, directorio + "/lugares_nacimiento.bin")) return false;
    if (!cargarVector(departamentos, directorio + "/departamentos.bin")) return false;
    if (!cargarVector(provincias, directorio + "/provincias.bin")) return false;
    if (!cargarVector(ciudades, directorio + "/ciudades.bin")) return false;
    if (!cargarVector(distritos, directorio + "/distritos.bin")) return false;
    if (!cargarVector(ubicaciones, directorio + "/ubicaciones.bin")) return false;
    if (!cargarVector(emails, directorio + "/emails.bin")) return false;

    if (!cargarMapa(mapa_nombres_apellidos, directorio + "/mapa_nombres_apellidos.bin")) return false;
    if (!cargarMapa(mapa_lugares_nacimiento, directorio + "/mapa_lugares_nacimiento.bin")) return false;
    if (!cargarMapa(mapa_departamentos, directorio + "/mapa_departamentos.bin")) return false;
    if (!cargarMapa(mapa_provincias, directorio + "/mapa_provincias.bin")) return false;
    if (!cargarMapa(mapa_ciudades, directorio + "/mapa_ciudades.bin")) return false;
    if (!cargarMapa(mapa_distritos, directorio + "/mapa_distritos.bin")) return false;
    if (!cargarMapa(mapa_ubicaciones, directorio + "/mapa_ubicaciones.bin")) return false;
    if (!cargarMapa(mapa_emails, directorio + "/mapa_emails.bin")) return false;

    return true;
}

template<typename T>
void Tablas::guardarVector(const std::vector<T>& vec, const std::string& nombre_archivo) {
    std::ofstream outfile(nombre_archivo, std::ios::binary | std::ios::trunc);
    if (!outfile.is_open()) {
        std::cerr << "No se pudo abrir el archivo " << nombre_archivo << " para guardar.\n";
        return;
    }

    uint32_t size = vec.size();
    outfile.write(reinterpret_cast<const char*>(&size), sizeof(size));

    for (const auto& item : vec) {
        uint32_t length = item.size();
        outfile.write(reinterpret_cast<const char*>(&length), sizeof(length));
        outfile.write(item.data(), length);
    }

    outfile.close();
}

template<typename T>
bool Tablas::cargarVector(std::vector<T>& vec, const std::string& nombre_archivo) {
    std::ifstream infile(nombre_archivo, std::ios::binary);
    if (!infile.is_open()) {
        std::cerr << "No se pudo abrir el archivo " << nombre_archivo << " para cargar.\n";
        return false;
    }

    uint32_t size;
    infile.read(reinterpret_cast<char*>(&size), sizeof(size));
    if (infile.fail()) {
        std::cerr << "Error al leer el tamaño del vector desde " << nombre_archivo << ".\n";
        infile.close();
        return false;
    }

    vec.clear();
    vec.reserve(size);

    for (uint32_t i = 0; i < size; ++i) {
        uint32_t length;
        infile.read(reinterpret_cast<char*>(&length), sizeof(length));
        if (infile.fail()) {
            std::cerr << "Error al leer la longitud de la cadena desde " << nombre_archivo << ".\n";
            infile.close();
            return false;
        }

        std::string item(length, '\0');
        infile.read(&item[0], length);
        if (infile.fail()) {
            std::cerr << "Error al leer la cadena desde " << nombre_archivo << ".\n";
            infile.close();
            return false;
        }

        vec.push_back(std::move(item));
    }

    infile.close();
    return true;
}

template<typename K, typename V>
void Tablas::guardarMapa(const std::unordered_map<K, V>& mapa, const std::string& nombre_archivo) {
    std::ofstream outfile(nombre_archivo, std::ios::binary | std::ios::trunc);
    if (!outfile.is_open()) {
        std::cerr << "No se pudo abrir el archivo " << nombre_archivo << " para guardar.\n";
        return;
    }

    uint32_t size = mapa.size();
    outfile.write(reinterpret_cast<const char*>(&size), sizeof(size));

    for (const auto& [key, value] : mapa) {
        uint32_t key_length = key.size();
        outfile.write(reinterpret_cast<const char*>(&key_length), sizeof(key_length));
        outfile.write(key.data(), key_length);
        outfile.write(reinterpret_cast<const char*>(&value), sizeof(value));
    }

    outfile.close();
}

template<typename K, typename V>
bool Tablas::cargarMapa(std::unordered_map<K, V>& mapa, const std::string& nombre_archivo) {
    std::ifstream infile(nombre_archivo, std::ios::binary);
    if (!infile.is_open()) {
        std::cerr << "No se pudo abrir el archivo " << nombre_archivo << " para cargar.\n";
        return false;
    }

    uint32_t size;
    infile.read(reinterpret_cast<char*>(&size), sizeof(size));
    if (infile.fail()) {
        std::cerr << "Error al leer el tamaño del mapa desde " << nombre_archivo << ".\n";
        infile.close();
        return false;
    }

    mapa.clear();

    for (uint32_t i = 0; i < size; ++i) {
        uint32_t key_length;
        infile.read(reinterpret_cast<char*>(&key_length), sizeof(key_length));
        if (infile.fail()) {
            std::cerr << "Error al leer la longitud de la clave desde " << nombre_archivo << ".\n";
            infile.close();
            return false;
        }

        std::string key(key_length, '\0');
        infile.read(&key[0], key_length);
        if (infile.fail()) {
            std::cerr << "Error al leer la clave desde " << nombre_archivo << ".\n";
            infile.close();
            return false;
        }

        V value;
        infile.read(reinterpret_cast<char*>(&value), sizeof(value));
        if (infile.fail()) {
            std::cerr << "Error al leer el valor desde " << nombre_archivo << ".\n";
            infile.close();
            return false;
        }

        mapa[key] = value;
    }

    infile.close();
    return true;
}

void Tablas::crearDirectorio(const std::string& directorio) {
    mkdir(directorio.c_str(), 0777); //Para sistemas POSIX
}

bool Tablas::existeDirectorio(const std::string& directorio) {
    struct stat info;
    if (stat(directorio.c_str(), &info) != 0) {
        return false; //No se puede ingresar
    } else if (info.st_mode & S_IFDIR) {
        return true; //Esta en el directorio
    } else {
        return false; //No esta en el directorio
    }
}