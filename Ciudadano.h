#ifndef CIUDADANO_H
#define CIUDADANO_H

#include <cstdint>

enum class Nacionalidad : uint8_t { Peruano = 0, Extranjero = 1 };
enum class EstadoCivil : uint8_t { Soltero = 0, Casado = 1, Divorciado = 2, Viudo = 3 };

struct DireccionCodificada {
    uint32_t ubicacion;
    uint16_t provincia;
    uint16_t ciudad;
    uint16_t distrito;
    uint16_t departamento;
} __attribute__((packed));

struct CiudadanoOptimizado {
    DireccionCodificada direccion;
    uint32_t dni;
    uint32_t nombres_apellidos;
    uint32_t lugar_nacimiento;
    uint32_t telefono;
    uint32_t email;
    EstadoCivil estado_civil ;
    Nacionalidad nacionalidad;
} __attribute__((packed));

#endif