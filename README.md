# Soal Shift Sisop Modul 2 2022

Repository untuk soal shift sisop modul 2 kelompok B07 tahun 2022

Anggota:

1. [Hans Sean Nathanael](https://gitlab.com/HansSeanNathanael) (5025201019)
2. [Jayanti Totti Andhina](https://gitlab.com/JayantiTA) (5025201037)
3. [Agnesfia Anggraeni](https://gitlab.com/agnesfiaa) (5025201059)

## Nomor 2

### client.c

```C++
void addTextToFileProblem(char *problemTitle, char *fileName, char *text)
{
	char fileNameWithDirectory[256];
	sprintf(fileNameWithDirectory, "%s/%s", problemTitle, fileName);
	
	FILE *fileWriter = fopen(fileNameWithDirectory, "a");
	fprintf(fileWriter, "%s\n", text);
	fclose(fileWriter);
}
```

Function addTextToFileProblem digunakan untuk menambahkan string ke dalam sebuah file yaitu pada saat mengunduh description.txt dan input.txt sebuah problem dari server.

```C++
void recvWithoutTestByte(int fd, char *buffer, size_t size)
{
	int byteCode = '0';
	while(byteCode == '0')
	{
		recv(fd, buffer, size, 0);
		byteCode = buffer[0];
	}
}
```

Function recvWithoutTestByte digunakan untuk mengambil message yang dikirim oleh server dan membuang ping byte yang dikirim oleh server. Ping byte dikirim oleh server untuk mengecek apakah user masing terhubung dengan server.

```C++
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
```

Function getProblemNameAndSubmitFile digunakan untuk memparse string agar string yang berada di antara dua tanda petik dua ("...") menjadi satu kesatuan. Ini digunakan untuk command submit agar problem dengan nama lebih dari satu kata dapat diakses dan file ouput yang akan disubmit juga dapat memiliki spasi pada nama filenya.

Masuk ke dalam main
```C++
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
```

pertama membuat koneksi dengan server kemudian menginisialisasi variable yang dibutuhkan untuk menyimpan input dari user.

```C++
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
```

currentPage == 0 adalah menu paling awal yang meminta user untuk register atau login.

```C++
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
```

currentPage == 1 adalah menu register, untuk data akun yang akan diregister dikirim dalam 1 message dengan byte terdepan adalah 'r' sebagai penanda user mengirim perintah register dan server dapat menterjemahkannya.

```C++
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
```

currentPage == 2 adalah menu login, mirip dengan sistem register dan byte terdepan message untuk server adalah 'l'.

```C++
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
```

currentPage == 3 adalah daftar command yang dapat diakses user yang telah login.

```C++
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
```

currentPage == 4 adalah menu untuk menambahkan problem ke server. User akan mengirim permintaan untuk mendaftarkan problem ke server dan server akan meminta judul problem. Judul problem akan dicek server, bila belum ada pada database maka dapat membuat problem tersebut, server akan memberitahu user bahwa user dapat mengirim deskripsi permasalahan kemudian user akan memasukkan lokasi file deskripsi permasalahan dan file tersebut akan dibaca satu persatu barisnya dan dikirim ke server, setelah EOF akan dikirim message untuk memberi tahu server bahwa upload file tersebut telah selesai, server kemudian akan meminta file lain yang perlu diupload dan langkah-langkahnya akan sama seperti mengirim file deskripsi.

```C++
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
```

currentPage == 5 adalah menu see, user akan mengirim byte "s" ke server untuk meminta server mengirimkan daftar problem dan setelahnya akan mengeluarkan nama-nama problem yang didapat dari server hingga mendapatkan message "f" dari server.

```C++
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
```

currentPage == 6 adalah menu download problem, user mengirimkan message dengan prefix "dpr" dan bila server menyatakan problem ada, program akan membuat directory problem tersebut kemudian akan menerima message isi file description.txt dan input.txt dari server dan disimpan hingga menemukan message untuk berhenti.

```C++
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
```

currentPage == 7 adalah menu submit, file yang akan disubmit akan dibaca satu persatu isinya dan dikirim ke server hingga EOF dan setelahnya akan dikirim message "f" untuk memberi tahu server bahwa file telah EOF dan kemudian akan membaca hasil yang akan dikirim dari server.

### server.c

```C++
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
```

Function createTCPServerSocket untuk membuat server socket TCP.

```C++
void setupEpollConnection(int epollFileDescriptor, int clientFileDescriptor, struct epoll_event * event) 
{
    event->events = EPOLLIN;
    event->data.fd = clientFileDescriptor;

    epoll_ctl(epollFileDescriptor, EPOLL_CTL_ADD, clientFileDescriptor, event);
}
```

Function setupEpollConnection untuk menambahkan file descriptor koneksi user pada epoll.

```C++
void strcpyOffset(char *dest, char *src, int offset, int length)
{
	strncpy(dest, src + offset, length);
	dest[length] = '\0';
}
```

Function strcpyOffset adalah modifikasi strcpy untuk mengambil substring dengan panjang tertentu pada sebuah string.

```C++
void getUsernameAndPasswordFromClient(char message[], char username[], char password[])
{
	int usernameFromClientLength = message[1];
	int passwordFromClientLength = message[2];
	
	strcpyOffset(username, message, 3, usernameFromClientLength);
	username[usernameFromClientLength] = '\0';
	
	strcpyOffset(password, message, 3 + usernameFromClientLength, passwordFromClientLength);
	password[passwordFromClientLength] = '\0';
}
```

Function getUsernameAndPasswordFromClient digunakan untuk parse byte message yang dikirim oleh user ketika login atau register.

```C++
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
```

Function checkPasswordCompatible untuk mengecek apakah sebuah string terdiri dari huruf besar, huruf kecil, dan angka.

```C++
void createProblemDatabaseFile()
{
	FILE *problemDatabaseFile = fopen("problems.tsv", "a");
	fclose(problemDatabaseFile);
}

```

Function createProblemDatabaseFile digunakan untuk membuat file database penyimpan problem

```C++
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
```

Function problemDatabaseFileCheckTitleExist digunakan untuk mengecek apakah terdapat sebuah problem pada database

```C++
int problemDatabaseFileAddNewProblem(char *problemTitle, char *userName)
{
	FILE *problemDatabaseFile = fopen("problems.tsv", "a");
	fprintf(problemDatabaseFile, "%s\t%s\n", problemTitle, userName);
	fclose(problemDatabaseFile);
	return mkdir(problemTitle, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
}
```

Function problemDatabaseFileAddNewProblem digunakan untuk menambahkan problem pada database dan membuat directory penyimpanan file-filenya.

```C++
void addTextToFileInProblemDirectory(char *problemTitle, char *fileName, char *text)
{
	char fileNameWithDirectory[256];
	sprintf(fileNameWithDirectory, "%s/%s", problemTitle, fileName);
	
	FILE *fileWriter = fopen(fileNameWithDirectory, "a");
	fprintf(fileWriter, "%s\n", text);
	fclose(fileWriter);
}
```

Function addTextToFileInProblemDirectory digunakan untuk menambahkan teks pada sebuah file yang akan digunakan ketika mengunduh permasalahan dari user.

Masuk ke dalam main

```C++
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
```

Pertama-tama membuat server dan epoll untuk menampung koneksi dan membuat database penyimpanan file.

```C++
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
```

Server akan mengecek terus message yang dikirim oleh user dan akan mengirimkan ping byte ke pada user untuk mengecek apakah user telah terputus koneksinya. Bila user telah terputus, maka akan ada slot kosong untuk user lain untuk connect.

```C++
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
```

Jika byte message "r" adalah penanda user meminta register, server akan mengecek apakah password memenuhi syarat dan username tersebut belum digunakan dan bila keduanya benar maka server akan menyimpan username dan password tersebut dan mengirim respon byte "a" untuk memberi tahu user bahwa register berhasil.

```C++
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
```

Bila byte message terdepan dari user adalah 'l' maka user mencoba login. Username dan password yang dikirim user akan dicek apakah ada di dalam database. Bila terdapat user lain yang telah login dan belum keluar, maka user tidak akan dapat login.

```C++
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
```

Prefix message "pa" adalah pemberi tahu bahwa user sedang melakukan upload permasalahan baru. Server akan menterjemahkan message tersebut apakah user melakukan upload judul problem baru, deskripsi, input, atau output.

```C++
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
```

Bila server menerima byte "s" dari user, itu artinya user meminta daftar problem. Server akan membaca daftar problem yang ada di database dan dikirim ke user satu-persatu dan diakhir dikirim message "sef" sebagai penanda bahwa request telah selesai.

```C++
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
```

Prefix "dpr" pada message adalah penanda user meminta unduh permasalahan. Server akan mengirimkan satu persatu line dari file description dan input kepada user dan setelah selesai akan mengirimkan byte "dprf".

```C++
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
```

Bila server menerima byte "b" itu artinya user sedang submit sebuah problem, server akan membandingkan semua line output dengan yang dikirim oleh user dan bila sama maka akan mengirimkan message "AC" kepada user dan bila tidak "WA". Newline kosong setelah semua output tidak akan dianggap.

### dokumentasi
![pengerjaan_1](/uploads/b6b656bcbb5d5912067c64771f3d5b9a/pengerjaan_1.png)

![pengerjaan_3](/uploads/fbd0ae28b13904f60a14d326a8b71085/pengerjaan_3.png)

![pengerjaan_4](/uploads/3aed6227764646737aea7cfd9074f10f/pengerjaan_4.png)

![pengerjaan_6](/uploads/e89f6e25ca23653b57fe0bfb04b3bf17/pengerjaan_6.png)

![pengerjaan_7](/uploads/fcda16899331694f7bda83e3c1b060aa/pengerjaan_7.png)

permasalahan yang dihadapi adalah membuat epoll dikarenakan dokumentasi yang tidak begitu jelas di internet dan mencari cara agar hanya 1 user yang dapat login pada satu waktu. Namun semua permasalahan berhasil diselesaikan.
