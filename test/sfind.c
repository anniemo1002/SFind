/* Jiaqing Mo*/
/*CPE357-09*/
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include "sfind.h"
int main(int argc, char* argv[]){
   int mode = 0;
   char* substr = NULL;
   char** cmd = NULL;
   char* filename; 
   int i,j;
   if (argc<2){
      perror("Usage: ./sfind filename [-name str] -print | -exec cmd");
      exit(-1);
   }
   filename = argv[1];
   if ((strcmp(";",argv[argc-1]))!=0){
      perror("Missing \\; at the end");
      exit(-1);
   }
   for (i = 2; i< (argc-1); i++){
      if (strcmp(argv[i],"-name")==0 && (++i)<(argc-1) ){
         substr = argv[i];
      }
      else if (strcmp(argv[i],"-print")==0){
         mode = 1;
      }
      else if (strcmp(argv[i], "-exec")==0){
         if (i>=(argc-2)){
            perror("no cmd");
            exit(-1);
         }
         cmd = malloc (sizeof(char*) * argc);
         mode = 2;
         j = 0;
         while (++i < (argc-1) && argv[i][0]!='-'){
            cmd[j++] = argv[i];
         }
         cmd[j] = NULL;
      }
   }
   if (mode==0) mode = 1;
   process(filename, "", mode, substr, cmd); 
   return 0;
}
void print_path(char* filename, char* path){
   if (filename==NULL|| path ==NULL){
      perror("filename or path is NULL");
      exit(-1);
   }
   if (strlen(path)==0) printf("%s\n",filename);
   else printf("%s/%s\n",path,filename);
   return;
}
void exec(char* filename, char* path, char* cmd[]){
   return;
}
void process(char* filename, char* path, int mode, char* substr, char* cmd[]){
   struct stat buf;
   char* new_path;
   char** strArr;
   int i;
   int size;
   DIR* dirp;
   struct dirent* dp;
   if (filename ==NULL||strlen(filename)==0){
      perror("");
      exit(-1);
   }
   if (lstat(filename, &buf)){
      perror("filename");
      exit(-1);
   }
   if (mode==1 && substr==NULL) print_path(filename, path);/*print the files*/
   if (S_ISLNK(buf.st_mode)){   /*skip the links*/
      return;
   }
   dirp = opendir(filename);
   if (dirp==NULL){/*not an directory, is a file*/
      if (substr==NULL && mode ==2){
         exec(filename, path, cmd);
      }
      else if (substr!=NULL && strstr(filename, substr)!=NULL){
         if (mode==2) exec(filename, path, cmd);
         else if (mode==1) print_path(filename, path);
      }
      return;
   }
   if (chdir(filename)!=0){ /*can't change into dir*/
      perror("Can't change into directory");
      return;
   }
   new_path = checked_malloc(sizeof(char)*(strlen(path)+strlen(filename)+2));
   if (strlen(path)==0) strcpy(new_path, filename);
   else sprintf(new_path,"%s/%s", path, filename);
   strArr = checked_malloc(sizeof(char*)*10);
   size =10;
   i = 0;
   while ((dp = readdir(dirp))!=NULL){
      if ((strcmp(dp->d_name,".")==0) ||strcmp(dp->d_name,"..")==0 ) continue;
      if (i>=(size-2)){ /*enlarge*/
         strArr = checked_realloc(strArr,(size+10)*sizeof(char*));
         size+=10;
      }
      strArr[i] = checked_malloc((strlen(dp->d_name)+1)*sizeof(char));
      strcpy(strArr[i],dp->d_name);
      i++;
   }
   strArr[i]=NULL;
   closedir(dirp);
   sort(strArr,i);
   i = 0;
   while(strArr[i]){
      process(strArr[i],new_path, mode, substr, cmd);
      i++;
   }
   free(new_path);
   chdir("..");
   freeArr(strArr);
   return;
}

void* checked_malloc(size_t size){
   void* p;
   p = malloc(size);
   if (p==NULL){
      perror("");
      exit(-1);
   }
   return p;
}
void* checked_realloc(void* p, size_t size){
   void* temp = realloc(p, size);
   if (temp==NULL){
      perror("");
      exit(-1);
   }
   return temp;
}
void sort(char* strArr[], int size){
   qsort(strArr,size,sizeof(char*),cstring_cmp);
   return;
}
int cstring_cmp(const void*a, const void*b){
   const char **ia = (const char**)a;
   const char **ib = (const char**)b;
   return strcmp(*ia, *ib);
}

void freeArr(char* strArr[]){
   int i = 0;
   if(strArr==NULL) return;
   while (strArr[i]!=NULL){
      free(strArr[i]);
      i++;
   }
   free(strArr);
   return;
}

