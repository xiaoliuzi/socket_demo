#include <stdio.h>
#define MAX_LINE 100



int main(int argc, char* argv[]
{
	char ch;
	char str[MAX_LINE];
	int i = 0;

	while( (ch=getchar()) != EOF ) {
		while (i<MAX_LINE || ch != '\n' ) {
			str[i++] = ch;
		}
		str[i] = '\n';
/*		str[i+1] = '\0';*/
	}

	return 0;
}
