/* Semigraphs : Given a number of vertices (a +ve integer), gives a set of 3-edges s.t. every vertex is atleast a m-vertex of some edge.
* Owner            : Roopesh S
* Source file      : $Source: /usr/local/cvsroot/src/code/Semigraphs/subset3.c,v $ 
* Current Revision : $Revision: 1.2 $
* Branch           : $Name:  $
* Latest check-in  : $Author: roopesh $ on $Date: 2010-02-10 15:41:15 $ 
* */

#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>

#define SUBSET_LEN 3
#define MIDDLE_VERTEX (SUBSET_LEN/2)
#define FREE(x) if(x){ \
            free(x); \
        }\
        else{\
            fprintf(stderr, "%s (%d): Trying to free (%s) which is NULL !!", __FILE__, __LINE__, #x);\
        }

int is_valid_subset(int *subset, int len)
{
    int i, j ;
    //for(i = len - 1 ; i >= 0 ; i--)
    for(i = len - 1 ; i >= len-1 ; i--) // optimization done for pro-check of only last element, as set elements are added at the last position
        for(j = i-1; j >= 0; j--)
            if (subset[i]==subset[j]) return 0;
    return 1;
}

int is_valid_intersect(int subsets[][SUBSET_LEN], const int *sc, const int *test_set)
{
    int k = 0;
    for(k=0; k < *sc; k++)
    {
       int i, j;
       int intersect = 0;
       for(i = 0 ; i < SUBSET_LEN; i++)
           for(j = 0; j < SUBSET_LEN; j++)
            {
                if (subsets[k][i]==test_set[j]) 
                    intersect++ ;
                if (intersect >= 2)
                    return 0;
            }
    }
    return 1;
}

void print_subset(int *subset)
{
    int j;
    printf("{");
    for(j=0;j<SUBSET_LEN; j++)
        printf("%d,",subset[j]+1);
    printf("\b}");
}

void print_subsets(int subsets[][SUBSET_LEN], const int *sc)
{
    int i;
    for(i = 0 ; i < *sc ; i++)
    {
        print_subset(subsets[i]);
        printf(", ");
    }
    printf("\b\b ---> (%d)\n", *sc );
}

void add_subset(int subsets[][SUBSET_LEN], int *sc, const int *add_set)
{
    int i ;
    for(i = 0 ; i < SUBSET_LEN ; i++)
        subsets[*sc][i] = add_set[i];
    (*sc)++;
}

void del_subset(int subsets[][SUBSET_LEN], int *sc, int *del_set)
{
    int i, index = -1;
    for(i = *sc - 1 ; i >= 0 ; i--)
    {
        int j, match = 1;
        for(j = 0 ; j < SUBSET_LEN ; j++)
            if(subsets[i][j] != del_set[j]){
                match = 0; break;
            }
        if (match == 1){
            index = i; break;
        }
    }
    if(index == -1) 
        return;
    //printf("Deleting subset : "); 
    //print_subset(subsets[index]);
    //printf("\n");
    for(i = index ; i < (*sc) ; i++)
    {
        int j;
        for(j = 0 ; j < SUBSET_LEN ; j++)
            subsets[i][j] = subsets[i+1][j];
    }
    (*sc)--;
}

int is_middle_vertex_added(int *middle_vertex, int n, int vertex)
{
    if(vertex >=0 && vertex < n)
        return middle_vertex[vertex];
    return 0;
}
int is_all_middle_vertices_covered(int *middle_vertex, int n)
{
    int i ;
    for(i = 0 ; i < n ; i++)
        if(!middle_vertex[i]) return 0;
    return 1;
}

void set_middle_vertex(int *middle_vertex, int n, int s)
{
    if(s >= 0 && s < n)
        middle_vertex[s] = 1;
}

void unset_middle_vertex(int *middle_vertex, int n, int s)
{
    if(s >= 0 && s < n)
        middle_vertex[s] = 0;
}

int get_next_permute_index(int *start_subset,int *cur_subset, int current_len)
{
    int i = 0;
    int ret = 0;
    if(current_len >= SUBSET_LEN)
        goto END;
    ret= start_subset[current_len];
    for(i = 0 ; i < current_len; i++)
    {
        if(cur_subset[i] > start_subset[i])
        {
            ret = 0;
            break;
        }
    }
END:
    return ret;
}
int permute(int n, int subsets[][SUBSET_LEN], int *sc, int *middle_vertex, int *current_subset, int current_subset_len, int *start_from_here)
{
    int i = 0, start_index = 0;
    int ret = 0;
    if(current_subset_len == SUBSET_LEN)
    {
        // If the subset formation is complete .... 
        int *temp = NULL;
        if(is_middle_vertex_added(middle_vertex, n, current_subset[MIDDLE_VERTEX]) || !is_valid_intersect(subsets, sc, current_subset ))
        {
            goto RETURN;
        }
        add_subset(subsets, sc, current_subset);
        set_middle_vertex(middle_vertex, n, current_subset[MIDDLE_VERTEX]);
        if(is_all_middle_vertices_covered(middle_vertex, n))
        {
            ret = 1;
            // Print valid subset, and
            printf("VALID SUBSETs SET : \n");
            print_subsets(subsets, sc);

            // Delete the subset and go for next permutation
            unset_middle_vertex(middle_vertex, n, current_subset[MIDDLE_VERTEX]);
            del_subset(subsets, sc, current_subset);
            ret = 1;
        }
        else 
        {
            temp = (int*) calloc(sizeof(int), n);
            if(temp == NULL)
            {
                printf("TEMP NULL ALLOCATION !\n");
                exit(0);
            } 
            ret = permute(n, subsets, sc, middle_vertex, temp, 0, current_subset);
            if(!ret)
            {
                unset_middle_vertex(middle_vertex, n, current_subset[MIDDLE_VERTEX]);
                del_subset(subsets, sc, current_subset);
            }
            //print_subsets(subsets, sc);
            FREE(temp);
        }
        goto RETURN;
    }
    if(start_from_here)
    {
        start_index = get_next_permute_index(start_from_here, current_subset, current_subset_len);
    }
    for(i = start_index ; i < n ; i++)
    { // Subset building loop
        current_subset[current_subset_len] = i;
        if(!is_valid_subset(current_subset, current_subset_len+1))
            continue;
        if(ret = permute(n, subsets, sc, middle_vertex, current_subset, current_subset_len+1, start_from_here))
        {
            break;
        }
    }
RETURN:
    return ret;
}

int main(int argc, char* argv[])
{
    int n =0 ;
    int **subsets = NULL, sc = 0; // subset's-set, sc : length of subset's-set 
    int *middle_vertex = NULL;// Middle vertices covered 
    int *cur_subset = NULL ;//Temporary current_subset set
    
    if (argc <= 1){
         printf("Usage %s <+ve integer>\n", argv[0]); 
         exit(0);
    }
    n = atoi(argv[1]);
    if (n < 1){
         printf("Not a valid input. Try once again !\n");
         exit(0);
    }
    subsets = (int**) calloc(sizeof(int), SUBSET_LEN*n);
    middle_vertex = (int* )calloc(sizeof(int), n);
    cur_subset = (int* )calloc(sizeof(int), n);

    if(subsets == NULL || middle_vertex == NULL || cur_subset == NULL)
    {
         printf("NULL ALLOCATION !\n");
         exit(0);
    }

    sc = 0;    
    permute(n, (int*)subsets, &sc, middle_vertex, cur_subset, 0, NULL);

    FREE(subsets);
    FREE(middle_vertex) ;
    FREE(cur_subset);
    exit(0);
}
