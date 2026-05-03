#include "login_system.h"

// hashPassword is DEFINED here (only once) and DECLARED in login_system.h
// This stops the "already has a body" linker error when multiple .cpp
// files include login_system.h
std::string hashPassword(const std::string& pw) {
    unsigned long h = 5381;
    for (unsigned char c : pw)
        h = ((h << 5) + h) ^ c;   // djb2 variant
    char buf[9];
    snprintf(buf, sizeof(buf), "%08lx", h & 0xFFFFFFFFUL);
    return std::string(buf);
}