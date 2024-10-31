#include "Compressor.h"
#include <zstd.h>
#include <iostream>
#include <vector>
#include <string>
using namespace std;

vector<char> Compressor::compress(const string& data) {
    // Obtener el tamaño máximo del buffer comprimido
    size_t maxCompressedSize = ZSTD_compressBound(data.size());
    vector<char> compressedData(maxCompressedSize);

    // Comprimir los datos utilizando Zstandard
    size_t compressedSize = ZSTD_compress(compressedData.data(), maxCompressedSize, data.data(), data.size(), 20);
    if (ZSTD_isError(compressedSize)) {
        cerr << "Error al comprimir los datos con Zstandard: " << ZSTD_getErrorName(compressedSize) << endl;
        return {};
    }

    compressedData.resize(compressedSize);  // Ajustar el tamaño del vector comprimido
    return compressedData;
}

// Función para descomprimir los datos utilizando Zstandard
string Compressor::decompress(const vector<char>& compressedData) {
    // Obtener el tamaño original de los datos a partir del flujo comprimido
    unsigned long long originalSize = ZSTD_getFrameContentSize(compressedData.data(), compressedData.size());
    if (originalSize == ZSTD_CONTENTSIZE_ERROR) {
        cerr << "Error: no es un flujo ZSTD válido" << endl;
        return {};
    } else if (originalSize == ZSTD_CONTENTSIZE_UNKNOWN) {
        cerr << "Error: el tamaño original es desconocido" << endl;
        return {};
    }

    vector<char> decompressedData(static_cast<size_t>(originalSize));

    // Descomprimir los datos utilizando Zstandard
    size_t decompressedSize = ZSTD_decompress(decompressedData.data(), originalSize, compressedData.data(), compressedData.size());
    if (ZSTD_isError(decompressedSize)) {
        cerr << "Error al descomprimir los datos con Zstandard: " << ZSTD_getErrorName(decompressedSize) << endl;
        return {};
    }

    return string(decompressedData.begin(), decompressedData.end());
}