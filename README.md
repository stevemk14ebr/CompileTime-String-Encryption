# VS2013-Compile-Time-XOR
Compile time string XOR for visual studio 2010-2013.

To setup:

1) Compile this project to .dll

2) Rename the compiled dll to a.dll

3) Navigate to Visual Studio Install Directory -> VC -> bin, locate c1xx.dll

4) Patch c1xx.dll to load a.dll

5) Place a.dll in the VC -> bin directory next to c1xx.dll

		

To use:

  1) Open the project you want to have CT XOR

  2) Place xor.h in your project

  3) Build Project

Notes:

  - Can handle escaped quotes

  - Can handle multiple XOR statements per line

  - Uses Unicode almost entirely

  - This version XOR's in debug and in release mode

  - Encrypted Files are stored in project directory /Temp

  - Original Code and ideas 100% SEGnosis, i simply made a VS2013 compatible a.dll version with a few changes, c1xx.dll patch   created by SEGnosis as well

Necessary directions to patch c1xx.dll and xor.h header can be found here:

http://www.unknowncheats.me/forum/c-and-c-/131623-compile-time-string-encryption-visual-studio-2010-2012-a.html

Example of usage scenario:

You have code such as 
```C++
#define SuperSecretKey "ImAKey"
if(input==SuperSecretKey)
	printf("You are a valid user!");
```
This would turn into
```C++
//The XOR macro is expanded at compile time by a.dll
#define SuperSecretKey XOR("ImAKey")
if(input==SuperSecretKey)
	printf("You are a valid user!");
```

Code-wise this is very simple, but in the 
decompiled binary your string would lose all xrefs
(due to how this uses templates, ida isn't good at following
thousands of unique instances of expanded function templates),
and would appear similar to 'x33/x33/x33/x33' instead of "ImAKey"
