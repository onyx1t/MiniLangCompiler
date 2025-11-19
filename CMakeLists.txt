cmake_minimum_required(VERSION 3.20)
project(LTLab CXX)

# Устанавливаем стандарт C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 1. Определяем все исходные файлы (.cpp)
set(SOURCE_FILES
        main.cpp
        src/Lexer.cpp
        src/ErrorHandler.cpp
        # Добавляем placeholder'ы для будущих модулей
        src/Parser.cpp
        src/CodeGenerator.cpp
        src/Optimizer.cpp
        src/IRInterpreter.cpp
        include/ASTVisualizer.h
        src/ASTVisualizer.cpp
        src/AST.cpp
)

# 2. Создаем исполняемый файл (Цель LTLab)
add_executable(LTLab ${SOURCE_FILES})

# 3. Указываем, где искать заголовочные файлы (.h)
# Эта команда ДОЛЖНА идти после add_executable
target_include_directories(LTLab
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)