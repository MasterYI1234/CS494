#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>


int min(int, int);

typedef struct Node {
    int source;
    int dest;
    int weight;
    struct Node* next;
} Node_t;

void floyds(int* S, const int size)
{
    int i, j, k;
    for (k = 0; k < size; k++)
    {
        for (i = 0; i < size; i++)
        {
            for (j = 0; j < size; j++)
            {
                if (i == j)
                    S[i * size + j] = 0;
                else
                {
                    S[i * size + j] = min(S[i * size + j], S[i * size + k] +
                        S[k * size + j]);
                }
            }
        }
    }
}

int min(int a, int b)
{
    if (a < b)
        return a;
    else
        return b;
}


void printMatrix(int* mat, const int size)
{
    int i, j;
    for (i = 0; i < size; i++)
    {
        for (j = 0; j < size; j++)
        {
            if (j != size - 1)
                printf("%d ", mat[i * size + j]);
            else
                printf("%d", mat[i * size + j]);

        }
        printf("\n");
    }
}

long findMin(int N, int from[], int to[], int weight[], int charges)
{
    return -9;
}

int main(int argc, char** argv)
{

    int size = 0;
    int charges = 0;
    char read_ch = fgetc(stdin);
    ungetc(read_ch, stdin);
    int row_num = 1, edges_count = 0;
    int val = 0;
    scanf("%d", &size);
    Node_t* list  = NULL, *last = NULL;
    while (read_ch != EOF )
    {
        if (read_ch == 13)
        {
            row_num++;
            last = list;
        }

        scanf("%d", &val);
        if (row_num == 1)
        {
            if (list == NULL)
            {
                list = last = (Node_t*)malloc(sizeof(Node_t));
                list->next = NULL;
                list->source = val;
            }
            else
            {
                Node_t* nod = (Node_t*)malloc(sizeof(Node_t));
                nod->next = NULL;
                nod->source = val;
                last->next = nod;
                last = nod;
            }

            edges_count++;
        }
        else if (row_num == 2)
        {
            last->dest = val;
            last = last->next;
        }
        else if (row_num == 3)
        {
            last->weight = val;
            last = last->next;
        }
        else
        {
            charges = val;
            break;
        }
        read_ch = fgetc(stdin);
        ungetc(read_ch, stdin);
    }
    int* L = (int*)malloc(size * size * sizeof(int));
    int* S = (int*)malloc(size * size * sizeof(int));

    int* sources = (int*)malloc(edges_count * sizeof(int));
    int* dests = (int*)malloc(edges_count * sizeof(int));
    int* weights = (int*)malloc(edges_count* sizeof(int));
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            L[i * size + j] = S[i * size + j] = 6000000;
        }
    }

    last = list;
    int k = 0;
    while (last != NULL)
    {
        sources[k] = last->source - 1;
        dests[k] = last->dest - 1;
        weights[k] = last->weight;
        if (L[(last->source - 1) * size + (last->dest - 1)] == 6000000 || L[(last->source - 1) * size + (last->dest - 1)] > last->weight)
        {
            L[(last->source - 1) * size + (last->dest - 1)] = last->weight;
            S[(last->source - 1) * size + (last->dest - 1)] = last->weight;
        }

        k++;

        last = last->next;
    }

    floyds(S, size);

    if (argc > 1)
    {
        if (strcmp(argv[1], "P0") == 0)
            printMatrix(L, size);
        else if(strcmp(argv[1], "P1") == 0)
            printMatrix(S, size);
    }

    printf("%ld\n", findMin(size, sources, dests, weights, charges));


    last = list;
    while (last != NULL)
    {
        Node_t* del = last;
        last = last->next;
        free(del);
    }

    free(L);
    free(sources);
    free(dests);
    free(weights);

    return 0;
}
