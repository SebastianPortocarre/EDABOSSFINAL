#ifndef COMPRESSOR_H
#define COMPRESSOR_H

#include <vector>
#include <string>

class Compressor {
public:
    static std::vector<char> compress(const std::string& data);

    static std::string decompress(const std::vector<char>& compressedData);
};

#endif