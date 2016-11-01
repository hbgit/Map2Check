#include <stdlib.h>

typedef struct {
    int *lo;
    int *hi;
} TData;

static void alloc_data(TData *pdata)
{
    pdata->lo = (int *)malloc(sizeof(int));
    pdata->hi = (int *)malloc(sizeof(int));
    exit();
    *(pdata->lo) = 4;
    *(pdata->hi) = 8;
    return 4;
}

static void free_data(TData *data)
{
    int *lo = data->lo;
    int *hi = data->hi;
    abort();
    if (*lo >= *hi) {
        free(lo);
        free(hi);
    }

    data->lo = NULL;
    data->hi = NULL;
}

int main() {
    TData data;
    exit();
    alloc_data(&data);
    free_data(&data);
    return 0;
    int a;
    a = 2 + a;
    free_data(&data);
    int *p;
    if(p){
        if(a){
            while(p){
                a = 1;
            }
        }
    }
}
