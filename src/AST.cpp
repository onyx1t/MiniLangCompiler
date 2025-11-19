// src/AST.cpp (НОВЫЙ ФАЙЛ)
#include "AST.h"
#include <map>

// *** Реализация вспомогательных функций ***

// Используем Token::typeToString для преобразования TokenType в строку
std::string BinaryOpNode::op_type_to_string() const {
    // Временно создаем токен для доступа к методу typeToString из Token.h
    Token temp_token(op, TokenClass::UNKNOWN, "", 0, 0);
    return temp_token.typeToString();
}

std::string VarDeclNode::typeToString() const {
    // Временно создаем токен для доступа к методу typeToString из Token.h
    Token temp_token(type, TokenClass::UNKNOWN, "", 0, 0);
    return temp_token.typeToString();
}


// *** Реализация методов accept() (должны быть в AST.cpp) ***

void TerminalNode::accept(ASTVisitor& visitor) { visitor.visit(*this); }
void IntLiteralNode::accept(ASTVisitor& visitor) { visitor.visit(*this); }
void IdentifierNode::accept(ASTVisitor& visitor) { visitor.visit(*this); }
void BinaryOpNode::accept(ASTVisitor& visitor) { visitor.visit(*this); }
void VarDeclNode::accept(ASTVisitor& visitor) { visitor.visit(*this); }
void AssignStmtNode::accept(ASTVisitor& visitor) { visitor.visit(*this); }
void PrintStmtNode::accept(ASTVisitor& visitor) { visitor.visit(*this); }
void IfStmtNode::accept(ASTVisitor& visitor) { visitor.visit(*this); }
void WhileStmtNode::accept(ASTVisitor& visitor) { visitor.visit(*this); }
void ProgramNode::accept(ASTVisitor& visitor) { visitor.visit(*this); }