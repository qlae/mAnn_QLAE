#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include "lab3.h"

extern int** sudoku_board;
int* worker_validation;

int** read_board_from_file(char* filename){
    FILE *fp = NULL;
    int** board = NULL;

    int** board = (int**)malloc(sizeof(int*)*ROW_SIZE);
    for(int row = 0; row < ROW_SIZE; row++){
        board[row] = (int*)malloc(sizeof(int)*COL_SIZE);
    }

    fp = fopen(filename,"r");
    for(int d = 0; d < ROW_SIZE; d++) {
        for(int r = 0; r < COL_SIZE; r++) {
            fscanf(fp, "%d%*c", &board[d][r]);
        }
    }   
    fclose(fp);

    return board;
}

void * row_check(void * params)
{
    param_struct * param = (param_struct*) params;
    int verifyArr[9] = {0};
    int row = param -> starting_row;
    int col = param -> starting_col;

    for (int d = 0; d < 9; d++) {
        int num = sudoku_board[row][d];
        if (num < 1 || num > 9 || verifyArr[num - 1] == 1) {
            pthread_exit(NULL);
        } else {
            verifyArr[num - 1] = 1;        
        }
    }
    worker_validation[9 + row] = 1;
    pthread_exit(NULL);
}

void * column_check(void* params)
{
    param_struct *p = (param_struct*) params;
    int validation_array[9] = {0};
    int row = p -> starting_row;
    int col = p -> starting_col;

    for(int d = 0; d< 9; d++)
    {   
        int num = sudoku_board[d][col];
        if (num > 9 || num < 1 || validation_array[num - 1]==1)
        {
            pthread_exit(NULL);
        }
        else{
            validation_array[num - 1] = 1;
        }
    } 

    worker_validation[18 + col] = 1;
    pthread_exit(NULL);
}

void * subarray_check(void* params)
{
    param_struct *p = (param_struct*) params;
    int validation_array[9] = {0};
    int row = p -> starting_row;
    int col = p -> starting_col;

    for(int d = row; d < row + 3; d++){
        for(int r = col; r < col + 3; r++)
        {
            int num = sudoku_board[d][r];
            if (num > 9 || num < 1 || validation_array[num - 1]==1)
            {
                pthread_exit(NULL);
            }
            else{
                validation_array[num - 1] = 1;
            }
        }
    }
    worker_validation[row + col / 3] = 1;
    pthread_exit(NULL);
}

int is_board_valid(){
    pthread_t* tid;  /* the thread identifiers */
    pthread_attr_t attr;
    int t_index = 0;
    worker_validation = (int*) malloc(sizeof(int) * NUM_OF_THREADS);
    tid = (pthread_t*) malloc(sizeof(pthread_t) * NUM_OF_THREADS);

    for(int d = 0; d < ROW_SIZE; d++)
    {
        for(int r = 0; r < COL_SIZE; r++)
        {
            if(d % 3 == 0 && r % 3 == 0){
                param_struct * subarray_workthread = (param_struct*) malloc(sizeof(param_struct));
                subarray_workthread->starting_row = d;
                subarray_workthread->starting_col = r;
                pthread_create(&tid[t_index++], NULL, subarray_check, subarray_workthread);
            }
            if(d==0){
                param_struct * col_workthread = (param_struct*) malloc(sizeof(param_struct));
                col_workthread->starting_row = d;
                col_workthread->starting_col = r;
                pthread_create(&tid[t_index++], NULL, column_check,col_workthread);
            }
            if(r==0){
                param_struct * row_workthread = (param_struct*) malloc(sizeof(param_struct));
                row_workthread->starting_row = d;
                row_workthread->starting_col = r;
                pthread_create(&tid[t_index++], NULL, row_check, row_workthread);
            }
            
        }
    }
    
    for(int d = 0; d < NUM_OF_THREADS; d++)
    {
        pthread_join(tid[d], NULL);
        if(worker_validation[d] == 0){
            return 0;
        }
    }

    free(worker_validation);
    free(tid);
    return 1;
}
