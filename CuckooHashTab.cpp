#include "CuckooHashTab.h"
#include <iostream>
#include <algorithm>
#include <fstream>

void CuckooHashTab::guardarEnArchivo(const std::string& filename) const {
    std::ofstream outfile(filename, std::ios::binary | std::ios::trunc);
    if (!outfile.is_open()) {
        std::cerr << "No se pudo abrir el archivo " << filename << " para guardar la tabla hash.\n";
        return;
    }

    // Guardar los parámetros de la tabla hash
    outfile.write(reinterpret_cast<const char*>(&size), sizeof(size));
    outfile.write(reinterpret_cast<const char*>(&num_tables), sizeof(num_tables));
    outfile.write(reinterpret_cast<const char*>(&seed1), sizeof(seed1));
    outfile.write(reinterpret_cast<const char*>(&seed2), sizeof(seed2));

    // Guardar las tablas
    for (int i = 0; i < num_tables; ++i) {
        for (int j = 0; j < size; ++j) {
            const Entry& entry = tables[i][j];
            outfile.write(reinterpret_cast<const char*>(&entry.dni), sizeof(entry.dni));
            outfile.write(reinterpret_cast<const char*>(&entry.offset), sizeof(entry.offset));
        }
    }

    outfile.close();
}

bool CuckooHashTab::cargarDesdeArchivo(const std::string& filename) {
    std::ifstream infile(filename, std::ios::binary);
    if (!infile.is_open()) {
        std::cerr << "No se pudo abrir el archivo " << filename << " para cargar la tabla hash.\n";
        return false;
    }

    // Leer los parámetros de la tabla hash
    infile.read(reinterpret_cast<char*>(&size), sizeof(size));
    infile.read(reinterpret_cast<char*>(&num_tables), sizeof(num_tables));
    infile.read(reinterpret_cast<char*>(&seed1), sizeof(seed1));
    infile.read(reinterpret_cast<char*>(&seed2), sizeof(seed2));

    // Inicializar las tablas
    tables.resize(num_tables);
    for (int i = 0; i < num_tables; ++i) {
        tables[i].resize(size);
    }

    // Leer las tablas
    for (int i = 0; i < num_tables; ++i) {
        for (int j = 0; j < size; ++j) {
            Entry& entry = tables[i][j];
            infile.read(reinterpret_cast<char*>(&entry.dni), sizeof(entry.dni));
            infile.read(reinterpret_cast<char*>(&entry.offset), sizeof(entry.offset));
        }
    }

    infile.close();
    return true;
}
// Implementación de MurmurHash3 para uint32_t
uint32_t CuckooHashTab::murmur3_32(uint32_t key, uint32_t seed) const {
    key ^= seed;
    key ^= key >> 16;
    key *= 0x85ebca6b;
    key ^= key >> 13;
    key *= 0xc2b2ae35;
    key ^= key >> 16;
    return key;
}

// Constructor con semillas únicas y tablas inicializadas a mayor tamaño
CuckooHashTab::CuckooHashTab(int initial_size, int num_tables)
    : size(initial_size), num_tables(num_tables) {
    tables.resize(num_tables, std::vector<Entry>(size, Entry{0, 0}));
    seed1 = 0xA1B2C3D4;
    seed2 = 0xD4C3B2A1;
}

// Funciones de hash únicas para cada tabla
size_t CuckooHashTab::hash1(uint32_t dni) const {
    return murmur3_32(dni, seed1) % size;
}

size_t CuckooHashTab::hash2(uint32_t dni) const {
    return murmur3_32(dni, seed2) % size;
}

// Insertar: utiliza una sola variante de hash por tabla
void CuckooHashTab::insertar(uint32_t dni, uint32_t offset) {
    Entry entry = { dni, offset };
    Entry temp_entry = entry;

    for (int count = 0; count < rehash_limit; ++count) {
        for (int i = 0; i < num_tables; ++i) {
            int h = (i == 0) ? hash1(temp_entry.dni) : hash2(temp_entry.dni);
            if (tables[i][h].dni == 0) {
                tables[i][h] = temp_entry;
                return;
            }
            std::swap(temp_entry, tables[i][h]);
        }
    }

    std::cerr << "Rehash limit reached. Performing rehashing...\n";
    rehash();
    insertar(dni, offset);
}

// Buscar sin variantes adicionales de hash
bool CuckooHashTab::buscar(uint32_t dni, uint32_t& offset) const {
    for (int i = 0; i < num_tables; ++i) {
        int h = (i == 0) ? hash1(dni) : hash2(dni);
        if (tables[i][h].dni == dni) {
            offset = tables[i][h].offset;
            return true;
        }
    }
    return false;
}

// Eliminar
void CuckooHashTab::eliminar(uint32_t dni) {
    for (int i = 0; i < num_tables; ++i) {
        int h = (i == 0) ? hash1(dni) : hash2(dni);
        if (tables[i][h].dni == dni) {
            tables[i][h].dni = 0;
            tables[i][h].offset = 0;
            return;
        }
    }
    std::cerr << "Error: DNI " << dni << " no encontrado para eliminar.\n";
}

// Rehashing optimizado
void CuckooHashTab::rehash() {
    std::cout << "Rehashing tables... Current size per table: " << size
              << " -> New size per table: " << size * 1.5 << "\n";
    size = static_cast<int>(size * 1.5);

    std::vector<std::vector<Entry>> new_tables(num_tables, std::vector<Entry>(size, Entry{0, 0}));

    for (int i = 0; i < num_tables; ++i) {
        for (const auto& entry : tables[i]) {
            if (entry.dni != 0) {
                bool inserted = false;
                Entry temp_entry = entry;

                for (int count = 0; count < rehash_limit; ++count) {
                    for (int j = 0; j < num_tables; ++j) {
                        int h = (j == 0) ? hash1(temp_entry.dni) : hash2(temp_entry.dni);
                        if (new_tables[j][h].dni == 0) {
                            new_tables[j][h] = temp_entry;
                            inserted = true;
                            break;
                        }
                        std::swap(temp_entry, new_tables[j][h]);
                    }
                    if (inserted) break;
                }

                if (!inserted) {
                    std::cerr << "Fallo en reinsertar DNI " << entry.dni << " en el rehashing.\n";
                }
            }
        }
    }

    tables = std::move(new_tables);
}
