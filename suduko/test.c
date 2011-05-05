/* Suduko Game for arbitrary m-rows and n-columns. This program tries to bound the number of valid combinations possible 
* Owner            : Roopesh S
* Source file      : $Source: /usr/local/cvsroot/src/code/suduko/test.c,v $ 
* Current Revision : $Revision: 1.1.1.1 $
* Branch           : $Name:  $
* Latest check-in  : $Author: roopesh $ on $Date: 2010-02-04 16:49:36 $ 
* */
#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>

typedef unsigned long LONG_NUM;
typedef unsigned char INDEX;

LONG_NUM fact(LONG_NUM n)
{
    LONG_NUM i = 1;
    LONG_NUM ret = 1;
    for(i = 1 ; i <= n ; i++)
    {
        ret *= i;
    }
    return ret;
}

LONG_NUM func_invalid_count(INDEX m, INDEX n)
{
    LONG_NUM ret = 0;
    LONG_NUM temp = 1;

    temp = fact(m*n-1);
    ret = (m*n)*(m*n)*(m*n)*temp*temp*(2*m*n-(m+n));
    return ret;
}

int main(int argc, char* argv[])
{
    INDEX m=0, n=0, i = 0;
    LONG_NUM total_combs = 1, temp = 1;
    LONG_NUM total_valid_combs = 0;
    LONG_NUM total_invalid_combs = 0;
    if(argc < 3)
    {
        printf("\nGive valid no. or <rows> and <columns>\n");
        exit(0);
    }
    /*temp = fact(9);
    total_combs = 1;
    for(i = 0 ; i < 9 ; i++)
        total_combs *= temp;
    printf("\n9! = %lu\n", total_combs);*/
    m = atoi(argv[1]);
    n = atoi(argv[2]);
    temp = fact(m*n);
    total_combs = 1;
    for(i = 0 ; i < m*n ; i++)
        total_combs *= temp;
    total_invalid_combs = func_invalid_count(m, n);
    total_valid_combs = total_combs - total_invalid_combs;
    printf("\n<valid> = <total> - <invalid> ---> %lu = %lu - %lu\n", total_valid_combs, total_combs, total_invalid_combs);
    exit(0);
}
