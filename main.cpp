#include <iostream>
#include "RegistroCiudadanos.h"
using namespace std;

void limpiarPantalla() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void mostrarMenu() {
    cout << "===========================\n";
    cout << "  Registro de Ciudadanos   \n";
    cout << "===========================\n";
    cout << "1. Buscar ciudadano por DNI\n";
    cout << "2. Insertar nuevo ciudadano\n";
    cout << "3. Eliminar ciudadano por DNI\n";
    cout << "4. Imprimir tres DNIs aleatorios\n";
    cout << "5. Exportar datos a CSV\n";
    cout << "6. Salir\n";
    cout << "===========================\n";
    cout << "Seleccione una opción: ";
}

int main() {
    RegistroCiudadanos registro;
    int opcion;
    uint32_t dni;
    string nombre_archivo;

    do {
        mostrarMenu();
        cin >> opcion;

        switch (opcion) {
            case 1: {
                cout << "Ingrese el DNI a buscar: ";
                cin >> dni;
                CiudadanoOptimizado* ciudadano = registro.buscarCiudadano(dni);
                if (ciudadano) {
                    cout << "===========================\n";
                    cout << "DNI: " << ciudadano->dni << "\n";
                    cout << "Nombre y Apellido: " << registro.tablas.nombres_apellidos[ciudadano->nombres_apellidos] << "\n";
                    cout << "Lugar de Nacimiento: " << registro.tablas.lugares_nacimiento[ciudadano->lugar_nacimiento] << "\n";
                    cout << "Nacionalidad: " << ((ciudadano->nacionalidad == Nacionalidad::Peruano) ? "Peruano" : "Extranjero") << "\n";
                    cout << "Departamento: " << registro.tablas.departamentos[ciudadano->direccion.departamento] << "\n";
                    cout << "Provincia: " << registro.tablas.provincias[ciudadano->direccion.provincia] << "\n";
                    cout << "Ciudad: " << registro.tablas.ciudades[ciudadano->direccion.ciudad] << "\n";
                    cout << "Distrito: " << registro.tablas.distritos[ciudadano->direccion.distrito] << "\n";
                    cout << "Ubicación: " << registro.tablas.ubicaciones[ciudadano->direccion.ubicacion] << "\n";
                    cout << "Teléfono: " << ciudadano->telefono << "\n";
                    cout << "Email: " << registro.tablas.emails[ciudadano->email] << "\n";
                    cout << "Estado Civil: ";
                    switch (ciudadano->estado_civil) {
                        case EstadoCivil::Soltero: cout << "Soltero"; break;
                        case EstadoCivil::Casado: cout << "Casado"; break;
                        case EstadoCivil::Divorciado: cout << "Divorciado"; break;
                        case EstadoCivil::Viudo: cout << "Viudo"; break;
                    }
                    cout << "\n===========================\n";
                    delete ciudadano;
                } else {
                    cout << "Ciudadano no encontrado.\n";
                }
                break;
            }
            case 2: {
                registro.insertarCiudadanoManual();
                break;
            }
            case 3: {
                cout << "Ingrese el DNI a eliminar: ";
                cin >> dni;
                registro.eliminarCiudadano(dni);
                break;
            }
            case 4: {
                registro.imprimirTresDniAleatorios();
                break;
            }
            case 5: {
                cout << "Ingrese el nombre del archivo CSV (ej. datos.csv): ";
                cin >> nombre_archivo;
                registro.exportarACSV(nombre_archivo);
                break;
            }
            case 6: {
                cout << "Saliendo del programa. ¡Hasta luego!\n";
                break;
            }
            default:
                cout << "Opción no válida. Intente nuevamente.\n";
        }

        if (opcion != 6 && opcion != 4) {
            cout << "\nPresione Enter para continuar...";
            cin.ignore();
            cin.get(); // Espera a que el usuario presione Enter
            limpiarPantalla();
        }

    } while (opcion != 6);

    return 0;
}
