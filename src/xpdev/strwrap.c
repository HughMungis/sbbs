#include <stdio.h>

#if !defined _MSC_VER && !defined __BORLANDC__
char* itoa(int val, char* str, int radix)
{
	switch(radix) {
		case 8:
			sprintf(str,"%o",val);
			break;
		case 10:
			sprintf(str,"%u",val);
			break;
		case 16:
			sprintf(str,"%x",val);
			break;
		default:
			sprintf(str,"bad radix: %d",radix);
			break;
	}
	return(str);
}

char* ltoa(long val, char* str, int radix)
{
	switch(radix) {
		case 8:
			sprintf(str,"%lo",val);
			break;
		case 10:
			sprintf(str,"%lu",val);
			break;
		case 16:
			sprintf(str,"%lx",val);
			break;
		default:
			sprintf(str,"bad radix: %d",radix);
			break;
	}
	return(str);
}
#endif

#ifdef _WIN32
/* From FreeBSD */
size_t
strnlen(const char *s, size_t maxlen)
{
	size_t len;

	for (len = 0; len < maxlen; len++, s++) {
		if (!*s)
			break;
	}
	return (len);
}

/* From FreeBSD */
char *
strndup(const char *str, size_t maxlen)
{
	char *copy;
	size_t len;

	len = strnlen(str, maxlen);
	copy = malloc(len + 1);
	if (copy != NULL) {
		(void)memcpy(copy, str, len);
		copy[len] = '\0';
	}

	return copy;
}
#endif
