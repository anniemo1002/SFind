#ifndef SFIND_H
#define SFIND_H

void freeArr(char* strArr[]);
int cstring_cmp(const void*a, const void*b);
void sort(char* strArr[], int size);
void* checked_realloc(void* p, size_t size);
void* checked_malloc(size_t size);
void process(char* filename, char* path, int mode, char* substr, char* cmd[], int* index, int fd);
void exec(char* new_path, char* cmd[], int* index, int fd);
/*void print_path(char* filename, char* path);*/

#endif
