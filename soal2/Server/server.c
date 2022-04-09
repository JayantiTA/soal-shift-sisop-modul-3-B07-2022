#include "stdio.h"
#include "unistd.h" 
#include "string.h"
#include "strings.h"
#include "errno.h"
#include "netinet/in.h" 
#include "sys/epoll.h"
#include "sys/types.h"
#include "sys/stat.h"

int createTCPServerSocket() 
{
    struct sockaddr_in socketAddress;
    int socketFileDescriptor;

    socketFileDescriptor = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); 
    if (socketFileDescriptor == -1) {
        fprintf(stderr, "socket failed: %s\n", strerror(errno));
        return -1;
    }

    socketAddress.sin_family = AF_INET;         
    socketAddress.sin_port = htons(8080);     
    socketAddress.sin_addr.s_addr = INADDR_ANY; 

    if (bind(socketFileDescriptor, (struct sockaddr *)&socketAddress, sizeof(struct sockaddr_in)) != 0) {
        fprintf(stderr, "bind failed: %s\n", strerror(errno));
        close(socketFileDescriptor);
        return -1;
    }

    if (listen(socketFileDescriptor, 5) != 0) {
        fprintf(stderr, "listen failed: %s\n", strerror(errno));
        close(socketFileDescriptor);
        return -1;
    }

    return socketFileDescriptor;
}

void setupEpollConnection(int epollFileDescriptor, int clientFileDescriptor, struct epoll_event * event) 
{
    event->events = EPOLLIN;
    event->data.fd = clientFileDescriptor;

    epoll_ctl(epollFileDescriptor, EPOLL_CTL_ADD, clientFileDescriptor, event);
}

void strcpyOffset(char *dest, char *src, int offset, int length)
{
	strncpy(dest, src + offset, length);
	dest[length] = '\0';
}

void getUsernameAndPasswordFromClient(char message[], char username[], char password[])
{
	int usernameFromClientLength = message[1];
	int passwordFromClientLength = message[2];
	
	strcpyOffset(username, message, 3, usernameFromClientLength);
	username[usernameFromClientLength] = '\0';
	
	strcpyOffset(password, message, 3 + usernameFromClientLength, passwordFromClientLength);
	password[passwordFromClientLength] = '\0';
}

int checkPasswordCompatible(char *password)
{
	int length = strlen(password);
	int containCapital = 0;
	int containSmall = 0;
	int containNumber = 0;
	for(int i = 0; i < length; i++)
	{
		if (password[i] >= 'a' && password[i] <= 'z')
		{
			containSmall = 1;
		}
		else if (password[i] >= 'A' && password[i] <= 'Z')
		{
			containCapital = 1;
		}
		else if (password[i] >= '0' && password[i] <= '9')
		{
			containNumber = 1;
		}
		else
		{
			return 0;
		}
	}
	return containCapital & containSmall & containNumber;
}

void createProblemDatabaseFile()
{
	FILE *problemDatabaseFile = fopen("problems.tsv", "a");
	fclose(problemDatabaseFile);
}

int problemDatabaseFileCheckTitleExist(char *problemTitle)
{
	FILE *problemDatabaseFile = fopen("problems.tsv", "r");
	char line[128];
	int problemTitleLength = strlen(problemTitle);
	 
	while(fscanf(problemDatabaseFile, " %[^\n]",  line) != EOF)
	{
		if (strncmp(problemTitle, line, problemTitleLength) == 0)
		{
			return 1;
		}
	}
	return 0;
}

int problemDatabaseFileAddNewProblem(char *problemTitle, char *userName)
{
	FILE *problemDatabaseFile = fopen("problems.tsv", "a");
	fprintf(problemDatabaseFile, "%s\t%s\n", problemTitle, userName);
	fclose(problemDatabaseFile);
	return mkdir(problemTitle, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
}

void addTextToFileInProblemDirectory(char *problemTitle, char *fileName, char *text)
{
	char fileNameWithDirectory[256];
	sprintf(fileNameWithDirectory, "%s/%s", problemTitle, fileName);
	
	FILE *fileWriter = fopen(fileNameWithDirectory, "a");
	fprintf(fileWriter, "%s\n", text);
	fclose(fileWriter);
}

int main()
{
	struct sockaddr_in newAddress;
	socklen_t addrlen;

	int serverFileDescriptor;
	int newClientFileDescriptor;
	int currentClientFileDescriptor = -1;
	int temp_fd;
	
	struct epoll_event epollEvents[1024], epoll_temp;
	int epollEvent;
	int epollFileDescriptor = epoll_create(1);
	
	int timeout_msecs = 1000;	
	char message[2048];

	serverFileDescriptor = createTCPServerSocket(); 

	if (serverFileDescriptor == -1) {
		fprintf(stderr, "Failed to create a server\n");
		return -1; 
	}

	setupEpollConnection(epollFileDescriptor, serverFileDescriptor, &epoll_temp);
	
	createProblemDatabaseFile();

	while (1) 
	{
		epollEvent = epoll_wait(epollFileDescriptor, epollEvents, 1024, timeout_msecs);

		if (currentClientFileDescriptor != -1)
		{
			strcpy(message, "0");
			if (send(currentClientFileDescriptor, message, sizeof(message), 0) == -1)
			{
    			epoll_ctl(epollFileDescriptor, EPOLL_CTL_DEL, currentClientFileDescriptor, &epoll_temp);
				close(currentClientFileDescriptor);
				
				currentClientFileDescriptor = -1;
			}
		}

		for (int i = 0; i < epollEvent; i++)
		{
			if (epollEvents[i].data.fd == serverFileDescriptor) 
			{ 
				newClientFileDescriptor = accept(serverFileDescriptor, (struct sockaddr*)&newAddress, &addrlen);

				if (newClientFileDescriptor >= 0) 
				{
					setupEpollConnection(epollFileDescriptor, newClientFileDescriptor, &epoll_temp);
				} 
				else 
				{
					fprintf(stderr, "accept failed [%s]\n", strerror(errno));
				}
			}
			else if (epollEvents[i].events & EPOLLIN) 
			{
				if (epollEvents[i].data.fd < 0) 
				{
					continue;
				}
				
				
				if (recv(epollEvents[i].data.fd, message, 2048, 0)) 
				{
					if (message[0] == 'r')
					{
						FILE *userDatabaseFile = fopen("user.txt", "r");
						if (userDatabaseFile == NULL)
						{
							userDatabaseFile = fopen("user.txt", "w");
							fclose(userDatabaseFile);
							
							userDatabaseFile = fopen("user.txt", "r");
						}
						
						char usernameFromClient[33];
						char passwordFromClient[33];
						getUsernameAndPasswordFromClient(message, usernameFromClient, passwordFromClient);
						int usernameFromClientLength = strlen(usernameFromClient);
						
						if (strlen(passwordFromClient) >= 6 && checkPasswordCompatible(passwordFromClient) != 0)
						{
							char usernameFromDatabase[128];
							
							char respon[] = "r";
							while(fscanf(userDatabaseFile, " %[^\n]", usernameFromDatabase) != EOF && respon[0] != 'd')
							{
								if (strncmp(usernameFromClient, usernameFromDatabase, usernameFromClientLength) == 0)
								{
									respon[0] = 'd';
								}
							}
							
							fclose(userDatabaseFile);
								
							strcpy(message, respon);
							send(epollEvents[i].data.fd, message, sizeof(message), 0);
							if (respon[0] == 'r')
							{
								userDatabaseFile = fopen("user.txt", "a");
								fprintf(userDatabaseFile, "%s:%s\n", usernameFromClient, passwordFromClient);
							}
						}
						else
						{
							strcpy(message, "d");
							send(epollEvents[i].data.fd, message, sizeof(message), 0);
						}
							
						fclose(userDatabaseFile);
					}
					else if (message[0] == 'l')
					{
						if (currentClientFileDescriptor == -1)
						{
							FILE *userDatabaseFile = fopen("user.txt", "r");
							if (userDatabaseFile == NULL)
							{
								userDatabaseFile = fopen("user.txt", "w");
								fclose(userDatabaseFile);
								strcpy(message, "d");
								send(epollEvents[i].data.fd, message, sizeof(message), 0);
							}
							else
							{
								char usernameFromClient[33];
								char passwordFromClient[33];
								getUsernameAndPasswordFromClient(message, usernameFromClient, passwordFromClient);
								
								if (strlen(passwordFromClient) >= 6 && checkPasswordCompatible(passwordFromClient) != 0)
								{
									char usernameFromDatabase[33];
									char passwordFromDatabase[33];
									
									char respon[] = "d";
									while(
										fscanf(
											userDatabaseFile, " %[^:]:%[^\n]", usernameFromDatabase, passwordFromDatabase
										) != EOF && respon[0] != 'l'
									)
									{
										if (
											strcmp(usernameFromClient, usernameFromDatabase) == 0 &&
											strcmp(passwordFromClient, passwordFromDatabase) == 0
										)
										{
											respon[0] = 'l';
										}
									}
									
									strcpy(message, respon);
									send(epollEvents[i].data.fd, message, sizeof(message), 0);	
									
									if (respon[0] == 'l')
									{
										currentClientFileDescriptor = epollEvents[i].data.fd;
									}
								}
								else
								{
									strcpy(message, "d");
									send(epollEvents[i].data.fd, message, sizeof(message), 0);
								}
									
								fclose(userDatabaseFile);
							}
						}
						else
						{
							strcpy(message, "u");
							send(epollEvents[i].data.fd, message, sizeof(message), 0);
						}
					}
					else if (message[0] == 'p')
					{
						if (message[1] == 'a')
						{
							if (message[2] == 'n')
							{
								strcpy(message, "Judul problem: ");
								send(epollEvents[i].data.fd, message, sizeof(message), 0);							
							}
							else if (message[2] == 't')
							{
								int titleLength = message[3];
								char problemTitle[64];
								strcpyOffset(problemTitle, message, 5, titleLength);
								
								int usernameLength = message[4];
								char username[33];
								strcpyOffset(username, message, 5 + titleLength, usernameLength);
								
								if (problemDatabaseFileCheckTitleExist(problemTitle) == 0)
								{
									problemDatabaseFileAddNewProblem(problemTitle, username);
									
									strcpy(message, "a");
									send(epollEvents[i].data.fd, message, sizeof(message), 0);
									
									strcpy(message, "Filepath description.txt: ");
									send(epollEvents[i].data.fd, message, sizeof(message), 0);
								}
								else
								{
									strcpy(message, "d");
									send(epollEvents[i].data.fd, message, sizeof(message), 0);
								}
							}
							else if (message[2] == 'd' || message[2] == 'i' || message[2] == 'o')
							{
								if (message[3] == 'a')
								{
									int titleLength = message[4];
									char problemTitle[64];
									strcpyOffset(problemTitle, message, 9, titleLength);
									
									int textLength;
									char text[1024];
									memcpy(&textLength, message + 5, 4);
									strcpyOffset(text, message, 9 + titleLength, textLength);
									
									if (message[2] == 'd')
									{
										addTextToFileInProblemDirectory(problemTitle, "description.txt", text);
									}
									else if (message[2] == 'i')
									{
										addTextToFileInProblemDirectory(problemTitle, "input.txt", text);
									}
									else if (message[2] == 'o')
									{
										addTextToFileInProblemDirectory(problemTitle, "output.txt", text);
									}
								}
								else if (message[3] == 'f')
								{
									if (message[2] == 'd')
									{
										strcpy(message, "Filepath input.txt: ");
										send(epollEvents[i].data.fd, message, sizeof(message), 0);
									}
									else if (message[2] == 'i')
									{
										strcpy(message, "Filepath output.txt: ");
										send(epollEvents[i].data.fd, message, sizeof(message), 0);
									}
									else if (message[2] == 'o')
									{
										strcpy(message, "New problem added");
										send(epollEvents[i].data.fd, message, sizeof(message), 0);
									}
								}
							}
						}
					}
					else if (message[0] == 's')
					{
						FILE *fileReaderProblemDatabase = fopen("problems.tsv", "r");
						
						char username[128];
						char problem[128];
						while(fscanf(fileReaderProblemDatabase, " %[^\t] %[^\n]", problem, username) != EOF)
						{
							sprintf(message, "see%s by %s", problem, username);
							send(epollEvents[i].data.fd, message, sizeof(message), 0);
						}
						sprintf(message, "sef");
						send(epollEvents[i].data.fd, message, sizeof(message), 0);
						
						fclose(fileReaderProblemDatabase);
					}
					else if (message[0] == 'd')
					{
						if (message[1] == 'p')
						{
							if (message[2] == 'r')
							{
								char problemTitle[128];
								strcpy(problemTitle, message + 3);
								if(problemDatabaseFileCheckTitleExist(problemTitle) == 1)
								{
									strcpy(message, "a");
									send(epollEvents[i].data.fd, message, sizeof(message), 0);
									
									char fileName[256];
									char line[1024];
									sprintf(fileName, "%s/description.txt", problemTitle);
									FILE *fileReader = fopen(fileName, "r");
									
									while(fscanf(fileReader, " %[^\n]", line) != EOF)
									{
										sprintf(message, "dpd%s", line);
										send(epollEvents[i].data.fd, message, sizeof(message), 0);
									}
									fclose(fileReader);
									
									sprintf(fileName, "%s/input.txt", problemTitle);
									fileReader = fopen(fileName, "r");
									while(fscanf(fileReader, " %[^\n]", line) != EOF)
									{
										sprintf(message, "dpi%s", line);
										send(epollEvents[i].data.fd, message, sizeof(message), 0);
									}
									fclose(fileReader);
									
									strcpy(message, "dprf");
									send(epollEvents[i].data.fd, message, sizeof(message), 0);
								}
								else
								{
									strcpy(message, "d");
									send(epollEvents[i].data.fd, message, sizeof(message), 0);
								}
							}
						}
					}
					else if (message[0] == 'b')
					{
						char problemTitle[128];
						strcpy(problemTitle, message + 1);
						
						if (problemDatabaseFileCheckTitleExist(problemTitle) == 1)
						{
							strcpy(message, "a");
							send(epollEvents[i].data.fd, message, sizeof(message), 0);
							
							char line[1024];
							char fileName[256];
							strcpy(fileName, problemTitle);
							strcat(fileName, "/output.txt");
							
							FILE *fileReaderOutputProblem = fopen(fileName, "r");
							
							int finished = 0;
							int AC = 1;
							while(finished == 0)
							{
								recv(epollEvents[i].data.fd, message, 2048, 0);
								
								if (message[0] == 'b' && AC == 1)
								{
									if (fscanf(fileReaderOutputProblem, " %[^\n]", line) != EOF)
									{
										if (strcmp(message + 1, line) != 0)
										{
											AC = 0;
										}
									}
									else
									{
										AC = 0;
									}
								}
								else if (message[0] == 'f')
								{
									if (AC == 1 && fscanf(fileReaderOutputProblem, " %[^\n]", line) != EOF)
									{
										AC = 0;
									}
									
									finished = 1;
								}
							}
							
							if (AC == 1)
							{
								strcpy(message, "AC");
							}
							else
							{
								strcpy(message, "WA");
							}
							send(epollEvents[i].data.fd, message, sizeof(message), 0);
						}
						else
						{
							strcpy(message, "d");
							send(epollEvents[i].data.fd, message, sizeof(message), 0);
						}
					}
				}
			}
		}
	}

	for (int i = 0; i < 1024; i++) {
		if (epollEvents[i].data.fd > 0) {
			close(epollEvents[i].data.fd);
		}
	}
	
	return 0;
}
