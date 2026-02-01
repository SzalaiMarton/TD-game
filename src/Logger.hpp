#pragma once

#ifndef DISABLE_LOG
#define LOG(message) std::cout << "[LOG] " << message << std::endl
#endif

#ifndef DISABLE_ERROR
#define ERROR(functionName, message) std::cout << "[ERROR] " << functionName << " " << message << std::endl
#endif
