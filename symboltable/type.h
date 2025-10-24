#pragma once
#include <string>

enum class TypeKind {
    Int, Long, Float, Double, Char, Void, Unknown
};

inline const char* toString(TypeKind k) {
    switch (k) {
        case TypeKind::Int:    return "int";
        case TypeKind::Long:   return "long";
        case TypeKind::Float:  return "float";
        case TypeKind::Double: return "double";
        case TypeKind::Char:   return "char";
        case TypeKind::Void:   return "void";
        default:               return "unknown";
    }
}

// Tiện ích (tùy dùng): map keyword→TypeKind
inline TypeKind parseTypeKeyword(const std::string& kw) {
    if (kw == "int")    return TypeKind::Int;
    if (kw == "long")   return TypeKind::Long;
    if (kw == "float")  return TypeKind::Float;
    if (kw == "double") return TypeKind::Double;
    if (kw == "char")   return TypeKind::Char;
    if (kw == "void")   return TypeKind::Void;
    return TypeKind::Unknown;
}
