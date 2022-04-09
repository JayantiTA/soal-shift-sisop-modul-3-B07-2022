#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <ctype.h>
#include <errno.h>

char *path_to_folder = "/home/oem/shift3/hartakarun/";

int total_files = 0;
char infos[1000][1000];
pthread_t tid[1000];

char* get_folder_name(char *path_to_file);
char* get_file_name(char *path_to_file);
void* create_directory(char *folder_name);
void* move_file(char *location, char *namaFile, char *ext);
void* process_file(void *location);
void* list_file_recursively(char *basePath);

int main ()
{
  list_file_recursively(path_to_folder);
  for (int i = 0; i < total_files; ++i) {
    pthread_create(&(tid[i]), NULL, process_file, (char*)infos[i]);
  }
  for(int i = 0; i < total_files; ++i){
      pthread_join(tid[i], NULL);
  }
}

char* get_folder_name(char *path_to_folder)
{
  char *token ;
  char *str = path_to_folder;
  char *ret;
  ret = strrchr(str, '/'); 
  if (ret != NULL) {
    if (ret[1] == '.') {
      token = "hidden";
      return token;
    }
  }

  token = strtok(str, ".");
  token = strtok(NULL, "");
  if (token == NULL) {
    return "unknown";
  }

  for (int i = 0; token[i]; ++i){
    token[i] = tolower(token[i]);
  }
  return token;
}

char* get_file_name(char *path_to_file)
{
  char *str = path_to_file;
  char *token = strtok(str, "/");
  int jumlah = 0;
  char *namaFile[10];
  while (token != NULL) {
    namaFile[jumlah] = token;
    token = strtok(NULL, "/");
    jumlah++;
  }
  return namaFile[jumlah - 1];
}

void* create_directory(char *folder_name)
{
  char path[100];
  strcpy(path, "hasil/");
  strcat(path, folder_name);
  mkdir(folder_name, 0777);
}

void* move_file(char *location, char *namaFile, char *ext)
{
  // int ret;
  char newname[2000];
  if (strcmp(ext, "unknown") == 0 || strcmp(ext, "hidden") == 0 ) {
      snprintf(newname, sizeof newname, "%s/%s", ext, namaFile);
  } else {
      snprintf(newname, sizeof newname, "%s/%s.%s", ext, namaFile, ext);
  }
  rename(location, newname);
  // printf("%s %s %s\n", location, namaFile, newname);
  // ret = rename(location, newname);
}

void* process_file(void *location)
{
    char *token, *fileName;
    char *fileLoc, fileLocCoba[2600], *location1;
    struct dirent *de;  // Pointer for directory entry
    
    // opendir() returns a pointer of DIR type. 
    fileLoc = (char *) location;
    location1 = (char *) location;
    DIR *dr = opendir(fileLoc);
  
    if (dr == NULL)  // opendir returns NULL if couldn't open directory
    {
        snprintf(fileLocCoba, sizeof fileLocCoba, "%s", fileLoc);
        // printf("FileLoc = %s\n", fileLoc);
        token = get_folder_name(location1);
        fileName = get_file_name(location);
        // printf("token = %s filename = %s\n", token, fileName);
        // printf("%s\n", fileName);
        // for(int i = 0; token[i]; i++){
        //    token[i] = tolower(token[i]);
        // }
        create_directory(token);
        move_file(fileLocCoba, fileName, token);
    }
}

void* list_file_recursively(char *basePath)
{
  char path[1000];
  struct dirent *dp;
  DIR *dir = opendir(basePath);

  if (!dir)
      return;
  while ((dp = readdir(dir)) != NULL)
  {
      if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
      {
          strcpy(path, basePath);
          strcat(path, "/");
          strcat(path, dp->d_name);
          char check[10000];
          snprintf(check, sizeof check, "%s", dp->d_name);
          if (check[0] == '.'){
              create_directory("hidden");
              char moves[10000];
              snprintf(moves, sizeof moves, "hidden/%s", dp->d_name);
              rename(path, moves);
          } else {
              // printf("%s\n", path);
              char loc[1000];
              strcpy(loc, path);
              // printf("%d\n", banyak);
              snprintf(infos[total_files], sizeof loc, "%s", loc);
              // printf("isi = %s", infos[banyak]);
              total_files += 1;
              // list_file_recursively(path);
          }
          // strcpy(path, basePath);
          // strcat(path, "/");
          // strcat(path, dp->d_name);
          // printf("path = %s\n", path);
          // pthread_create(&(tid[banyakFile]), NULL, process_file, (char*)path);
          // printf("processPath = %s\n", path);
          // printf("banyakFile = %d\n", banyakFile);
          // char loc[1000];
          // strcpy(loc, path);
          // // info[banyak] = loc;
          // // banyak++;
          // // printf("info %d = %s\n", banyakFile, info[2][1]);
          // //jagnan lupa join
          // // process_file(path);
          // snprintf(infos[banyak], sizeof loc, "%s", loc);
          // // info[banyak] = loc;
          // // if(banyak >0 ){
          // //     printf("info %d = %s\n", banyakFile, infos[banyak-1]);
          // // }
          // // printf("info %d = %s\n", banyakFile, infos[banyak]);
          // banyak = banyak + 1;
          // banyakFile++;
          list_file_recursively(path);
          // pthread_create(&(tid[banyakFile]), NULL, process_file, (char*)path);
          // process_file(path);
      }
  }
  closedir(dir);
}
