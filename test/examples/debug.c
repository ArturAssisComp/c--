/* This file may be used for debugging purposes */
void minloc(int i, int b, int c) {;}
void sort( int a[], int low, int high)
{ 
    int i; int k;
    i = low;
    while (i < high - 1)
    { 
        int t;
        k = minloc(a,i,high);
        t = a[k];
        a[k] = a[i];
        a[i] = t;
        i = i + 1;
    }
}
