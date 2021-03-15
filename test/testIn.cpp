#include<WordDetect.h>
#define constant 30
int i,j;
i = 30L;
double b = 3.0;
const float a = 4.1F;
int array[30];
/* This is annotation
   of region.
*/
// my function-1
int func1(const int m, int n){	// definition of function
const int b = 3;
int j = m+n;
n=b++ + m--;
n = m+n-b;
n = n * 2;
n = n / 2;
n = n % 3;
double p[10], q[20];
n = 0x123;
n = 0123;
char ch = 'a';
if (m > n){
	m = a||b;
	n = a&&b;
while (b == 3){
n++;
break;
		}
	}
	else{
		m=b;
continue;
	}
for (i=0; i<10; i++){
if (i == 5){
			break;
		}
		continue;
	} 
	return m-n;
}
// my function-2
double func2(int d);		// declaration of function
// my function-3
float func3(float c)		// definition of function
{
int i = 0;
	int j = 1;
	do {
		i++;
		j = i+1;
		c = i+j;
	}while(i < j);
	return c;
}
int main()
{
	int i=0;
	int j=0;
	func1(i, j);			// call of a function
return 0;
}
