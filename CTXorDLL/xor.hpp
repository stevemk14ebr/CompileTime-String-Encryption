#pragma once
#include <string>
template <int XORSTART, int BUFLEN>
class XorStr
{
private:
	XorStr();

	std::string s;
public:
	std::string get();

	XorStr(const char * xs);

	~XorStr()
	{
		s.clear();
	}
};

template <int XORSTART, int BUFLEN>
std::string XorStr<XORSTART, BUFLEN>::get() {
	return s;
}

template <int XORSTART, int BUFLEN>
XorStr<XORSTART, BUFLEN>::XorStr(const char * xs)
{
	s.resize(BUFLEN);
	uint8_t xvalue = XORSTART;
	for (int i = 0; i < BUFLEN; i++) {
		s[i] = xs[i] ^ xvalue++;
	}
}

std::string fileAsStr("#pragma once\n#include <string>\ntemplate <int XORSTART, int BUFLEN>\nclass XorStr\n{\nprivate:\n\tXorStr();\n\n\tstd::string s;\npublic:\n\tstd::string get();\n\n\tXorStr(const char * xs);\n\n\t~XorStr()\n\t{\n\t\ts.clear();\n\t}\n};\n\ntemplate <int XORSTART, int BUFLEN>\nstd::string XorStr<XORSTART, BUFLEN>::get() {\n\treturn s;\n}\n\ntemplate <int XORSTART, int BUFLEN>\nXorStr<XORSTART, BUFLEN>::XorStr(const char * xs)\n{\n\ts.resize(BUFLEN);\n\tuint8_t xvalue = XORSTART;\n\tfor (int i = 0; i < BUFLEN; i++) {\n\t\ts[i] = xs[i] ^ xvalue++;\n\t}\n}");