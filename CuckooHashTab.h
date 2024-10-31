#ifndef CUCKOOHASHTAB_H
#define CUCKOOHASHTAB_H

#include <vector>
#include <cstdint>

struct Entry {
    uint32_t dni = 0;
    uint32_t offset = 0;
};

class CuckooHashTab {
public:
    CuckooHashTab(int initial_size = 22000000, int num_tables = 2);
    void insertar(uint32_t dni, uint32_t offset);
    bool buscar(uint32_t dni, uint32_t& offset) const;
    void eliminar(uint32_t dni);

private:
    size_t hash1(uint32_t dni) const;
    size_t hash2(uint32_t dni) const;
    size_t hash3(uint32_t dni) const;
    size_t hash4(uint32_t dni) const;

    int hash(int table_idx, uint32_t dni, int hash_variant) const;

    void rehash();

    int size;
    int num_tables;
    std::vector<std::vector<Entry>> tables;

    const int rehash_limit = 500;
    uint32_t seed1;
    uint32_t seed2;
    uint32_t seed3;
    uint32_t seed4;

    // Función de hash MurmurHash3 para uint32_t
    uint32_t murmur3_32(uint32_t key, uint32_t seed) const;
};

#endif