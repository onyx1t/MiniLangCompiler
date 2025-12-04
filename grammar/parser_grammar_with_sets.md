## Грамматика для Синтаксического Анализатора (Рекурсивный Спуск)

Грамматика преобразована для устранения левой рекурсии. Множество Выбора (Selection Set) для каждого правила равно **FIRST** правила, а для правил с `ε` — **FIRST $\cup$ FOLLOW**.

### 1. Начальное Правило

| Правило (Func) | Определение | Множество Выбора |
| :--- | :--- | :--- |
| **P** (parseProgram) | `S EOF` | `INT, ID, PRINT, IF, WHILE, EOF, RBRACE` |

### 2. Операторы (Statements)

| Правило (Func) | Определение | Множество Выбора (FIRST / FOLLOW) |
| :--- | :--- | :--- |
| **S** (parseStmtList) | `Stmt S_rest` | `INT, ID, PRINT, IF, WHILE` |
| | `ε` | `RBRACE, EOF` |
| **S_rest** (parseStmtListRest) | `Stmt S_rest` | `INT, ID, PRINT, IF, WHILE` |
| | `ε` | `RBRACE, EOF` |
| **Stmt** (parseStmt) | `INT ID ;` | `INT` |
| | `ID = E ;` | `ID` |
| | `PRINT E ;` | `PRINT` |
| | `IF (C) {S} StmtElse` | `IF` |
| | `WHILE (C) {S}` | `WHILE` |
| **StmtElse** (parseStmtElse) | `ELSE {S}` | `ELSE` |
| | `ε` | `INT, ID, PRINT, IF, WHILE, RBRACE, EOF` |

### 3. Условия (Conditions)

| Правило (Func) | Определение | Множество Выбора (FIRST) |
| :--- | :--- | :--- |
| **C** (parseCondition) | `E RELOP E` | `(`, `ID`, `INT_LITERAL` |

### 4. Выражения (Expressions)

| Правило (Func) | Определение | Множество Выбора (FIRST / FOLLOW) |
| :--- | :--- | :--- |
| **E** (parseExpr) | `T E_rest` | `(`, `ID`, `INT_LITERAL` |
| **E_rest** (parseExprRest) | `+ T E_rest` | `+` |
| | `- T E_rest` | `-` |
| | `ε` | `), ;, ==, !=, <, >` |
| **T** (parseTerm) | `F T_rest` | `(`, `ID`, `INT_LITERAL` |
| **T_rest** (parseTermRest) | `* F T_rest` | `*` |
| | `ε` | `+, -, ), ;, ==, !=, <, >` |
| **F** (parseFactor) | `(E)` | `(` |
| | `ID` | `ID` |
| | `INT_LITERAL` | `INT_LITERAL` |