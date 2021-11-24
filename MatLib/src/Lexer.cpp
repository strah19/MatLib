#include "Lexer.h"
#include "Logger.h"

namespace MatLib {
	Lexer::Lexer() { 
		symbols[Tok::T_LTE] = "<=";
		symbols[Tok::T_GTE] = ">=";
		symbols[Tok::T_DOUBLE_EQUAL] = "==";
		symbols[Tok::T_NOT] = "!=";
	}

	void Lexer::Input(const std::string& input) {
		this->input = input;
	}

	void Lexer::CreateToken(int type) {
		tokens.push_back(Token());
		tokens.back().type = type;
		tokens.back().line = current_line;
	}
	
	void Lexer::Run() {
		current_character = 0;
		current_line = 1;
		while (current_character < input.size()) {
			if (input[current_character] == '\n') {
				current_line++;
				current_character++;
				CreateToken(Tok::T_NEWLINE);
				continue;
			}

			working += input[current_character];


			if (current_possible_token_type == TokenCategories::NONE) {	//Don't know what the current character could be
				if (isdigit(working.back()))
					current_possible_token_type = TokenCategories::NUMERIC;
				else if (isalpha(working.back())) 
					current_possible_token_type = TokenCategories::ID;
				else if (working.back() != ' ')
					current_possible_token_type = TokenCategories::SYMBOL;
			}

			if (current_possible_token_type == TokenCategories::NUMERIC && (Limit() || !IsDigit(1))) {
				CreateToken(Tok::T_NUM_CONST);
				tokens.back().num_const = atoi(working.c_str());
				ResetStatus();
			}
			else if (current_possible_token_type == TokenCategories::ID && (Limit() || !IsCharacter(1) || NextChar() == ' ')) {
				std::string temp = SpaceLess();
				uint32_t offset = 0;
				offset = ReadTill(temp, [&]() {
					return (!Limit() && IsCharacter(1) && NextChar() != ' ');
				});
				if (Search(temp, keywords)) continue;
				else {
					CreateToken(Tok::T_IDENTIFIER);
					tokens.back().id = working;
					ResetStatus();
				}
			}
			else if (current_possible_token_type == TokenCategories::SYMBOL) {
				std::string temp = SpaceLess();
				uint32_t offset = ReadTill(temp, [&]() {
					return (!Limit() && IsSymbol(1) && NextChar() != ' ');
				});
				if (Search(temp, symbols)) continue;
				else {
					CreateToken(working[0]);
					tokens.back().id = working;
					current_character -= offset;
					ResetStatus();
				}
			}

			current_character++;
		}

		CreateToken(Tok::T_EOF);
		ResetStatus();
	}

	bool Lexer::Search(const std::string& possible, Searchable& search) {
		for (auto& s : search) {
			if (s.second == possible) {
				CreateToken(s.first);
				current_character++;
				ResetStatus();
				return true;
			}
		}
		return false;
	}

	char Lexer::NextChar() {
		return input[(current_character + 1) % input.size()];
	}

	bool Lexer::IsDigit(uint32_t offset) {
		return (isdigit(input[current_character + offset]));
	}

	bool Lexer::IsCharacter(uint32_t offset) {
		return (isalpha(input[current_character + offset]));
	}

	bool Lexer::IsSymbol(uint32_t offset) {
		return (!IsDigit(offset) && !IsCharacter(offset));
	}

	bool Lexer::Limit() {
		return (current_character + 1 == input.size());
	}

	void Lexer::ResetStatus() {
		current_possible_token_type = TokenCategories::NONE;
		working.clear();
	}

	uint32_t Lexer::ReadTill(std::string& temp, const std::function<bool(void)>& condition) {
		uint32_t offset = 0;
		while (condition()) {
			temp += input[current_character + 1];
			current_character++;
			offset++;
		}
		return offset;
	}

	std::string Lexer::SpaceLess() {
		working.erase(std::remove_if(working.begin(), working.end(), isspace), working.end());	
		return working;
	}

	void Lexer::Log() {
		printf("----Lexer Tokens----\n");
		
		for (size_t i = 0; i < tokens.size(); i++) {
			std::string s = DecodeToken(&tokens[i]);
			if (!s.empty())
				printf("[%d] : %d : '%s'\n", i, tokens[i].type, s.c_str());
			else
				printf("[%d] : %d : '%c'\n", i, tokens[i].type, tokens[i].type);
		}
	}

	std::string Lexer::DecodeToken(Token* token) {
		if (token->type > 255 && token->type != Tok::T_IDENTIFIER && token->type != Tok::T_NUM_CONST) {
			return symbols[token->type];
		}
		switch (token->type) {
		case Tok::T_IDENTIFIER:
			return token->id;
		case Tok::T_NUM_CONST:
			return std::to_string(token->num_const);
		default:
			return "";
		}
	}
}