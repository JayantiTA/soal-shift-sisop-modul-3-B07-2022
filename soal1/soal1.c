#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>

const char *link1 = "https://docs.google.com/uc?export=download&id=1_djk0z-cx8bgISFsMiUaXzty2cT4VZp1";
const char *link2 = "https://docs.google.com/uc?export=download&id=1jR67_JAyozZPs2oYtYqEZxeBLrs-k3dt";

void* download_and_unzip(char* folder_name);
void* decode_base64(char* folder_name, char* file_name);
void* move_to_folder(char* filename);

void *print_message_function( void *ptr );

int main()
{
    pthread_t thread1, thread2;//inisialisasi awal
    const char *file_name1 = "music";
    const char *file_name2 = "quote";
    int  iret1, iret2;

    iret1 = pthread_create( &thread1, NULL, download_and_unzip, (void*) file_name1); //membuat thread pertama
    if (iret1)
    {
        fprintf(stderr,"Error - pthread_create() return code: %d\n",iret1);
        exit(EXIT_FAILURE);
    }

    iret2 = pthread_create( &thread2, NULL, download_and_unzip, (void*) file_name2);//membuat thread kedua
    if (iret2)
    {
        fprintf(stderr,"Error - pthread_create() return code: %d\n",iret2);
        exit(EXIT_FAILURE);
    }

    // printf("pthread_create() for thread 1 returns: %d\n",iret1);
    // printf("pthread_create() for thread 2 returns: %d\n",iret2);

    pthread_join( thread1, NULL);
    pthread_join( thread2, NULL); 

    exit(EXIT_SUCCESS);
}

void *print_message_function( void *ptr )
{
    char *message;
    message = (char *) ptr;
    printf("%s \n", message);

    for(int i=0;i<10;i++){
        printf("%s %d \n", message, i);
    }
}

void* download_and_unzip(char* folder_name)
{
  pid_t child_id_1, child_id_2, child_id_3;
  child_id_1 = fork();
  int status1;

  if (child_id_1 == 0) {
    child_id_2 = fork();
    int status2;
    
    if (child_id_2 == 0) {
      if (strcmp(folder_name, "music") == 0) {
        char *argv[] = {"wget", "--no-check-certificate", link1, "-O", "music.zip", NULL};
        execv("/usr/bin/wget", argv);
      } else if (strcmp(folder_name, "quote") == 0) {
        char *argv[] = {"wget", "--no-check-certificate", link2, "-O", "quote.zip", NULL};
        execv("/usr/bin/wget", argv);
      }
    } else {
      while ((wait(&status2)) > 0);
      char *argv[] = {"unzip", folder_name, "-d", folder_name, NULL};
      execv("/usr/bin/unzip", argv);
    }
  } else {
    while ((wait(&status1)) > 0);
    child_id_3 = fork();
    int status3;

    if (child_id_3 == 0) {
      char *file_name = malloc(sizeof(char) * 100);
      strcpy(file_name, folder_name);
      strcat(file_name, ".zip");
      char *argv[] = {"rm", file_name};
      execv("/usr/bin/rm", argv);
    } else {
      while ((wait(&status3)) > 0);
      // printf("success");
      DIR *dir_path;
      struct dirent *dir;
      dir_path = opendir(folder_name);
      while((dir = readdir(dir_path))) {
        if (strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0) {
          decode_base64(folder_name, dir->d_name);
        }
      }
    }
  }
}

void* decode_base64(char* folder_name, char* file_name)
{
  char filepath[100];
  strcpy(filepath, folder_name);
  strcat(filepath, "/");
  strcat(filepath, file_name);

  FILE *file;
  file = fopen(filepath, "a+");
  char str[255];
  char decoded[255];
  // fprintf(file, "%s\n", file_name);
  // printf("%s\n", filepath);
  while(fgets(str, 255, file))
    printf("%s\n", str);
  // freopen("")

  pid_t child_id_1;
  if (child_id_1 == 0) {
    char *argv[] = {"base64", "-d", filepath, NULL};
    execv("/usr/bin/base64", argv);
  } else {
    while ((wait(NULL)) > 0);
    // move_to_folder(file_name);
  }
  // while(fgets(str, 255, file)) {
  //   char *argv[] = {"base64", "-d", file_name, NULL};
  // }
  // while ((ch = fgetc(file)) != EOF)
  //   printf("%c", ch);
  
  // puts("");
  fclose(file);
}
