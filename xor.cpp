#include "xor.hpp"

std::string Decrypt(std::string toEncrypt) {
    char key = 'I';
    std::string output = toEncrypt;

    for (unsigned int i = 0; i < toEncrypt.size(); i++)
        output[i] = toEncrypt[i] ^ key;

    return output;
}

void printashex(char* toprint) {
    for (unsigned int i = 0; i < strlen(toprint); i++) {
        std::cout << "[" << std::hex << (int)toprint[i] << std::dec << "]";
    }
    std::cout << std::endl;
}