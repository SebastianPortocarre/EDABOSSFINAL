#include "CuckooHashTab.h"
#include <iostream>
#include <algorithm>

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

CuckooHashTab::CuckooHashTab(int initial_size, int num_tables)
    : size(initial_size), num_tables(num_tables) {
    tables.resize(num_tables, std::vector<Entry>(size, Entry{0, 0}));
    seed1 = 0xA1B2C3D4;
    seed2 = 0xD4C3B2A1;
    seed3 = 0xB1A2C3D4;
    seed4 = 0xC3D2A1B4;
}

size_t CuckooHashTab::hash1(uint32_t dni) const {
    return murmur3_32(dni, seed1) % size;
}

size_t CuckooHashTab::hash2(uint32_t dni) const {
    return murmur3_32(dni, seed2) % size;
}

size_t CuckooHashTab::hash3(uint32_t dni) const {
    return murmur3_32(dni, seed3) % size;
}

size_t CuckooHashTab::hash4(uint32_t dni) const {
    return murmur3_32(dni, seed4) % size;
}

int CuckooHashTab::hash(int table_idx, uint32_t dni, int hash_variant) const {
    if (table_idx == 0) {
        return hash_variant == 0 ? hash1(dni) : hash3(dni);
    } else if (table_idx == 1) {
        return hash_variant == 0 ? hash2(dni) : hash4(dni);
    }
    return 0;
}

void CuckooHashTab::insertar(uint32_t dni, uint32_t offset) {
    Entry entry = { dni, offset };
    Entry temp_entry = entry;

    for (int count = 0; count < rehash_limit; ++count) {
        for (int i = 0; i < num_tables; ++i) {
            for (int variant = 0; variant < 2; ++variant) { // Dos variantes de hash por tabla
                int h = hash(i, temp_entry.dni, variant);
                if (tables[i][h].dni == 0) {
                    tables[i][h] = temp_entry;
                    return;
                }
                std::swap(temp_entry, tables[i][h]);
            }
        }
    }

    std::cerr << "Rehash limit reached. Performing rehashing...\n";
    rehash();
    insertar(dni, offset);
}

bool CuckooHashTab::buscar(uint32_t dni, uint32_t& offset) const {
    for (int i = 0; i < num_tables; ++i) {
        for (int variant = 0; variant < 2; ++variant) { // Buscar en ambas variantes de hash
            int h = hash(i, dni, variant);
            if (tables[i][h].dni == dni) {
                offset = tables[i][h].offset;
                return true;
            }
        }
    }
    return false;
}

void CuckooHashTab::eliminar(uint32_t dni) {
    for (int i = 0; i < num_tables; ++i) {
        for (int variant = 0; variant < 2; ++variant) { // Eliminar en ambas variantes de hash
            int h = hash(i, dni, variant);
            if (tables[i][h].dni == dni) {
                tables[i][h].dni = 0;
                tables[i][h].offset = 0;
                return;
            }
        }
    }
    std::cerr << "Error: DNI " << dni << " no encontrado para eliminar.\n";
}

void CuckooHashTab::rehash() {
    std::cout << "Rehashing tables... Current size per table: " << size
              << " -> New size per table: " << size * 1.5 << "\n";
    size *= 1.5;

    std::vector<std::vector<Entry>> new_tables(num_tables, std::vector<Entry>(size, Entry{0, 0}));

    for (int i = 0; i < num_tables; ++i) {
        for (const auto& entry : tables[i]) {
            if (entry.dni != 0) {
                bool inserted = false;
                Entry temp_entry = entry;

                for (int count = 0; count < rehash_limit; ++count) {
                    for (int j = 0; j < num_tables; ++j) {
                        for (int variant = 0; variant < 2; ++variant) {
                            int h = hash(j, temp_entry.dni, variant);
                            if (new_tables[j][h].dni == 0) {
                                new_tables[j][h] = temp_entry;
                                inserted = true;
                                break;
                            }
                            std::swap(temp_entry, new_tables[j][h]);
                        }
                        if (inserted) break;
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