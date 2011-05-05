/* Suduko Game for arbitrary m-rows and n-columns
* Owner            : Roopesh S
* Source file      : $Source: /usr/local/cvsroot/src/code/suduko/sudoku.c,v $ 
* Current Revision : $Revision: 1.7 $
* Branch           : $Name:  $
* Latest check-in  : $Author: roopesh $ on $Date: 2010-02-07 09:49:33 $ 
* */

/* Suduko with m-rows and n-columns. 
 * This program generates all possible valid combinations of suduko game.
 * TODO : 1. Generate slots out of a valid combination.
 *        2. Create levels
 *        3. User interaction to be added
 *        4. To find a formula for total valid combinations */

#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<string.h>

#define MAX_ROW 8
#define MAX_COLUMN 8
#define MAX_TOTAL (MAX_ROW * MAX_COLUMN)
#define GET_ROW(n) ((n)/COLUMN)
#define GET_COLUMN(n) ((n)%COLUMN)

//typedef bool BOOL;
typedef enum bool{FALSE, TRUE} BOOL;

typedef unsigned char UNIT;
typedef UNIT INDEX;

typedef struct unit_box{
    UNIT value[MAX_COLUMN][MAX_ROW];
}unit_box;

typedef unsigned long inv_unit_box;
const inv_unit_box UNITCONST=1;
#define SET_FLAG(v, i) (v|=UNITCONST<<(i))
#define CLEAR_FLAG(v, i) (v&=~(UNITCONST<<(i)))
#define IS_FLAG(v, i) (v&(UNITCONST<<(i)))
#define AND(a,b) ((a)&(b))
#define OR(a,b) ((a)|(b))
#define NOT(a) (~(a))


inv_unit_box box[MAX_TOTAL], row[MAX_TOTAL], column[MAX_TOTAL];

INDEX ROW=0, COLUMN=0, TOTAL=0;
BOOL debug = FALSE;

void print_matrix(const unit_box boxes[])
{
    INDEX bi = 0, i = 0 , j = 0, k = 0;
    unit_box *bptr[MAX_COLUMN] = {NULL};
    printf("\n");
    for(bi = 0 ; bi < ROW; bi++)
    {
        for(i = 0 ; i < COLUMN ; i++)
        {
            bptr[i] = (unit_box*)&boxes[bi*COLUMN+i];
            printf("==");
            for(j = 0 ; j < ROW ; j++)
            {
                printf("=====");
            }
        }
        printf("\n");
        for(i = 0 ; i < COLUMN ; i++)
        {
            printf("||");
            for(j = 0 ; j < COLUMN; j++)
            {
                for(k = 0 ; k < ROW ; k++)
                {
                    printf("%3d |", bptr[j]->value[i][k]);
                }
                printf("|");
            }
            printf("\n");
        }
    }
    for(i = 0 ; i < COLUMN ; i++)
    {
        printf("==");
        for(j = 0 ; j < ROW ; j++)
        {
            printf("=====");
        }
    }
    printf("\n");
}

BOOL insert_value(unit_box *boxes, INDEX index, UNIT val, INDEX i,INDEX j)
{
    BOOL ret = TRUE;
    INDEX ri = 0, ci = 0, bi = 0;
    bi = index;
    ri = GET_ROW(bi)*COLUMN + i; 
    ci = GET_COLUMN(bi)*ROW + j;
    if(val > 0 && val <= TOTAL)
    {
        ret = (IS_FLAG(box[bi], val-1) || IS_FLAG(row[ri], val-1) || IS_FLAG(column[ci],val-1)) ? FALSE : TRUE; 
    }
    else 
        ret = FALSE;
    if(ret)
    {
        if(debug)
            printf("\nPREV VAL (%d, %d, %d) : %d",index, i, j, boxes[index].value[i][j]);
        boxes[index].value[i][j] = val;
        SET_FLAG(box[bi], val-1);
        SET_FLAG(row[ri], val-1);
        SET_FLAG(column[ci], val-1);
        if(debug)
            printf("\nNEW VAL (%d, %d, %d) : %d",index, i, j, boxes[index].value[i][j]);
    }
    return ret;
}

void delete_value(unit_box *boxes, INDEX index, INDEX i,INDEX j)
{
    INDEX ri = 0, ci = 0, bi = 0;
    UNIT val = 0;
    bi = index;
    ri = GET_ROW(bi)*COLUMN + i; 
    ci = GET_COLUMN(bi)*ROW + j;
    val = boxes[index].value[i][j];
    if(val > 0 && val <= TOTAL)
    {
        boxes[index].value[i][j] = 0;
        CLEAR_FLAG(box[bi], val-1);
        CLEAR_FLAG(row[ri], val-1);
        CLEAR_FLAG(column[ci], val-1);
    }
}

void update_params(unit_box *boxes, inv_unit_box *box, inv_unit_box *row, inv_unit_box *column)
{
    INDEX i=0, j=0, bi=0;
    for(i = 0 ; i < TOTAL ; i++)
        for(j = 0 ; j < TOTAL ; j++)
        {
            CLEAR_FLAG(box[i], j);
            CLEAR_FLAG(row[i], j);
            CLEAR_FLAG(column[i], j);
        }
    for( bi= 0; bi < TOTAL; bi++)
        for( i= 0; i < COLUMN; i++)
        {
            INDEX ri = GET_ROW(bi)*COLUMN + i;
            for(j= 0; j < ROW; j++)
            {
                INDEX ci = GET_COLUMN(bi)*ROW + j;
                UNIT val = boxes[bi].value[i][j]-1;
                SET_FLAG(box[bi], val);
                SET_FLAG(row[ri], val);
                SET_FLAG(column[ci], val);
            }
        }
}

BOOL generate(unit_box boxes[],INDEX index, INDEX r, INDEX c, BOOL *is_going_down)
{
    UNIT val = 0;
    UNIT k = 0;
    if(index >= TOTAL)
    {
        if(*is_going_down == TRUE)
        {
            *is_going_down = FALSE;
            return FALSE;
        }
        return TRUE;
    }
    if(debug)
        printf("\nPROCESSING (%d,%d,%d)", index, r, c);
    k = val = boxes[index].value[r][c];
    if(k == 0)
    {
        *is_going_down = FALSE;
        k = 1;
    }
    for ( ; k <= TOTAL ; k++)
    {
        INDEX newIndex = index, newr = r, newc = c+1;
        delete_value(boxes, index, r, c);
        if(debug)
            printf("\nINSERTING (%d,%d,%d) : %d", index, r, c, k);
        if(insert_value(boxes, index, k, r, c))
        {
            if(newc >= ROW){ newr++; newc = 0;}
            if(newr >= COLUMN) {newIndex++; newr = 0;}
            if(generate(boxes, newIndex, newr, newc, is_going_down))
                return TRUE;
        }
   }
   delete_value(boxes, index, r, c);
   return FALSE;
}

void new_game()
{
    unsigned long count = 0;
    BOOL is_going_down = TRUE;
    unit_box boxes[MAX_TOTAL];
    memset(boxes, 0, sizeof(boxes));
    while(1)
    {
        update_params(boxes, box, row, column);
        is_going_down = TRUE;
        if(0 == generate(boxes, (INDEX)0, (INDEX) 0, (INDEX)0, &is_going_down))
        {
            printf("Total Possible Combination = %lu", count);
            return;
        }
        count++;
        if(debug)
        {
            printf("\nIter : %d", count);
        } 
        //print_matrix(boxes);
    }
}

void print_usage(char* filename)
{
    printf("\nUsage: %s <ROWs> <COLUMNs> {--debug}\n", filename);
    printf("\t where, 1<= ROW <= %d and 1<= COLUMN <= %d\n\n", MAX_ROW, MAX_COLUMN);
    exit(0);
}
int main(int argc, char* argv[])
{
    int i = 0, dimCount = 0;
    INDEX dim[2]={0};
    if(argc < 3)
        print_usage(argv[0]);
    for(i = 1; i < argc ; i++)
    {
        if(!strcmp(argv[i], "--debug"))
        {
            debug = TRUE;
            continue;
        }
        if(dimCount < sizeof(dim)/sizeof(INDEX))
            dim[dimCount++] = (INDEX)atoi(argv[i]);
    }
    ROW = dim[0];
    COLUMN = dim[1];
    /*ROW = 3;
    COLUMN =7;*/
    TOTAL = ROW*COLUMN;
    if(ROW == 0 || COLUMN == 0 || ROW > MAX_ROW || COLUMN > MAX_COLUMN)
        print_usage(argv[0]);
    new_game();
    printf("\n");
    exit(0);
}
