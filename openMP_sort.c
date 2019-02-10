#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>


void Parallel_bubble_sort(int* mas, int num_elem)
{
        for (int i = 0; i < num_elem; i++)
        {
                int tmp = 0;
                if (i % 2 == 0) //even iterations
                {
                        #pragma omp parallel for private(tmp) shared(mas)
                        for (int j = 0; j < num_elem; j += 2)
                                if (j < num_elem - 1)
                                        if(mas[j] > mas[j+1])
                                        {
                                                tmp = mas[j];
                                                mas[j] = mas[j+1];
                                                mas[j+1] = tmp;
                                        }
                }
                else //odd iterations
                {
                        #pragma omp parallel for private(tmp) shared(mas)
                        for (int j = 1; j < num_elem; j += 2)
                                if (j < num_elem - 1)
                                        if(mas[j] > mas[j+1])
                                        {
                                                tmp = mas[j];
                                                mas[j] = mas[j+1];
                                                mas[j+1] = tmp;
                                        }
                }
        }
}

int main(void)
{
    int a[20000]; //massive, we need to sort
    double time0;
    srand(time(NULL)); //provides correct randomising to fill massive
    for(int j = 0; j < 20000; j++)
    {
        a[j] = -10000 + rand() % 20000;
    } 
    omp_set_num_threads(1);
    time0 = omp_get_wtime();
    Parallel_bubble_sort(a, 20000); //as in sequental program
    time0 = omp_get_wtime() - time0;
    printf("1;%lf\n",time0);
    for(int i = 2; i <= 64; i = i + 2) //if you test on your computer change 64 to your number of cores
    {
        for(int j = 0; j < 20000; j++)
        {       
            a[j] = -10000 + rand() % 20000;
        }
        omp_set_num_threads(i);
        time0 = omp_get_wtime();
        Parallel_bubble_sort(a, 20000); //use of sorting itself
        time0 = omp_get_wtime() - time0;
        printf("%d;%lf\n",i,time0);
    }
    //for(int j = 0; j < 10000; j++)
        //printf("%d ", a[j]);
    return 0;
}
