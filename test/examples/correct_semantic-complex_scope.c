
int global_var;
int global_arr[100];

int input(void)
{
    int i;
    i = 4;
    {
    int i;
    i = 100;
    }
    i = 101;
    return i;
}
int main(void)
{
    int i;
    {
        while(i){;}
        while(1)
        {
            if(i == 1) return i; else return i + 1;
        }
    }
    {
        {
            int i;
            i = 1;
            if(i) input();
        }
    }
}
