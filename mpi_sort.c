#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

int* merge (int* mas1, int l1, int* mas2, int l2)
{
 int i, j, count;
 int* res;

 res = (int *)malloc((l1 + l2) * sizeof(int));;
 i = 0;
 j = 0;
 count = 0;
//вставляем каждый раз меньший из крайних
 while (i < l1 && j < l2)
  if (mas1[i] < mas2[j]) 
  {
   res[count] = mas1[i];
   i++;
   count++;
  } 
  else 
  {
   res[count] = mas2[j];
   j++;
   count++;
  }
//если кончился один из массивов
 if (i == l1)
  while (j < l2) 
  {
   res[count] = mas2[j];
   j++;
   count++;
  }
 if(j == l2)
  while (i < l1) 
  {
   res[count] = mas1[i];
   i++;
   count++;
  }
 return res;
}

void swap (int* v, int i, int j)
{
	int t;
	t = v[i];
	v[i] = v[j];
	v[j] = t;
}

//обычныая сортировка пузырьком
void sort (int* v, int n)
{
	int i, j;
	for (i = n - 2; i >= 0; i--)
		for (j = 0; j <= i; j++)
			if (v[j] > v[j + 1])
				swap (v, j, j + 1);
}

int main (int argc, char **argv)
{
	int n = 10000; //размер массива
	int default_mas[n]; //сортируемый маccив
	int rank, num_p, s, diff, status, addlen;
	int* result_mas;
	int* add;
	MPI_Status status;
    double time0;
    srand(time(NULL)); //рандомизация как в прошлом задании
    for(int j = 0; j < n; j++)
    {
        default_mas[j] = -10000 + rand() % 20000;
    } 

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank); //номер процесса
	MPI_Comm_size(MPI_COMM_WORLD, &num_p); //всего процессов

	if (rank == 0)
	{
		time0 = MPI_Wtime();
		s = n / num_p;
		MPI_Bcast(&s, 1, MPI_INT, 0, MPI_COMM_WORLD); //0й рассылает всем размер доли каждого процесса
		result_mas = (int *)malloc(s * sizeof(int));
		MPI_Scatter(default_mas, s, MPI_INT, result_mas, s, MPI_INT, 0, MPI_COMM_WORLD); //0й рассылает iому процессу iую часть исходного массива
		sort(result_mas,s); //сортировка своей части
	}
	else
	{
		MPI_Bcast(&s, 1, MPI_INT, 0, MPI_COMM_WORLD);
		result_mas = (int *)malloc(s * sizeof(int));
		MPI_Scatter(default_mas, s, MPI_INT, result_mas, s, MPI_INT, 0, MPI_COMM_WORLD);
		sort(result_mas, s);
	}

	diff = 1;
	while (diff < num_p)
	{
		if (rank % (2 * diff) == 0) //если процесс "главный" в паре
		{
			if (rank + diff < num_p)
			{
				MPI_Recv(&addlen, 1, MPI_INT, rank + diff, 0, MPI_COMM_WORLD, &status); 
				add = (int *)malloc(addlen * sizeof(int));
				MPI_Recv(add, addlen, MPI_INT, rank+diff, 0, MPI_COMM_WORLD, &status); //принимаем от соседа результат его работы
				result_mas = merge(result_mas, s, add, addlen); //слияние полученной отсортированной части со своей отсортированной частью
				s = s + addlen;
			}
		}
		else
		{
			int neighbour = rank - diff; 
			MPI_Send(&s, 1, MPI_INT, neighbour, 0, MPI_COMM_WORLD); 
			MPI_Send(result_mas, s, MPI_INT, neighbour, 0, MPI_COMM_WORLD); //отправляем левому соседу свой массив
			break; //каждый раз число процессов уменьшается в 2 раза
		}
		diff = diff * 2; //проход по степеням двойки: деление процессов на пары соседние, потом через 1 и т.д.
	}
	
	if (rank == 0) //теперь весь отсортированный массив в первом в result_mas
	{
		time0 = MPI_Wtime() - time0;
		printf("%d;%lf\n", num_p, time0);
		//for(int j = 0; j < n; j++)
			//printf("%d ", result_mas[j]);
	}
    return 0;
}