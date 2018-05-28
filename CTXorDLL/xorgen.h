
// XorGen from d4rc converted to C++ and modified
#include <string>
#include <sstream>
#include <codecvt>
#include <random>

std::mt19937 initTrueRand()
{
	uint_least32_t seed[std::mt19937::state_size];
	std::random_device randDevice;
	std::generate_n(seed, std::mt19937::state_size, std::ref(randDevice));
	std::seed_seq seedseq(std::begin(seed), std::end(seed));
	std::mt19937 Gen(seedseq);
	return Gen;
}

uint8_t randByte()
{
	std::mt19937 randGen = initTrueRand();
	std::uniform_int_distribution<uint_least32_t> randDist(0, 256);
	return static_cast<uint8_t>(randDist(randGen));
}

std::wstring decToHex(uint8_t value)
{
	wchar_t buf[3] = { 0 };
	std::swprintf(buf, 3, L"%02X", value);
	return buf;
}
 
std::wstring blub(std::wstring s1)
{
	std::wstringstream sstream;
	uint8_t xvaluestart = randByte();
	sstream << "XorStr<0x" << decToHex(xvaluestart) << "," << s1.length() <<  ">(\"";
	
	uint8_t xvalue = xvaluestart;
	for (int i = 0; i < s1.length(); i++)
	{
		uint8_t ch = s1[i];
		ch ^= xvalue++;
		sstream << "\\x" << decToHex(ch);
	}
	sstream << "\"" << ").get()";

	std::wcout << "Encrypting: " << s1 << std::endl;
	return sstream.str();
}

std::wstring lower_wstring(const std::wstring& str)
{
	std::wstring upper;
	transform(str.begin(), str.end(), std::back_inserter(upper), towlower);
	return upper;
}