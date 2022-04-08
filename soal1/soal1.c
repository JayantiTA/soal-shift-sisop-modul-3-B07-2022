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

int status = 0;
char base46_map[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                     'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                     'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                     'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'};

void* download_and_unzip(char* folder_name);
void* read_line(char* folder_name, char* file_name);
char* decode_base64(char* str);
void* move_to_folder(char* folder_name, char* file_name);
void* zip_hasil_folder();
void* unzip_file_using_password();
void* create_no_txt();

int main()
{
    pthread_t thread1, thread2, thread3;
    const char *file_name1 = "music";
    const char *file_name2 = "quote";
    
    // download music.zip and quote.zip (1a - 1d)
    pthread_create(&thread1, NULL, download_and_unzip, (char*) file_name1);
    pthread_create(&thread2, NULL, download_and_unzip, (char*) file_name2);
    pthread_create(&thread3, NULL, zip_hasil_folder, NULL);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL); 
    pthread_join(thread3, NULL); 

    status = 0;

    // unzip and create file no.txt (1e)
    pthread_create(&thread1, NULL, unzip_file_using_password, "hasil.zip");
    pthread_create(&thread2, NULL, create_no_txt, NULL);
    pthread_create(&thread3, NULL, zip_hasil_folder, NULL);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    pthread_join(thread3, NULL);

    exit(EXIT_SUCCESS);
}

void* download_and_unzip(char* folder_name)
{
  pid_t child_id_1, child_id_2, child_id_3,
    child_id_4, child_id_5;
  child_id_1 = fork();
  int status1;

  if (child_id_1 == 0) {
    child_id_2 = fork();
    int status2;

    if (child_id_2 == 0) { // download zip file
      if (strcmp(folder_name, "music") == 0) {
        char *argv[] = {"wget", "--no-check-certificate", link1, "-O", "music.zip", NULL};
        execv("/usr/bin/wget", argv);
      } else if (strcmp(folder_name, "quote") == 0) {
        char *argv[] = {"wget", "--no-check-certificate", link2, "-O", "quote.zip", NULL};
        execv("/usr/bin/wget", argv);
      }
    } else { // unzip file
      while ((wait(&status2)) > 0);
      char *argv[] = {"unzip", folder_name, "-d", folder_name, NULL};
      execv("/usr/bin/unzip", argv);
    }
  } else {
    while ((wait(&status1)) > 0);
    child_id_3 = fork();
    int status3;

    if (child_id_3 == 0) {
      child_id_4 = fork();
      int status4;
      if (child_id_4 == 0) { // remove zip file
        char *file_name = malloc(sizeof(char) * 100);
        strcpy(file_name, folder_name);
        strcat(file_name, ".zip");
        char *argv[] = {"rm", file_name};
        execv("/usr/bin/rm", argv);
      } else { // create .txt file using folder name
        while((wait(&status4)) > 0);
        char *file_name = malloc(sizeof(char) * 100);
        strcpy(file_name, folder_name);
        strcat(file_name, ".txt");
        char *argv[] = {"touch", file_name, NULL};
        execv("/usr/bin/touch", argv);
      }
    } else {
      while ((wait(&status3)) > 0);
      child_id_5 = fork();
      int status5;

      if (child_id_5 == 0) { // listing directory
        DIR *dir_path;
        struct dirent *dir;
        dir_path = opendir(folder_name);
        while((dir = readdir(dir_path))) {
          if (strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0) {
            read_line(folder_name, dir->d_name); // read line function
          }
        }
      } else {
        while ((wait(&status5)) > 0);
        move_to_folder(folder_name, folder_name); // move .txt files to folder hasil/
      }
    }
  }
}

void* read_line(char* folder_name, char* file_name)
{
  char filepath[100]; // .txt files inside folder (q1 - q9 and m1 - m9)
  strcpy(filepath, folder_name);
  strcat(filepath, "/");
  strcat(filepath, file_name);

  char file_txt[100]; // .txt file after being decoded
  strcpy(file_txt, folder_name);
  strcat(file_txt, ".txt");

  FILE *file;
  file = fopen(filepath, "a+");

  FILE *file_base64;
  file_base64 = fopen(file_txt, "a");

  char str[255];
  while(fgets(str, 255, file)) {
    printf("%s\n", str);
    fprintf(file_base64, "%s\n", decode_base64(str)); // decode every line in file
  }

  fclose(file);
}

char* decode_base64(char* str) // I found this function on google:)
{
  char counts = 0;
  char buffer[4];
  char* plain = malloc(strlen(str) * 3 / 4 + 1);
  int i = 0, p = 0;

  for(i = 0; str[i] != '\0'; i++) {
      char k;
      for(k = 0 ; k < 64 && base46_map[k] != str[i]; k++);
      buffer[counts++] = k;
      if(counts == 4) {
          plain[p++] = (buffer[0] << 2) + (buffer[1] >> 4);
          if(buffer[2] != 64) {
            plain[p++] = (buffer[1] << 4) + (buffer[2] >> 2);
          }
          if(buffer[3] != 64) {
            plain[p++] = (buffer[2] << 6) + buffer[3];
          }
          counts = 0;
      }
  }
  plain[p] = '\0';
  return plain;
}

void* move_to_folder(char* folder_name, char* file_name)
{
  pid_t child_id_1, child_id_2;

  child_id_1 = fork();
  int status1;
  if (child_id_1 == 0) {
    child_id_2 = fork();
    int status2;

    if (child_id_2 == 0) { // remove folder after decoded all .txt files
      char *argv[] = {"rm", "-rf", folder_name, NULL};
      execv("/usr/bin/rm", argv);
    } else { // create new directory named hasil/
      while ((wait(&status2)) > 0);
      char *argv[] = {"mkdir", "-p", "hasil", NULL};
      execv("/usr/bin/mkdir", argv);
    }
  } else {
    while ((wait(&status1)) > 0);
    pid_t child_id_3 = fork();
    int status3;

    if (child_id_3 == 0) { // move .txt that contains decoded message into hasil/
      char filename[100];
      strcpy(filename, folder_name);
      strcat(filename, ".txt");
      char *argv[] = {"mv", filename, "hasil", NULL};
      execv("/usr/bin/mv", argv);
    } else { // status count (zip folder after music.txt and quote.txt being moved)
      while ((wait(&status3)) > 0);
      status += 1;
    }
  }
}


void* zip_hasil_folder()
{
  pid_t child_id_1, child_id_2;
  int status1;

  while (status != 2) { // music.txt or quote.txt haven't being moved yet
  }
  
  child_id_1 = fork();
  if (child_id_1 == 0) {
    int status2;
    child_id_2 = fork();

    if (child_id_2 == 0) { // zip folder hasil/
      char* argv[] = {"zip", "-r", "-P", "mihinomenestjay", "hasil.zip", "hasil", NULL};
      execv("/usr/bin/zip", argv);
    } else { // remove folder hasil/
      while ((wait(&status2)) > 0);
      char *argv[] = {"rm", "-rf", "hasil", NULL};
      execv("/bin/rm", argv);
    }
  } else {
    while ((wait(&status1)) > 0);
  }
}

void* unzip_file_using_password()
{
  pid_t child_id_1;
  int status1;

  child_id_1 = fork();
  if (child_id_1 == 0) {
    pid_t child_id_2 = fork();
    int status2;
    if (child_id_2 == 0) { // to unzip hasil.zip using password
      char* argv[] = {"unzip", "-P", "mihinomenestjay", "hasil.zip", NULL};
      execv("/usr/bin/unzip", argv);
    } else { // remove hasil.zip
      while ((wait(&status2)) > 0);
      char *argv[] = {"rm", "hasil.zip", NULL};
      execv("/bin/rm", argv);
    }
  } else {
    while ((wait(&status1)) > 0);
    status = 3;
  }
}

void* create_no_txt()
{
  pid_t child_id_1;
  int status1;

  child_id_1 = fork();
  if (child_id_1 == 0) {
    pid_t child_id_2 = fork();
    int status2;
    if (child_id_2 == 0) { // create new file named no.txt
      char* argv[] = {"touch", "no.txt", NULL};
      execv("/usr/bin/touch", argv);
    } else { // write "No" to file and then move it to folder hasil/
      while ((wait(&status2)) > 0);
      FILE *file_no_txt;
      file_no_txt = fopen("no.txt", "w+");
      fprintf(file_no_txt, "No");
      fclose(file_no_txt);
      char *argv[] = {"mv", "no.txt", "hasil", NULL};
      execv("/bin/mv", argv);
    }
  } else { // set status to 2 and then zip folder hasil again
    while ((wait(&status1)) > 0);
    status = 2;
  }
}
