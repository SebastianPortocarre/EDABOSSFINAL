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
    CuckooHashTab(int initial_size = 33000000, int num_tables = 2);
    void insertar(uint32_t dni, uint32_t offset);
    bool buscar(uint32_t dni, uint32_t& offset) const;
    void eliminar(uint32_t dni);
    void guardarEnArchivo(const std::string& filename) const;
    bool cargarDesdeArchivo(const std::string& filename);

private:
    size_t hash1(uint32_t dni) const;
    size_t hash2(uint32_t dni) const;

    void rehash();

    int size;
    int num_tables;
    std::vector<std::vector<Entry>> tables;

    const int rehash_limit = 3000;
    uint32_t seed1;
    uint32_t seed2;

    uint32_t murmur3_32(uint32_t key, uint32_t seed) const;
};

#endif
