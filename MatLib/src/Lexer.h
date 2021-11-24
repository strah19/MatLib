#ifndef LEXER_H
#define LEXER_H

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

namespace MatLib {
    namespace Tok {
        enum {
            T_COLON = ':',
            T_PLUS = '+',
            T_SLASH = '/',
            T_STAR = '*',
            T_MINUS = '-',
            T_EQUAL = '=',
            T_CARET = '^',
            T_AMBERSAND = '&',

            T_LPAR = '(',
            T_RPAR = ')',
            T_LBRACKET = '[',
            T_RBRACKET = ']',
            T_LCURLY = '{',
            T_RCURLY = '}',
            T_PERCENT = '%',
            T_LARROW = '<',
            T_RARROW = '>',
            T_COMMA = ',',

            T_EOF = 255,      

            T_IDENTIFIER,
            T_NUM_CONST,
            T_LTE,
            T_DOUBLE_EQUAL,
            T_NOT,
            T_GTE,
            T_NEWLINE,
        };
    }

    struct Token {
        int type = 0;
        uint32_t line = 0;

        double num_const = 0.0;
        std::string id = "";
    };

    class Lexer {
    public:
        using Searchable = std::unordered_map<int, std::string>;
        Lexer();

        void Input(const std::string& input);
        void Run();
        void Log();
        void Clear() { tokens.clear(); }
        std::string DecodeToken(Token* token);
        std::vector<Token>& Tokens() { return tokens; }

        Searchable& Symbols() { return symbols; }
        Searchable& Keywords() { return keywords; }

        bool Search(const std::string& possible, Searchable& search);
    private:
        enum class TokenCategories {
            NONE,
            NUMERIC,
            ID,
            SYMBOL
        };

        std::string input;
        std::vector<Token> tokens;

        TokenCategories current_possible_token_type = TokenCategories::NONE;
        std::string working;

        std::unordered_map<int, std::string> symbols;
        std::unordered_map<int, std::string> keywords;
        uint32_t current_character = 0;
        uint32_t current_line = 0;
    private:
        void CreateToken(int type);
        char NextChar();
        bool IsDigit(uint32_t offset = 0);
        bool IsCharacter(uint32_t offset = 0);
        bool IsSymbol(uint32_t offset = 0);
        bool Limit();
        void ResetStatus();
        std::string SpaceLess();
        uint32_t ReadTill(std::string& temp, const std::function<bool(void)>& condition);
    };
}

#endif // !LEXER_H
