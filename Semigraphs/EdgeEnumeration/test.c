/* Test code to check working of some concepts 
* Owner            : Roopesh S
* Source file      : $Source: /usr/local/cvsroot/src/code/Semigraphs/EdgeEnumeration/test.c,v $
* Current Revision : $Revision: 1.1 $
* Branch           : $Name:  $
* Latest check-in  : $Author: roopesh $ on $Date: 2010-03-18 10:48:55 $
* */
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdarg.h>

#define EXPAND(x,type)  type##_init(x)

#define SCALE_MIN 0
#define SCALE_MAX 4 

#define SCALE(x) (SCALE_MIN+(x)%(SCALE_MAX-SCALE_MIN))
#define RAND() (rand())

typedef void* (*lef_init)(va_list ap);
typedef void* (*lef_deinit)(void* e);
typedef void* (*lef_val)(void* e);
typedef int* EDGE;
void* func(va_list ap)
{
    return "Roopesh";
}

void int_init(lef_init x)
    {
        printf ("X = %s\n", x(NULL));
    }

int main()
{
    EDGE x = NULL;
    int i = 0;
    /*for(i = 0; i < 4 ; i++)
        printf("%d, ", x[i]);*/
    srand(time(NULL));
    for(i = 0 ; i < 10 ;i++)
        printf("%3d. %u\n",i+1, SCALE(RAND()));
    printf("\n");
    EXPAND(func, int);
}
