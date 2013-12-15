/**
 * @author Vaibhav Bhembre
 * @version 2013/12/14
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define HEAP_DEFAULTSIZE 100

typedef struct heap {
    int *heaparr;
    int capacity;
    int size;
} heap_t;

int compare(int a, int b) {
    if (a < b)
        return -1;
    else if (a > b)
        return 1;
    return 0;
}

void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

void heapify_down(heap_t *heap, int pos) {
    if (pos > heap->size) {
        return;
    }

    int leftpos = pos << 1, rightpos = (pos << 1) + 1;
    if (leftpos > heap->size) {
        return;
    }

    int minpos = leftpos;
    if (rightpos <= heap->size && compare(heap->heaparr[leftpos], heap->heaparr[rightpos]) > 0) {
        minpos = rightpos;
    }

    if (compare(heap->heaparr[pos], heap->heaparr[minpos]) > 0) {
        swap(&heap->heaparr[pos], &heap->heaparr[minpos]);
        heapify_down(heap, minpos);
    }
}

void heapify_up(heap_t *heap, int pos) {
    int parentpos = pos >> 1;
    if (parentpos < 1) {
        return;
    }

    if (compare(heap->heaparr[pos], heap->heaparr[parentpos]) < 0) {
        swap(&heap->heaparr[pos], &heap->heaparr[parentpos]);
        heapify_up(heap, parentpos);
    }
}

void heap_push(heap_t *heap, int value) {
    heap->heaparr[++heap->size] = value;
    heapify_up(heap, heap->size);
}

int heap_poll(heap_t *heap) {
    if (heap->size < 1) {
        return -1;
    }

    swap(&heap->heaparr[1], &heap->heaparr[heap->size]);
    int root = heap->heaparr[heap->size];
    heap->heaparr[heap->size--] = -1;

    heapify_down(heap, 1);
    return root;
}

heap_t *init_heap() {
    heap_t *heap = malloc(sizeof (*heap));

    heap->capacity = HEAP_DEFAULTSIZE;
    heap->size = 0;
    heap->heaparr = malloc(heap->capacity * sizeof (int));

    memset(heap->heaparr, -1, heap->capacity * sizeof (int));

    return heap;
}

void free_heap(heap_t *heap) {
    if (heap->capacity > 0) {
        free(heap->heaparr);
    }
    free(heap);
}

void print_heap_arr(heap_t *heap) {
    int i = 1;
    for (; i <= heap->size; i++) {
        printf("%d\t", heap->heaparr[i]);
    }
    printf("\n");
}

int main(int argc, char *argv[]) {
    heap_t *heap = init_heap();
    heap_push(heap, 3);
    heap_push(heap, 1);
    heap_push(heap, 5);

    printf("%d\n", heap_poll(heap));
    printf("%d\n", heap_poll(heap));
    printf("%d\n", heap_poll(heap));
    printf("%d\n", heap_poll(heap));

    heap_push(heap, 3);
    heap_push(heap, 1);
    heap_push(heap, 5);
    heap_push(heap, 8);
    heap_push(heap, 4);
    heap_push(heap, 105);

    printf("%d\n", heap_poll(heap));
    printf("%d\n", heap_poll(heap));
    printf("%d\n", heap_poll(heap));
    printf("%d\n", heap_poll(heap));
    printf("%d\n", heap_poll(heap));
    printf("%d\n", heap_poll(heap));

    free_heap(heap);
    return 0;
}

