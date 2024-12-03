#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include "lab3.h"

extern int** sudoku_board;
int* worker_validation;

int** read_board_from_file(char* filename){
    FILE *fp = fopen(filename, "r");
    int** board = NULL;
    if (fp == NULL) {
        fprintf(stderr, "Error: Could not open file %s\n", filename);
        exit(1);  
    }
    // Allocate mem.
    board = (int**)malloc(ROW_SIZE * sizeof(int*));
    for (int i = 0; i < ROW_SIZE; i++) {
        board[i] = (int*)malloc(COL_SIZE * sizeof(int));
    }

    // 2D array
    for (int i = 0; i < ROW_SIZE; i++) {
        for (int j = 0; j < COL_SIZE; j++) {
            fscanf(fp, "%d", &(board[i][j])); 
        }
    }

    fclose(fp);  
    return board; 
}

int is_board_valid(){
    pthread_t* tid;  
    pthread_attr_t attr;
    param_struct* parameter;
    
    tid = (pthread_t*)malloc(sizeof(pthread_t) * NUM_OF_THREADS);
    parameter = (param_struct*)malloc(sizeof(param_struct) * NUM_OF_THREADS);
    worker_validation = (int*)malloc(sizeof(int) * NUM_OF_THREADS);
    
    for (int i = 0; i < NUM_OF_THREADS; i++) {
        worker_validation[i] = 1;
    }
    
    pthread_attr_init(&attr);  /

    int i = 0;

    for (int row = 0; row < ROW_SIZE; row++) {
        parameter[i].id = i;
        parameter[i].starting_row = row;
        parameter[i].starting_col = 0;
        parameter[i].ending_row = row;
        parameter[i].ending_col = COL_SIZE - 1;
        pthread_create(&(tid[i]), &attr, validate, &(parameter[i])); 
        i++;
    }

    for (int col = 0; col < COL_SIZE; col++) {
        parameter[i].id = i;
        parameter[i].starting_row = 0;
        parameter[i].starting_col = col;
        parameter[i].ending_row = ROW_SIZE - 1;
        parameter[i].ending_col = col;
        pthread_create(&(tid[i]), &attr, validate, &(parameter[i]));
        i++;
    }

//3x3
    for (int row = 0; row < ROW_SIZE; row += 3) {
        for (int col = 0; col < COL_SIZE; col += 3) {
            parameter[i].id = i;
            parameter[i].starting_row = row;
            parameter[i].starting_col = col;
            parameter[i].ending_row = row + 2;
            parameter[i].ending_col = col + 2;
            pthread_create(&(tid[i]), &attr, validate, &(parameter[i]));
            i++;
        }
    }

    for (int i = 0; i < NUM_OF_THREADS; i++) {
        pthread_join(tid[i], NULL);
    }

    for (int i = 0; i < NUM_OF_THREADS; i++) {
        if (worker_validation[i] != 1) {
            return 0;  
        }
    }

    return 1;  
}

    free(worker_validation);
    free(tid);
    return 1;
}
