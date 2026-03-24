#pragma once

#define DISABLE_ERROR 0
#define DISABLE_LOG 0
#define DISABLE_WARNING 0

#if DISABLE_LOG == 0
#define LOG(message) std::cout << "[LOG] " << message << std::endl
#else
#define LOG(message) void(0)
#endif

#if DISABLE_ERROR == 0
#define ERROR(functionName, message) std::cout << "[ERROR] " << functionName << " " << message << std::endl
#else
#define ERROR(functionName, message) void(0)
#endif

#if DISABLE_WARNING == 0
#define WARNING(functionName, message) std::cout << "[WARNING] " << functionName << " " << message << std::endl
#else
#define WARNING(functionName, message) void(0)
#endif
