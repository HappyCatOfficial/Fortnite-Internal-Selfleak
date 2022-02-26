#pragma once

#include <xmmintrin.h>
#include <immintrin.h>

u_int c5;
#define	wsize	sizeof(u_int)
#define	wmask	(wsize - 1)

#ifdef BZERO
#define	RETURN	return
#define	VAL	0
#define	WIDEVAL	0
void
bzero(dst0, length)
void* dst0;
register size_t length;
#else
#define	RETURN	return (dst0)
#define	VAL	c0
#define	WIDEVAL	c5
#endif

#define UC(c)	((unsigned char)c)

namespace custom {

	int toupper(int c)
	{
		if (c >= 'a' && c <= 'z') return c - 'a' + 'A';
		return c;
	}

	int tolower(int c)
	{
		if (c >= 'A' && c <= 'Z') return c - 'A' + 'a';
		return c;
	}

	void* memcpy(void* dest, const void* src, unsigned __int64 count)
	{
		char* char_dest = (char*)dest;
		char* char_src = (char*)src;
		if ((char_dest <= char_src) || (char_dest >= (char_src + count)))
		{
			while (count > 0)
			{
				*char_dest = *char_src;
				char_dest++;
				char_src++;
				count--;
			}
		}
		else
		{
			char_dest = (char*)dest + count - 1;
			char_src = (char*)src + count - 1;
			while (count > 0)
			{
				*char_dest = *char_src;
				char_dest--;
				char_src--;
				count--;
			}
		}
		return dest;
	}


	void* memset(void* dst0, register int c0, register size_t length)
	{
		register size_t t;
		register u_char* dst;

		dst = (u_char*)dst0;

		if (length < 3 * wsize) {
			while (length != 0) {
				*dst++ = c0;
				--length;
			}
			RETURN;
		}

		if ((t = (int)dst & wmask) != 0) {
			t = wsize - t;
			length -= t;
			do {
				*dst++ = c0;
			} while (--t != 0);
		}

		t = length / wsize;
		do {
			*(u_int*)dst = WIDEVAL;
			dst += wsize;
		} while (--t != 0);

		t = length & wmask;
		if (t != 0)
			do {
				*dst++ = c0;
			} while (--t != 0);
			RETURN;
	}

	void* memccpy(void* to, const void* from, int c, unsigned __int64 count)
	{
		char t;
		unsigned __int64 i;
		char* dst = (char*)to;
		const char* src = (const char*)from;
		for (i = 0; i < count; i++)
		{
			dst[i] = t = src[i];
			if (t == 0) break;
			if (t == c) return &dst[i + 1];
		}
		return 0;
	}

	void* memchr(const void* s, int c, unsigned __int64 n)
	{
		if (n)
		{
			const char* p = (const char*)s;
			do
			{
				if (*p++ == c) return (void*)(p - 1);
			} while (--n != 0);
		}
		return 0;
	}

	int memcmp(const void* s1, const void* s2, unsigned __int64 n)
	{
		if (n != 0)
		{
			const unsigned char* p1 = (unsigned char*)s1, * p2 = (unsigned char*)s2;
			do
			{
				if (*p1++ != *p2++) return (*--p1 - *--p2);
			} while (--n != 0);
		}
		return 0;
	}

	int memicmp(const void* s1, const void* s2, unsigned __int64 n)
	{
		if (n != 0)
		{
			const unsigned char* p1 = (unsigned char*)s1, * p2 = (unsigned char*)s2;
			do
			{
				if (toupper(*p1) != toupper(*p2)) return (*p1 - *p2);
				p1++;
				p2++;
			} while (--n != 0);
		}
		return 0;
	}

	void* memmove(void* dest, const void* src, unsigned __int64 count)
	{
		char* char_dest = (char*)dest;
		char* char_src = (char*)src;
		if ((char_dest <= char_src) || (char_dest >= (char_src + count)))
		{
			while (count > 0)
			{
				*char_dest = *char_src;
				char_dest++;
				char_src++;
				count--;
			}
		}
		else
		{
			char_dest = (char*)dest + count - 1;
			char_src = (char*)src + count - 1;
			while (count > 0)
			{
				*char_dest = *char_src;
				char_dest--;
				char_src--;
				count--;
			}
		}
		return dest;
	}

	char* _cslwr(char* x)
	{
		char* y = x;
		while (*y)
		{
			*y = tolower(*y);
			y++;
		}
		return x;
	}

	char* _csupr(char* x)
	{
		char* y = x;
		while (*y)
		{
			*y = tolower(*y);
			y++;
		}
		return x;
	}

	int strlen(const char* string)
	{
		int cnt = 0;
		if (string)
		{
			for (; *string != 0; ++string) ++cnt;
		}
		return cnt;
	}

	int wcslen(const wchar_t* string)
	{
		int cnt = 0;
		if (string)
		{
			for (; *string != 0; ++string) ++cnt;
		}
		return cnt;
	}

	const char* strcpy(char* buffer, const char* string)
	{
		char* ret = buffer;
		while (*string) *buffer++ = *string++;
		*buffer = 0;
		return ret;
	}

	int strcmp(const char* cs, const char* ct)
	{
		if (cs && ct)
		{
			while (*cs == *ct)
			{
				if (*cs == 0 && *ct == 0) return 0;
				if (*cs == 0 || *ct == 0) break;
				cs++;
				ct++;
			}
			return *cs - *ct;
		}
		return -1;
	}

	int stricmp(const char* cs, const char* ct)
	{
		if (cs && ct)
		{
			while (tolower(*cs) == tolower(*ct))
			{
				if (*cs == 0 && *ct == 0) return 0;
				if (*cs == 0 || *ct == 0) break;
				cs++;
				ct++;
			}
			return tolower(*cs) - tolower(*ct);
		}
		return -1;
	}



	float fabsf(float x)
	{
		{
			if (x < 0) 
			{
				return -x;
			}
			return x;
		}
	}

	const char* strstr(const char* string, char* substring)
	{
		const char* a, * b;
		b = substring;
		if (*b == 0) {
			return string;
		}
		for (; *string != 0; string += 1) {
			if (*string != *b) {
				continue;
			}
			a = string;
				while (1) {
					if (*b == 0) {
						return string;
					}
					if (*a++ != *b++) {
						break;
					}
				}
			b = substring;
		}
		return NULL;
	}

	bool is_bad_write_ptr(LPVOID ptr, UINT_PTR size)
	{
		return ptr ? false : true;
	}

	int _wcsicmp(const wchar_t* string1, const wchar_t* string2)
	{
		wchar_t c1, c2;
		do
		{
			c1 = *string1++;
			c2 = *string2++;
			if (c2 == L'\0')
				return c1 - c2;
		} while (c1 == c2);
		return c1 < c2 ? -1 : 1;
	}

	size_t strnlen(const char* s, size_t maxlen)
	{
		size_t i;
		for (i = 0; i < maxlen; ++i)
			if (s[i] == '\0')
				break;
		return i;
	}

	char* strncpy(void* s1, const void* s2, size_t n)
	{
		size_t size = strnlen((char*)s2, n);
		if (size != n)
			memset((char*)s1 + size, '\0', n - size);
		return (char*)memcpy(s1, s2, size);
	}

	double floor(double num)
	{
		if (num < 0)
			return (int)num - 1;
		else
			return (int)num;
	}

	int isspace(int c)
	{
		return (c == '\t' || c == '\n' ||
			c == '\v' || c == '\f' || c == '\r' || c == ' ' ? 1 : 0);
	}

	int isdigit(int c)
	{
		return (c >= '0' && c <= '9' ? 1 : 0);
	}

	int isalpha(int c)
	{
		return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ? 1 : 0);
	}

	char isupper(unsigned char c)
	{
	  if (c >= UC('A') && c <= UC('Z'))
	    return 1;
		return 0;
	}

	unsigned long strtoul(const char* nptr, char** endptr, register int base)
	{
		register const char* s = nptr;
		register unsigned long acc;
		register int c;
		register unsigned long cutoff;
		register int neg = 0, any, cutlim;
		do {
			c = *s++;
		} while (isspace(c));
		if (c == '-') {
			neg = 1;
			c = *s++;
		}
		else if (c == '+')
			c = *s++;
		if ((base == 0 || base == 16) &&
			c == '0' && (*s == 'x' || *s == 'X')) {
			c = s[1];
			s += 2;
			base = 16;
		}
		if (base == 0)
			base = c == '0' ? 8 : 10;
		cutoff = (unsigned long)ULONG_MAX / (unsigned long)base;
		cutlim = (unsigned long)ULONG_MAX % (unsigned long)base;
		for (acc = 0, any = 0;; c = *s++) {
			if (isdigit(c))
				c -= '0';
			else if (isalpha(c))
				c -= isupper(c) ? 'A' - 10 : 'a' - 10;
			else
				break;
			if (c >= base)
				break;
			if (any < 0 || acc > cutoff || acc == cutoff && c > cutlim)
				any = -1;
			else {
				any = 1;
				acc *= base;
				acc += c;
			}
		}
		if (any < 0) {
			acc = ULONG_MAX;
			errno = ERANGE;
		}
		else if (neg)
			acc = -acc;
		if (endptr != 0)
			*endptr = (char*)(any ? s - 1 : nptr);
		return (acc);
	}

	inline int wcompare(const wchar_t* X, const wchar_t* Y)
	{
		while (*X && *Y) {
			if (*X != *Y) {
				return 0;
			}
			X++;
			Y++;
		}

		return (*Y == L'\0');
	}

	inline const wchar_t* wcsstr(const wchar_t* X, const wchar_t* Y)
	{
		while (*X != L'\0') {
			if ((*X == *Y) && wcompare(X, Y)) {
				return X;
			}
			X++;
		}
		return NULL;
	}


	inline float sqrtf(float number)
	{
		float temp, mysqrt;
		mysqrt = number / 2;
		temp = 0;
		while (mysqrt != temp) {
			temp = mysqrt;
			mysqrt = (number / temp + temp) / 2;
		}
		return mysqrt;
	}


	std::string to_string(int n)
	{
		int m = n;
		char s[100]; //max
		char ss[100]; //max
		int i = 0, j = 0;
		if (n < 0)
		{
			m = 0 - m;
			j = 1;
			ss[0] = '-';
		}
		while (m > 0)
		{
			s[i++] = m % 10 + '0';
			m /= 10;
		}
		s[i] = '\0';
		i = i - 1;
		while (i >= 0)
		{
			ss[j++] = s[i--];
		}
		ss[j] = '\0';
		return ss;
	}

	float powf(float _X, float _Y) { return _mm_cvtss_f32(_mm_pow_ps(_mm_set_ss(_X), _mm_set_ss(_Y))); }
}
