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
int status = 0;
char infos[1000][1000];
pthread_t tid[1000];

char* get_folder_name(char *path_to_file);
char* get_file_name(char *path_to_file);
void* create_directory(char *folder_name);
void* move_file(char *file_buffer, char *file_name, char *folder_name);
void* process_file(void *path_to_file);
void* list_file_recursively(char *base_path);

int main()
{
  list_file_recursively(path_to_folder);
  for (int i = 0; i < total_files; ++i) {
    pthread_create(&(tid[i]), NULL, process_file, (char*) infos[i]);
  }
  
  for (int i = 0; i <= total_files; ++i){
    pthread_join(tid[i], NULL);
  }
  
  exit(EXIT_SUCCESS);
}

char* get_folder_name(char *path_to_folder)
{
  char *folder_name ;
  char *str = path_to_folder;
  char *ret;
  ret = strrchr(str, '/'); 
  if (ret != NULL) {
    if (ret[1] == '.') {
      return "hidden";
    }
  }

  folder_name = strtok(str, ".");
  folder_name = strtok(NULL, "");
  if (folder_name == NULL) {
    return "unknown";
  }

  for (int i = 0; folder_name[i]; ++i){
    folder_name[i] = tolower(folder_name[i]);
  }
  return folder_name;
}

char* get_file_name(char *path_to_file)
{
  char *str = path_to_file;
  char *temp = strtok(str, "/");
  int count = 0;
  char *file_name[10];
  while (temp != NULL) {
    file_name[count] = temp;
    temp = strtok(NULL, "/");
    count++;
  }
  return file_name[count - 1];
}

void* create_directory(char *folder_name)
{
  mkdir(folder_name, 0777);
}

void* move_file(char *file_buffer, char *file_name, char *folder_name)
{
  char newname[2000];
  if (strcmp(folder_name, "unknown") == 0 || strcmp(folder_name, "hidden") == 0 ) {
    snprintf(newname, sizeof newname, "%s/%s", folder_name, file_name);
  } else {
    snprintf(newname, sizeof newname, "%s/%s.%s", folder_name, file_name, folder_name);
  }
  rename(file_buffer, newname);
}

void* process_file(void *path_to_file)
{
  char *folder_name, *file_name;
  char *file_path, file_buffer[10000];

  file_path = (char *) path_to_file;
  DIR *dir_path = opendir(file_path);

  if (dir_path == NULL)
  {
    snprintf(file_buffer, sizeof file_buffer, "%s", file_path);
    folder_name = get_folder_name(path_to_file);
    file_name = get_file_name(path_to_file);
    create_directory(folder_name);
    move_file(file_buffer, file_name, folder_name);
  }
}

void* list_file_recursively(char *base_path)
{
  char path[1000];
  struct dirent *dp;
  DIR *dir = opendir(base_path);

  if (!dir) return;

  while ((dp = readdir(dir)) != NULL) {
    if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0) {
      strcpy(path, base_path);
      strcat(path, "/");
      strcat(path, dp->d_name);
      char check[10000];
      snprintf(check, sizeof check, "%s", dp->d_name);
      if (check[0] == '.') {
        create_directory("hidden");
        char moves[10000];
        snprintf(moves, sizeof moves, "hidden/%s", dp->d_name);
        rename(path, moves);
      } else {
        char loc[1000];
        strcpy(loc, path);
        snprintf(infos[total_files], sizeof loc, "%s", loc);
        total_files += 1;
      }
      list_file_recursively(path);
    }
  }
  closedir(dir);
}
