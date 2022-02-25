cmake_minimum_required(VERSION 3.9)
project(compiler)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_BUILD_TYPE Debug)

include_directories(
#        include
)

add_executable(compiler
        龙书附录A/Lexer.cpp
        龙书附录A/Lexer.h
        龙书附录A/main.cpp
        龙书附录A/Parser.cpp
        龙书附录A/Parser.h
        龙书附录A/symbol/Type.cpp
        龙书附录A/symbol/Type.h
        龙书附录A/astNode/Node.cpp
        龙书附录A/astNode/Node.h
        龙书附录A/astNode/Stmt.cpp
        龙书附录A/astNode/Stmt.h
        龙书附录A/astNode/Expr.cpp
        龙书附录A/astNode/Expr.h
        龙书附录A/Token.cpp
        龙书附录A/Token.h
        龙书附录A/symbol/Env.cpp
        龙书附录A/symbol/Env.h)

target_link_libraries(compiler)