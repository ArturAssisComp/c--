/* Um programa para calcular o mdc
Segundo o algoritmo de Euclides. */
void input(void){;}
void output(int i){;}
int gcd (int u, int v)
{ 
    if (v == 0) return u;
    else return gcd(v, u-u/v*v);
    /* u-u/v*v == u mod v */
}

void main(void)
{ 
    int x; int y;
    x = input();
    y = input();
    output(gcd(x,y));
}
