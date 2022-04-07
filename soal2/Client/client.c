#include "stdio.h"
#include "errno.h"
#include "netdb.h"
#include "unistd.h" 
#include "string.h"
#include "strings.h"
#include "netinet/in.h"
#include "sys/stat.h"

void addTextToFileProblem(char *problemTitle, char *fileName, char *text)
{
	char fileNameWithDirectory[256];
	sprintf(fileNameWithDirectory, "%s/%s", problemTitle, fileName);
	
	FILE *fileWriter = fopen(fileNameWithDirectory, "a");
	fprintf(fileWriter, "%s\n", text);
	fclose(fileWriter);
}

void recvWithoutTestByte(int fd, char *buffer, size_t size)
{
	int byteCode = '0';
	while(byteCode == '0')
	{
		recv(fd, buffer, size, 0);
		byteCode = buffer[0];
	}
}
	

void getProblemNameAndSubmitFile(char *text, char *problemName, char *submitFile)
{
	int i = 0;
	
	if (text[0] == '"')
	{
		text += 1;
		while(*text != '"')
		{
			problemName[i] = *text;
			text += 1;
			i += 1;
		}
		text += 1;
	}
	else
	{
		while(*text != ' ')
		{
			problemName[i] = *text;
			text += 1;
			i += 1;
		}
	}
	problemName[i] = '\0';
	
	while(*text == ' ')
	{
		text += 1;
	}
	
	i = 0;
	if (text[0] == '"')
	{
		text += 1;
		while(*text != '"')
		{
			submitFile[i] = *text;
			text += 1;
			i += 1;
		}
		text += 1;
	}
	else
	{
		while(*text != ' ')
		{
			submitFile[i] = *text;
			text += 1;
			i += 1;
		}
	}
	submitFile[i] = '\0';
}

int main()
{
	struct sockaddr_in socketAddress;
	int socketFileDescriptor;
	struct hostent *serverHostData;
	char message[2048];
	
	socketFileDescriptor = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (socketFileDescriptor == -1)
	{
		fprintf(stderr, "Socket failed: %s\n", strerror(errno));
		return -1;
	}
	
	serverHostData = gethostbyname("localhost");
	socketAddress.sin_family = AF_INET;
	socketAddress.sin_port = htons(8080);
	socketAddress.sin_addr = *((struct in_addr *)serverHostData->h_addr);
	
	if (connect(socketFileDescriptor, (struct sockaddr *)&socketAddress, sizeof(struct sockaddr_in)) == -1)
	{
		fprintf(stderr, "Socket connection failed: %s\n", strerror(errno));
		close(socketFileDescriptor);
		return -1;
	}
	
	int currentPage = 0;
	int respon = 0;
	char username[33];
	char password[33];
	char command[2][256];
	
	while(1)
	{
		respon = 0;
		
		
		
		if (currentPage == 0)
		{
			while(respon <= 0 || respon >= 3)
			{
				printf("Pilih menu\n");
				printf("1. Register\n");
				printf("2. Login\n");
				scanf("%d", &respon);	
			}
			
			if (respon == 1)
			{
				currentPage = 1;
			}
			else if (respon == 2)
			{
				currentPage = 2;
			}
		}
		else if (currentPage == 1)
		{
			printf("Register\n");
			printf("Username dan password max length is 32\n");
			printf("Password must contain capital, small letter, and numeric\n");
			printf("Username: ");
			scanf("%s", username);
			printf("Password: ");
			scanf("%s", password);
			
			message[0] = 'r';
			message[1] = strlen(username);
			message[2] = strlen(password);
			message[3] = '\0';
			strcat(message, username);
			strcat(message, password);
			
			send(socketFileDescriptor, message, sizeof(message), 0);
			
			recvWithoutTestByte(socketFileDescriptor, message, sizeof(message));
			
			if (message[0] == 'd')
			{
				printf("\nRegister Failed\n\n");
			}
			currentPage = 0;
		}
		else if (currentPage == 2)
		{
			
			printf("\nLogin\n");
			printf("Username: ");
			scanf("%s", username);
			printf("Password: ");
			scanf("%s", password);
			
			message[0] = 'l';
			message[1] = strlen(username);
			message[2] = strlen(password);
			message[3] = '\0';
			strcat(message, username);
			strcat(message, password);
			
			send(socketFileDescriptor, message, sizeof(message), 0);
			
			recvWithoutTestByte(socketFileDescriptor, message, sizeof(message));
			
			if (message[0] == 'l')
			{
				currentPage = 3;
			}
			else if (message[0] == 'd')
			{
				printf("\nLogin Failed\n\n");
				currentPage = 0;
			}
			else if (message[0] == 'u')
			{
				printf("\nAnother user still login\n\n");
				currentPage = 0;
			}
		}
		else if (currentPage == 3)
		{
			printf("\nMenu\n");
			printf("add - Add new problem\n");
			printf("see - Show problem list\n");
			printf("download <judul problem> - untuk mengunduh deskripsi dan input permasalahan\n");
			printf("submit <judul problem> <path-file-output.txt>- untuk submit hasil output ke server\n");
			
			char choosenMenu[256];
			scanf(" %[^\n]", choosenMenu);
			
			if (strcasecmp(choosenMenu, "add") == 0)
			{
				currentPage = 4;
			}
			else if (strcasecmp(choosenMenu, "see") == 0)
			{
				currentPage = 5;
			}
			else if (strncasecmp(choosenMenu, "download ", 9) == 0)
			{
				strcpy(command[0], choosenMenu + 9);
				currentPage = 6;
			}
			else if (strncasecmp(choosenMenu, "submit  ", 7) == 0)
			{
				getProblemNameAndSubmitFile(choosenMenu + 7, command[0], command[1]);
				currentPage = 7;
			}
		}
		else if (currentPage == 4)
		{
			char problemName[64];
			strcpy(message, "pan");
			send(socketFileDescriptor, message, sizeof(message), 0);
			
			recvWithoutTestByte(socketFileDescriptor, message, sizeof(message));
			printf("%s", message);
			scanf(" %[^\n]", problemName);
			int problemNameLength = strlen(problemName);
			sprintf(message, "pat%c%c%s%s", problemNameLength, (char)strlen(username), problemName, username);
			send(socketFileDescriptor, message, sizeof(message), 0);
			
			recvWithoutTestByte(socketFileDescriptor, message, sizeof(message));
			if (message[0] == 'a')
			{
				recvWithoutTestByte(socketFileDescriptor, message, sizeof(message));
				printf("%s", message);
				
				char filePathDescription[256];
				scanf(" %[^\n]", filePathDescription);
				
				FILE *descriptionFile = fopen(filePathDescription, "r");
				if (descriptionFile == NULL)
				{
					printf("\nFile doesn't exist...\n\n");
				}
				else
				{
					char line[1024];
					int lineLength;
					while(fscanf(descriptionFile, " %[^\n]", line) != EOF)
					{
						sprintf(message, "pada%c", problemNameLength);
						
						lineLength = strlen(line);
						memcpy(message + 5, &lineLength, 4);
						message[9] = '\0';
						strcat(message + 9, problemName);
						strcat(message + 9, line);
						
						//printf("%c%c%c%c %d %d %s\n", message[0], message[1], message[2], message[3], message[4], *(message + 5), message + 9);
						send(socketFileDescriptor, message, sizeof(message), 0);
					}
					fclose(descriptionFile);
					
					strcpy(message, "padf");
					send(socketFileDescriptor, message, sizeof(message), 0);
					
					recvWithoutTestByte(socketFileDescriptor, message, sizeof(message));
					printf("%s", message);
					
					char filePathInput[256];
					scanf(" %[^\n]", filePathInput);
					FILE *inputFile = fopen(filePathInput, "r");
					
					if (inputFile == NULL)
					{
						printf("\nFile doesn't exist...\n\n");
					}
					else
					{
						while(fscanf(inputFile, " %[^\n]", line) != EOF)
						{
							sprintf(message, "paia%c", problemNameLength);
							
							lineLength = strlen(line);
							memcpy(message + 5, &lineLength, 4);
							message[9] = '\0';
							strcat(message + 9, problemName);
							strcat(message + 9, line);
							send(socketFileDescriptor, message, sizeof(message), 0);
						}
						fclose(inputFile);
					
						strcpy(message, "paif");
						send(socketFileDescriptor, message, sizeof(message), 0);
						
						recvWithoutTestByte(socketFileDescriptor, message, sizeof(message));
						printf("%s", message);
						
						char filePathOutput[256];
						scanf(" %[^\n]", filePathOutput);
						FILE *outputFile = fopen(filePathOutput, "r");
						
						if (outputFile == NULL)
						{
							printf("\nFile doesn't exist...\n\n");
						}
						else
						{
							while(fscanf(outputFile, " %[^\n]", line) != EOF)
							{
								sprintf(message, "paoa%c", problemNameLength);
								
								lineLength = strlen(line);
								memcpy(message + 5, &lineLength, 4);
								message[9] = '\0';
								strcat(message + 9, problemName);
								strcat(message + 9, line);
								send(socketFileDescriptor, message, sizeof(message), 0);
							}
							fclose(outputFile);
						
							strcpy(message, "paof");
							send(socketFileDescriptor, message, sizeof(message), 0);
							
							recvWithoutTestByte(socketFileDescriptor, message, sizeof(message));
							printf("%s\n", message);
						}
					}
				}
				
			}
			else
			{
				printf("\nProblem Already Exist...\n\n");
			}
			
			currentPage = 3;
		}
		else if (currentPage == 5)
		{
			strcpy(message, "s");
			send(socketFileDescriptor, message, sizeof(message), 0);
	
			printf("\nDaftar problem\n");
			int finish = 0;
			while(finish == 0)
			{
				recvWithoutTestByte(socketFileDescriptor, message, sizeof(message));
				
				if (message[2] == 'e')
				{
					printf("%s\n", message + 3);
				}
				else if (message[2] == 'f')
				{
					finish = 1;
				}
			}
			printf("\n");
			
			currentPage = 3;
		}
		else if (currentPage == 6)
		{
			// dpr = download problem request
			sprintf(message, "dpr%s", command[0]);
			send(socketFileDescriptor, message, sizeof(message), 0);
			
			recvWithoutTestByte(socketFileDescriptor, message, sizeof(message));
			if (message[0] == 'a')
			{
				mkdir(command[0], S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
				
				int downloadFinish = 0;
				char text[1024];
				while(downloadFinish == 0)
				{
					recvWithoutTestByte(socketFileDescriptor, message, sizeof(message));
					
					if (message[2] == 'd')
					{
						strcpy(text, message + 3);
						addTextToFileProblem(command[0], "description.txt", text);  
					}
					else if (message[2] == 'i')
					{
						strcpy(text, message + 3);
						addTextToFileProblem(command[0], "input.txt", text);  
					}
					else if (message[2] == 'r' && message[3] == 'f')
					{
						downloadFinish = 1;
					}
				}
				
				printf("\nProblem successfully downloaded from server\n\n");
			}
			else if (message[0] == 'd')
			{
				printf("\nProblem didn't exist...\n\n");
			}
			
			currentPage = 3;
		}
		else if (currentPage == 7)
		{
			FILE *fileReaderOutputSubmission = fopen(command[1], "r");
			char line[1024];
			
			if (fileReaderOutputSubmission == NULL)
			{
				printf("\nFile didn't exist...\n\n");
			}
			else
			{
				sprintf(message, "b%s", command[0]);
				send(socketFileDescriptor, message, sizeof(message), 0);
				
				recvWithoutTestByte(socketFileDescriptor, message, sizeof(message));
				if (message[0] == 'a')
				{
					while(fscanf(fileReaderOutputSubmission, " %[^\n]", line) != EOF)
					{
						sprintf(message, "b%s", line);
						send(socketFileDescriptor, message, sizeof(message), 0);
					}
					
					strcpy(message, "f");
					send(socketFileDescriptor, message, sizeof(message), 0);
					
				recvWithoutTestByte(socketFileDescriptor, message, sizeof(message));
					printf("Result: %s\n", message);
				}
				else if (message[1] == 'd')
				{
					printf("\nProblem didn't exist...\n\n");
				}
				
				
				fclose(fileReaderOutputSubmission);
			}
			
			currentPage = 3;
		}
	}
		
	
	return 0;
}
