#include<stdio.h>
#include<stdlib.h>
#include<conio.h>

#define TRUE 1
#define FALSE 0

//#define VERBOSE TRUE

typedef char BOOL;
typedef char BYTE;

#define MAX_RECURSION_LIMIT 20

#define TOTAL_AXES (3)
#define X_AXIS (0)
#define Y_AXIS (1)
#define Z_AXIS (2)

#define NULL_AXIS 0
#define BY_AXIS X_AXIS+1
#define OR_AXIS Y_AXIS+1
#define WG_AXIS Z_AXIS+1
const char* const str_axes[] = {"NULL","BY","OR","WG"};

#define CLOCKWISE (1)
#define ANTICLOCKWISE ((-1)%TOTAL_AXES)

#define TOTAL_DIRECTIONS 2
const enum Dir{FRONT=0, BACK=1};
#define TOTAL_ROTATIONS 3
const enum Rotations{RIGHT=0, BOTTOM=1, LEFT=2};

#define TOTAL_MOVES_POSSIBLE ((TOTAL_AXES)*(TOTAL_DIRECTIONS)*(TOTAL_ROTATIONS))

typedef struct{
    BYTE point[TOTAL_AXES]; // (X,Y,Z) Co-ordinates
    BYTE colors[TOTAL_AXES]; // Colors which are present on the unit : Non-zero indicate color present
    BYTE origin[TOTAL_AXES]; // (X,Y,Z) original Co-ordinates
    int offset_wt; // Sum of diff |point[x]-origin[x]|
}unit;

typedef struct{
    int wt; // Weight after axis rotation
    BYTE axis; // rotating about which axis
                // and sign indicates front or backside of axis (E.g. +1 for Blue and -1 for Yellow)
    BYTE rotation; // rotation amount (RIGHT, BOTTOM, LEFT)
    //BOOL valid; // TRUE --> if leads to a solution FIXME : Check whether this is required or not 
}axis_weight;

typedef unit (*status)[TOTAL_AXES][TOTAL_AXES];
typedef axis_weight (aw_list)[TOTAL_AXES*2*TOTAL_ROTATIONS];

#define INDEX(a, b, mf, af) ((a)*(mf)+(b)+(af))

// SWAP Two Integers
#define SWAP(a,b) { \
    BYTE temp; \
    temp = a; \
    a = b; \
    b = temp; \
}

int get_wt(unit *u)
{
    int wt = 0, i =0;
    for(i = 0 ; i < TOTAL_AXES ; i++)
        wt += u->point[i] > u->origin[i] ? (u->point[i] - u->origin[i]) : (u->origin[i] - u->point[i]) ;
    return wt;
}

void init(status s)
{
    BYTE i = 0, j = 0, k = 0;
    for(i = 0; i < TOTAL_AXES ; i++)
    {
        for(j = 0; j < TOTAL_AXES ; j++)
        {
            for(k = 0; k < TOTAL_AXES ; k++)
            {
                s[i][j][k].origin[X_AXIS] = s[i][j][k].point[X_AXIS] = (i == 2 ? -1 : i);
                s[i][j][k].origin[Y_AXIS] = s[i][j][k].point[Y_AXIS] = (j == 2 ? -1 : j);
                s[i][j][k].origin[Z_AXIS] = s[i][j][k].point[Z_AXIS] = (k == 2 ? -1 : k);
                s[i][j][k].colors[X_AXIS] = i ? X_AXIS + 1 : 0;
                s[i][j][k].colors[Y_AXIS] = j ? Y_AXIS + 1 : 0;
                s[i][j][k].colors[Z_AXIS] = k ? Z_AXIS + 1 : 0;
                s[i][j][k].offset_wt = get_wt(&s[i][j][k]); 
            }
        }
    }
}

void print_status(status s)
{
    int i = 0, j = 0, k = 0, l = 0;
    printf ("********************BEGIN OF STATUS******************************\n");
    for(i = 0; i < TOTAL_AXES ; i++)
    {
        for(j = 0; j < TOTAL_AXES ; j++)
        {
            for(k = 0; k < TOTAL_AXES ; k++)
            {
                printf("(");
                for(l = 0; l < TOTAL_AXES; l++)
                    printf("%d, ", s[i][j][k].point[l]);
                printf("\b\b)");
                printf("-(");
                for(l = 0; l < TOTAL_AXES; l++)
                    printf("%s, ", str_axes[s[i][j][k].colors[l]]);
                printf("\b\b)");
                    if(j-1 < TOTAL_AXES)
                        printf(" <-----> ");
            }
            printf ("\n");
        }
    }
    printf ("********************END OF STATUS********************************\n");
}

int move(status s, BYTE axis, BYTE rot)
{
    int rot_wt = -1;
    BYTE fb = 0, i = 0 , j= 0;
    BOOL should_swap = FALSE;
    BYTE offset_index = 0;
    BYTE x_mp = 1, y_mp = 1;
    unit *u = NULL;

    if(axis == 0)
        goto END;
    if(axis < 0)
    {
        fb = 2;
        axis *= -1;
    }
    else
        fb = 1;
    switch(rot)
    {
        case RIGHT:should_swap = TRUE;
                  x_mp = -1;
            break;
        case BOTTOM:x_mp = y_mp = -1;
            break;
        case LEFT:should_swap = TRUE;
                    y_mp = -1;
            break;
        default:
            goto END;
    }
    rot_wt = 0; // Reseting the rotation weight
    j = 1;
    for(i = 0 ; i < TOTAL_AXES ; i++)
    {
        for(; j < TOTAL_AXES ; j++)
        {
            // (i,j) = (0,0) has no effect... so ignoring this iteration
            switch(axis)
            {
                case 1:
                    offset_index = ((fb*TOTAL_AXES) + i)*TOTAL_AXES + j;
                    break;
                case 2:
                    offset_index = ((i*TOTAL_AXES) + fb)*TOTAL_AXES + j;
                    break;
                case 3:
                    offset_index = ((i*TOTAL_AXES) + j)*TOTAL_AXES + fb;
                    break;
            }
            u = (unit*)s+offset_index;
            switch(axis)
            {
                case 1:
                    if(should_swap)
                    {
                        SWAP(u->point[1],u->point[2]);
                        SWAP(u->colors[1],u->colors[2]);
                    }
                    u->point[1] *= x_mp; u->point[2] *= y_mp;
                    break;
                case 2:
                    if(should_swap)
                    {
                        SWAP(u->point[2],u->point[0]);
                        SWAP(u->colors[2],u->colors[0]);
                    }
                    u->point[2] *= x_mp; u->point[0] *= y_mp;
                    break;
                case 3:
                    if(should_swap)
                    {
                        SWAP(u->point[0],u->point[1]);
                        SWAP(u->colors[0],u->colors[1]);
                    }
                    u->point[0] *= x_mp; u->point[1] *= y_mp;
                    break;
            }
            u->offset_wt = get_wt(u);
            rot_wt += u->offset_wt;
        }
        j = 0;
    }
#ifdef VERBOSE
    print_status(s);
#endif
END:
    return rot_wt;
}

/*
BOOL get_factors(BYTE axis, BYTE *fb, BYTE *mf, BYTE *af)
{
    BYTE i = 0, tmf = 1;
    BOOL ret = FALSE;
    *fb = 0; *mf = 1; *af = 0;

    if(axis == 0 || axis > TOTAL_AXES)
    {
#ifdef VERBOSE
        printf("ERRO: axis = 0 (NULL AXIS)!!\n");
#endif
        goto END;
    }
    if(axis > 0)
        fb = 1;
    else
    {
        fb = 2;
        axis *= -1;
    }

    for(i = TOTAL_AXES ; i > axis ; i--)
        tmf *= TOTAL_AXES;
    *af = tmf * fb;
    *mp = tmp * TOTAL_AXES;
    ret = TRUE;
END:
    return ret;
}
*/

/* Rotates 's' along axis and returns 0 if successful */
/*BYTE move(status s, BYTE axis, BYTE rot)
{
    BYTE ret = 1, fb = 0;
    BOOL should_swap = FALSE;
    BYTE mf = 1, af = 0;
    BYTE offset_index = 0;

    if(axis == 0 or rot == 0)
        goto END;
    if(!get_factors(axis, &fb, &mf, &af))
        goto END;
    for(i = 0 ; i < TOTAL_AXES ; i++)
    {
        for(j = 0 ; j < TOTAL_AXES ; j++)
        {
            switch(axis)
            {
                case 1:
                case -1:
                    offset_index = INDEX(0, i*TOTAL_AXES + j, mp, af); 
                    break;
                case 2:
                case -2:
                    offset_index = INDEX(i, j, mp, af);
                    break;
                case 3:
                case -3:
                    offset_index = INDEX(i*TOTAL_AXES + j, 0, mp, af);
                break;
            }
            switch(rot)
            {
                case LEFT:
                    break;
                case BOTTOM:
                    break;
                case RIGHT:
                    break;
            }
        }
    }
    ret = 0;
END:
    return ret;
}*/

BYTE unmove(status s, BYTE axis, BYTE rot)
{
    return move(s, axis, TOTAL_ROTATIONS - rot);
}

void print_move(BYTE axis, BYTE rotation)
{
    const char* face_str[] = {"G",  "R", "Y", "B", "O", "W"}; // FIXME : code this better
    const char* rot_str[] = {"RIGHT", "BOTTOM", "LEFT"}; 
    printf("MOVE (%s, %9s)\n",face_str[axis+3],rot_str[rotation]);
}

void print_awl(aw_list awl)
{
    BYTE i = 0;
    const char* face_str[] = {"G", "R", "Y", "NULL", "B", "O", "W"}; // FIXME : code this better
    const char* rot_str[] = {"RIGHT", "BOTTOM", "LEFT"}; 
    printf("---------------AxIS WT ROTATION LIST----------------\n");
    for(i = 0; i < TOTAL_MOVES_POSSIBLE ; i++)
    {
        if(awl[i].axis == 0)
            break; // End of list
        printf("(%s, %9s, %d)\n",face_str[awl[i].axis+3],rot_str[awl[i].rotation],awl[i].wt);
    }
    printf("---------------AxIS WT ROTATION LIST----------------\n");
}

BYTE insert_aw(aw_list awl, int wt, BYTE axis, BYTE rotation, BYTE aw_list_count)
{// Returns list count of 'awl' after insertion
    BYTE ret = 0, i = 0;
    if(!awl || aw_list_count >= TOTAL_MOVES_POSSIBLE)
    {
        ret = -1;
        goto END;
    }
    if(aw_list_count > 0)
    {
        i = aw_list_count-1;
        ret = aw_list_count+1;
    }
    else
    {
        for(i = 0; i < TOTAL_MOVES_POSSIBLE ; i++)
            if(awl[i].axis == 0)
            {
                i--;
                ret = i + 2;
                break; // End of list
            }
    }
    // Traversing in reverse direction of the list    
    for(;i>=0;i--)
    {
        if(awl[i].wt <= wt)
            break;
        awl[i+1] = awl[i];
    }
    i++;
    awl[i].wt = wt;
    awl[i].axis = axis;
    awl[i].rotation = rotation;
#ifdef VERBOSE
    print_awl(awl);
#endif
END:
    return ret;
}

void init_aw_list(aw_list awl)
{
    int i = 0;
    for(i = TOTAL_MOVES_POSSIBLE-1 ; i >= 0 ; i--)
    {
        awl[i].wt = -1;
        //awl[i].valid = FALSE;
        awl[i].rotation = 0;
        awl[i].axis = 0;
    }
}

void build_axes_wt(status s, aw_list awl)
{
    BYTE i = 0, j = 0, k = 0, axis = 0, move_count = 0;
    int wt = 0;

    init_aw_list(awl);
    for(i = 0 ; i < TOTAL_AXES ; i++)
        for(j = 0 ; j < TOTAL_DIRECTIONS ; j++)
        {
            for(k = 0 ; k < TOTAL_ROTATIONS; k++)
            {
                axis = (i+1)*(j == 0 ? 1 : -1);
                wt = move(s, axis, RIGHT);
                if(wt < 0)
                    printf("ERR: move(....) function failed\n");
                move_count = insert_aw(awl, wt, axis, k, move_count);
            }
            move(s, axis, RIGHT); // Get back to original position (1 round complete)
        }
#ifdef VERBOSE
    print_awl(awl);
#endif
}

BOOL is_solved(status s)
{
    BOOL ret = FALSE;
    int i = 0, j = 0, k = 0, l=0;
    for(i = 0; i < TOTAL_AXES ; i++)
        for(j = 0; j < TOTAL_AXES ; j++)
            for(k = 0; k < TOTAL_AXES ; k++)
                if(s[i][j][k].offset_wt)
                    goto END;
    for(i = 0; i < TOTAL_AXES ; i++)
        for(j = 0; j < TOTAL_AXES ; j++)
            for(k = 0; k < TOTAL_AXES ; k++)
                for(l = 0 ; l < TOTAL_AXES ; l++)
                    if(s[i][j][k].origin[l] && s[i][j][k].colors[l] != l+1)
                           goto END;
    ret = TRUE;
END:
    return ret;
}

BOOL make_move(status s, BYTE limit)
{
    BOOL ret = TRUE;
    BYTE i = 0;
    aw_list awl;

    if(!limit)
    {
        ret = FALSE;
        goto END;
    }

    if(is_solved(s))
        goto END;

    build_axes_wt(s, awl);
    for(i = 0 ; i < TOTAL_MOVES_POSSIBLE ; i++)
    {
        if(limit==MAX_RECURSION_LIMIT)
            print_move(awl[i].axis, awl[i].rotation);
        move(s, awl[i].axis, awl[i].rotation);
        ret = make_move(s, limit-1);
        if(ret)
        {
            print_move(awl[i].axis, awl[i].rotation);
            goto END;
        }
        unmove(s, awl[i].axis, awl[i].rotation);
    }
    if(limit==MAX_RECURSION_LIMIT)
        printf("\t\t==>FAILED\n");
    ret = FALSE;
END:
    return ret;
}

void add_test_rotations(status s)
{
	/*orange plane*/
	s[0][1][1].point[0] =0;s[0][1][1].point[1] =1;s[0][1][1].point[2] =-1;
	s[0][1][1].colors[0]=0;s[0][1][1].colors[1]=2;s[0][1][1].colors[2]=3;
	s[0][1][2].point[0] =0;s[0][1][2].point[1] =-1;s[0][1][2].point[2] =-1;
	s[0][1][2].colors[0]=0;s[0][1][2].colors[1]=3;s[0][1][2].colors[2]=2;


	s[2][1][0].point[0]=0;s[2][1][0].point[1]=1;s[2][1][0].point[2]=1;
	s[2][1][0].colors[0]=0;s[2][1][0].colors[1]=1;s[2][1][0].colors[2]=2;
	s[2][1][2].point[0]=-1;s[2][1][2].point[1]=-1;s[2][1][2].point[2]=-1;
	s[2][1][2].colors[0]=1;s[2][1][2].colors[1]=3;s[2][1][2].colors[2]=2;
	/*orange plane*/

	/*Red Plane*/
	//s[0][2][1].point[0]=;s[0][2][1].point[1]=;s[0][2][1].point[2]=; // Becoz in same place
	s[0][2][1].colors[0]=0;s[0][2][1].colors[1]=3;s[0][2][1].colors[2]=2;
	s[0][2][2].point[0]=-1;s[0][2][2].point[1]=-1;s[0][2][2].point[2]=0;
	s[0][2][2].colors[0]=2;s[0][2][2].colors[1]=3;s[0][2][2].colors[2]=0;

	s[2][2][0].point[0]=-1;s[2][2][0].point[1]=1;s[2][2][0].point[2]=0;
	s[2][2][0].colors[0]=1;s[2][2][0].colors[1]=2;s[2][2][0].colors[2]=0;
	s[2][2][1].point[0]=-1;s[2][2][1].point[1]=1;s[2][2][1].point[2]=-1;
	s[2][2][1].colors[0]=1;s[2][2][1].colors[1]=2;s[2][2][1].colors[2]=3;
	s[2][2][2].point[0]=-1;s[2][2][2].point[1]=-1;s[2][2][2].point[2]=1;
	s[2][2][2].colors[0]=1;s[2][2][2].colors[1]=3;s[2][2][2].colors[2]=2;
	/*Red Plane*/
}

int main(int argc, char *argv[])
{
    unit state[TOTAL_AXES][TOTAL_AXES][TOTAL_AXES];
    clrscr();
    init(state);
    add_test_rotations(state);
    print_status(state);
    if(make_move(state, MAX_RECURSION_LIMIT))
        printf ("SUCCESSFULLY SOLVED THE PUZZLE !!! \n");
    getch();
    return 0;
}
