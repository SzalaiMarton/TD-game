#pragma once

#ifndef DISABLE_LOG
#define LOG(message) std::cout << "[LOG] " << message << std::endl
#endif

#define DISABLE_ERROR 1

#ifndef DISABLE_ERROR
#define ERROR(functionName, message) std::cout << "[ERROR] " << functionName << " " << message << std::endl
#else
#define ERROR(functionName, message) void(0)
#endif
