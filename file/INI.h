/***************************************
* @file     INI.h
* @brief    ini文件读写工具
* @details  不支持#注释，不支持空行（解析会崩溃）
换行符必须是unix格式，不然会解析错误
* @author   phata, wqvbjhc@gmail.com
* @date     2014-7-28
* @mod      2014-07-31 phata 在github上添加issue作者没回应，自己修改了
                             支持//,#,;注释
							 支持空行
							 支持win & unix格式的换行符
			2014-09-19 phata 修复一处内存泄露.github上别人提出的bug.
****************************************/
/*
Feather INI Parser - 1.32
You are free to use this for whatever you wish.

If you find a bug, please debug the cause and look into a solution.
Post your compiler, version and the cause or fix in the issues section.

Written by Turbine.

Website:
https://github.com/Turbine1991/feather-ini-parser
http://code.google.com/p/feather-ini-parser/downloads

Help:
Bundled example & readme.
http://code.google.com/p/feather-ini-parser/wiki/Tutorials
*/

#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define FINI_SAFE
#define FINI_BUFFER_SIZE 128

#if defined(__GXX_EXPERIMENTAL_CXX0X) || __cplusplus >= 201103L
#include <unordered_map>
#define FINI_CPP11
#define ALLOCATE_SECTIONS 100
#define ALLOCATE_KEYS 5
#else
#include <map>
#endif

#ifdef FINI_WIDE_SUPPORT
#include <wchar.h>

typedef std::wstringstream fini_sstream_t;
typedef std::wstring fini_string_t;
typedef wchar_t fini_char_t;
typedef unsigned wchar_t fini_uchar_t;
typedef std::wifstream fini_ifstream_t;
typedef std::wofstream fini_ofstream_t;

#define fini_strlen(a) wcslen(a)
#define fini_strncpy(a, b, c) wcsncpy(a, b, c)
#define fini_strtok(a, b) wcstok(a, b)

#define _T(x) L ##x
#else
#include <cstring>

typedef std::stringstream fini_sstream_t;
typedef std::string fini_string_t;
typedef char fini_char_t;
typedef unsigned char fini_uchar_t;
typedef std::ifstream fini_ifstream_t;
typedef std::ofstream fini_ofstream_t;

#define fini_strlen(a) strlen(a)
#define fini_strncpy(a, b, c) strncpy(a, b, c)
#define fini_strtok(a, b) strtok(a, b)

#define _T(x) x
#endif

#define char_size_t sizeof(fini_char_t)

///Simple converter required for the flexibility of handling templated types generically
class Converters
{
public:
	template<class T>
	static fini_string_t ToString(T value)
	{
#ifdef FINI_CPP11
		return std::to_string(value);
#else
		return Convert<fini_string_t>(value);
#endif
	}

	static fini_string_t& ToString(fini_string_t& value)
	{
		return value;
	}

	template<class T, class U>
	static T Convert(U value)
	{
		fini_sstream_t sout;
		T result;

		sout << value;
		sout >> result;

		sout.str(fini_string_t());

		return result;
	}

	static fini_string_t Convert(fini_char_t value)
	{
		return Converters::ToString(+value); //Numeric value
	}

	static fini_string_t Convert(fini_uchar_t value)
	{
		return Converters::ToString(+value); //Numeric value
	}

	static fini_string_t& Convert(fini_string_t& value)
	{
		return value;
	}

	static void GetLine(fini_sstream_t& out, fini_string_t& value)
	{
		std::getline(out, value);
	}

	template<class T>
	static void GetLine(fini_sstream_t& out, T& value)
	{
		out >> value;
	}

	static size_t GetDataSize(fini_string_t value)
	{
		return value.size() + 1;
	}

	template<class T>
	static size_t GetDataSize(T& value)
	{
		return sizeof(value);
	}
};

///
template<class T = fini_string_t, class U = fini_string_t, class V = fini_string_t>
class INI
{
public:
	typedef T section_t;
	typedef U key_t;
	typedef V value_t;

	///Type definition declarations
#ifdef FINI_CPP11
	typedef typename std::unordered_map<key_t, value_t> keys_t;
	typedef typename std::unordered_map<section_t, keys_t*> sections_t;
#else
	typedef typename std::map<key_t, value_t> keys_t;
	typedef typename std::map<section_t, keys_t*> sections_t;
#endif

	typedef typename keys_t::iterator keysit_t;
	typedef typename sections_t::iterator sectionsit_t;

	typedef typename std::pair<key_t, value_t> keyspair_t;
	typedef typename std::pair<section_t, keys_t*> sectionspair_t;

	typedef char data_t;

	enum source_e {SOURCE_FILE, SOURCE_MEMORY};

	///Data members
	std::string filename;
	data_t* data;
	size_t dataSize;
	keys_t* current;
	sections_t sections;
	source_e source;

	///Constuctor/Destructor
	//Specify the filename to associate and whether to parse immediately
	INI(std::string filename, bool doParse = true): filename(filename)
	{
		init(SOURCE_FILE, doParse);
	}

	//Used for loading INI from memory
	INI(void* data, size_t dataSize, bool doParse): data((data_t*)data), dataSize(dataSize)
	{
		init(SOURCE_MEMORY, doParse);
	}

	~INI()
	{
		clean();
	}

	///Access Content
	//Provide bracket access to section contents
	keys_t& operator[](section_t section)
	{
#ifdef FINI_SAFE
		if (!sections[section])
			sections[section] = new keys_t;
#endif

		return *sections[section];
	}

	//Create a new section and select it
	bool create(const section_t section)
	{
		if (select(section))
			return false;

		current = new keys_t;
		sections[section] = current;

		reserveKeys(current);

		return true;
	}

	//Select a section for performing operations
	bool select(const section_t section)
	{
		sectionsit_t sectionsit = sections.find(section);
		if (sectionsit == sections.end())
			return false;

		current = sectionsit->second;

		return true;
	}

	///Set
	//Assign a value for key under the selected section
	bool set(const key_t key, const value_t value)
	{
		if (current == NULL)
			return false;

		(*current)[key] = value;

		return true;
	}

	template<class W, class X>
	bool set(const W key, const X value)
	{ return set(Converters::Convert<key_t>(key), Converters::Convert<value_t>(value)); }

	///Get
	//Retrieve a value of a key under the selected section and return default return type
	value_t get(const key_t key, value_t def = value_t())
	{
		keysit_t keys = current->find(key);
		if (current == NULL || keys == current->end())
			return def;

		return keys->second;
	}

	template<class X, class W>
	X get(const W key, X def = X())
	{ return Converters::Convert<X>(get(Converters::Convert<value_t>(key), Converters::Convert<value_t>(def))); }
	template<class W>
	fini_string_t get(const W key, const fini_char_t* def = _T(""))  //Handle C string default value without casting
	{ return get(key, fini_string_t(def)); }

	///Functions
	void parse(std::istream& file)
	{
		fini_char_t line[FINI_BUFFER_SIZE];
		bool first = true;
		fini_sstream_t out;

		while(!file.eof())
		{
			file.getline(line, FINI_BUFFER_SIZE);

			if (first)
			{
				first = false;
				if (line[0] == 0xEF)
				{
					memmove(line, line + (char_size_t * 3), char_size_t * (FINI_BUFFER_SIZE - 3));
					return;
				}
			}

			nake(line);

			if (line[0])
			{
				if ((fini_strlen(line) >= 2 && line[0] == '/' && line[1] == '/')
					|| (fini_strlen(line) >= 1 && line[0] == '#')
					|| (fini_strlen(line) >= 1 && line[0] == ';')
					|| fini_strlen(line) <= 0)  //Ignore comment
				{
					continue;
				}
				if (line[0] == '[')  //Section
				{
					section_t section;
					size_t length = fini_strlen(line) - 2;  //Without section brackets
					while(isspace(line[length + 1]))  //Leave out any additional new line characters, not "spaces" as the name suggests
						--length;

					fini_char_t* ssection = (fini_char_t*)calloc(char_size_t, length + 1);
					fini_strncpy(ssection, line + 1, length);  //Count after first bracket

					current = new keys_t;

					out << ssection;
					free(ssection);
					Converters::GetLine(out, section);

					sections[section] = current;
				}
				else  //Key
				{
					key_t key;
					value_t value;

					fini_char_t* skey;
					fini_char_t* svalue;

					skey = fini_strtok(line, _T("="));
					svalue = fini_strtok(NULL, _T("\n"));

					out << skey;
					Converters::GetLine(out, key);

					out.clear();
					out.str(fini_string_t());

					out << svalue;
					Converters::GetLine(out, value);

					if (value != value_t())
						(*current)[key] = value;
				}

				out.clear();
				out.str(fini_string_t()); //Clear existing stream;
			}
		}
	}

	//Parse an INI's contents into memory from the filename given during construction
	bool parse()
	{
		switch(source)
		{
		case SOURCE_FILE: {
			fini_ifstream_t file(filename);

			if (!file.is_open())
				return false;

			parse(file);

			file.close();
						  }
						  break;

		case SOURCE_MEMORY: {
			std::istringstream sstream;
			sstream.rdbuf()->pubsetbuf(data, dataSize);

			parse(sstream);
							}
							break;
		}

		return true;
	}

	//Clear the contents from memory
	void clear()
	{
		clean();
		sections.clear();
	}

	///Output
	//Save from memory into file
	bool save(std::string filename = "")
	{
		if (!hasFileAssociation(filename))
			return false;

		fini_ofstream_t file((filename == "")? this->filename: filename, std::ios::trunc);
		if (!file.is_open())
			return false;

		//Loop through sections
		for(typename INI::sectionsit_t i = sections.begin(); i != sections.end(); i++)
		{
			if (i->second->size() == 0)  //No keys/values in section, skip to next
				continue;

			//Write section
			const fini_string_t temp = makeSection(i->first);
			const fini_char_t* line = temp.c_str();
			file.write(line, fini_strlen(line));

			for(typename INI::keysit_t j = i->second->begin(); j != i->second->end(); j++)
			{
				//Write key and value
				const fini_string_t temp = makeKeyValue(j->first, j->second);
				const fini_char_t* line = temp.c_str();
				file.write(line, fini_strlen(line));
			}
		}

		file.close();

		return true;
	}

private:
	///Functions
	//Init the INI in with values set by constructor
	void init(source_e source, bool doParse)
	{
		this->source = source;

		reserveSections();
		if (doParse)
			parse();
	}

	//Clean the contents for descruction
	void clean()
	{
		for(sectionsit_t i = sections.begin(); i != sections.end(); i++)
			delete i->second;

		current = NULL;
	}

	//Make any alterations to the raw line
	void nake(fini_char_t* line)  //Strip the line of any non-interpretable characters
	{
		//erase space on begin
		int index;
		int len = strlen( line ) ;
		for( index=0; index<len; index++)
		{
			if( line[index] != ' ') break ;
		}
		memmove(line,line+index,len-index+1);
		//erase space \r \n on end
		len = strlen( line ) ;
		for(index = len-1;index>=0;index--)
		{
			if( (line[index] != ' ') && (line[index] != 0x0a) && (line[index] != 0x0d) )  break ;
		}
		line[index+1] = 0 ;
	}

	void reserveSections()
	{
#ifdef FINI_CPP11
		sections.reserve(ALLOCATE_SECTIONS);
#endif
	}

	void reserveKeys(keys_t* current)
	{
#ifdef FINI_CPP11
		current->reserve(ALLOCATE_KEYS);
#endif
	}

	bool hasFileAssociation(std::string filename)
	{
		if (source == SOURCE_MEMORY && filename == "") //No association to a file
			return false;

		return true;
	}

	///Output
	//Creates a section as a string
	template<class X>
	fini_string_t makeSection(const X& section)
	{
		fini_sstream_t line;
		line << '[' << Converters::Convert(section) << ']' << std::endl;

		return line.str();
	}

	//Creates a key and a value as a string
	fini_string_t makeKeyValue(const key_t& key, const value_t& value)
	{
		fini_sstream_t line;
		line << key << '=' << value << std::endl;

		return line.str();
	}
};