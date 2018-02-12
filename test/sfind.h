
void freeArr(char* strArr[]);
int cstring_cmp(const void*a, const void*b);
void sort(char* strArr[], int size);
void* checked_realloc(void* p, size_t size);
void* checked_malloc(size_t size);
void process(char* filename, char* path, int mode, char* substr, char* cmd[]);
void exec(char* filename, char* path, char* cmd[]);
void print_path(char* filename, char* path);

