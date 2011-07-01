#include <ctype.h>

int strcasecmp(char *s1, char *s2) {
	while (*s1 && *s2 && (toupper(*s1) == toupper(*s2)))
	{
		s1++;
		s2++;
	}
	return (toupper(*s1) - toupper(*s2));
}
