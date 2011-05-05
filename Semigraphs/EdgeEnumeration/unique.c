/* Semigraphs : Given a matrix, this program tries to enumerate all possible edges. 
* Owner            : Roopesh S
* Source file      : $Source: /usr/local/cvsroot/src/code/Semigraphs/EdgeEnumeration/unique.c,v $
* Current Revision : $Revision: 1.1 $
* Branch           : $Name:  $
* Latest check-in  : $Author: roopesh $ on $Date: 2010-03-18 10:48:55 $
* */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdarg.h>

char outputFileName[1024];

typedef enum bool
{
    FALSE,
    TRUE
}bool;

#define FREE(x) if(x){ \
            free(x); \
        }

#define SWAP(a, b) { \
            int temp; \
            temp = (a); \
            (a) = (b); \
            (b) = temp; \
         }

// Defines MAX length of an edge. (which is a constant)
#define EDGE_LEN 4  
#define MAX_VERTICES 100

#define SCALE_MIN 0
#define SCALE_MAX EDGE_LEN  
#define RAND() (SCALE_MIN+(rand())%(SCALE_MAX-SCALE_MIN))
#define RAND_EX(range) (SCALE_MIN+(rand())%((range)-SCALE_MIN))
    
typedef char VERTEX;
typedef int* EDGE;

typedef void* (*lef_init) (va_list ap);
typedef void* (*lef_deinit) (void* e);
typedef void* (*lef_val) (void* e);
typedef void* (*lef_cmp) (void *e, va_list ap);

typedef struct list_elem{
    void *e; 
    struct list_elem *next;
}list_elem;

typedef struct list{
    list_elem *start;
    list_elem *last;
    int len;
    lef_init init;
    lef_deinit deinit;
    lef_val val;
    lef_cmp cmp;
}list;

typedef struct list_iter{
    list_elem *cur; // current element
    list* l; // Pointer to its actual list (back pointer)
}list_iter;

typedef struct graph{
    VERTEX matrix[MAX_VERTICES][MAX_VERTICES];
    list *v_ledge[MAX_VERTICES];
    list *edges;
    list *new_edges; // List of new edges to be added (so that both Graphs A and B becomes same).
}graph;

typedef struct container{
    graph A;
    graph B;
    list *end_vertex[MAX_VERTICES];
    int nv ; // No. of vertices active. (out of MAX_VERTICES)
}container;

// TODO : Define Random number generator here (and change its seed by getting time by using 'time()' func
#define LIST_CREATE(x) {\
    (x) = (list*) calloc(1, sizeof(list)); \
    if(!(x)) \
        printf("ERROR (%d) : Allocation failed", __LINE__); \
    list_init(x, NULL, NULL, NULL); \
    }

#define LIST_CREATE_EX(x, type) {\
    (x) = (list*) calloc(1, sizeof(list)); \
    list_init(x, type##_init, type##_deinit, type##_val, type##cmp); \
    }

#define LIST_DESTROY(x) {\
    list_deinit(x); \
    FREE(x); \
    (x) = NULL; \
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

////////////////////////////////////////////
// **** Vertex Array definitions starts here
void* va_init(va_list ap)
{ 
    int i = 0;
    VERTEX *vals = NULL, *ret = NULL;

    ret = (VERTEX*) calloc(EDGE_LEN,sizeof(VERTEX));
    if(ret)
    {
        vals = va_arg(ap, VERTEX*);
        for(i = 0 ; i < EDGE_LEN ; i++)
            ret[i] = vals[i];
    }
    return ret;
}

void* va_deinit(void *e)
{
    FREE(e);
    return NULL; 
}

void* va_val(void *e)
{
    return *e;
}

void* va_cmp(void *e, va_list ap)
{
    BOOL ret = 0;
    VERTEX *arg1 = NULL, *arg2 = NULL;
    int i = 0 ;

    arg1 = (VERTEX*) e;
    arg2 = (VERTEX*) va_arg(ap, (VERTEX*));
    if(arg1 != arg2)
    {
        for(i = 0; i < EDGE_LEN; i++)
        {
            if(arg1[i] == arg2[i])
                continue;
            if(arg1[1] < arg2[v])
                ret = -1;
            else 
                ret = 1;
            break;
        }
    }
    return ret;
}

// **** Vertex Array definitions ends here
////////////////////////////////////////////

bool list_init(list *l, lef_init init, lef_deinit deinit, lef_val val, lef_cmp cmp)
{ // Initialises list and returns TRUE on success or FALSE otherwise
    bool ret = FALSE;
    if(l)
    {
        l->len = 0;
        l->start = l->last = (list_elem*) calloc(1, sizeof(list_elem)); // Creating a dummy node
        l->init = init;
        l->deinit = deinit;
        l->val = val;
        l->cmp = cmp;
        if(l->start)
        {
            l->start->e = NULL;
            l->start->next = NULL;
            ret = TRUE;
        }
    }
    return ret;
}

void list_deinit(list *l)
{
    list_elem *cur = NULL, *next = NULL;
    if(!l || !l->start)
        return;
    cur = l->start->next;
    FREE(l->start);
    while(cur)
    {
        next = cur->next;
        if(l->deinit)
            l->deinit(cur->e);
        FREE(cur);
        cur = next;
    }
}

void* list_add(list *l, ...)
{
    void* ret = NULL;
    list_elem *new_elem = NULL;
    va_list ap = NULL;
    
    if(l && l->last)
    {
        new_elem = (list_elem*) calloc(1, sizeof(list_elem));
        if(new_elem)
        {
            va_start(ap, l);
            if(l->init)
                new_elem->e = l->init(ap);
            else
                new_elem->e = va_arg(ap, (void*));
            va_end(ap); 
            new_elem->next = NULL;
            l->last->next = new_elem;
            l->last = new_elem;
            l->len++;
            ret = new_elem;
        }
    }
    return ret; 
}

void* list_del(list *l, ...)
{
    va_list ap = NULL;
    list_elem *cur = NULL;
    void *ret = NULL;
    list_elem *tmp = NULL;

    if(l && l->start)
    {
        cur = l->start;
        if(l->cmp)
        {
            int cmp_val = 0;
            while(cur->next)
            {
                va_start(ap, l);
                cmp_val = l->cmp(cur->next->e, ap);
                va_end(ap);
                if(cmp_val==0)
                    break;
                cur = cur->next;
            }
        }
        else
        {
            while(cur->next)
            {
                if(cur->next->e == e)
                    break;
                cur = cur->next;
            }
        }
        if(!cur->next)
        {
            ret = cur->next->e;
            tmp = cur->next;
            if(!cur->next->next)
                l->last = cur;
            cur->next = cur->next->next;
            FREE(tmp);
            l->len--;
        }
    }
    return ret;
}

bool list_search(list *l, ...)
{
    bool ret = FALSE;
    list_elem *cur = NULL;
    if(l && l->start)
    {
        cur = l->start->next;
        if(l->cmp)
        {
            int cmp_val = 0;
            while(cur)
            {
                va_start(ap, l);
                cmp_val = l->cmp(cur->e, ap);
                va_end(ap);
                if(cmp_val==0)
                {
                    ret = TRUE;
                    break;
                }
                cur = cur->next;
            }
        }
        else
        {
            while(cur)
            {
                if(cur->e == e)
                {
                    ret = TRUE;
                    break;
                }
                cur = cur->next;
            }
        }
    }
    return ret;
}

int list_len(list *l)
{
    if (l)
        return l->len;
    else 
        return 0;
}

void* list_iter_start(list *l)
{
    list_iter *ret =  NULL;
    if(l && l->start)
    {
        ret = (list_iter*) calloc(1, sizeof(list_iter));
        if(ret)
        {
            ret->l = l;
            ret->cur = l->start->next;
        }
    }
    return ret;
}

void* list_iter_next(list_iter *it)
{
    void *ret = NULL;
    if(it)
    {
        if(it->cur)
        {
            if(it->l->val)
                ret = it->l->val(it->cur->e);
            else
                ret = it->cur->e;
            it->cur = it->cur->next;
        }
    }
    return ret;
}

void list_iter_end(list_iter *it)
{
    if(it)
        FREE(it);
}

bool add_edge_to_matrix(VERTEX mat[][EDGE_LEN], VERTEX edge[])
{
    int i=0, j=0;
    VERTEX val = 0;
    //Sanity check here.....
    for(i=0 ; i < EDGE_LEN ; i++)
        for(j = i+1; j < EDGE_LEN ; j++)
            {
                val = mat[edge[i]][edge[j]] 
                if(val > 0 && val == j-i)
                    continue;
                printf("\nERROR (%d) : Sanity check fail !!! V(%d, %d) = %d <--- %d", __LINE__, edge[i],edge[j],val,j-i);
                return FALSE;
            }

    // Adding here after sanity check is ok.
    for(i=0 ; i < EDGE_LEN ; i++)
        for(j = 0; j < EDGE_LEN ; j++)
            {
                mat[edge[i]][edge[j]] = (VERTEX)(j-i);
            }
    return TRUE;
}

VERTEX* add_edge(container *c, graph *g, const VERTEX e[])
{
    int i = 0;
    VERTEX tmp_e[EDGE_LEN] = {0};
    VERTEX *ret = NULL;

    if(!list_search(g->v_ledge[e[RAND()]], (VERTEX *)e))
        goto END;
    if(!add_edge_to_matrix(g->matrix, e))
        goto END;

    if(e[0] > e[EDGE_LEN-1])
    {// Reverse edge, so that less-index vertex comes to left (follow this order to optimize the comparision)
        for(i = 0 ; i < EDGE_LEN, i++)
            tmp_e[i] = e[EDGE_LEN-1-i];
    }
    else
    {
        for(i = 0 ; i < EDGE_LEN, i++)
            tmp_e[i] = e[i];
    }
    ret = (VERTEX*) list_add(g->edges, tmp_e);
    if(!ret)
    {
        printf("ERROR (%d) : Fatal error, couldn't add edge !\n", __LINE__);
        return FALSE;
    }
    for(i = 0 ; i < EDGE_LEN; i++)
        list_add(g->v_ledge, ret);
END:
    return ret;
}

void add_vertex(container *c)
{
    int vi = 0;
    vi = c->nv;
    if(vi == MAX_VERTICES-1)
    {
        printf("ERROR : Maximum vertices generated. No more can be added !! \n");
        return;
    }
    LIST_CREATE(c->end_vertex[vi]);
    LIST_CREATE(c->A.v_ledge[vi]);
    LIST_CREATE(c->B.v_ledge[vi]);
    c->nv++;
}

void create_initial_edges(container *c)
{
    // ASSUMPTION : vertices 0..EDGE_LEN has already been created 
    int i = 0;
    VERTEX e[EDGE_LEN]={0};
    VERTEX *tmp = NULL;

    for(i = 0 ; i < EDGE_LEN, i++)
    {
        add_vertex(c);
        e[i] = i;
    }
    tmp = add_edge(c, c->A, (VERTEX*)e);
    if(tmp)
        list_add(c->B.new_edges, tmp);
    
    e[EDGE_LEN-2]=EDGE_LEN; // In B, edge is changed by one m-vertex from e.

    tmp = add_edge(c, c->B, (VERTEX*)e);
    if(tmp)
        list_add(c->A.new_edges, tmp);
}

void init_container(container *c)
{
    int i = 0;
    VERTEX eA[EDGE_LEN]= {0}, eB[EDGE_LEN] = {0}; 
    memset(c->A.matrix, 0, sizeof(c->A.matrix));
    memset(c->B.matrix, 0, sizeof(c->B.matrix));
    c->nv = 0; // No. of vertices active. (out of MAX_VERTICES)
    LIST_CREATE_EX(c->A.edges, va);
    LIST_CREATE_EX(c->B.edges, va);
    LIST_CREATE(c->A.new_edges);
    LIST_CREATE(c->B.new_edges);
    for(i = 0 ; i <= EDGE_LEN, i++)
        add_vertex(c); // Adding (EDGE_LEN + 1) number of vertices
    create_initial_edges(c);
}

void deinit_container(container *c)
{
    int i = 0, nv = 0;
    LIST_DESTROY(c->A.edges);
    LIST_DESTROY(c->B.edges);
    LIST_DESTROY(c->A.new_edges);
    LIST_DESTROY(c->B.new_edges);
    nv = c->nv;
    for(i = 0; i < nv ; i++)
    {
        LIST_DESTROY(c->A.v_ledge[i]);
        LIST_DESTROY(c->B.v_ledge[i]);
        LIST_DESTROY(c->end_vertex[i]);
    }
    c->nv = 0;
}

int main(int argc, char* argv[])
{
    srand(time(NULL));
    container c;
    return 0;
}

