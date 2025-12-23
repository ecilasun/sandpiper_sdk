#include <cctype>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

namespace {

// Matches SDK/include/vcp.h
constexpr uint32_t VCP_NOOP = 0x00;
constexpr uint32_t VCP_LOADIMM = 0x01;
constexpr uint32_t VCP_PALWRITE = 0x02;
constexpr uint32_t VCP_WAITSCANLINE = 0x03;
constexpr uint32_t VCP_WAITPIXEL = 0x04;
constexpr uint32_t VCP_MATHOP = 0x05;
constexpr uint32_t VCP_JUMP = 0x06;
constexpr uint32_t VCP_CMP = 0x07;
constexpr uint32_t VCP_BRANCH = 0x08;
constexpr uint32_t VCP_STORE = 0x09;
constexpr uint32_t VCP_LOAD = 0x0A;
constexpr uint32_t VCP_READSCANINFO = 0x0B;
constexpr uint32_t VCP_LOGICOP = 0x0D;

constexpr uint32_t COND_INV = 0x08;
constexpr uint32_t COND_LE = 0x01;
constexpr uint32_t COND_LT = 0x02;
constexpr uint32_t COND_EQ = 0x04;
constexpr uint32_t COND_GT = (COND_LE | COND_INV);
constexpr uint32_t COND_GE = (COND_LT | COND_INV);
constexpr uint32_t COND_NE = (COND_EQ | COND_INV);

constexpr uint32_t OP_ADD = 0x00;
constexpr uint32_t OP_SUB = 0x01;
constexpr uint32_t OP_INC = 0x02;
constexpr uint32_t OP_DEC = 0x03;

constexpr uint32_t OPL_AND = 0x00;
constexpr uint32_t OPL_OR = 0x01;
constexpr uint32_t OPL_XOR = 0x02;
constexpr uint32_t OPL_ASR = 0x03;
constexpr uint32_t OPL_SHR = 0x04;
constexpr uint32_t OPL_SHL = 0x05;
constexpr uint32_t OPL_NEG = 0x06;
constexpr uint32_t OPL_RCMP = 0x07;
constexpr uint32_t OPL_RCTL = 0x08;

constexpr uint32_t VREG_ZERO = 0x00;

constexpr uint32_t DESTREG(uint32_t reg) { return (reg & 0xF) << 4; }
constexpr uint32_t SRCREG1(uint32_t reg) { return (reg & 0xF) << 8; }
constexpr uint32_t SRCREG2(uint32_t reg) { return (reg & 0xF) << 12; }
constexpr uint32_t IMMED24(uint32_t value) { return (value & 0xFFFFFFu) << 8; }
constexpr uint32_t IMMED16(uint32_t value) { return (value & 0xFFFFu) << 16; }
constexpr uint32_t IMMED8(uint32_t value) { return (value & 0xFFu) << 24; }

// Program sizes in bytes (EVCPBufferSize)
static const uint32_t kAllowedProgramSizes[] = {128, 256, 512, 1024, 2048, 4096};

struct SymbolInfo {
    std::string name;
    uint32_t offsetBytes = 0;
    uint32_t initValue = 0;
    bool isInternal = false;
};

struct CompiledProgram {
    std::vector<uint32_t> words;
    uint32_t codeBytes = 0;
    uint32_t dataBytes = 0;
    uint32_t paddedBytes = 0;
    std::vector<SymbolInfo> symbols;
    bool stackDeclared = false;
    uint32_t stackWords = 0;
};

static std::string hex(uint32_t v, int width) {
    std::ostringstream oss;
    oss.setf(std::ios::hex, std::ios::basefield);
    oss.setf(std::ios::uppercase);
    oss.fill('0');
    oss.width(width);
    oss << v;
    return oss.str();
}

static std::string regName(uint32_t r) {
    return "r" + std::to_string(r & 0xFu);
}

static const char* mathOpName(uint32_t op) {
    switch (op) {
        case OP_ADD: return "add";
        case OP_SUB: return "sub";
        case OP_INC: return "inc";
        case OP_DEC: return "dec";
        default: return "math?";
    }
}

static const char* logicOpName(uint32_t op) {
    switch (op) {
        case OPL_AND: return "and";
        case OPL_OR: return "or";
        case OPL_XOR: return "xor";
        case OPL_ASR: return "asr";
        case OPL_SHR: return "shr";
        case OPL_SHL: return "shl";
        case OPL_NEG: return "neg";
        case OPL_RCMP: return "rcmp";
        case OPL_RCTL: return "rctl";
        default: return "logic?";
    }
}

static const char* condName(uint32_t cond) {
    switch (cond) {
        case COND_LE: return "le";
        case COND_LT: return "lt";
        case COND_EQ: return "eq";
        case COND_GT: return "gt";
        case COND_GE: return "ge";
        case COND_NE: return "ne";
        default: return "cond?";
    }
}

static void dumpDisassembly(const CompiledProgram& p, std::ostream& os) {
    const uint32_t codeWords = p.codeBytes / 4u;

    os << "; vcpcompiler -S disassembly\n";
    os << "; codeBytes=" << p.codeBytes << " dataBytes=" << p.dataBytes << " paddedBytes=" << p.paddedBytes << "\n\n";

    os << "; --- CODE ---\n";
    for (uint32_t i = 0; i < codeWords && i < p.words.size(); ++i) {
        uint32_t pc = i * 4u;
        uint32_t w = p.words[i];

        // Encoding matches SDK/include/vcp.h: low 4 bits opcode, next nibbles are regs.
        uint32_t opcode = (w & 0x0Fu);
        uint32_t dest = (w >> 4) & 0xFu;
        uint32_t src1 = (w >> 8) & 0xFu;
        uint32_t src2 = (w >> 12) & 0xFu;
        uint32_t imm8 = (w >> 24) & 0xFFu;
        uint32_t imm16 = (w >> 16) & 0xFFFFu;
        uint32_t imm24 = (w >> 8) & 0xFFFFFFu;

        std::ostringstream inst;
        switch (opcode) {
            case VCP_NOOP:
                inst << "noop";
                break;
            case VCP_LOADIMM:
                inst << "ldimm " << regName(dest) << ", 0x" << hex(imm24, 6);
                break;
            case VCP_PALWRITE:
                inst << "pal_write [" << regName(src1) << "], " << regName(src2);
                break;
            case VCP_WAITSCANLINE:
                inst << "wait_scanline " << regName(src1);
                break;
            case VCP_WAITPIXEL:
                inst << "wait_pixel " << regName(src1);
                break;
            case VCP_MATHOP:
                inst << mathOpName(imm8) << " " << regName(dest) << ", " << regName(src1) << ", " << regName(src2);
                break;
            case VCP_JUMP: {
                int16_t rel = static_cast<int16_t>(imm16);
                int32_t target = static_cast<int32_t>(pc) + static_cast<int32_t>(rel);
                inst << "jump 0x" << hex(static_cast<uint32_t>(target), 4) << " ; rel=" << rel;
                break;
            }
            case VCP_CMP:
                inst << "cmp." << condName(imm8) << " " << regName(src1) << ", " << regName(src2);
                break;
            case VCP_BRANCH: {
                int16_t rel = static_cast<int16_t>(imm16);
                int32_t target = static_cast<int32_t>(pc) + static_cast<int32_t>(rel);
                inst << "branch 0x" << hex(static_cast<uint32_t>(target), 4) << " ; rel=" << rel;
                break;
            }
            case VCP_STORE:
                inst << "store [" << regName(src1) << "], " << regName(src2);
                break;
            case VCP_LOAD:
                inst << "load " << regName(dest) << ", [" << regName(src1) << "]";
                break;
            case VCP_READSCANINFO:
                inst << "read_scaninfo(" << src1 << ") " << regName(dest);
                break;
            case VCP_LOGICOP:
                inst << logicOpName(imm8) << " " << regName(dest) << ", " << regName(src1) << ", " << regName(src2);
                break;
            default:
                inst << "word 0x" << hex(w, 8) << " ; opcode=0x" << hex(opcode, 1);
                break;
        }

        os << "0x" << hex(pc, 4) << ": 0x" << hex(w, 8) << "  " << inst.str() << "\n";
    }

    // Dump initialized data words (includes internal temps, user vars, and stack slots).
    // Per request, use the 'word' indicator for data/stack only.
    if (p.dataBytes != 0) {
        auto isStackSym = [](const std::string& n) {
            return n == "__sp" || (n.rfind("__stack_", 0) == 0);
        };

        // Map byte offset -> symbol names for annotation.
        std::unordered_map<uint32_t, std::vector<std::string>> namesAt;
        namesAt.reserve(p.symbols.size());
        for (const auto& s : p.symbols) {
            namesAt[s.offsetBytes].push_back(s.name);
        }

        const uint32_t dataWords = p.dataBytes / 4u;
        const uint32_t dataBase = p.codeBytes;

        if (p.stackDeclared) {
            os << "\n; --- STACK (data words) ---\n";
            os << "; stackDeclared=true stackWords=" << p.stackWords << "\n";
            for (uint32_t j = 0; j < dataWords && (codeWords + j) < p.words.size(); ++j) {
                const uint32_t addr = dataBase + j * 4u;
                auto it = namesAt.find(addr);
                if (it == namesAt.end()) continue;
                bool anyStack = false;
                for (const auto& n : it->second) {
                    if (isStackSym(n)) { anyStack = true; break; }
                }
                if (!anyStack) continue;
                const uint32_t w = p.words[codeWords + j];
                os << "0x" << hex(addr, 4) << ": word 0x" << hex(w, 8);
                os << " ; ";
                for (size_t k = 0; k < it->second.size(); ++k) {
                    if (k) os << ", ";
                    os << it->second[k];
                }
                os << "\n";
            }
        }

        os << "\n; --- DATA (data words) ---\n";
        for (uint32_t j = 0; j < dataWords && (codeWords + j) < p.words.size(); ++j) {
            const uint32_t addr = dataBase + j * 4u;
            const uint32_t w = p.words[codeWords + j];
            auto it = namesAt.find(addr);
            if (it != namesAt.end()) {
                bool anyStack = false;
                for (const auto& n : it->second) {
                    if (isStackSym(n)) { anyStack = true; break; }
                }
                if (anyStack) continue;
            }

            os << "0x" << hex(addr, 4) << ": word 0x" << hex(w, 8);
            if (it != namesAt.end()) {
                os << " ; ";
                for (size_t k = 0; k < it->second.size(); ++k) {
                    if (k) os << ", ";
                    os << it->second[k];
                }
            }
            os << "\n";
        }
    }
}

struct CompileError : std::runtime_error {
    using std::runtime_error::runtime_error;
};

struct SourceLoc {
    int line = 1;
    int col = 1;
};

enum class TokenKind {
    End,
    Ident,
    Number,

    LParen, RParen,
    LBrace, RBrace,
    LBracket, RBracket,
    Semicolon,
    Comma,
    Colon,

    Plus, Minus,
    Star, Slash, Percent,
    Amp, Pipe, Caret,
    Tilde,

    Assign,

    Less, Greater,
    LessEq, GreaterEq,
    EqEq, NotEq,

    ShiftLeft, ShiftRight,

    Kw_u32,
    Kw_int,
    Kw_if,
    Kw_else,
    Kw_while,
    Kw_goto,

    // intrinsic-like keywords
    Kw_stack,
};

struct Token {
    TokenKind kind;
    std::string text;
    uint32_t number = 0;
    SourceLoc loc;
};

class Lexer {
public:
    explicit Lexer(std::string src) : m_src(std::move(src)) {}

    Token next() {
        skipTrivia();
        Token t;
        t.loc = m_loc;
        if (m_pos >= m_src.size()) {
            t.kind = TokenKind::End;
            return t;
        }

        char c = m_src[m_pos];
        if (isIdentStart(c)) {
            std::string s;
            while (m_pos < m_src.size() && isIdentCont(m_src[m_pos])) {
                s.push_back(m_src[m_pos]);
                bump();
            }
            t.text = s;
            t.kind = classifyIdent(s);
            return t;
        }

        if (std::isdigit(static_cast<unsigned char>(c))) {
            t.kind = TokenKind::Number;
            t.text = readNumber(t.number);
            return t;
        }

        auto two = [&](char a, char b) -> bool {
            return m_pos + 1 < m_src.size() && m_src[m_pos] == a && m_src[m_pos + 1] == b;
        };

        if (two('<', '<')) {
            t.kind = TokenKind::ShiftLeft;
            t.text = "<<";
            bump(); bump();
            return t;
        }
        if (two('>', '>')) {
            t.kind = TokenKind::ShiftRight;
            t.text = ">>";
            bump(); bump();
            return t;
        }
        if (two('=', '=')) {
            t.kind = TokenKind::EqEq;
            t.text = "==";
            bump(); bump();
            return t;
        }
        if (two('!', '=')) {
            t.kind = TokenKind::NotEq;
            t.text = "!=";
            bump(); bump();
            return t;
        }
        if (two('<', '=')) {
            t.kind = TokenKind::LessEq;
            t.text = "<=";
            bump(); bump();
            return t;
        }
        if (two('>', '=')) {
            t.kind = TokenKind::GreaterEq;
            t.text = ">=";
            bump(); bump();
            return t;
        }

        switch (c) {
            case '(': t.kind = TokenKind::LParen; t.text = "("; bump(); return t;
            case ')': t.kind = TokenKind::RParen; t.text = ")"; bump(); return t;
            case '{': t.kind = TokenKind::LBrace; t.text = "{"; bump(); return t;
            case '}': t.kind = TokenKind::RBrace; t.text = "}"; bump(); return t;
            case '[': t.kind = TokenKind::LBracket; t.text = "["; bump(); return t;
            case ']': t.kind = TokenKind::RBracket; t.text = "]"; bump(); return t;
            case ';': t.kind = TokenKind::Semicolon; t.text = ";"; bump(); return t;
            case ',': t.kind = TokenKind::Comma; t.text = ","; bump(); return t;
            case ':': t.kind = TokenKind::Colon; t.text = ":"; bump(); return t;
            case '+': t.kind = TokenKind::Plus; t.text = "+"; bump(); return t;
            case '-': t.kind = TokenKind::Minus; t.text = "-"; bump(); return t;
            case '*': t.kind = TokenKind::Star; t.text = "*"; bump(); return t;
            case '/': t.kind = TokenKind::Slash; t.text = "/"; bump(); return t;
            case '%': t.kind = TokenKind::Percent; t.text = "%"; bump(); return t;
            case '&': t.kind = TokenKind::Amp; t.text = "&"; bump(); return t;
            case '|': t.kind = TokenKind::Pipe; t.text = "|"; bump(); return t;
            case '^': t.kind = TokenKind::Caret; t.text = "^"; bump(); return t;
            case '~': t.kind = TokenKind::Tilde; t.text = "~"; bump(); return t;
            case '=': t.kind = TokenKind::Assign; t.text = "="; bump(); return t;
            case '<': t.kind = TokenKind::Less; t.text = "<"; bump(); return t;
            case '>': t.kind = TokenKind::Greater; t.text = ">"; bump(); return t;
            default:
                throw CompileError(errPrefix(t.loc) + "unexpected character '" + std::string(1, c) + "'");
        }
    }

private:
    std::string m_src;
    size_t m_pos = 0;
    SourceLoc m_loc{};

    static bool isIdentStart(char c) {
        return std::isalpha(static_cast<unsigned char>(c)) || c == '_';
    }
    static bool isIdentCont(char c) {
        return isIdentStart(c) || std::isdigit(static_cast<unsigned char>(c));
    }

    static std::string errPrefix(const SourceLoc& loc) {
        std::ostringstream oss;
        oss << "(" << loc.line << ":" << loc.col << ") ";
        return oss.str();
    }

    void bump() {
        if (m_pos >= m_src.size()) return;
        char c = m_src[m_pos++];
        if (c == '\n') {
            m_loc.line++;
            m_loc.col = 1;
        } else {
            m_loc.col++;
        }
    }

    void skipTrivia() {
        while (m_pos < m_src.size()) {
            char c = m_src[m_pos];
            if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
                bump();
                continue;
            }
            // // comment
            if (c == '/' && (m_pos + 1 < m_src.size()) && m_src[m_pos + 1] == '/') {
                bump(); bump();
                while (m_pos < m_src.size() && m_src[m_pos] != '\n') bump();
                continue;
            }
            // /* comment */
            if (c == '/' && (m_pos + 1 < m_src.size()) && m_src[m_pos + 1] == '*') {
                bump(); bump();
                while (m_pos + 1 < m_src.size()) {
                    if (m_src[m_pos] == '*' && m_src[m_pos + 1] == '/') {
                        bump(); bump();
                        break;
                    }
                    bump();
                }
                continue;
            }
            break;
        }
    }

    std::string readNumber(uint32_t& out) {
        size_t start = m_pos;
        bool isHex = false;
        if (m_src[m_pos] == '0' && m_pos + 1 < m_src.size() && (m_src[m_pos + 1] == 'x' || m_src[m_pos + 1] == 'X')) {
            isHex = true;
            bump(); bump();
            start = m_pos - 2;
            uint64_t v = 0;
            bool any = false;
            while (m_pos < m_src.size()) {
                char c = m_src[m_pos];
                int d = -1;
                if (c >= '0' && c <= '9') d = c - '0';
                else if (c >= 'a' && c <= 'f') d = 10 + (c - 'a');
                else if (c >= 'A' && c <= 'F') d = 10 + (c - 'A');
                else break;
                any = true;
                v = (v << 4) | static_cast<uint64_t>(d);
                bump();
            }
            if (!any) throw CompileError(errPrefix(m_loc) + "invalid hex literal");
            out = static_cast<uint32_t>(v);
            return m_src.substr(start, m_pos - start);
        }

        uint64_t v = 0;
        while (m_pos < m_src.size() && std::isdigit(static_cast<unsigned char>(m_src[m_pos]))) {
            v = v * 10 + static_cast<unsigned>(m_src[m_pos] - '0');
            bump();
        }
        out = static_cast<uint32_t>(v);
        return m_src.substr(start, m_pos - start);
    }

    TokenKind classifyIdent(const std::string& s) {
        if (s == "u32") return TokenKind::Kw_u32;
        if (s == "int") return TokenKind::Kw_int;
        if (s == "if") return TokenKind::Kw_if;
        if (s == "else") return TokenKind::Kw_else;
        if (s == "while") return TokenKind::Kw_while;
        if (s == "goto") return TokenKind::Kw_goto;
        if (s == "stack") return TokenKind::Kw_stack;
        return TokenKind::Ident;
    }
};

// --- AST ---

struct Expr;
using ExprPtr = std::unique_ptr<Expr>;

struct Expr {
    struct Number { uint32_t value; };
    struct Var { std::string name; };
    struct Unary { TokenKind op; ExprPtr rhs; };
    struct Binary { TokenKind op; ExprPtr lhs; ExprPtr rhs; };
    struct Call { std::string name; std::vector<ExprPtr> args; };

    std::variant<Number, Var, Unary, Binary, Call> node;
    SourceLoc loc;
};

struct Stmt;
using StmtPtr = std::unique_ptr<Stmt>;

struct Stmt {
    struct Block { std::vector<StmtPtr> stmts; };
    struct Decl { std::string name; std::optional<uint32_t> initConst; };
    struct Assign { std::string name; ExprPtr value; };
    struct If { ExprPtr condL; TokenKind condOp; ExprPtr condR; StmtPtr thenS; StmtPtr elseS; };
    struct While { ExprPtr condL; TokenKind condOp; ExprPtr condR; StmtPtr body; };
    struct Label { std::string name; };
    struct Goto { std::string target; };
    struct ExprStmt { ExprPtr expr; };

    std::variant<Block, Decl, Assign, If, While, Label, Goto, ExprStmt> node;
    SourceLoc loc;
};

class Parser {
public:
    explicit Parser(Lexer lex) : m_lex(std::move(lex)) { m_tok = m_lex.next(); }

    std::vector<StmtPtr> parseProgram() {
        std::vector<StmtPtr> out;
        while (m_tok.kind != TokenKind::End) {
            out.push_back(parseTopLevelStmt());
        }
        return out;
    }

private:
    Lexer m_lex;
    Token m_tok;

    [[noreturn]] void failHere(const std::string& msg) {
        std::ostringstream oss;
        oss << "(" << m_tok.loc.line << ":" << m_tok.loc.col << ") " << msg;
        throw CompileError(oss.str());
    }

    void consume(TokenKind k, const char* what) {
        if (m_tok.kind != k) {
            failHere(std::string("expected ") + what + ", got '" + m_tok.text + "'");
        }
        m_tok = m_lex.next();
    }

    bool match(TokenKind k) {
        if (m_tok.kind == k) {
            m_tok = m_lex.next();
            return true;
        }
        return false;
    }

    StmtPtr parseTopLevelStmt() {
        // declarations are allowed anywhere, so reuse statement parser
        return parseStmt();
    }

    StmtPtr parseStmt() {
        if (match(TokenKind::LBrace)) {
            auto b = std::make_unique<Stmt>();
            Stmt::Block blk;
            while (!match(TokenKind::RBrace)) {
                if (m_tok.kind == TokenKind::End) failHere("unexpected end of file in block");
                blk.stmts.push_back(parseStmt());
            }
            b->node = std::move(blk);
            return b;
        }

        if (m_tok.kind == TokenKind::Kw_u32 || m_tok.kind == TokenKind::Kw_int) {
            auto loc = m_tok.loc;
            m_tok = m_lex.next();
            if (m_tok.kind != TokenKind::Ident) failHere("expected identifier after type");
            std::string name = m_tok.text;
            m_tok = m_lex.next();
            std::optional<uint32_t> init;
            if (match(TokenKind::Assign)) {
                uint32_t v = parseConstExpr();
                init = v;
            }
            consume(TokenKind::Semicolon, "';'");
            auto s = std::make_unique<Stmt>();
            s->loc = loc;
            s->node = Stmt::Decl{std::move(name), init};
            return s;
        }

        // label:  ident ':'
        if (m_tok.kind == TokenKind::Ident) {
            Token look = m_tok;
            // peek by consuming then re-injecting is annoying; handle via manual scan: if next token is colon
            // We'll do: if ident then lex next into temp.
            m_tok = m_lex.next();
            if (m_tok.kind == TokenKind::Colon) {
                m_tok = m_lex.next();
                auto s = std::make_unique<Stmt>();
                s->loc = look.loc;
                s->node = Stmt::Label{look.text};
                return s;
            }
            // not a label; treat as start of assignment or call/expression statement.
            // Put back by using a small hack: store the consumed ident and current tok as lookahead.
            // We'll implement a small "unread" buffer by keeping lastIdent.
            m_unreadIdent = look;
            return parseAfterLeadingIdent();
        }

        if (match(TokenKind::Kw_if)) {
            auto loc = m_tok.loc;
            consume(TokenKind::LParen, "'('");
            auto [l, op, r] = parseCond();
            consume(TokenKind::RParen, "')'");
            auto thenS = parseStmt();
            StmtPtr elseS;
            if (match(TokenKind::Kw_else)) {
                elseS = parseStmt();
            }
            auto s = std::make_unique<Stmt>();
            s->loc = loc;
            s->node = Stmt::If{std::move(l), op, std::move(r), std::move(thenS), std::move(elseS)};
            return s;
        }

        if (match(TokenKind::Kw_while)) {
            auto loc = m_tok.loc;
            consume(TokenKind::LParen, "'('");
            auto [l, op, r] = parseCond();
            consume(TokenKind::RParen, "')'");
            auto body = parseStmt();
            auto s = std::make_unique<Stmt>();
            s->loc = loc;
            s->node = Stmt::While{std::move(l), op, std::move(r), std::move(body)};
            return s;
        }

        if (match(TokenKind::Kw_goto)) {
            auto loc = m_tok.loc;
            if (m_tok.kind != TokenKind::Ident) failHere("expected label identifier after goto");
            std::string target = m_tok.text;
            m_tok = m_lex.next();
            consume(TokenKind::Semicolon, "';'");
            auto s = std::make_unique<Stmt>();
            s->loc = loc;
            s->node = Stmt::Goto{std::move(target)};
            return s;
        }

        // expression statement
        auto e = parseExpr();
        consume(TokenKind::Semicolon, "';'");
        auto s = std::make_unique<Stmt>();
        s->node = Stmt::ExprStmt{std::move(e)};
        return s;
    }

    // unread ident buffer used after consuming an ident to check label
    std::optional<Token> m_unreadIdent;

    StmtPtr parseAfterLeadingIdent() {
        if (!m_unreadIdent) failHere("internal parser error");
        Token identTok = *m_unreadIdent;
        m_unreadIdent.reset();

        // If current token is '=' => assignment
        if (m_tok.kind == TokenKind::Assign) {
            m_tok = m_lex.next();
            auto rhs = parseExpr();
            consume(TokenKind::Semicolon, "';'");
            auto s = std::make_unique<Stmt>();
            s->loc = identTok.loc;
            s->node = Stmt::Assign{identTok.text, std::move(rhs)};
            return s;
        }

        // Otherwise parse as expression statement starting with ident
        // Rebuild a Var or Call based on whether '(' follows
        ExprPtr e;
        if (m_tok.kind == TokenKind::LParen) {
            e = std::make_unique<Expr>();
            e->loc = identTok.loc;
            Expr::Call call;
            call.name = identTok.text;
            consume(TokenKind::LParen, "'('");
            if (!match(TokenKind::RParen)) {
                while (true) {
                    call.args.push_back(parseExpr());
                    if (match(TokenKind::RParen)) break;
                    consume(TokenKind::Comma, "','");
                }
            }
            e->node = std::move(call);
        } else {
            e = std::make_unique<Expr>();
            e->loc = identTok.loc;
            e->node = Expr::Var{identTok.text};
        }

        // Continue parsing any binary ops after this primary
        return finishExprStmt(std::move(e));
    }

    StmtPtr finishExprStmt(ExprPtr first) {
        auto e = parseBinRhs(0, std::move(first));
        consume(TokenKind::Semicolon, "';'");
        auto s = std::make_unique<Stmt>();
        s->node = Stmt::ExprStmt{std::move(e)};
        return s;
    }

    uint32_t parseConstExpr() {
        auto e = parseExpr();
        auto v = evalConst(*e);
        if (!v) failHere("initializer must be a constant expression");
        return *v;
    }

    std::optional<uint32_t> evalConst(const Expr& e) {
        if (auto* n = std::get_if<Expr::Number>(&e.node)) return n->value;
        if (auto* u = std::get_if<Expr::Unary>(&e.node)) {
            auto rv = evalConst(*u->rhs);
            if (!rv) return std::nullopt;
            switch (u->op) {
                case TokenKind::Minus: return static_cast<uint32_t>(0u - *rv);
                case TokenKind::Tilde: return ~(*rv);
                default: return std::nullopt;
            }
        }
        if (auto* b = std::get_if<Expr::Binary>(&e.node)) {
            auto lv = evalConst(*b->lhs);
            auto rv = evalConst(*b->rhs);
            if (!lv || !rv) return std::nullopt;
            switch (b->op) {
                case TokenKind::Plus: return (*lv) + (*rv);
                case TokenKind::Minus: return (*lv) - (*rv);
                case TokenKind::Amp: return (*lv) & (*rv);
                case TokenKind::Pipe: return (*lv) | (*rv);
                case TokenKind::Caret: return (*lv) ^ (*rv);
                case TokenKind::ShiftLeft: return (*lv) << (*rv);
                case TokenKind::ShiftRight: return (*lv) >> (*rv);
                default: return std::nullopt;
            }
        }
        return std::nullopt;
    }

    // condition ::= expr (== != < <= > >=) expr
    struct CondParts { ExprPtr l; TokenKind op; ExprPtr r; };

    CondParts parseCond() {
        auto l = parseExpr();
        TokenKind op = m_tok.kind;
        switch (op) {
            case TokenKind::EqEq:
            case TokenKind::NotEq:
            case TokenKind::Less:
            case TokenKind::LessEq:
            case TokenKind::Greater:
            case TokenKind::GreaterEq:
                m_tok = m_lex.next();
                break;
            default:
                failHere("expected comparison operator in condition");
        }
        auto r = parseExpr();
        return {std::move(l), op, std::move(r)};
    }

    ExprPtr parseExpr() {
        auto lhs = parseUnary();
        return parseBinRhs(0, std::move(lhs));
    }

    ExprPtr parseUnary() {
        if (m_tok.kind == TokenKind::Minus || m_tok.kind == TokenKind::Tilde) {
            TokenKind op = m_tok.kind;
            auto loc = m_tok.loc;
            m_tok = m_lex.next();
            auto rhs = parseUnary();
            auto e = std::make_unique<Expr>();
            e->loc = loc;
            e->node = Expr::Unary{op, std::move(rhs)};
            return e;
        }
        return parsePrimary();
    }

    ExprPtr parsePrimary() {
        if (m_tok.kind == TokenKind::Number) {
            auto e = std::make_unique<Expr>();
            e->loc = m_tok.loc;
            e->node = Expr::Number{m_tok.number};
            m_tok = m_lex.next();
            return e;
        }
        if (m_tok.kind == TokenKind::Ident || m_tok.kind == TokenKind::Kw_stack) {
            std::string name = m_tok.text;
            auto loc = m_tok.loc;
            if (m_tok.kind == TokenKind::Kw_stack) name = "stack";
            m_tok = m_lex.next();
            if (match(TokenKind::LParen)) {
                Expr::Call call;
                call.name = name;
                if (!match(TokenKind::RParen)) {
                    while (true) {
                        call.args.push_back(parseExpr());
                        if (match(TokenKind::RParen)) break;
                        consume(TokenKind::Comma, "','");
                    }
                }
                auto e = std::make_unique<Expr>();
                e->loc = loc;
                e->node = std::move(call);
                return e;
            }
            auto e = std::make_unique<Expr>();
            e->loc = loc;
            e->node = Expr::Var{std::move(name)};
            return e;
        }
        if (match(TokenKind::LParen)) {
            auto e = parseExpr();
            consume(TokenKind::RParen, "')'");
            return e;
        }
        failHere("expected expression");
        return nullptr;
    }

    int precedence(TokenKind op) {
        switch (op) {
            case TokenKind::Pipe: return 1;
            case TokenKind::Caret: return 2;
            case TokenKind::Amp: return 3;
            case TokenKind::ShiftLeft:
            case TokenKind::ShiftRight: return 4;
            case TokenKind::Plus:
            case TokenKind::Minus: return 5;
            default: return -1;
        }
    }

    ExprPtr parseBinRhs(int exprPrec, ExprPtr lhs) {
        while (true) {
            int tokPrec = precedence(m_tok.kind);
            if (tokPrec < exprPrec) return lhs;

            TokenKind op = m_tok.kind;
            auto loc = m_tok.loc;
            m_tok = m_lex.next();
            auto rhs = parseUnary();

            int nextPrec = precedence(m_tok.kind);
            if (tokPrec < nextPrec) rhs = parseBinRhs(tokPrec + 1, std::move(rhs));

            auto e = std::make_unique<Expr>();
            e->loc = loc;
            e->node = Expr::Binary{op, std::move(lhs), std::move(rhs)};
            lhs = std::move(e);
        }
    }
};

// --- Codegen ---

struct Symbol {
    uint32_t dataOffsetBytes = 0; // resolved after layout
    uint32_t initValue = 0;
    bool isInternal = false;
};

struct Fixup {
    enum class Kind { JumpRel16, BranchRel16, LoadImm24Abs };
    Kind kind;
    size_t instrIndex; // word index in program
    std::string symbol;
    SourceLoc loc;
};

class Codegen {
public:
    explicit Codegen(std::vector<StmtPtr> program) : m_program(std::move(program)) {
        // internal temporaries
        declareInternal("__tmp0");
        declareInternal("__tmp1");
    }

    CompiledProgram compileProgram() {
        // first pass: gather decls and emit code
        for (auto& s : m_program) {
            emitStmt(*s);
        }

        // finalize data layout
        const uint32_t codeBytes = static_cast<uint32_t>(m_words.size() * 4);
        uint32_t dataCursor = 0;
        for (const auto& name : m_symOrder) {
            auto& sym = m_syms.at(name);
            sym.dataOffsetBytes = codeBytes + dataCursor;
            dataCursor += 4;
        }

        // build final blob = code + data
        std::vector<uint32_t> out = m_words;
        for (const auto& n : m_symOrder) {
            out.push_back(m_syms.at(n).initValue);
        }

        // apply fixups (labels & abs addresses)
        applyFixups(out, codeBytes);

        CompiledProgram result;
        result.codeBytes = codeBytes;
        result.dataBytes = static_cast<uint32_t>(m_symOrder.size() * 4u);
        result.stackDeclared = m_stackDeclared;
        result.stackWords = m_stackWords;
        result.symbols.reserve(m_symOrder.size());
        for (const auto& n : m_symOrder) {
            const auto& sym = m_syms.at(n);
            SymbolInfo si;
            si.name = n;
            si.offsetBytes = sym.dataOffsetBytes;
            si.initValue = sym.initValue;
            si.isInternal = sym.isInternal;
            result.symbols.push_back(std::move(si));
        }

        // padding to allowed sizes
        uint32_t totalBytes = static_cast<uint32_t>(out.size() * 4);
        uint32_t padded = pickPaddedSize(totalBytes);
        while (out.size() * 4u < padded) out.push_back(0);

        result.paddedBytes = padded;
        result.words = std::move(out);
        return result;
    }

    std::vector<uint32_t> compile() {
        return compileProgram().words;
    }

private:
    std::vector<StmtPtr> m_program;
    std::vector<uint32_t> m_words;

    std::unordered_map<std::string, Symbol> m_syms;
    std::vector<std::string> m_symOrder;
    std::unordered_map<std::string, uint32_t> m_labels; // byte address of label (code only)
    std::vector<Fixup> m_fixups;

    bool m_stackDeclared = false;
    uint32_t m_stackWords = 0;

    static uint32_t pickPaddedSize(uint32_t bytes) {
        for (uint32_t s : kAllowedProgramSizes) {
            if (bytes <= s) return s;
        }
        throw CompileError("program too large: " + std::to_string(bytes) + " bytes (max 4096)");
    }

    void declareInternal(const std::string& name) {
        if (m_syms.find(name) != m_syms.end()) return;
        Symbol sym;
        sym.isInternal = true;
        sym.initValue = 0;
        m_syms.emplace(name, sym);
        m_symOrder.push_back(name);
    }

    void declareVar(const Stmt::Decl& d, const SourceLoc& loc) {
        if (m_syms.find(d.name) != m_syms.end()) {
            throw CompileError(err(loc) + "duplicate variable: " + d.name);
        }
        Symbol sym;
        sym.isInternal = false;
        sym.initValue = d.initConst.value_or(0);
        if (sym.initValue > 0xFFFFFFu) {
            throw CompileError(err(loc) + "initializer out of range for LOADIMM (24-bit): " + d.name);
        }
        m_syms.emplace(d.name, sym);
        m_symOrder.push_back(d.name);
    }

    static std::string err(const SourceLoc& loc) {
        std::ostringstream oss;
        oss << "(" << loc.line << ":" << loc.col << ") ";
        return oss.str();
    }

    uint32_t labelPcBytes() const {
        return static_cast<uint32_t>(m_words.size() * 4);
    }

    void emit(uint32_t word) { m_words.push_back(word); }

    void emit_ldim(uint32_t dest, uint32_t imm24, const SourceLoc& loc) {
        if (imm24 > 0xFFFFFFu) throw CompileError(err(loc) + "immediate out of range (0..0xFFFFFF)");
        emit(IMMED24(imm24) | DESTREG(dest) | VCP_LOADIMM);
    }

    void emit_math(uint32_t op, uint32_t dest, uint32_t src1, uint32_t src2) {
        emit(IMMED8(op) | SRCREG2(src2) | SRCREG1(src1) | DESTREG(dest) | VCP_MATHOP);
    }

    void emit_logic(uint32_t op, uint32_t dest, uint32_t src1, uint32_t src2) {
        emit(IMMED8(op) | SRCREG2(src2) | SRCREG1(src1) | DESTREG(dest) | VCP_LOGICOP);
    }

    void emit_cmp(uint32_t cond, uint32_t src1, uint32_t src2) {
        emit(IMMED8(cond) | SRCREG2(src2) | SRCREG1(src1) | 0 | VCP_CMP);
    }

    void emit_branchim_label(const std::string& label, const SourceLoc& loc) {
        Fixup f;
        f.kind = Fixup::Kind::BranchRel16;
        f.instrIndex = m_words.size();
        f.symbol = label;
        f.loc = loc;
        emit(IMMED16(0) | DESTREG(1) | VCP_BRANCH);
        m_fixups.push_back(std::move(f));
    }

    void emit_jumpim_label(const std::string& label, const SourceLoc& loc) {
        Fixup f;
        f.kind = Fixup::Kind::JumpRel16;
        f.instrIndex = m_words.size();
        f.symbol = label;
        f.loc = loc;
        emit(IMMED16(0) | DESTREG(1) | VCP_JUMP);
        m_fixups.push_back(std::move(f));
    }

    // load absolute address of variable into dest
    void emit_ldaddr(uint32_t dest, const std::string& varName, const SourceLoc& loc) {
        Fixup f;
        f.kind = Fixup::Kind::LoadImm24Abs;
        f.instrIndex = m_words.size();
        f.symbol = varName;
        f.loc = loc;
        emit(IMMED24(0) | DESTREG(dest) | VCP_LOADIMM);
        m_fixups.push_back(std::move(f));
    }

    // load variable value into dest
    void emit_loadVar(uint32_t dest, const std::string& varName, const SourceLoc& loc) {
        ensureVar(varName, loc);
        // addr in r3, then LOAD(addr, dest)
        emit_ldaddr(3, varName, loc);
        emit(SRCREG1(3) | DESTREG(dest) | VCP_LOAD);
    }

    void emit_storeVar(const std::string& varName, uint32_t src, const SourceLoc& loc) {
        ensureVar(varName, loc);
        emit_ldaddr(3, varName, loc);
        emit(SRCREG2(src) | SRCREG1(3) | VCP_STORE);
    }

    void ensureVar(const std::string& name, const SourceLoc& loc) {
        if (m_syms.find(name) == m_syms.end()) {
            throw CompileError(err(loc) + "unknown variable: " + name);
        }
    }

    // Expr codegen: result in reg2, uses reg3/reg4 as scratch, spills to __tmp0/__tmp1.
    void emitExprToR2(const Expr& e) {
        if (auto* n = std::get_if<Expr::Number>(&e.node)) {
            emit_ldim(2, n->value, e.loc);
            return;
        }
        if (auto* v = std::get_if<Expr::Var>(&e.node)) {
            emit_loadVar(2, v->name, e.loc);
            return;
        }
        if (auto* u = std::get_if<Expr::Unary>(&e.node)) {
            emitExprToR2(*u->rhs);
            if (u->op == TokenKind::Minus) {
                // 0 - r2
                emit_ldim(3, 0, e.loc);
                emit_math(OP_SUB, 2, 3, 2);
                return;
            }
            if (u->op == TokenKind::Tilde) {
                // NEG (bitwise NOT)
                emit(IMMED8(OPL_NEG) | SRCREG1(2) | DESTREG(2) | VCP_LOGICOP);
                return;
            }
            throw CompileError(err(e.loc) + "unsupported unary operator");
        }
        if (auto* b = std::get_if<Expr::Binary>(&e.node)) {
            // evaluate lhs -> r2, spill, rhs -> r2, move to r3, reload lhs to r2
            emitExprToR2(*b->lhs);
            emit_storeVar("__tmp0", 2, e.loc);
            emitExprToR2(*b->rhs);
            emit_math(OP_ADD, 3, 2, VREG_ZERO); // r3 = rhs
            emit_loadVar(2, "__tmp0", e.loc);

            switch (b->op) {
                case TokenKind::Plus: emit_math(OP_ADD, 2, 2, 3); return;
                case TokenKind::Minus: emit_math(OP_SUB, 2, 2, 3); return;
                case TokenKind::Amp: emit_logic(OPL_AND, 2, 2, 3); return;
                case TokenKind::Pipe: emit_logic(OPL_OR, 2, 2, 3); return;
                case TokenKind::Caret: emit_logic(OPL_XOR, 2, 2, 3); return;
                case TokenKind::ShiftLeft: emit_logic(OPL_SHL, 2, 2, 3); return;
                case TokenKind::ShiftRight: emit_logic(OPL_SHR, 2, 2, 3); return;
                default: throw CompileError(err(e.loc) + "unsupported binary operator");
            }
        }
        if (auto* c = std::get_if<Expr::Call>(&e.node)) {
            // calls are only allowed as statements; still allow here for stack/pop etc if needed.
            throw CompileError(err(e.loc) + "call expression not allowed in value context: " + c->name);
        }
        throw CompileError(err(e.loc) + "unknown expression node");
    }

    uint32_t condToCode(TokenKind op, const SourceLoc& loc) {
        switch (op) {
            case TokenKind::EqEq: return COND_EQ;
            case TokenKind::NotEq: return COND_NE;
            case TokenKind::Less: return COND_LT;
            case TokenKind::LessEq: return COND_LE;
            case TokenKind::Greater: return COND_GT;
            case TokenKind::GreaterEq: return COND_GE;
            default: throw CompileError(err(loc) + "unsupported comparison operator");
        }
    }

    void emitCondBranchToLabel(const Expr& l, TokenKind op, const Expr& r, const std::string& trueLabel, const SourceLoc& loc) {
        emitExprToR2(l);
        emit_math(OP_ADD, 4, 2, VREG_ZERO); // r4 = lhs
        emitExprToR2(r);
        emit_cmp(condToCode(op, loc), 4, 2);
        emit_branchim_label(trueLabel, loc);
    }

    void emitStmt(const Stmt& s) {
        if (auto* d = std::get_if<Stmt::Decl>(&s.node)) {
            declareVar(*d, s.loc);
            return;
        }
        if (auto* a = std::get_if<Stmt::Assign>(&s.node)) {
            emitExprToR2(*a->value);
            emit_storeVar(a->name, 2, s.loc);
            return;
        }
        if (auto* b = std::get_if<Stmt::Block>(&s.node)) {
            for (auto& ss : b->stmts) emitStmt(*ss);
            return;
        }
        if (auto* l = std::get_if<Stmt::Label>(&s.node)) {
            if (m_labels.find(l->name) != m_labels.end()) {
                throw CompileError(err(s.loc) + "duplicate label: " + l->name);
            }
            m_labels[l->name] = labelPcBytes();
            return;
        }
        if (auto* g = std::get_if<Stmt::Goto>(&s.node)) {
            emit_jumpim_label(g->target, s.loc);
            return;
        }
        if (auto* iff = std::get_if<Stmt::If>(&s.node)) {
            std::string thenLabel = freshLabel("then");
            std::string endLabel = freshLabel("endif");
            std::string elseLabel = freshLabel("else");

            emitCondBranchToLabel(*iff->condL, iff->condOp, *iff->condR, thenLabel, s.loc);
            if (iff->elseS) {
                emitStmt(*iff->elseS);
                emit_jumpim_label(endLabel, s.loc);
                m_labels[thenLabel] = labelPcBytes();
                emitStmt(*iff->thenS);
                m_labels[endLabel] = labelPcBytes();
            } else {
                emit_jumpim_label(endLabel, s.loc);
                m_labels[thenLabel] = labelPcBytes();
                emitStmt(*iff->thenS);
                m_labels[endLabel] = labelPcBytes();
            }
            (void)elseLabel;
            return;
        }
        if (auto* wh = std::get_if<Stmt::While>(&s.node)) {
            std::string startLabel = freshLabel("while");
            std::string bodyLabel = freshLabel("body");
            std::string endLabel = freshLabel("endwhile");

            m_labels[startLabel] = labelPcBytes();
            emitCondBranchToLabel(*wh->condL, wh->condOp, *wh->condR, bodyLabel, s.loc);
            emit_jumpim_label(endLabel, s.loc);
            m_labels[bodyLabel] = labelPcBytes();
            emitStmt(*wh->body);
            emit_jumpim_label(startLabel, s.loc);
            m_labels[endLabel] = labelPcBytes();
            return;
        }
        if (auto* es = std::get_if<Stmt::ExprStmt>(&s.node)) {
            emitIntrinsicStmt(*es->expr);
            return;
        }
        throw CompileError(err(s.loc) + "unsupported statement");
    }

    void emitIntrinsicStmt(const Expr& e) {
        auto* call = std::get_if<Expr::Call>(&e.node);
        if (!call) {
            // allow bare variable expression? no-op
            return;
        }

        const auto& name = call->name;
        if (name == "wait_scanline") {
            expectArgs(*call, 1, e.loc);
            emitExprToR2(*call->args[0]);
            emit(SRCREG1(2) | VCP_WAITSCANLINE);
            return;
        }
        if (name == "wait_pixel") {
            expectArgs(*call, 1, e.loc);
            emitExprToR2(*call->args[0]);
            emit(SRCREG1(2) | VCP_WAITPIXEL);
            return;
        }
        if (name == "pal_write") {
            expectArgs(*call, 2, e.loc);
            emitExprToR2(*call->args[0]);
            emit_math(OP_ADD, 3, 2, VREG_ZERO); // r3 = addr
            emitExprToR2(*call->args[1]);
            emit(SRCREG2(2) | SRCREG1(3) | VCP_PALWRITE);
            return;
        }
        if (name == "scanline_read") {
            expectArgs(*call, 1, e.loc);
            auto* v = std::get_if<Expr::Var>(&call->args[0]->node);
            if (!v) throw CompileError(err(e.loc) + "scanline_read expects a variable");
            emit(SRCREG1(0) | DESTREG(2) | VCP_READSCANINFO);
            emit_storeVar(v->name, 2, e.loc);
            return;
        }
        if (name == "scanpixel_read") {
            expectArgs(*call, 1, e.loc);
            auto* v = std::get_if<Expr::Var>(&call->args[0]->node);
            if (!v) throw CompileError(err(e.loc) + "scanpixel_read expects a variable");
            emit(SRCREG1(1) | DESTREG(2) | VCP_READSCANINFO);
            emit_storeVar(v->name, 2, e.loc);
            return;
        }
        if (name == "store") {
            expectArgs(*call, 2, e.loc);
            if (auto ca = evalConstExpr(*call->args[0]); ca && ((*ca % 4u) != 0u)) {
                throw CompileError(err(e.loc) + "store address must be 4-byte aligned");
            }
            emitExprToR2(*call->args[0]);
            emit_math(OP_ADD, 3, 2, VREG_ZERO); // r3 = addr
            emitExprToR2(*call->args[1]);
            emit(SRCREG2(2) | SRCREG1(3) | VCP_STORE);
            return;
        }
        if (name == "load") {
            expectArgs(*call, 2, e.loc);
            if (auto ca = evalConstExpr(*call->args[0]); ca && ((*ca % 4u) != 0u)) {
                throw CompileError(err(e.loc) + "load address must be 4-byte aligned");
            }
            emitExprToR2(*call->args[0]);
            emit_math(OP_ADD, 3, 2, VREG_ZERO); // r3 = addr
            auto* v = std::get_if<Expr::Var>(&call->args[1]->node);
            if (!v) throw CompileError(err(e.loc) + "load(addr, var) expects a variable as second arg");
            emit(SRCREG1(3) | DESTREG(2) | VCP_LOAD);
            emit_storeVar(v->name, 2, e.loc);
            return;
        }
        if (name == "stack") {
            expectArgs(*call, 1, e.loc);
            if (m_stackDeclared) throw CompileError(err(e.loc) + "stack() already declared");
            auto v = evalConstExpr(*call->args[0]);
            if (!v || *v == 0) throw CompileError(err(e.loc) + "stack(words) expects a positive constant");
            m_stackDeclared = true;
            m_stackWords = *v;
            // allocate __sp and stack slots as separate variables (word-addressable)
            declareInternal("__sp");
            for (uint32_t i = 0; i < m_stackWords; ++i) {
                declareInternal("__stack_" + std::to_string(i));
            }
            // init __sp later in fixup stage once addresses are known (we'll patch in applyFixups)
            // Represent init via a special fixup: LoadImm24Abs to __sp isn't an instruction, so instead we set initValue after layout.
            return;
        }
        if (name == "push") {
            expectArgs(*call, 1, e.loc);
            ensureStack(e.loc);
            // r2 = __sp
            emit_loadVar(2, "__sp", e.loc);
            emit_math(OP_ADD, 3, 2, VREG_ZERO); // r3 = sp addr
            // r2 = value
            emitExprToR2(*call->args[0]);
            // store [sp] = value
            emit(SRCREG2(2) | SRCREG1(3) | VCP_STORE);
            // sp += 4
            emit_ldim(4, 4, e.loc);
            emit_math(OP_ADD, 3, 3, 4);
            emit_storeVar("__sp", 3, e.loc);
            return;
        }
        if (name == "pop") {
            expectArgs(*call, 1, e.loc);
            ensureStack(e.loc);
            auto* v = std::get_if<Expr::Var>(&call->args[0]->node);
            if (!v) throw CompileError(err(e.loc) + "pop(var) expects a variable");
            // r3 = __sp
            emit_loadVar(3, "__sp", e.loc);
            // sp -= 4
            emit_ldim(4, 4, e.loc);
            emit_math(OP_SUB, 3, 3, 4);
            emit_storeVar("__sp", 3, e.loc);
            // load [sp] into r2
            emit(SRCREG1(3) | DESTREG(2) | VCP_LOAD);
            emit_storeVar(v->name, 2, e.loc);
            return;
        }

        throw CompileError(err(e.loc) + "unknown intrinsic: " + name);
    }

    void ensureStack(const SourceLoc& loc) {
        if (!m_stackDeclared) throw CompileError(err(loc) + "stack(words) must be declared before push/pop");
    }

    std::optional<uint32_t> evalConstExpr(const Expr& e) {
        // minimal const evaluator
        if (auto* n = std::get_if<Expr::Number>(&e.node)) return n->value;
        if (auto* u = std::get_if<Expr::Unary>(&e.node)) {
            auto rv = evalConstExpr(*u->rhs);
            if (!rv) return std::nullopt;
            if (u->op == TokenKind::Minus) return static_cast<uint32_t>(0u - *rv);
            if (u->op == TokenKind::Tilde) return ~(*rv);
            return std::nullopt;
        }
        if (auto* b = std::get_if<Expr::Binary>(&e.node)) {
            auto lv = evalConstExpr(*b->lhs);
            auto rv = evalConstExpr(*b->rhs);
            if (!lv || !rv) return std::nullopt;
            switch (b->op) {
                case TokenKind::Plus: return (*lv) + (*rv);
                case TokenKind::Minus: return (*lv) - (*rv);
                case TokenKind::Amp: return (*lv) & (*rv);
                case TokenKind::Pipe: return (*lv) | (*rv);
                case TokenKind::Caret: return (*lv) ^ (*rv);
                case TokenKind::ShiftLeft: return (*lv) << (*rv);
                case TokenKind::ShiftRight: return (*lv) >> (*rv);
                default: return std::nullopt;
            }
        }
        return std::nullopt;
    }

    void expectArgs(const Expr::Call& c, size_t n, const SourceLoc& loc) {
        if (c.args.size() != n) {
            throw CompileError(err(loc) + "wrong argument count for " + c.name + ": expected " + std::to_string(n));
        }
    }

    std::string freshLabel(const std::string& base) {
        return "__" + base + "_" + std::to_string(m_labelCounter++);
    }

    size_t m_labelCounter = 0;

    void applyFixups(std::vector<uint32_t>& out, uint32_t codeBytes) {
        // initialize __sp to the address of first stack slot if stack declared
        if (m_stackDeclared) {
            // data appended in declaration/insertion order; set initValue now based on resolved symbol offsets.
            // __sp should point to the first stack slot's address
            auto itSp = m_syms.find("__sp");
            auto it0 = m_syms.find("__stack_0");
            if (itSp != m_syms.end() && it0 != m_syms.end()) {
                itSp->second.initValue = it0->second.dataOffsetBytes;
            }
        }

        for (const auto& f : m_fixups) {
            if (f.kind == Fixup::Kind::JumpRel16 || f.kind == Fixup::Kind::BranchRel16) {
                auto it = m_labels.find(f.symbol);
                if (it == m_labels.end()) throw CompileError(err(f.loc) + "unknown label: " + f.symbol);
                uint32_t srcPc = static_cast<uint32_t>(f.instrIndex * 4);
                int32_t rel = static_cast<int32_t>(it->second) - static_cast<int32_t>(srcPc);
                if (rel < -32768 || rel > 32767) throw CompileError(err(f.loc) + "branch/jump offset out of range: " + f.symbol);
                uint32_t u16 = static_cast<uint32_t>(static_cast<uint16_t>(rel));
                out[f.instrIndex] = (out[f.instrIndex] & 0x0000FFFFu) | IMMED16(u16);
                continue;
            }
            if (f.kind == Fixup::Kind::LoadImm24Abs) {
                auto it = m_syms.find(f.symbol);
                if (it == m_syms.end()) throw CompileError(err(f.loc) + "unknown symbol: " + f.symbol);
                uint32_t abs = it->second.dataOffsetBytes;
                if (abs > 0xFFFFFFu) throw CompileError(err(f.loc) + "address out of range for LOADIMM: " + f.symbol);
                // Preserve low byte (dest reg + opcode), overwrite upper 24 bits.
                out[f.instrIndex] = (out[f.instrIndex] & 0x000000FFu) | IMMED24(abs);
                continue;
            }
        }

        // patch data initializers into output data region (since we just updated initValue for __sp potentially)
        // Data words start at codeWords index
        const size_t codeWords = codeBytes / 4;
        for (size_t i = 0; i < m_symOrder.size(); ++i) {
            out[codeWords + i] = m_syms[m_symOrder[i]].initValue;
        }
    }
};

static std::string readAllText(const std::string& path) {
    std::ifstream f(path, std::ios::binary);
    if (!f) throw std::runtime_error("failed to open input: " + path);
    std::ostringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

static void writeBinaryLE32(const std::string& path, const std::vector<uint32_t>& words) {
    std::ofstream o(path, std::ios::binary);
    if (!o) throw std::runtime_error("failed to open output: " + path);
    for (uint32_t w : words) {
        uint8_t b[4] = {
            static_cast<uint8_t>(w & 0xFFu),
            static_cast<uint8_t>((w >> 8) & 0xFFu),
            static_cast<uint8_t>((w >> 16) & 0xFFu),
            static_cast<uint8_t>((w >> 24) & 0xFFu),
        };
        o.write(reinterpret_cast<const char*>(b), 4);
    }
}

struct Args {
    std::string inPath;
    std::string outPath;
    bool dumpAsm = false;
};

static Args parseArgs(int argc, char** argv) {
    Args a;
    for (int i = 1; i < argc; ++i) {
        std::string s = argv[i];
        if (s == "-o" || s == "--out") {
            if (i + 1 >= argc) throw std::runtime_error("missing value for -o");
            a.outPath = argv[++i];
            continue;
        }
        if (s == "-S") {
            a.dumpAsm = true;
            continue;
        }
        if (!s.empty() && s[0] == '-') {
            throw std::runtime_error("unknown argument: " + s);
        }
        if (a.inPath.empty()) {
            a.inPath = s;
            continue;
        }
        throw std::runtime_error("unexpected extra arg: " + s);
    }
    if (a.inPath.empty()) throw std::runtime_error("usage: vcpcompiler <input.vcp> [-o <output.bin>] [-S]");
    if (a.outPath.empty()) {
        a.outPath = a.inPath + ".bin";
    }
    return a;
}

} // namespace

int main(int argc, char** argv) {
    try {
        auto args = parseArgs(argc, argv);
        std::string src = readAllText(args.inPath);
        Lexer lex(std::move(src));
        Parser parser(std::move(lex));
        auto program = parser.parseProgram();

        Codegen cg(std::move(program));
        auto compiled = cg.compileProgram();
        if (args.dumpAsm) {
            dumpDisassembly(compiled, std::cout);
            std::cout << "\n";
        }
        writeBinaryLE32(args.outPath, compiled.words);

        std::cerr << "Wrote " << (compiled.words.size() * 4u) << " bytes to " << args.outPath << "\n";
        return 0;
    } catch (const CompileError& e) {
        std::cerr << "compile error: " << e.what() << "\n";
        return 2;
    } catch (const std::exception& e) {
        std::cerr << "error: " << e.what() << "\n";
        return 1;
    }
}
