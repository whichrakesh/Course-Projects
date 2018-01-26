#include <conio.h> //This is not C/C++ header file, this is in GeekOS
#include <sched.h>
int main()
{
	int a = 0;
	Get_NewTOD(&a);
Print("%d",a);
return 0;
}