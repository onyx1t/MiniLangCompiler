#include "AST.h"

// Вспомогательные методы преобразования типов в строки
std::string BinaryOpNode::op_type_to_string() const {
    Token temp_token(op, TokenClass::UNKNOWN, "", 0, 0);
    return temp_token.typeToString();
}

std::string VarDeclNode::typeToString() const {
    Token temp_token(type, TokenClass::UNKNOWN, "", 0, 0);
    return temp_token.typeToString();
}

// Реализации методов accept для узлов AST
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