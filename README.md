# CompileTime-String-Encryption
C++ 17 compile time string encryption supporting vs2010-2017. Rewrite and update of: https://github.com/stevemk14ebr/VS2013-Compile-Time-XOR

# Changelog compared to other repo
- File parsing supports widechars, UTF-8 recommended
- Directory resolution and redirection fixed to support source files in folders
- Extensive use of C++ features instead of C apis (filesystem, stringstreams, regex, etc)
- Temp files are now stored along-side originals with _crypt postfix. By-product of fixing directory resolution
- Xor header no longer needs to be included, it is injected automatically
- Use after destruction bug fixed. XorStr now returns std::string
- Xrefkiller removed, it was unstable.

# Setup
1) This hooks the compilers wsopen_s api at runtime using my polyhook library. Go clone that repo, and build capstone. 
2) Build this project
3) Find you c1xx.dll in the visual studio installation directory. Use CFF explorer to add a new import address table entry for the dll built by this project
4) Copy the correct versions of the capstone dll and the dll from this into the same directory as your c1xx.dll
5) Make a new visual studio project and copy the .editorconfig file to the root. This forces VS to save your source files as UTF8
6) `#define ENC(x) x` where you want to use encrption, compile, and enjoy
