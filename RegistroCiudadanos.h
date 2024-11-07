#ifndef REGISTROCIUDADANOS_H
#define REGISTROCIUDADANOS_H

#include <cstdint>
#include <string>
#include <vector>
#include <fstream>
#include "CuckooHashTab.h"
#include "Ciudadano.h"
#include "Tablas.h"
#include "Sugeridor.h"

class RegistroCiudadanos {
public:
    RegistroCiudadanos();
    ~RegistroCiudadanos();

    void generarCiudadanosAleatorios(int cantidad);
    void insertarCiudadanoManual();
    CiudadanoOptimizado* buscarCiudadano(uint32_t dni);
    bool eliminarCiudadano(uint32_t dni);
    void imprimirTresDniAleatorios();
    void exportarACSV(const std::string& nombre_archivo);

    Tablas tablas;
    std::string hash_archivo;

private:
    CuckooHashTab cuckooHash;
    std::vector<uint32_t> dnis;

    static const uint32_t DNI_MIN = 10000000;
    static const uint32_t DNI_MAX = 99999999;

    std::string ciudadano_archivo_data;
    std::string pk_archivo_index;
    uint32_t next_data_offset;

    // Archivo de datos mantenido abierto durante la ejecución
    std::ofstream outfile_data;

    // Métodos auxiliares
    uint32_t leerEntero(const std::string& mensaje, uint32_t min, uint32_t max);
    std::string leerTexto(const std::string& mensaje);
    std::string leerEmail(const std::string& mensaje);
    std::string leerTexto(const std::string& mensaje, const std::vector<std::string>& opciones);
    std::string generarEmailAleatorio(const std::string& nombre, const std::string& dominio);
    void reconstruirTablaHash();
    bool hash_modificado;

    bool cargarDesdeArchivo();
    bool guardarEnArchivo(); // Guarda todo el índice
    bool guardarEnArchivoIncremental(uint32_t dni, uint32_t offset); // Agrega un registro al índice
};

#endif
