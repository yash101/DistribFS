#ifndef _CONFIGURATION_ENGINE_H
#define _CONFIGURATION_ENGINE_H

class ConfigurationParser
{
	void* internals;
	void ConfigurationParser::lex();
public:
	ConfigurationParser();
	~ConfigurationParser();
	
	void load_configuration_file(const char* file);
};

extern const char* CONFIGURATION_PARSER_ERROR_SYNTAX;

#endif
