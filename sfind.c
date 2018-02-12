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
#include <sys/wait.h>
#include "sfind.h"
int main(int argc, char* argv[]){
   int mode = 0;
   char* substr = NULL;
   char** cmd = NULL;
   char* filename; 
   int i,j,k=0;
   int* index;
   int fd;
   if (argc<2){
      fprintf(stderr, "Usage: ./sfind filename [-name str] -print | -exec cmd\n");
      exit(-1);
   }
   fd  = open(".", O_RDONLY);
   filename = argv[1];
   for (i = 2; i< argc; i++){
      if (strcmp(argv[i],"-name")==0 && (i+1)<argc ){      /* -name switch */
         substr = argv[++i];
      }
      else if (strcmp(argv[i],"-print")==0){                /* print switch */
         if (mode==2){
            fprintf(stderr, "Usage: ./sfind filename [-name str] -print | -exec cmd\n");
            exit(-1);
         }
         mode = 1; 
      }
      else if (strcmp(argv[i], "-exec")==0){             /*exec switch */
         if (mode==1){
            fprintf(stderr, "Usage: ./sfind filename [-name str] -print | -exec cmd\n");
            exit(-1);
         }
         if ((i+2)>=argc){
            perror("no cmd or missing \\;");
            exit(-1);
         }
         cmd = checked_malloc (sizeof(char*) * argc);    /*store the commands in an array */
         index = checked_malloc(sizeof(int)*argc);
         mode = 2;
         j = 0;
         while (strcmp(";",argv[++i])!=0){
            if (i>=(argc-1)){
               perror("Missing \\; after -exec");
               exit(-1);
            }
            if (strcmp("{}", argv[i])==0){              /* store the {} locations in an array*/
               index[k++] = j;
            }
            cmd[j++] = argv[i];
         }
         index[k] = -1;
         cmd[j] = NULL;
         if (i!=(argc-1)){
            fprintf(stderr, "Usage: ./sfind filename [-name str] -print | -exec cmd\n");
            free(cmd);
            free(index);
            exit(-1); 
         }
      }
   }
   if (mode==0){
      fprintf(stderr, "Usage: ./sfind filename [-name str] -print | -exec cmd\n");
      exit(-1);
   }
   process(filename, "", mode, substr, cmd,index,fd); 
   if (cmd!=NULL){
      free(cmd);
      free(index);
   }
   close(fd);
   return 0;
}
/*void print_path(char* filename, char* path){    
   if (filename==NULL|| path ==NULL){
      perror("filename or path is NULL");
      exit(-1);
   }
   if (strlen(path)==0) printf("%s\n",filename);
   else printf("%s/%s\n",path,filename);
   return;
}*/
void exec(char* new_path, char* cmd[], int index[],int fd){
   int i = 0;
   int pid;
   int wd;
   if (cmd==NULL) return;
   wd = open(".",O_RDONLY);
   fchdir(fd);
   while (index[i]!=-1){
      cmd[index[i]] = new_path;
      i++;
   }
   if ((pid = fork())!=0){  /*parent*/
      if (pid<0) {
         perror("");
         exit(-1);
      }
      waitpid(pid,NULL,0);
      fchdir(wd);
      close(wd);
      return;
   }
   else{                /*child*/
      if (execvp(cmd[0], cmd)){
         perror("");
         exit(-1);
      }
   }
}
void process(char* filename, char* path, int mode, char* substr, char* cmd[], int* index, int fd){
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
   new_path = checked_malloc(sizeof(char)*(strlen(path)+strlen(filename)+2));
   if (strlen(path)==0) strcpy(new_path, filename);
   else{
      if (path[strlen(path)-1]=='/') path[strlen(path)-1]='\0';
      /*sprintf(new_path,"%s/%s", path, filename);*/
      strcpy(new_path, path);
      strcat(new_path,"/");
      strcat(new_path,filename);
   
   }
   if (mode==1 && substr==NULL) printf("%s\n",new_path);/*print the files*/
   if (S_ISLNK(buf.st_mode)){   /*skip the links*/
      free(new_path);
      return;
   }

   
   if (substr==NULL && mode ==2){
      exec(new_path, cmd, index, fd);
   }
   dirp = opendir(filename);
   if (dirp==NULL){/*not an directory, is a file*/
      if (substr!=NULL && strstr(filename, substr)!=NULL){
         if (mode==2) exec(new_path, cmd, index, fd);
         else if (mode==1) printf("%s\n", new_path);
      }
      free(new_path);
      return;
   }
   if (chdir(filename)!=0){ /*can't change into dir*/
      perror("Can't change into directory");
      free(new_path);
      return;
   }

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
      process(strArr[i],new_path, mode, substr, cmd, index, fd);
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

