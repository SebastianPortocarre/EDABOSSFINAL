#ifndef TABLAS_H
#define TABLAS_H

#include <vector>
#include <string>
#include <unordered_map>

class Tablas {
public:
    std::vector<std::string> nombres_apellidos;
    std::vector<std::string> lugares_nacimiento;
    std::vector<std::string> departamentos;
    std::vector<std::string> provincias;
    std::vector<std::string> ciudades;
    std::vector<std::string> distritos;
    std::vector<std::string> ubicaciones;
    std::vector<std::string> emails;

    std::unordered_map<std::string, uint32_t> mapa_nombres_apellidos;
    std::unordered_map<std::string, uint32_t> mapa_lugares_nacimiento;
    std::unordered_map<std::string, uint8_t> mapa_departamentos;
    std::unordered_map<std::string, uint16_t> mapa_provincias;
    std::unordered_map<std::string, uint16_t> mapa_ciudades;
    std::unordered_map<std::string, uint16_t> mapa_distritos;
    std::unordered_map<std::string, uint32_t> mapa_ubicaciones;
    std::unordered_map<std::string, uint32_t> mapa_emails;

    uint32_t obtenerIndiceNombreApellido(const std::string& nombre_apellido);
    uint32_t obtenerIndiceLugarNacimiento(const std::string& lugar_nacimiento);
    uint8_t obtenerIndiceDepartamento(const std::string& departamento);
    uint16_t obtenerIndiceProvincia(const std::string& provincia);
    uint16_t obtenerIndiceCiudad(const std::string& ciudad);
    uint16_t obtenerIndiceDistrito(const std::string& distrito);
    uint32_t obtenerIndiceUbicacion(const std::string& ubicacion);
    uint32_t obtenerIndiceEmail(const std::string& email);

    void guardarTablas(const std::string& directorio);
    bool cargarTablas(const std::string& directorio);

private:
    template<typename T>
    void guardarVector(const std::vector<T>& vec, const std::string& nombre_archivo);

    template<typename T>
    bool cargarVector(std::vector<T>& vec, const std::string& nombre_archivo);

    template<typename K, typename V>
    void guardarMapa(const std::unordered_map<K, V>& mapa, const std::string& nombre_archivo);

    template<typename K, typename V>
    bool cargarMapa(std::unordered_map<K, V>& mapa, const std::string& nombre_archivo);

    void crearDirectorio(const std::string& directorio);
    bool existeDirectorio(const std::string& directorio);
};

#endif