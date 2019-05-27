#include "configuration_engine.h"
#include <sstream>
#include <vector>
#include <string>
#include <cstring>

using std::istringstream;
using std::ostringstream;
using std::vector;
using std::string;

#define i (*reinterpret_cast<ConfigurationParserInternals*>(internals))

template <typename T>
T from_string(string in)
{
	istringstream sin(in);
	T x;
	sin >> x;
	return x;
}

enum class TokenType
{
	MODULE, NAME, EXTENDS, LOAD, LBRACE, RBRACE, INTEGER, STRING, DOUBLE, FLOAT, INTNO, DOUBLENO, STRINGLIT, SEMICOLON
};

class Token
{
public:
	string lexeme;
	TokenType type;
};

class ConfigurationParserInternals
{
public:
	istringstream input_stream;
	vector<Token> tokens;
};

class TokenReturn
{
	bool status;
	Token token;
};

ConfigurationParser::ConfigurationParser()
{
	internals = reinterpret_cast<void*>(new ConfigurationParserInternals);
}

ConfigurationParser::~ConfigurationParser()
{
	if (internals != nullptr)
		delete reinterpret_cast<ConfigurationParserInternals*>(internals);
}

// Lex the configuration file
void ConfigurationParser::load_configuration_file(const char* file)
{
	input_stream = istringstream(file);
}

ConfigurationParser::possibly_get_token(std::string str)
{
	if (str == "Module" || str == "module")
	{
		TokenReturn t;
		t.token.type = TokenType::MODULE;
		t.status = true;
		return t;
	}

	if (str == "extends" || str == "EXTENDS")
	{
		TokenReturn t;
		t.token.type = TokenType::EXTENDS;
		t.status = true;
		return t;
	}

	if (str == "Load" || str == "load")
	{
		Token t;
		t.token.type = TokenType::LOAD;
		TokenReturn ret;
		ret.token = t;
		ret.status = true;
		return ret;
	}

	if (str == "{")
	{
		TokenReturn t;
		t.token.type = TokenType::LBRACE;
		t.status = true;
		return t;
	}

	if (str == "}")
	{
		TokenReturn t;
		t.token.type = TokenType::RBRACE;
		t.status = true;
		return t;
	}

	if (str == "Integer")
	{
		TokenReturn t;
		t.token.type = TokenType::INTEGER;
		t.status = true;
		return t;
	}

	if (str == "String")
	{
		TokenReturn t;
		t.token.type = TokenType::STRING;
		t.status = true;
		return t;
	}

	if (str == "Double")
	{
		TokenReturn t;
		t.token.type = TokenType::DOUBLE;
		t.status = true;
		return t;
	}

	if (str == "Float")
	{
		TokenReturn t;
		t.token.type = TokenType::DOUBLE;
		t.status = true;
		return t;
	}

	// strings
	if (str.front() == '\"')
	{
		std::ostringstream str_out;
		TokenReturn t;
		t.token.type = TokenType::STRINGLIT;
		while (!i.input_stream.eof())
		{
			char in = e.input_stream.read();
			if (in == '\\')
			{
				// String started, failed to end
				if (e.input_stream.eof())
				{
					std::runtime_exception e(CONFIGURATION_PARSER_ERROR_SYNTAX);
					throw e;
				}
				char ch = e.input_stream.read();

				if (ch == 'n')
					str_out << '\n';
				else if (ch == 't')
					str_out << '\t';
				else if (ch == '\"')
					str_out << '\"';
				else if (ch == '\'')
					str_out << '\'';
				else if (ch == '\\')
					str_out << '\\';
				else if (ch == 'f')
					str_out << 'f';
				else if (ch == 'r')
					str_out << '\r';
				else
				{
					std::runtime_exception e(CONFIGURATION_PARSER_ERROR_SYNTAX);
					throw e;
				}
			}
			else if (in == '\"')
			{
				break;
			}
			else
			{
				str_out << in;
			}
		}
		t.token.lexeme = str_out.str();
		t.status = true;
		return t;
	}
	
	// number literals
	if (isalpha(str.front()))
	{
		TokenReturn t;
		ostringstream str_out;
		str_out << str.front();
		bool flt = false;
		while (!i.input_stream.eof())
		{
			char ch = i.input_stream.read();
			if (isalpha(ch) || ch == '.')
			{
				if (ch == '.')
				{
					if (flt)
					{
						std::runtime_exception e(CONFIGURATION_PARSER_ERROR_SYNTAX);
						throw e;
					}
					flt = true;
				}
				str_out << ch;
			}
			else if (isspace(ch))
			{
				break;
			}
			else
			{
				std::runtime_exception e(CONFIGURATION_PARSER_ERROR_SYNTAX);
				throw e;
			}
		}
		t.token.lexeme = str.str();
		t.token.type = (flt) ? TokenType::DOUBLENO : TokenType::INTNO;
		t.status = true;
		return t;
	}
	
	// name
	if (isalpha(str.front()))
	{
		TokenReturn t;
		t.status = false;
		for (size_t i = 1; i < str.size() - 1; i++)
		{
			if (!isalnum(str[i]) && !isspace(str[i]))
			{
				std::runtime_exception e(CONFIGURATION_PARSER_ERROR_SYNTAX);
				throw e;
			}
			if (isspace(str[i]))
			{
				t.token.lexeme = str.substr(0, str.size() - 2);
				t.status = true;
			}
		}

		return t;
	}

	else
	{
		TokenReturn t;
		t.status = false;
		return t;
	}
}

void ConfigurationParser::lex()
{
	std::string buffer;
	char ch;
	while (!i.input_stream.eof())
	{
		ch = i.read();
		if (isspace(ch))
			continue;

		buffer += ch;
		TokenRet ret = possibly_get_token(buffer);
		if (ret.success)
		{
			i.tokens.push_back(ret.token);
		}
	}
}

const char* CONFIGURATION_PARSER_ERROR_SYNTAX = "Syntax error in the configuration file";
