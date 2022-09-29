#include "sparsemat.h"

#include <stdio.h>
#include <stdlib.h>





sp_tuples * load_tuples(char* input_file)
{
sp_tuples * tuples = (sp_tuples *) malloc(sizeof(struct sp_tuples));

    int rows, cols;
    tuples->nz = 0;
    int row, col;
    double value;
    FILE * fp;
    fp = fopen(input_file, "r");
    fscanf(fp, "%d %d", &rows, &cols);
    tuples->m = rows;
    tuples->n = cols;
    sp_tuples_node * new_node, * head;
    head = NULL;

    while(fscanf(fp, "%d %d %lf", &row, &col, &value) != EOF) {
        if (value != 0) {
            new_node = (sp_tuples_node *) malloc(sizeof(struct sp_tuples_node));
            new_node->row = row;
            new_node->col = col;
            new_node->value = value;
            new_node->next = head;
            tuples->nz++;
            head = new_node;
        }
    }
    fclose(fp);
    tuples->tuples_head = head;
    sp_tuples_node *target, *prev, *curr, *new_head, *currPrev;
    new_head = NULL;
    int count = 0;
    while (count<tuples->nz) {
        prev = NULL;
        currPrev = NULL;
        curr = head;
        target = curr;
        while(curr != NULL) {
            if (curr->row * tuples->m + curr->col > target->row * tuples->m + target->col) {
                prev   = currPrev;
                target = curr;
            }
            currPrev = curr;
            curr = curr->next;
        }
        if (target == head) {
            head = head->next;
        }
        if (prev != NULL ) {
            prev->next = target->next;
        }
        target->next = new_head;
        new_head = target;
        count++;
    }
    tuples->tuples_head = new_head;
    return tuples;
}



double gv_tuples(sp_tuples * mat_t,int row,int col)

{
sp_tuples_node * curr;
    curr = mat_t->tuples_head;
    while(curr->next != NULL) {
        if((curr->row == row) && (curr->col == col)) {
            return curr->value;
        }
        curr = curr->next;
    }
    return 0;
}



void set_tuples(sp_tuples * mat_t, int row, int col, double value)
{
    int wasFound = 0;
    sp_tuples_node * prevNode = NULL;
    sp_tuples_node * curr;
    curr = mat_t->tuples_head;
    while(curr->next != NULL) {
         if(curr->row >= row && curr->col >= col && !wasFound){
            sp_tuples_node * new_node = (sp_tuples_node *) malloc(sizeof(struct sp_tuples_node));
            new_node->row = row;
            new_node->col = col;
            new_node->value = value;
            new_node->next = curr;
            prevNode->next = new_node;
        }
        if((curr->row == row) && (curr->col == col)) {
            wasFound = 1;
            if(value == 0) {
                prevNode->next = curr->next;
                free(curr);
            } else {
                curr->value = value;
            }
        }
        prevNode = curr;
        curr = curr->next;
    }
    return;
}



void save_tuples(char * file_name, sp_tuples * mat_t)
{
if(mat_t != NULL) {
        FILE * fp;
        fp = fopen(file_name, "w");
        fprintf(fp, "%d %d\n", mat_t->m, mat_t->n);
        sp_tuples_node * curr_node = mat_t->tuples_head;
        while(curr_node != NULL) {
            int row = curr_node->row;
            int col = curr_node->col;
            double value = curr_node->value;
            fprintf(fp, "%d %d %lf\n", row, col, value);
            curr_node = curr_node->next;
        }
        fclose(fp);
    }
    return;
}



sp_tuples * add_tuples(sp_tuples * matA, sp_tuples * matB){
if(matA->m != matB->m || matA->n != matB->n || matA == NULL || matB == NULL) {
        return NULL;
    }
    sp_tuples * matC = (sp_tuples *) malloc(sizeof(struct sp_tuples));
    matC->m = matA->m;
    matC->n = matA->n;
    matC->nz = 0;
    sp_tuples_node * curr_a_node = matA->tuples_head;
    sp_tuples_node * curr_b_node = matB->tuples_head;
    int isFirst = 1;
    while(curr_a_node != NULL || curr_b_node != NULL) {
        sp_tuples_node * new_node = (sp_tuples_node *) malloc(sizeof(struct sp_tuples_node));
        sp_tuples_node * prev_node;
        if(curr_a_node == NULL) {
            new_node->row = curr_b_node->row;
            new_node->col = curr_b_node->col;
            new_node->value = curr_b_node->value;
            new_node->next = NULL;
            if(prev_node != NULL) {
                prev_node->next = new_node;
            }
            prev_node = new_node;
            curr_b_node = curr_b_node->next;
        } else if (curr_b_node == NULL) {
            new_node->row = curr_a_node->row;
            new_node->col = curr_a_node->col;
            new_node->value = curr_a_node->value;
            new_node->next = NULL;
            if(prev_node != NULL) {
                prev_node->next = new_node;
            }
            prev_node = new_node;
            curr_a_node = curr_a_node->next;
        } else if(curr_a_node->row < curr_b_node->row) {
            new_node->row = curr_a_node->row;
            new_node->col = curr_a_node->col;
            new_node->value = curr_a_node->value;
            new_node->next = NULL;
            if(prev_node != NULL) {
                prev_node->next = new_node;
            }
            prev_node = new_node;
            curr_a_node = curr_a_node->next;
        } else if (curr_b_node->row < curr_a_node->row) {
            new_node->row = curr_b_node->row;
            new_node->col = curr_b_node->col;
            new_node->value = curr_b_node->value;
            new_node->next = NULL;
            if(prev_node != NULL) {
                prev_node->next = new_node;
            }
            prev_node = new_node;
            curr_b_node = curr_b_node->next;
        } else {
            if(curr_a_node->col < curr_b_node->col) {
                new_node->row = curr_a_node->row;
                new_node->col = curr_a_node->col;
                new_node->value = curr_a_node->value;
                new_node->next = NULL;
                if(prev_node != NULL) {
                    prev_node->next = new_node;
                }
                prev_node = new_node;
                curr_a_node = curr_a_node->next;
            } else if(curr_b_node->col < curr_a_node->col) {
                new_node->row = curr_b_node->row;
                new_node->col = curr_b_node->col;
                new_node->value = curr_b_node->value;
                new_node->next = NULL;
                if(prev_node != NULL) {
                    prev_node->next = new_node;
                }
                prev_node = new_node;
                curr_b_node = curr_b_node->next;
            } else {
                new_node->row = curr_a_node->row;
                new_node->col = curr_a_node->col;
                new_node->value = curr_a_node->value + curr_b_node->value;
                new_node->next = NULL;
                if(prev_node != NULL) {
                    prev_node->next = new_node;
                }
                prev_node = new_node;
                curr_a_node = curr_a_node->next;
                curr_b_node = curr_b_node->next;
            }
        }
        matC->nz++;
        if(isFirst) {
            matC->tuples_head = new_node;
            isFirst = 0;
        }
    }

    return matC;
}

double ** ListToArray(sp_tuples * tuples) {
    int nz, row, col, i;
    int rows = tuples->m;
    int cols = tuples->n;
    double** array;
    array = (double**) malloc(rows*sizeof(double*));
    for (i = 0; i < rows; i++)
       array[i] = (double*) malloc(cols*sizeof(double));
    sp_tuples_node *curr = tuples->tuples_head;
    for (row = 0; row < tuples->m; row++) {
        for (col = 0; col < tuples->n; col++) {
            array[row][col] = 0;
        }
    }
    for (nz = 0; nz < tuples->nz; nz++) {
        array[curr->row][curr->col] = curr->value;
        curr = curr->next;
    }

    return array;
       
}

sp_tuples * ArrayToList(double ** array, int rows, int cols) {
    sp_tuples * tuples = (sp_tuples *) malloc(sizeof(struct sp_tuples));
    sp_tuples_node *prev, *new_node;
    tuples->m = rows;
    tuples->n = cols;
    tuples->nz = 0;
    tuples->tuples_head = NULL;
    int isFirst=1;
    int row, col;
    for (row = 0; row < tuples->m; row++) {
        for (col = 0; col < tuples->n; col++) {
            if (array[row][col] != 0) {
                tuples->nz++;
                new_node = (sp_tuples_node *) malloc(sizeof(struct sp_tuples_node));
                new_node->row = row;
                new_node->col = col;
                new_node->value = array[row][col];
                new_node->next  = NULL;
                if (isFirst) {
                    tuples->tuples_head = new_node;
                    isFirst = 0;
                } else {
                    prev->next = new_node;
                }
                prev = new_node;
            }
        }
    }
    return tuples;
}

sp_tuples * mult_tuples(sp_tuples * matA, sp_tuples * matB){ 
    if(matA->n != matB->m || matA == NULL || matB == NULL) {
        return NULL;
    }
    sp_tuples * matC = (sp_tuples *) malloc(sizeof(struct sp_tuples));
    matC->m = matA->m;
    matC->n = matB->n;
    matC->nz = 0;
    double sum;
    int i, j, k;
    double** arrayA;
    arrayA = (double**) malloc(matA->m*sizeof(double*));
    for (i = 0; i < matA->m; i++)
       arrayA[i] = (double*) malloc(matA->n*sizeof(double));
    arrayA = ListToArray(matA);
    double** arrayB;
    arrayB = (double**) malloc(matB->m*sizeof(double*));
    for (i = 0; i < matB->m; i++)
       arrayB[i] = (double*) malloc(matB->n*sizeof(double));
    arrayB = ListToArray(matB);
    double** arrayC;
    arrayC = (double**) malloc(matC->m*sizeof(double*));
    for (i = 0; i < matC->m; i++)
       arrayC[i] = (double*) malloc(matC->n*sizeof(double));
    for (i = 0; i <matC->m; i++ ) {
        for (j = 0; j<matC->n; j++) {
            arrayC[i][j] = 0;
        }
    }
    for (i = 0; i < matA->m; i++) {
        for (j = 0; j < matB->n; j++) {
            sum = 0;
            for (k = 0; k < matA->n; k++) {
                sum += arrayA[i][k] * arrayB[k][j];
            }
            arrayC[i][j] = sum;

            if (sum !=0) {
                matC->nz++;
            }
        }
    }
    matC = ArrayToList(arrayC, matC->m, matC->n);
    free(arrayA);
    free(arrayB);
    return matC;

}


	
void destroy_tuples(sp_tuples * mat_t){
	
    return;
}  






