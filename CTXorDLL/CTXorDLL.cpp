// CTXorDLL.cpp : Defines the exported functions for the DLL application.
//

#include <iostream>
#include <fstream>
#include <experimental/filesystem>
#include <chrono>
namespace fs = std::experimental::filesystem;

#include "PolyHook.hpp"
#include "regexReplace.h"
#include "xorgen.h"
#include "xor.hpp"

fs::v1::path ProjectDir;
fs::v1::path XorHdrFileName(L"xor_decrypt.hpp");
fs::v1::path XorHdrFullPath;
PLH::Detour* PLHwsopen;
bool skipHook = false;

#define ENC(x) x
std::string m = ENC("Multiline magic"

);

std::string m2 = ENC("Magic is about to happen...again!");

inline void XTrace(const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vfprintf_s(stdout, fmt, args);
	va_end(args);
}

std::wstring encryptMacroContents(const std::wsmatch& m) {
	std::wstring encryptedStr = blub(m[1]);
	return encryptedStr;
}

void encryptFileCopy(fs::v1::path orig, fs::v1::path copy) {
	std::wifstream InputFile;
	std::wofstream OutputFile;

	skipHook = true;
	InputFile.open(orig);

	//// UCS-2-LE is default VS encoding, add this section and std::ios::binary to open() to read. We assume utf-8 now
	//InputFile.imbue(std::locale(InputFile.getloc(),
	//	new std::codecvt_utf16<wchar_t, 0x10ffff, std::little_endian>));

	OutputFile.open(copy);
	if (InputFile.is_open())
	{
		std::wstringstream ss;
		ss << InputFile.rdbuf();
		OutputFile << "#include \"" << XorHdrFileName << "\"" << std::endl
			<< regexReplaceMacro(ss.str(), encryptMacroContents) << std::endl;
	}
	InputFile.close();
	OutputFile.close();
	skipHook = false;
}

/*Visual studio gives source files paths relative to projectdirectoy, either as just filename if in the root or with folders relative to root. Headers
are given absolute paths that include project directory, or relative paths that include root like c:/projDir/../../include/folder/header.h.*/
fs::v1::path redirectFilePath(fs::v1::path filepath) {
	if (!filepath.has_extension())
		return filepath;

	std::wstring ext = filepath.extension();
	// if it's anything but a source or heder ignore
	if (ext != L".h" && ext != L".hpp" && ext != L".c" && ext != L".cpp")
		return filepath;

	// VS converts to lower, so comparisons must be lower too. also normalize delimeters
	std::wstring tmp = lower_wstring(filepath.wstring());
	std::replace(tmp.begin(), tmp.end(), L'/', L'\\'); 
	fs::v1::path lowFilePath = tmp;

	// if it's not a header in our project directory return. But source files allowed because VS passes them w/o path
	if (lowFilePath.wstring().find(ProjectDir.wstring()) == std::wstring::npos && ext != L".c" && ext != L".cpp")
		return filepath;

	// ignore the decryptor header
	if (lowFilePath == XorHdrFullPath)
		return filepath;

	// append _crypt to headers and sources with paths, append _crypt to source files without paths
	if (lowFilePath.has_parent_path())
		lowFilePath = lowFilePath.replace_filename(lowFilePath.stem().wstring() + L"_crypt" + ext);
	else
		lowFilePath = lowFilePath.stem().wstring() + L"_crypt" + ext;
	
	XTrace("[+] Encrypting: %ls -> %ls\n", filepath.c_str(), lowFilePath.c_str());
	encryptFileCopy(filepath, lowFilePath);
	return lowFilePath;
}

decltype(&_wsopen_s) owsopen_s;
errno_t hkwsopen_s(int* pfh, wchar_t *filename, int oflag, int shflag, int pmode)
{
	if(skipHook)
		return owsopen_s(pfh, filename, oflag, shflag, pmode);

	auto filteredPath = redirectFilePath(filename);
	return owsopen_s(pfh, filteredPath.c_str() , oflag, shflag, pmode);
}

void initialize()
{
	HMODULE hModule;
	FARPROC targetFunction;

	//We can't determine exactly which runtime they use so try em all
	hModule = GetModuleHandle(L"MSVCR100.dll"); //vs 2010
	if (hModule == NULL)
		hModule = GetModuleHandle(L"MSVCR110.dll"); //vs 2012
	if (hModule == NULL)
		hModule = GetModuleHandle(L"MSVCR120.dll"); //vs 2013
	if (hModule == NULL)
		hModule = GetModuleHandle(L"api-ms-win-crt-stdio-l1-1-0.dll"); // renamed in vs 2015-2017

	if (hModule == NULL)
		MessageBox(NULL, L"Failed to find C runtime for string crypt", L"Compile-Time string crypt failed", MB_OK);

	ProjectDir = lower_wstring(fs::current_path().wstring());
	XTrace("[+]Project Dir:%ls\n", ProjectDir.c_str());
	
	XorHdrFullPath = ProjectDir / XorHdrFileName;

	std::ofstream xorHeader;
	xorHeader.open(XorHdrFullPath);
	xorHeader << fileAsStr;
	xorHeader.close();
	XTrace("[+]Wrote Decryptor Header to:%ls\n", XorHdrFullPath.c_str());

	targetFunction = GetProcAddress(hModule, "_wsopen_s");

	PLHwsopen = new PLH::Detour();
	PLHwsopen->SetupHook((BYTE*)targetFunction, (BYTE*)&hkwsopen_s);
	PLHwsopen->Hook();
	owsopen_s = PLHwsopen->GetOriginal<decltype(&_wsopen_s)>();
}

std::chrono::time_point<std::chrono::steady_clock> startTime;
__declspec(dllexport) BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpvReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH) {
		XTrace("[+] Loading String Crypt...\n");
		XTrace("[+] %s\n", m.c_str());
		startTime = std::chrono::steady_clock::now();
		initialize();
	}else if (dwReason == DLL_PROCESS_DETACH) {
		PLHwsopen->UnHook();
		delete PLHwsopen;
		XTrace("[+] Encrypted all the things...Unloading String Crypt\n");
		auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - startTime);
		XTrace("[+] Encryption took %lld seconds\n", elapsed.count());
	}
	
	return TRUE;
}

