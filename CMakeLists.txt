cmake_minimum_required(VERSION 3.20)
project(LTLab CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(SOURCE_FILES
        main.cpp
        src/Lexer.cpp
        src/ErrorHandler.cpp
        src/Parser.cpp
        src/IRGenerator.cpp
        src/IROptimizer.cpp
        src/IRInterpreter.cpp
        include/ASTVisualizer.h
        src/ASTVisualizer.cpp
        src/AST.cpp
        src/IR.cpp
)

add_executable(LTLab ${SOURCE_FILES})

target_include_directories(LTLab
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)