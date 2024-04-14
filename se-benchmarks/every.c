#include <stdio.h>
#include <stdlib.h>

#define  nil		0
#define	 false		0
#define  true		1
#define  bubblebase	1.61f
#define  dnfbase 	3.5f
#define  permbase 	1.75f
#define  queensbase 1.83f
#define  towersbase 2.39f
#define  quickbase 	1.92f
#define  intmmbase 	1.46f
#define  treebase 	2.5f
#define  mmbase 	0.0f
#define  fpmmbase 	2.92f
#define  puzzlebase	0.5f
#define  fftbase 	0.0f
#define  fpfftbase 	4.44f
    /* Towers */
#define maxcells 	 18

    /* Intmm, Mm */
#define rowsize 	 40

    /* Puzzle */
#define size	 	 511
#define classmax 	 3
#define typemax 	 12
#define d 		     8

    /* Bubble, Quick */
#define sortelements 5000
#define srtelements  500

    /* fft */
#define fftsize 	 256
#define fftsize2 	 129
/*
type */
    /* Perm */
#define permrange     10

   /* tree */
struct node
{
        struct node *left,*right;
        int val;
};

    /* Towers */ /*
    discsizrange = 1..maxcells; */
#define    stackrange	3
/*    cellcursor = 0..maxcells; */
struct    element
{
        int discsize;
        int next;
};
/*    emsgtype = packed array[1..15] of char;
*/
    /* Intmm, Mm */ /*
    index = 1 .. rowsize;
    intmatrix = array [index,index] of integer;
    realmatrix = array [index,index] of real;
*/
    /* Puzzle */ /*
    piececlass = 0..classmax;
    piecetype = 0..typemax;
    position = 0..size;
*/
    /* Bubble, Quick */ /*
    listsize = 0..sortelements;
    sortarray = array [listsize] of integer;
*/
    /* FFT */
struct    complex
{
    float rp, ip; } ;
/*
    carray = array [1..fftsize] of complex ;
    c2array = array [1..fftsize2] of complex ;
*/

float value, fixed, floated;

    /* global */
long    seed;  /* converted to long for 16 bit WR*/

    /* Perm */
int    permarray[permrange+1];
/* converted pctr to unsigned int for 16 bit WR*/
unsigned int    pctr;

    /* tree */
struct node *tree;

    /* Towers */
int	   stack[stackrange+1];
struct element    cellspace[maxcells+1];
int    freelist,  movesdone;

    /* Intmm, Mm */

int ima[rowsize+1][rowsize+1];
int imb[rowsize+1][rowsize+1];
int imr[rowsize+1][rowsize+1];
float rma[rowsize+1][rowsize+1];
float rmb[rowsize+1][rowsize+1];
float rmr[rowsize+1][rowsize+1];

    /* Puzzle */
int	piececount[classmax+1],	class[typemax+1], piecemax[typemax+1];
int	puzzl[size+1], p[typemax+1][size+1], n, kount;

    /* Bubble, Quick */
int sortlist[sortelements+1], biggest, littlest, top;

    /* FFT */
struct complex    z[fftsize+1], w[fftsize+1], e[fftsize2+1];
float    zr, zi;

void Initrand () {
    seed = 74755L;   /* constant to long WR*/
}

int Rand () {
    seed = (seed * 1309L + 13849L) & 65535L;  /* constants to long WR*/
    return( (int)seed );     /* typecast back to int WR*/
}


    /* A compute-bound program from Forest Baskett. */

int Fit (int i, int j) {
        int k;
        for ( k = 0; k <= piecemax[i]; k++ )
            if ( p[i][k] ) if ( puzzl[j+k] ) return (false);
        return (true);
}

int Place (int i, int j) {
        int k;
        for ( k = 0; k <= piecemax[i]; k++ ) if ( p[i][k] ) puzzl[j+k] = true;
        piececount[class[i]] = piececount[class[i]] - 1;
        for ( k = j; k <= size; k++ ) if ( ! puzzl[k] ) return (k);
        return (0);
}

void Remove (int i, int j) {
        int k;
        for ( k = 0; k <= piecemax[i]; k++ ) if ( p[i][k] ) puzzl[j+k] = false;
        piececount[class[i]] = piececount[class[i]] + 1;
}

int Trial (int j) {
        int i, k;
        kount = kount + 1;
        for ( i = 0; i <= typemax; i++ )
            if ( piececount[class[i]] != 0 )
                if ( Fit (i, j) ) {
                    k = Place (i, j);
                    if ( Trial(k) || (k == 0) )return (true);
                        else Remove (i, j);
                }
        return (false);
}

void Puzzle ()  {
    int i, j, k, m;
    for ( m = 0; m <= size; m++ ) puzzl[m] = true;
    for ( i = 1; i <= 5; i++ )
        for ( j = 1; j <= 5; j++ )
            for ( k = 1; k <= 5; k++ )
                puzzl[i+d*(j+d*k)] = false;
    for ( i = 0; i <= typemax; i++ )
        for ( m = 0; m<= size; m++ )
            p[i][m] = false;
    for ( i = 0; i <= 3; i++ )
        for ( j = 0; j <= 1; j++ )
            for ( k = 0; k <= 0; k++ )
                p[0][i+d*(j+d*k)] = true;
    class[0] = 0;
    piecemax[0] = 3+d*1+d*d*0;
    for ( i = 0; i <= 1; i++ )
        for ( j = 0; j <= 0; j++ )
            for ( k = 0; k <= 3; k++ )
                p[1][i+d*(j+d*k)] = true;
    class[1] = 0;
    piecemax[1] = 1+d*0+d*d*3;
    for ( i = 0; i <= 0; i++ )
        for ( j = 0; j <= 3; j++ )
            for ( k = 0; k <= 1; k++ )
                p[2][i+d*(j+d*k)] = true;
    class[2] = 0;
    piecemax[2] = 0+d*3+d*d*1;
    for ( i = 0; i <= 1; i++ )
        for ( j = 0; j <= 3; j++ )
            for ( k = 0; k <= 0; k++ )
                p[3][i+d*(j+d*k)] = true;
    class[3] = 0;
    piecemax[3] = 1+d*3+d*d*0;
    for ( i = 0; i <= 3; i++ )
        for ( j = 0; j <= 0; j++ )
            for ( k = 0; k <= 1; k++ )
                p[4][i+d*(j+d*k)] = true;
    class[4] = 0;
    piecemax[4] = 3+d*0+d*d*1;
    for ( i = 0; i <= 0; i++ )
        for ( j = 0; j <= 1; j++ )
            for ( k = 0; k <= 3; k++ )
                p[5][i+d*(j+d*k)] = true;
    class[5] = 0;
    piecemax[5] = 0+d*1+d*d*3;
    for ( i = 0; i <= 2; i++ )
        for ( j = 0; j <= 0; j++ )
            for ( k = 0; k <= 0; k++ )
                p[6][i+d*(j+d*k)] = true;
    class[6] = 1;
    piecemax[6] = 2+d*0+d*d*0;
    for ( i = 0; i <= 0; i++ )
        for ( j = 0; j <= 2; j++ )
            for ( k = 0; k <= 0; k++ )
                p[7][i+d*(j+d*k)] = true;
    class[7] = 1;
    piecemax[7] = 0+d*2+d*d*0;
    for ( i = 0; i <= 0; i++ )
        for ( j = 0; j <= 0; j++ )
            for ( k = 0; k <= 2; k++ )
                p[8][i+d*(j+d*k)] = true;
    class[8] = 1;
    piecemax[8] = 0+d*0+d*d*2;
    for ( i = 0; i <= 1; i++ )
        for ( j = 0; j <= 1; j++ )
            for ( k = 0; k <= 0; k++ )
                p[9][i+d*(j+d*k)] = true;
    class[9] = 2;
    piecemax[9] = 1+d*1+d*d*0;
    for ( i = 0; i <= 1; i++ )
        for ( j = 0; j <= 0; j++ )
            for ( k = 0; k <= 1; k++ )
                p[10][i+d*(j+d*k)] = true;
    class[10] = 2;
    piecemax[10] = 1+d*0+d*d*1;
    for ( i = 0; i <= 0; i++ )
        for ( j = 0; j <= 1; j++ )
            for ( k = 0; k <= 1; k++ )
                p[11][i+d*(j+d*k)] = true;
    class[11] = 2;
    piecemax[11] = 0+d*1+d*d*1;
    for ( i = 0; i <= 1; i++ )
        for ( j = 0; j <= 1; j++ )
            for ( k = 0; k <= 1; k++ )
                p[12][i+d*(j+d*k)] = true;
    class[12] = 3;
    piecemax[12] = 1+d*1+d*d*1;
    piececount[0] = 13;
    piececount[1] = 3;
    piececount[2] = 1;
    piececount[3] = 1;
    m = 1+d*(1+d*1);
    kount = 0;
    if ( Fit(0, m) ) n = Place(0, m);
    else printf("Error1 in Puzzle\n");
    if ( ! Trial(n) ) printf ("Error2 in Puzzle.\n");
    else if ( kount != 2005 ) printf ( "Error3 in Puzzle.\n");
         printf("%d\n", n);
         printf("%d\n", kount);
}

    /* Permutation program, heavily recursive, written by Denny Brown. */

void Swap ( int *a, int *b ) {
        int t;
        t = *a;  *a = *b;  *b = t;
}

void Initialize () {
        int i;
        for ( i = 1; i <= 7; i++ ) {
            permarray[i]=i-1;
        }
}

void Permute (int n) {   /* permute */
        int k;
        pctr = pctr + 1;
        if ( n!=1 )  {
            Permute(n-1);
            for ( k = n-1; k >= 1; k-- ) {
                        Swap(&permarray[n],&permarray[k]);
                        Permute(n-1);
                        Swap(&permarray[n],&permarray[k]);
                }
    }
}     /* permute */

void Perm ()    {   /* Perm */
    int i;
    pctr = 0;
    for ( i = 1; i <= 5; i++ ) {
                Initialize();
                Permute(7);
        }
    if ( pctr != 43300 )
        printf(" Error in Perm.\n");
        printf("%d\n", pctr);
}     /* Perm */


    /*  Program to Solve the Towers of Hanoi */

void Error (char *emsg) 	{
        printf(" Error in Towers: %s\n",emsg);
}

void Makenull (int s) {
        stack[s]=0;
}

int Getelement () {
        int temp = 0;  /* force init of temp WR*/
        if ( freelist>0 ) {
            temp = freelist;
            freelist = cellspace[freelist].next;
        }
        else
            Error("out of space   ");
        return (temp);
}

void Push(int i, int s)	{
        int errorfound, localel;
        errorfound=false;
        if ( stack[s] > 0 )
                if ( cellspace[stack[s]].discsize<=i ) {
                        errorfound=true;
                        Error("disc size error");
                }
        if ( ! errorfound )	{
                localel=Getelement();
                cellspace[localel].next=stack[s];
                stack[s]=localel;
                cellspace[localel].discsize=i;
        }
}

void Init (int s, int n) {
        int discctr;
        Makenull(s);
        for ( discctr = n; discctr >= 1; discctr-- )
            Push(discctr,s);
}

int Pop (int s)	{
        int temp, temp1;
        if ( stack[s] > 0 ) {
                temp1 = cellspace[stack[s]].discsize;
                temp = cellspace[stack[s]].next;
                cellspace[stack[s]].next=freelist;
                freelist=stack[s];
                stack[s]=temp;
                return (temp1);
        }
        else
                Error("nothing to pop ");
        return 0;
}

void Move (int s1, int s2) {
        Push(Pop(s1),s2);
        movesdone=movesdone+1;
}

void tower(int i, int j, int k) {
        int other;
        if ( k==1 ) Move(i,j);
        else {
            other=6-i-j;
            tower(i,other,k-1);
            Move(i,j);
            tower(other,j,k-1);
        }
}

void Towers ()    { /* Towers */
    int i;
    for ( i=1; i <= maxcells; i++ ) cellspace[i].next=i-1;
    freelist=maxcells;
    Init(1,14);
    Makenull(2);
    Makenull(3);
    movesdone=0;
    tower(1,2,14);
    if ( movesdone != 16383 ) printf (" Error in Towers.\n");
         printf("%d\n", movesdone);
} /* Towers */


    /* Multiplies two real matrices. */

void rInitmatrix ( float m[rowsize+1][rowsize+1] ) {
        int temp, i, j;
        for ( i = 1; i <= rowsize; i++ )
            for ( j = 1; j <= rowsize; j++ ) {
                temp = Rand();
                        m[i][j] = (float)(temp - (temp/120)*120 - 60)/3;
        }
}

void rInnerproduct(float *result, float a[rowsize+1][rowsize+1],
                   float b[rowsize+1][rowsize+1], int row, int column) {
        /* computes the inner product of A[row,*] and B[*,column] */
        int i;
        *result = 0.0f;
        for (i = 1; i<=rowsize; i++) *result = *result+a[row][i]*b[i][column];
}

void Mm (int run)    {
    int i, j;
    Initrand();
    rInitmatrix (rma);
    rInitmatrix (rmb);
    for ( i = 1; i <= rowsize; i++ )
                for ( j = 1; j <= rowsize; j++ )
                        rInnerproduct(&rmr[i][j],rma,rmb,i,j);
        printf("%f\n", rmr[run + 1][run + 1]);
}

float
Cos (float x)
{
/* computes cos of x (x in radians) by an expansion */
  int i, factor;
  float result, power;

  result = 1.0f;
  factor = 1;
  power = x;
  for (i = 2; i <= 10; i++)
    {
      factor = factor * i;
      power = power * x;
      if ((i & 1) == 0)
        {
          if ((i & 3) == 0)
            result = result + power / factor;
          else
            result = result - power / factor;
        }
    }
  return (result);
}

int
Min0 (int arg1, int arg2)
{
  if (arg1 < arg2)
    return (arg1);
  else
    return (arg2);
}

void
Printcomplex (struct complex zarray[], int start, int finish, int increment)
{				/* removed unused arg1, arg2 arguments WR */
  int i;
  printf ("\n");

  i = start;
  do
    {
      printf ("  %15.3f%15.3f", zarray[i].rp, zarray[i].ip);
      i = i + increment;
      printf ("  %15.3f%15.3f", zarray[i].rp, zarray[i].ip);
      printf ("\n");
      i = i + increment;
    }
  while (i <= finish);

}

void
Uniform11 (int *iy, float *yfl)
{
  *iy = (4855 * *iy + 1731) & 8191;
  *yfl = *iy / 8192.0f;
}				/* uniform */

void
Exptab (int n, struct complex e[])
{				/* exptab */
  float theta, divisor, h[26];
  int i, j, k, l, m;

  theta = 3.1415926536f;
  divisor = 4.0f;
  for (i = 1; i <= 25; i++)
    {
      h[i] = 1 / (2 * Cos (theta / divisor));
      divisor = divisor + divisor;
    }

  m = n / 2;
  l = m / 2;
  j = 1;
  e[1].rp = 1.0f;
  e[1].ip = 0.0f;
  e[l + 1].rp = 0.0f;
  e[l + 1].ip = 1.0f;
  e[m + 1].rp = -1.0f;
  e[m + 1].ip = 0.0f;

  do
    {
      i = l / 2;
      k = i;

      do
        {
          e[k + 1].rp = h[j] * (e[k + i + 1].rp + e[k - i + 1].rp);
          e[k + 1].ip = h[j] * (e[k + i + 1].ip + e[k - i + 1].ip);
          k = k + l;
        }
      while (k <= m);

      j = Min0 (j + 1, 25);
      l = i;
    }
  while (l > 1);

}				/* exptab */

void
Fft (int n, struct complex z[], struct complex w[], struct complex e[],
     float sqrinv)
{
  int i, j, k, l, m, index;
  m = n / 2;
  l = 1;

  do
    {
      k = 0;
      j = l;
      i = 1;

      do
        {

          do
            {
              w[i + k].rp = z[i].rp + z[m + i].rp;
              w[i + k].ip = z[i].ip + z[m + i].ip;
              w[i + j].rp = e[k + 1].rp * (z[i].rp - z[i + m].rp)
                - e[k + 1].ip * (z[i].ip - z[i + m].ip);
              w[i + j].ip = e[k + 1].rp * (z[i].ip - z[i + m].ip)
                + e[k + 1].ip * (z[i].rp - z[i + m].rp);
              i = i + 1;
            }
          while (i <= j);

          k = j;
          j = k + l;
        }
      while (j <= m);

      /*z = w ; */ index = 1;
      do
        {
          z[index] = w[index];
          index = index + 1;
        }
      while (index <= n);
      l = l + l;
    }
  while (l <= m);

  for (i = 1; i <= n; i++)
    {
      z[i].rp = sqrinv * z[i].rp;
      z[i].ip = -sqrinv * z[i].ip;
    }

}

void
Oscar ()
{				/* oscar */
  int i;
  Exptab (fftsize, e);
  seed = 5767;
  for (i = 1; i <= fftsize; i++)
    {
      int s = seed;
      Uniform11 (&s, &zr);	/* typecast seed for 16 bit WR */
      seed = s;
      Uniform11 (&s, &zi);	/* typecast seed for 16 bit WR */
      seed = s;
      z[i].rp = 20.0f * zr - 10.0f;
      z[i].ip = 20.0f * zi - 10.0f;
    }
  for (i = 1; i <= 20; i++)
    {
      Fft (fftsize, z, w, e, 0.0625f);
    }
/* removed 1st 2 args 6, 99, unused by printcomplex WR */
  Printcomplex (z, 1, 256, 17);
}				/* oscar */


    /* Multiplies two integer matrices. */

void Initmatrix (int m[rowsize+1][rowsize+1]) {
        int temp, i, j;
        for ( i = 1; i <= rowsize; i++ )
            for ( j = 1; j <= rowsize; j++ ) {
            temp = Rand();
                m[i][j] = temp - (temp/120)*120 - 60;
        }
}

void Innerproduct( int *result, int a[rowsize+1][rowsize+1],
                   int b[rowsize+1][rowsize+1], int row, int column) {
        /* computes the inner product of A[row,*] and B[*,column] */
        int i;
        *result = 0;
        for (i = 1; i <= rowsize; i++ )
          *result = *result+a[row][i]*b[i][column];
}

void Intmm (int run) {
    int i, j;
    Initrand();
    Initmatrix (ima);
    Initmatrix (imb);
    for ( i = 1; i <= rowsize; i++ )
                for ( j = 1; j <= rowsize; j++ )
                        Innerproduct(&imr[i][j],ima,imb,i,j);
        printf("%d\n", imr[run + 1][run + 1]);
}

int main()
{
        int i;
        for (i = 0; i < 100; i++) Puzzle();
        for (i = 0; i < 100; i++) Perm();
    for (i = 0; i < 100; i++) Towers();
    for (i = 0; i < 10; i++) Oscar ();
        for (i = 0; i < 10; i++) Intmm(i);
#define float double
        for (i = 0; i < 10; i++) Mm(i);
        return 0;
}
