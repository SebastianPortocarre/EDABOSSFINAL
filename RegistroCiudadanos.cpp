#include "RegistroCiudadanos.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <random>
#include <string>
#include <cstring>
#include <cctype>

using namespace std;

RegistroCiudadanos::RegistroCiudadanos()
    : ciudadano_archivo_data("ciudadanos_data.bin"),
      pk_archivo_index("ciudadanos_index.bin"),
      cuckooHash(30000000, 2),
      hash_archivo("cuckoohash.bin"),
      next_data_offset(0),
      hash_modificado(false) { // Añadimos un flag para cambios en la tabla hash

    // Reservar capacidad en el vector dnis para evitar realojamientos
    dnis.reserve(33000000); // Ajusta el número según tus necesidades

    // Abrir el archivo de datos en modo append y mantenerlo abierto
    outfile_data.open(ciudadano_archivo_data, ios::binary | ios::app);
    if (!outfile_data.is_open()) {
        cerr << "No se pudo abrir el archivo " << ciudadano_archivo_data << " para escribir.\n";
        // Manejar el error según corresponda
    }

    if (!tablas.cargarTablas("tablas")) {
        std::cout << "No se pudieron cargar las tablas. Se crearán nuevas.\n";
    }

    if (!cargarDesdeArchivo()) {
        cout << "Generando 33 millones de ciudadanos aleatorios...\n";
        generarCiudadanosAleatorios(33000000);
        tablas.guardarTablas("tablas");
    } else {
        cout << "Datos cargados exitosamente desde los archivos binarios.\n";
        if (!cuckooHash.cargarDesdeArchivo(hash_archivo)) {
            cout << "No se pudo cargar la tabla hash, reconstruyéndola...\n";
            // Ahora sí necesitamos reconstruir la tabla hash
            reconstruirTablaHash();
            // Marcamos que la tabla hash ha sido modificada
            hash_modificado = true;
        }
    }
}

RegistroCiudadanos::~RegistroCiudadanos() {
    // Cerrar el archivo de datos
    if (outfile_data.is_open()) {
        outfile_data.close();
    }

    // Guardar el archivo de índice completo
    guardarEnArchivo();

    // Guardar las tablas auxiliares
    tablas.guardarTablas("tablas");

    // Guardar la tabla hash solo si ha sido modificada
    if (hash_modificado) {
        cuckooHash.guardarEnArchivo(hash_archivo);
    }
}

void RegistroCiudadanos::generarCiudadanosAleatorios(int cantidad) {
    if (cantidad <= 0) {
        cerr << "Cantidad inválida para generar ciudadanos.\n";
        return;
    }

    // Cerrar y abrir el archivo de datos en modo truncamiento para generar nuevos datos
    outfile_data.close(); // Cerrar si estaba abierto
    outfile_data.open(ciudadano_archivo_data, ios::binary | ios::trunc);
    if (!outfile_data.is_open()) {
        cerr << "No se pudo abrir el archivo " << ciudadano_archivo_data << " para escribir.\n";
        return;
    }

    random_device rd;
    mt19937 gen(rd());

    vector<string> nombres = {
        "Juan", "María", "Carlos", "Ana", "Luis", "Carmen",
        "Pedro", "Lucía", "Jorge", "Sofía", "Miguel", "Elena",
        "Andrés", "Isabel", "Fernando", "Laura", "Ricardo", "Patricia",
        "Daniel", "Claudia", "Gabriel", "Fernanda", "Santiago", "Valeria",
        "Diego", "Paula", "Roberto", "Verónica", "Sebastián", "Natalia"
    };
    vector<string> apellidos = {
        "Pérez", "González", "Rodríguez", "López", "García", "Martínez",
        "Sánchez", "Ramírez", "Torres", "Flores", "Díaz", "Morales",
        "Vásquez", "Jiménez", "Rojas", "Álvarez", "Castillo", "Vega",
        "Ortiz", "Silva", "Mendoza", "Cortez", "Ruiz", "Reyes",
        "Fernández", "Herrera", "Molina", "Navarro", "Ramos", "Romero"
    };
    vector<string> departamentos_lista = { "Amazonas", "Ancash", "Apurímac", "Arequipa", "Ayacucho", "Cajamarca", "Callao", "Cusco", "Huancavelica", "Huánuco", "Ica", "Junín", "La Libertad", "Lambayeque", "Lima", "Loreto", "Madre de Dios", "Moquegua", "Pasco", "Piura", "Puno", "San Martín", "Tacna", "Tumbes", "Ucayali"};
    vector<string> provincias_lista = { "Lima", "Arequipa", "Cusco", "Trujillo", "Piura", "Huancayo", "Huaraz", "Juliaca", "Abancay", "Tacna" };
    vector<string> ciudades_lista = { "Lima", "Arequipa", "Cusco", "Trujillo", "Piura", "Huancayo", "Huaraz", "Juliaca", "Abancay", "Tacna" };
    vector<string> distritos_lista = { "Miraflores", "San Isidro", "Cusco", "Trujillo", "Piura", "Huancayo", "Huaraz", "Juliaca", "Abancay", "Tacna" };
    vector<string> ubicaciones_lista = { "Av. Pardo 123", "Calle 50 #456", "Jr. Las Flores 789", "Av. Arequipa 321", "Calle Lima 654", "Av. Cusco 987", "Calle Trujillo 111", "Jr. Piura 222", "Av. Junín 333", "Calle Tacna 444" };
    vector<string> dominios = { "example.com", "correo.com", "mail.com", "peru.com", "hotmail.com", "gmail.com", "yahoo.com", "live.com", "outlook.com", "universidad.edu.pe" };

    // Generador para seleccionar elementos aleatorios de las listas
    uniform_int_distribution<> dis_nombres(0, nombres.size() - 1);
    uniform_int_distribution<> dis_apellidos(0, apellidos.size() - 1);
    uniform_int_distribution<> dis_departamentos(0, departamentos_lista.size() - 1);
    uniform_int_distribution<> dis_provincias(0, provincias_lista.size() - 1);
    uniform_int_distribution<> dis_ciudades(0, ciudades_lista.size() - 1);
    uniform_int_distribution<> dis_distritos(0, distritos_lista.size() - 1);
    uniform_int_distribution<> dis_ubicaciones(0, ubicaciones_lista.size() - 1);
    uniform_int_distribution<> dis_dominios(0, dominios.size() - 1);
    uniform_int_distribution<> dis_nacionalidad(0, 1);
    uniform_int_distribution<> dis_estado_civil(0, 3);
    uniform_int_distribution<uint32_t> dis_telefono(900000000, 999999999);

    // Buffer para almacenar múltiples ciudadanos antes de escribirlos al archivo
    const size_t BUFFER_SIZE = 100000; // Puedes ajustar este valor según la memoria disponible
    vector<CiudadanoOptimizado> buffer_ciudadanos;
    buffer_ciudadanos.reserve(BUFFER_SIZE);

    // Generar DNIs secuenciales dentro del rango permitido
    uint32_t dni_actual = DNI_MIN;

    next_data_offset = 0; // Inicializamos el offset al principio del archivo

    for (int i = 0; i < cantidad; ++i) {
        CiudadanoOptimizado ciudadano;

        // Asignar DNI secuencialmente
        ciudadano.dni = dni_actual++;
        dnis.push_back(ciudadano.dni);

        // Generar datos aleatorios
        string nombre = nombres[dis_nombres(gen)];
        string apellido = apellidos[dis_apellidos(gen)];
        string nombre_completo = nombre + " " + apellido;
        ciudadano.nombres_apellidos = tablas.obtenerIndiceNombreApellido(nombre_completo);

        string lugar_nacimiento = ciudades_lista[dis_ciudades(gen)];
        ciudadano.lugar_nacimiento = tablas.obtenerIndiceLugarNacimiento(lugar_nacimiento);

        ciudadano.nacionalidad = (dis_nacionalidad(gen) == 0) ? Nacionalidad::Peruano : Nacionalidad::Extranjero;

        string departamento = departamentos_lista[dis_departamentos(gen)];
        ciudadano.direccion.departamento = tablas.obtenerIndiceDepartamento(departamento);
        string provincia = provincias_lista[dis_provincias(gen)];
        ciudadano.direccion.provincia = tablas.obtenerIndiceProvincia(provincia);
        string ciudad = ciudades_lista[dis_ciudades(gen)];
        ciudadano.direccion.ciudad = tablas.obtenerIndiceCiudad(ciudad);
        string distrito = distritos_lista[dis_distritos(gen)];
        ciudadano.direccion.distrito = tablas.obtenerIndiceDistrito(distrito);
        string ubicacion = ubicaciones_lista[dis_ubicaciones(gen)];
        ciudadano.direccion.ubicacion = tablas.obtenerIndiceUbicacion(ubicacion);

        ciudadano.telefono = dis_telefono(gen);

        string email = generarEmailAleatorio(nombre + apellido, dominios[dis_dominios(gen)]);
        ciudadano.email = tablas.obtenerIndiceEmail(email);

        ciudadano.estado_civil = static_cast<EstadoCivil>(dis_estado_civil(gen));

        // Agregar ciudadano al buffer
        buffer_ciudadanos.push_back(ciudadano);
        // Actualizar next_data_offset para cada ciudadano
        uint32_t current_offset = next_data_offset + i * sizeof(CiudadanoOptimizado);

        // Insertar en la tabla hash
        cuckooHash.insertar(ciudadano.dni, current_offset);

        // Agregar el DNI al índice incrementalmente
        guardarEnArchivoIncremental(ciudadano.dni, current_offset);

        // Cuando el buffer alcanza el tamaño definido, escribir al archivo
        if (buffer_ciudadanos.size() == BUFFER_SIZE) {
            outfile_data.write(reinterpret_cast<const char*>(buffer_ciudadanos.data()),
                               buffer_ciudadanos.size() * sizeof(CiudadanoOptimizado));
            buffer_ciudadanos.clear();
        }

        if ((i + 1) % 1000000 == 0) {
            cout << (i + 1) << " ciudadanos generados.\n";
        }
    }

    // Escribir cualquier ciudadano restante en el buffer
    if (!buffer_ciudadanos.empty()) {
        outfile_data.write(reinterpret_cast<const char*>(buffer_ciudadanos.data()),
                           buffer_ciudadanos.size() * sizeof(CiudadanoOptimizado));
        buffer_ciudadanos.clear();
    }

    outfile_data.flush(); // Asegurar que los datos se escriban en disco

    // Actualizar next_data_offset
    next_data_offset += cantidad * sizeof(CiudadanoOptimizado);

    cout << "Generación de ciudadanos aleatorios completada.\n";
}

CiudadanoOptimizado* RegistroCiudadanos::buscarCiudadano(uint32_t dni) {
    uint32_t offset;
    if (cuckooHash.buscar(dni, offset)) {
        ifstream infile_data(ciudadano_archivo_data, ios::binary);
        if (!infile_data.is_open()) {
            cerr << "No se pudo abrir el archivo " << ciudadano_archivo_data << " para leer.\n";
            return nullptr;
        }

        // Mover el cursor al offset
        infile_data.seekg(offset, ios::beg);

        // Leer la estructura del ciudadano
        CiudadanoOptimizado* ciudadano = new CiudadanoOptimizado();
        infile_data.read(reinterpret_cast<char*>(ciudadano), sizeof(CiudadanoOptimizado));

        if (infile_data.fail()) {
            cerr << "Error al leer los datos del ciudadano con DNI " << dni << ".\n";
            delete ciudadano;
            infile_data.close();
            return nullptr;
        }

        infile_data.close();
        return ciudadano;
    } else {
        cout << "Ciudadano no encontrado.\n";
        return nullptr;
    }
}

void RegistroCiudadanos::insertarCiudadanoManual() {
    auto start_time = std::chrono::high_resolution_clock::now();

    CiudadanoOptimizado ciudadano;

    ciudadano.dni = leerEntero("Ingrese su número de DNI: ", DNI_MIN, DNI_MAX);
    uint32_t offset;
    if (cuckooHash.buscar(ciudadano.dni, offset)) {
        cerr << "Error: El DNI ya existe.\n";
        return;
    }

    ciudadano.nombres_apellidos = tablas.obtenerIndiceNombreApellido(leerTexto("Ingrese el Nombre: ") + " " + leerTexto("Ingrese el Apellido: "));

    ciudadano.lugar_nacimiento = tablas.obtenerIndiceLugarNacimiento(leerTexto("Ingrese el Lugar de Nacimiento: "));

    ciudadano.nacionalidad = (leerEntero("Ingrese su nacionalidad, 0->Peruano, 1->Extranjero: ", 0, 1) == 0) ? Nacionalidad::Peruano : Nacionalidad::Extranjero;

    vector<string> departamentos_lista = {"Amazonas", "Ancash", "Apurímac", "Arequipa", "Ayacucho", "Cajamarca", "Callao", "Cusco", "Huancavelica", "Huanuco", "Ica", "Junín", "La Libertad", "Lambayeque", "Lima", "Loreto", "Madre de Dios", "Moquegua", "Pasco", "Piura", "Puno", "San Martín", "Tacna", "Tumbes", "Ucayali"};
    ciudadano.direccion.departamento = tablas.obtenerIndiceDepartamento(leerTexto("Ingrese el Departamento: ", departamentos_lista));

    vector<string> provincias_lista = {"Lima", "Arequipa", "Cusco", "Trujillo", "Piura", "Huancayo", "Huaraz", "Juliaca", "Abancay", "Tacna" };
    ciudadano.direccion.provincia = tablas.obtenerIndiceProvincia(leerTexto("Ingrese la Provincia: ", provincias_lista));

    vector<string> ciudades_lista = {"Lima", "Arequipa", "Cusco", "Trujillo", "Piura", "Huancayo", "Huaraz", "Juliaca", "Abancay", "Tacna"};
    ciudadano.direccion.ciudad = tablas.obtenerIndiceCiudad(leerTexto("Ingrese la Ciudad: ", ciudades_lista));

    ciudadano.direccion.distrito = tablas.obtenerIndiceDistrito(leerTexto("Ingrese el Distrito: "));
    ciudadano.direccion.ubicacion = tablas.obtenerIndiceUbicacion(leerTexto("Ingrese la Ubicación: "));

    ciudadano.telefono = leerEntero("Ingrese su número de teléfono, entre 900000000 - 999999999: ", 900000000, 999999999);

    string email = leerEmail("Ingrese el Email: ");
    ciudadano.email = tablas.obtenerIndiceEmail(email);

    ciudadano.estado_civil = static_cast<EstadoCivil>(leerEntero("Ingrese su estado civil, 0->Soltero, 1->Casado, 2->Divorciado, 3->Viudo: ", 0, 3));

    if (!outfile_data.is_open()) {
        cerr << "El archivo de datos no está abierto.\n";
        return;
    }

    // Obtener el offset actual (posición al final del archivo)
    outfile_data.seekp(0, ios::end);
    uint32_t current_offset = outfile_data.tellp();

    // Escribir los datos serializados al archivo de datos
    outfile_data.write(reinterpret_cast<const char*>(&ciudadano), sizeof(CiudadanoOptimizado));
    outfile_data.flush(); // Asegurar que los datos se escriban en disco

    cuckooHash.insertar(ciudadano.dni, current_offset);
    hash_modificado = true;

    // Agregar el DNI a la lista
    dnis.push_back(ciudadano.dni);

    // Agregar el nuevo registro al índice de forma incremental
    guardarEnArchivoIncremental(ciudadano.dni, current_offset);

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end_time - start_time;
    std::cout << "Ciudadano insertado correctamente en " << elapsed.count() << " segundos.\n";
}

bool RegistroCiudadanos::eliminarCiudadano(uint32_t dni) {
    uint32_t offset;
    if (cuckooHash.buscar(dni, offset)) {
        cuckooHash.eliminar(dni);
        hash_modificado = true;

        // Eliminar el DNI de la lista de DNIs en memoria
        auto it = std::find(dnis.begin(), dnis.end(), dni);
        if (it != dnis.end()) {
            dnis.erase(it);
        }

        std::cout << "Ciudadano con DNI " << dni << " eliminado correctamente.\n";
        return true;
    } else {
        std::cout << "Ciudadano con DNI " << dni << " no encontrado.\n";
        return false;
    }
}

bool RegistroCiudadanos::cargarDesdeArchivo() {
    // Asegúrate de abrir el archivo de índice correcto
    ifstream infile_index(pk_archivo_index, ios::binary);
    if (!infile_index.is_open()) {
        cerr << "Archivo de índice no encontrado. Se generarán nuevos datos.\n";
        return false;
    }

    // Leer todos los pares dni y offset
    dnis.clear(); // Asegurarse de que esté vacío
    uint32_t dni, offset;
    while (infile_index.read(reinterpret_cast<char*>(&dni), sizeof(dni))) {
        infile_index.read(reinterpret_cast<char*>(&offset), sizeof(offset));

        if (infile_index.fail()) {
            cerr << "Error al leer el índice.\n";
            infile_index.close();
            return false;
        }

        dnis.push_back(dni);

        // Actualizar next_data_offset si es necesario
        if (offset + sizeof(CiudadanoOptimizado) > next_data_offset) {
            next_data_offset = offset + sizeof(CiudadanoOptimizado);
        }
    }

    infile_index.close();

    // Asegurarse de que el archivo de datos esté abierto en modo append
    if (!outfile_data.is_open()) {
        outfile_data.open(ciudadano_archivo_data, ios::binary | ios::app);
    }

    cout << "Cargados " << dnis.size() << " registros desde los archivos binarios.\n";
    return true;
}

bool RegistroCiudadanos::guardarEnArchivo() {
    // Reescribir el archivo de índice completo
    ofstream outfile_index(pk_archivo_index, ios::binary | ios::trunc);
    if (!outfile_index.is_open()) {
        cerr << "No se pudo abrir el archivo " << pk_archivo_index << " para escribir el índice.\n";
        return false;
    }

    for (uint32_t dni : dnis) {
        uint32_t offset;
        if (cuckooHash.buscar(dni, offset)) {
            outfile_index.write(reinterpret_cast<const char*>(&dni), sizeof(dni));
            outfile_index.write(reinterpret_cast<const char*>(&offset), sizeof(offset));
        }
    }

    outfile_index.close();

    cout << "Índice guardado correctamente en " << pk_archivo_index << ".\n";
    return true;
}

bool RegistroCiudadanos::guardarEnArchivoIncremental(uint32_t dni, uint32_t offset) {
    ofstream outfile_index(pk_archivo_index, ios::binary | ios::app);
    if (!outfile_index.is_open()) {
        cerr << "No se pudo abrir el archivo " << pk_archivo_index << " para escribir el índice.\n";
        return false;
    }

    outfile_index.write(reinterpret_cast<const char*>(&dni), sizeof(dni));
    outfile_index.write(reinterpret_cast<const char*>(&offset), sizeof(offset));
    outfile_index.close();

    return true;
}

// Resto del código permanece igual
// Función para imprimir tres DNIs aleatorios
void RegistroCiudadanos::imprimirTresDniAleatorios() {
    if (dnis.empty()) {
        cerr << "No hay ciudadanos registrados.\n";
        return;
    }
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<size_t> dis(0, dnis.size() - 1);
    cout << "Tres DNIs aleatorios:\n";
    for (int i = 0; i < 3; ++i) {
        size_t idx = dis(gen);
        cout << (i + 1) << ". " << dnis[idx] << "\n";
    }
}

// Función para exportar los datos a un archivo CSV
void RegistroCiudadanos::exportarACSV(const string& nombre_archivo) {
    cout << "Iniciando exportación a CSV: " << nombre_archivo << "\n";
    ofstream archivo_csv(nombre_archivo);
    if (!archivo_csv.is_open()) {
        cerr << "No se pudo abrir el archivo " << nombre_archivo << " para escribir.\n";
        return;
    }
    // Escribir la cabecera del CSV
    archivo_csv << "DNI,NombreApellido,LugarNacimiento,Nacionalidad,Departamento,Provincia,Ciudad,Distrito,Ubicacion,Telefono,Email,EstadoCivil\n";

    for (uint32_t dni : dnis) {
        CiudadanoOptimizado* ciudadano = buscarCiudadano(dni);
        if (ciudadano) {
            // Verificar índices y escribir los datos
            if (ciudadano->nombres_apellidos >= tablas.nombres_apellidos.size() ||
                ciudadano->lugar_nacimiento >= tablas.lugares_nacimiento.size() ||
                ciudadano->direccion.departamento >= tablas.departamentos.size() ||
                ciudadano->direccion.provincia >= tablas.provincias.size() ||
                ciudadano->direccion.ciudad >= tablas.ciudades.size() ||
                ciudadano->direccion.distrito >= tablas.distritos.size() ||
                ciudadano->direccion.ubicacion >= tablas.ubicaciones.size() ||
                ciudadano->email >= tablas.emails.size()) {
                cerr << "Error: Índices fuera de rango para el ciudadano con DNI " << ciudadano->dni << ".\n";
                delete ciudadano;
                continue; // Saltar este registro
            }
            archivo_csv << ciudadano->dni << ","
                       << "\"" << tablas.nombres_apellidos[ciudadano->nombres_apellidos] << "\","
                       << "\"" << tablas.lugares_nacimiento[ciudadano->lugar_nacimiento] << "\","
                       << "\"" << ((ciudadano->nacionalidad == Nacionalidad::Peruano) ? "Peruano" : "Extranjero") << "\","
                       << "\"" << tablas.departamentos[ciudadano->direccion.departamento] << "\","
                       << "\"" << tablas.provincias[ciudadano->direccion.provincia] << "\","
                       << "\"" << tablas.ciudades[ciudadano->direccion.ciudad] << "\","
                       << "\"" << tablas.distritos[ciudadano->direccion.distrito] << "\","
                       << "\"" << tablas.ubicaciones[ciudadano->direccion.ubicacion] << "\","
                       << ciudadano->telefono << ","
                       << "\"" << tablas.emails[ciudadano->email] << "\","
                       << "\"" << ((ciudadano->estado_civil == EstadoCivil::Soltero) ? "Soltero" :
                                   (ciudadano->estado_civil == EstadoCivil::Casado) ? "Casado" :
                                   (ciudadano->estado_civil == EstadoCivil::Divorciado) ? "Divorciado" :
                                   "Viudo") << "\"\n";

            delete ciudadano;
        }
    }

    archivo_csv.close();
    cout << "Datos exportados correctamente a " << nombre_archivo << ".\n";
}

string RegistroCiudadanos::generarEmailAleatorio(const string& nombre, const string& dominio) {
    string email = nombre;
    // Reemplazar espacios con puntos y convertir a minúsculas
    replace(email.begin(), email.end(), ' ', '.');
    transform(email.begin(), email.end(), email.begin(), [](unsigned char c){ return tolower(c); });
    email += "@" + dominio;
    return email;
}

string RegistroCiudadanos::leerEmail(const string& mensaje) {
    string email;

    while (true) {
        cout << mensaje;
        getline(cin, email);

        // Validar que el email tenga al menos un '@' y un dominio
        size_t atPos = email.find('@');
        if (atPos == string::npos || atPos == 0 || atPos == email.length() - 1) {
            cout << "Entrada inválida: el correo debe contener '@' y un dominio.\n";
            continue;
        }

        // Validar el dominio
        string dominio = email.substr(atPos + 1);
        vector<string> dominiosPermitidos = {
            "example.com", "correo.com", "mail.com", "peru.com",
            "hotmail.com", "gmail.com", "yahoo.com", "live.com",
            "outlook.com", "universidad.edu.pe"
        };

        if (find(dominiosPermitidos.begin(), dominiosPermitidos.end(), dominio) != dominiosPermitidos.end()) {
            return email; // Retorna el email si es válido
        } else {
            cout << "Correo inválido: el dominio no está permitido.\n";
        }
    }
}

uint32_t RegistroCiudadanos::leerEntero(const string& mensaje, uint32_t min, uint32_t max) {
    uint32_t numero;

    while (true) {
        cout << mensaje;
        cin >> numero;

        // Verificar si la entrada es numérica y está en el rango
        if (!cin.fail() && numero >= min && numero <= max) {
            // Si es válida, limpiar el buffer y retornar el número
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return numero;
        } else {
            cin.clear(); // Limpiar el estado de error
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Ignorar el resto de la línea
            cout << "Entrada inválida. Por favor, ingrese un número entre " << min << " y " << max << ".\n";
        }
    }
}

string RegistroCiudadanos::leerTexto(const string& mensaje) {
    string entrada;

    while (true) {
        cout << mensaje;
        getline(cin, entrada);

        // Validar que solo contenga letras y espacios
        bool esValido = !entrada.empty() && all_of(entrada.begin(), entrada.end(), [](char c) {
            return isalpha(static_cast<unsigned char>(c)) || isspace(static_cast<unsigned char>(c));
        });

        if (esValido) {
            return entrada; // Retornar el texto válido
        } else {
            cout << "Entrada inválida. Por favor, ingrese solo letras y espacios.\n";
        }
    }
}

string RegistroCiudadanos::leerTexto(const string& mensaje, const vector<string>& opciones) {
    Sugeridor sugeridor;
    string entrada;

    while (true) {
        cout << mensaje;
        getline(cin, entrada);

        bool esValido = !entrada.empty() && all_of(entrada.begin(), entrada.end(), [](char c) {
            return isalpha(static_cast<unsigned char>(c)) || isspace(static_cast<unsigned char>(c));
        });

        if (esValido) {
            // Si la entrada es válida, buscar la sugerencia más cercana
            string sugerencia = sugeridor.sugerir(entrada, opciones);

            if (sugerencia != entrada) {
                cout << "¿Quisiste decir \"" << sugerencia << "\"? (s/n): ";
                char respuesta;
                cin >> respuesta;
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Limpiar el buffer
                if (tolower(respuesta) == 's') {
                    entrada = sugerencia;
                }
            }

            return entrada;
        } else {
            cout << "Entrada inválida. Por favor, ingrese solo letras y espacios.\n";
        }
    }
}

void RegistroCiudadanos::reconstruirTablaHash() {
    cout << "Reconstruyendo la tabla hash desde el índice...\n";

    // Abrir el archivo de índice para leer los DNIs y offsets
    ifstream infile_index(pk_archivo_index, ios::binary);
    if (!infile_index.is_open()) {
        cerr << "No se pudo abrir el archivo " << pk_archivo_index << " para leer el índice.\n";
        return;
    }

    uint32_t num_registros;
    infile_index.read(reinterpret_cast<char*>(&num_registros), sizeof(num_registros));

    for (uint32_t i = 0; i < num_registros; ++i) {
        uint32_t dni;
        uint32_t offset;
        infile_index.read(reinterpret_cast<char*>(&dni), sizeof(dni));
        infile_index.read(reinterpret_cast<char*>(&offset), sizeof(offset));

        if (infile_index.fail()) {
            cerr << "Error al leer el índice para el registro " << i + 1 << ".\n";
            infile_index.close();
            return;
        }

        cuckooHash.insertar(dni, offset);
    }

    infile_index.close();
    cout << "Tabla hash reconstruida correctamente.\n";
}
