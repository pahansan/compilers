// Integer literal
int d = 42;   // Decimal integer literal (base 10).
int o = 052;  // Octal integer literal (base 8).
int x = 0x2a; // Hexadecimal integer literal (base 16, the letters 'a' through 'f' represent values (decimal) 10 through 15).
int X = 0X2A;
int b = 0b101010;                                    // C++14, Binary integer literal (base 2).
unsigned long long l1 = 18446744073709550592ull;       // C++11
unsigned long long l2 = 18'446'744'073'709'550'592llu; // C++14
unsigned long long l3 = 1844'6744'0737'0955'0592uLL;   // C++14
unsigned long long l4 = 184467'440737'0'95505'92LLU;   // C++14
size_t sz = 1024 * 1024Z;
// Floating-point literals
float f = 3.4028234e38f;
double d0 = 58.;
double d1 = 4e2;
double d2 = 123.456e-67;
double d3 = .1;
double d4 = .1E4f;
double d = 2.4028234e38;
double d = 0x1.4p3; // hex fraction 1.4 (decimal 1.25) scaled by 23, that is 10.0
long double d = -1.4028234e38l;
std::bfloat16_t bfc = 0.1f64BF16; // C++23
// String literal
char *s = "s-char-seq";
const wchar_t *s5 = L"ABCDEF";
#if
