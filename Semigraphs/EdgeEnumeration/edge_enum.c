/* Semigraphs : Given a matrix, this program tries to enumerate all possible edges. 
* Owner            : Roopesh S
* Source file      : $Source: /usr/local/cvsroot/src/code/Semigraphs/EdgeEnumeration/edge_enum.c,v $
* Current Revision : $Revision: 1.4 $
* Branch           : $Name:  $
* Latest check-in  : $Author: roopesh $ on $Date: 2010-02-10 15:39:03 $
* */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdarg.h>

typedef struct edge{
    int len;
    int *nodes;
}edge;

typedef struct matrix{
    int size;
    int *order;
    int *inv_order;
    int *val;
    edge tmp_edge;
}matrix;

char outputFileName[1024];

typedef enum bool
{
    FALSE,
    TRUE
}bool;

#define M_FAILURE 1
#define M_OK 0

#define INIT_MATRIX(x) \
   { \
    (x).size = 0; \
    (x).val = (x).order = (x).inv_order = NULL; \
    (x).tmp_edge.len = 0 ; (x).tmp_edge.nodes = NULL; \
   }

#define MATRIX(src, i, j) \
    ((src)->val[i*(src)->size+j])

#define FREE(x) if(x){ \
            free(x); \
        }

void empty_file(char *fn)
{
    FILE *fp = NULL;
    if(!fn || !fn[0])
        return;
    fp = fopen(fn, "w");
    if(fp)
       fclose(fp); 
}

void output(const char *str, ...)
{
    va_list ap;
    FILE *fp = stdout;
    
    va_start(ap, str);
    if(outputFileName[0]!= '\0')
        fp = fopen(outputFileName, "a+");
    if(fp)
    {
        vfprintf(fp, str, ap); 
        if(fp != stdout)
            fclose(fp);
    }
    else
        fprintf(stderr, "ERROR : %s cannot be opened !", outputFileName);
    va_end(ap);
}

void outputMatrix(char *fileName, matrix *dest)
{
    FILE *fp = stdout;
    int i = 0, j = 0, mat_size = 0;
    int sum = 0;
    
    if(fileName && fileName[0])
        fp = (FILE*) fopen(fileName, "a+");    
    if(!fp)
    {
        fprintf(stderr, "ERROR : Outputfile cannot be opened !");
        return;
    }
    
    mat_size = dest->size;
    fprintf(fp, "\n%d\n", mat_size);
    if(!mat_size)
    {
        fprintf(stderr, "ERROR : NULL Matrix !");
        return;
    }
    for(i = 0 ; i < mat_size ; i++)
        fprintf(fp,"%4d",dest->order[i]+1);
    fprintf(fp,"\n---------------------------------------------------------------------------------\n");
    
    for(i = 0 ; i < mat_size ; i++)
    {
        int val = 0;
        for(j = 0 ; j < mat_size ; j++)
        {
            val = MATRIX(dest, i, j);
            sum += val;
            fprintf(fp,"%4d",val);
        }
        fprintf(fp, "\n");
    }
    fprintf(fp,"\nSUM = %4d", sum);
    fprintf(fp,"\n\n");
    if(fp != stdout)
        fclose(fp);
}

void initEdge(edge *e, int len)
{
    if(!e)
        return;
    FREE(e->nodes);
    e->len = len;
    e->nodes = (int*) calloc(len, sizeof(int));
}

void freeEdge(edge *e)
{
    FREE(e->nodes);
    e->nodes = NULL;
}

void addEdgeToMatrix(matrix *src)
{
    
}

void printEdge(edge *e)
{
    int n = 0, i=0;
    if(!e)
        return;
    n = e->len ; 
    output("\nEDGE : [ " );
    for(i = 0 ; i < n ; i++)
        output("v%d ", e->nodes[i]+1);
    output(" ]\n");
}


void duplicateMatrix(matrix *src, matrix* dest)
{
    FREE(dest->val);
    dest->size = src->size;
    dest->val = (int*) calloc(dest->size*dest->size, sizeof(int));
    dest->order = (int*) calloc(dest->size, sizeof(int));
    dest->inv_order = (int*) calloc(dest->size, sizeof(int));
    if(!dest->val || !dest->order || !dest->inv_order)
    {
        FREE(dest->val);
        FREE(dest->order);
        dest->val= NULL;
        dest->order = NULL;
        dest->size = 0;
        return;
    }
    memcpy(dest->val, src->val, dest->size*dest->size*sizeof(int));
    memcpy(dest->order, src->order, dest->size*sizeof(int));
    memcpy(dest->inv_order, src->inv_order, dest->size*sizeof(int));
}

bool isSymmetricMatrix(matrix *d)
{
    int n = 0;
    int i = 0 , j = 0;
    if(!d) return FALSE;
    n = d->size;
    for(i = 0 ; i < n ; i++)
        if(MATRIX(d, i, i) != 0)
            return FALSE;
    for(i = 0 ; i < n ; i++)
        for(j = 0 ; j < n ;j++)
            if(MATRIX(d, i, j) != MATRIX(d, j, i))
                return FALSE;
    return TRUE;
}

typedef enum MATRIX_TYPE {
    ERROR_MATRIX,
    NULL_MATRIX,
    SINGLE_EDGE_MATRIX,
    COMPOUND_MATRIX
}MATRIX_TYPE;

MATRIX_TYPE findMatrixType(matrix *d)
{
    MATRIX_TYPE ret = NULL_MATRIX;
    int n = 0, i = 0, j = 0 ;
    bool isZero = TRUE;
    if(!d)
        return ERROR_MATRIX;
    n = d->size;
    if(!n)
        goto END;
    
    for(i = 0; i < n; i++)
        for(j = 0 ; j < n ; j++)
            if(MATRIX(d, i, j))
            {
                isZero = FALSE;
                break;
            }
    if(isZero)
        goto END;
    ret = COMPOUND_MATRIX;
END:
    return ret;
}

#define SWAP(a, b) { \
            int temp; \
            temp = (a); \
            (a) = (b); \
            (b) = temp; \
         }

void swapVertices(matrix* d, int a, int b)
{
    int i = 0;
    int n = 0;
    int c_a = -1;
    int c_b = -1;
 
    if(a == b || a >= d->size || b >= d->size)
        return;
    n = d->size;
    c_a = d->inv_order[a];
    c_b = d->inv_order[b];
    for(i = 0 ; i < n ; i++)
        SWAP(MATRIX(d, c_a, i), MATRIX(d, c_b, i));
    for(i = 0 ; i < n ; i++)
        SWAP(MATRIX(d, i, c_a), MATRIX(d, i, c_b));
    SWAP(d->order[c_a], d->order[c_b]);
    d->inv_order[a] = c_b;
    d->inv_order[b] = c_a;
}

int getMaxVal(matrix *d, int *row, int *col)
{
    int ret = -1;
    int i = 0, n = 0;
    
    *row = *col = 0; 
    if(!d)
        goto END;
    n = d->size * d->size;
    
    for(i = 0 ; i < n ; i++)
        if(ret < d->val[i])
        {
            *row = i / d->size;
            *col = (i - (*row) * d->size);
            ret = d->val[i];
        }
END:
    return ret; 
}

int getMatrixSum(matrix *d)
{   
    int ret = 0;
    int i = 0, n = 0;
    if(!d)
        goto END;
    n = d->size * d->size;
    for(i = 0 ; i < n ; i++)
        ret += d->val[i];
END:
    return ret; 
}



void getMatrixFromFile(char *fileName, matrix *dest)
{
    FILE *fp = NULL;
    int mat_size = 0, i = 0;
    int *newMat = NULL;
    
    if(!fileName || !fileName[0])
    {
        fprintf(stderr, "ERROR : Input file name empty! " );
        return;
    }
    fp = (FILE*) fopen(fileName, "r");    
    if(!fp)
    {
        fprintf(stderr, "ERROR : File cannot be opened !");
        return;
    }
    fscanf(fp, "%d", &mat_size);
    if(!mat_size)
    {
        fprintf(stderr, "ERROR : NULL Matrix !");
        goto END;
    }
    
    newMat = (int*) calloc(mat_size*mat_size, sizeof(int));
    for(i = 0 ; i < mat_size*mat_size ; i++)
        fscanf(fp, "%d", (int*)&newMat[i]);
        
    dest->size = mat_size;
    dest->val = (int*) newMat;
    dest->order = (int*) calloc(mat_size, sizeof(int));
    dest->inv_order = (int*) calloc(mat_size, sizeof(int));
    for(i = 0 ; i < mat_size ; i++)
        dest->inv_order[i] = dest->order[i] = i;
END:
    //fclose(fp);
    ;
}


bool canThisBeEdgeVertex(matrix *m, int dist, int edgeLen, int checkVertex)
{
    int i = 0;
    bool ret = TRUE;
    int vid = 0, index = 0;
    for(i = edgeLen; i > dist ; i--)
    {
        vid = m->tmp_edge.nodes[i];
        index = m->inv_order[vid];
        if(MATRIX(m, checkVertex, index) != i - dist)
        {
            ret = FALSE;
            break;
        }
    }
    return ret;
}

bool findEdge(matrix *m, int dist, int edgeLen)
{
    bool ret = TRUE;
    int vertex_index = 0;
    int i = 0, n = 0, j = 0;
    
    if(dist == 0 || edgeLen == 0)
        goto END;
    ret = FALSE;
    if(dist > edgeLen)
        goto END;
    vertex_index = m->inv_order[m->tmp_edge.nodes[0]];
    n = m->size;

    for(i = 0 ; i < n ; i++)
    {
        if(MATRIX(m, vertex_index, i) == dist && canThisBeEdgeVertex(m, dist, edgeLen, i))
        {
            m->tmp_edge.nodes[dist] = m->order[i];
            ret = findEdge(m, dist-1, edgeLen);
            if(ret == TRUE)
                break;
        }
    }
END: 
    return ret;
}

void getAllEdges(matrix *m)
{
    int row = 0, col = 0;
    int maxEdgeLen = 0;
    int vertexId = 0;
    int ret = TRUE;
    
    initEdge(&m->tmp_edge, m->size);
    
    while((maxEdgeLen = getMaxVal(m, &row, &col)) > 0)
    {
        vertexId = m->order[row];
        m->tmp_edge.nodes[0] = vertexId;
        m->tmp_edge.len = maxEdgeLen+1;
        ret = findEdge(m, maxEdgeLen, maxEdgeLen);
        if(ret == FALSE)
        {
            output("\nThis matrix may not represent a semigraph !!!\n");
            break;
        }
        else
        {
            int i = 0, j = 0, n = m->size;
            printEdge(&m->tmp_edge);
            for(i = n-(maxEdgeLen+1), j = 0 ; i < n ; i++, j++)
            {
                swapVertices(m, m->order[i], m->tmp_edge.nodes[j]);
            }
            outputMatrix(outputFileName, m);
            for(i = n-(maxEdgeLen+1) ; i < n ; i++)
                for(j = n-(maxEdgeLen+1) ; j < n ; j++)
                    MATRIX(m, i, j) = 0;
            output("**********************************************************************");
        }
    }
}

int main(int argc, char* argv[])
{
    matrix src_matrix, tmp;
    int i = 0;
    
	if(argc < 2)
	{
		printf("USAGE :- %s <input matrix file-name> \n\t\t{-o <output matrix file-name>}\n", argv[0]);
		exit(0);
	}
    INIT_MATRIX(tmp);
    INIT_MATRIX(src_matrix);    
    for(i = 1 ; i < argc-1; i++)
    {
        if(strcmp(argv[i], "-o") == 0)
        {
            if(argv[i+1])
            {
                strcpy(outputFileName, argv[i+1]);
                //strlcpy(outputFileName, argv[i+1], sizeof(outputFileName));
            }
            if(outputFileName[0])
            {
                FILE *fp = NULL;
                fp = fopen(outputFileName, "w");
                if(fp)
                    fclose(fp);
            }
            break;
        }
    }
    empty_file(outputFileName);
    getMatrixFromFile(argv[1], &src_matrix);
    if(isSymmetricMatrix(&src_matrix) == FALSE)
    {
        fprintf(stderr, "\nThis is not a symmetric matrix!!!\n");
        return 0;
    }
    outputMatrix(outputFileName, &src_matrix);
    duplicateMatrix(&src_matrix, &tmp);
    getAllEdges(&tmp);
    output("\n--------------------END------------------------------------\n");
    return 0;
}

