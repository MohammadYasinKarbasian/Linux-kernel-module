#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char* argv[])
{
    if(argc<2){
    
    char name[100] = {0};
    char path[100] = {0};
    size_t size = 100;
    printf("what is device name?");
    scanf("%[^\n]s", name);
    getchar();
    sprintf(path,"/dev/%s", name);
    int fd = open(path, O_RDWR);
    char *buffer = malloc(1024);
    while (1)
    {
        printf("1.New transaction\n2.Read accounts balances\n3.Exit\n");
        int state;
        scanf("%d", &state);
        getchar();
        if (state == 1)
        {
            printf("what type of transaction you want(you can only choose t or w or d?");
            char type[100];
            scanf("%[^\n]s", type);
            getchar();
            printf("what is sender account ID?");
            char sender[100];
            scanf("%[^\n]s", sender);
            getchar();
            printf("what is reciever account ID?");
            char reciever[100];
            scanf("%[^\n]s", reciever);
            getchar();
            printf("what is amount of money?");
            char amount[100];
            scanf("%[^\n]s", amount);
            getchar();
            sprintf(buffer, "%s,%s,%s,%s", type, sender, reciever, amount);
            write(fd, buffer, 1024);
            system("sudo dmesg | tail -1 | cut -d \"]\" -f 2");
            // read(fd, buffer, 1024);
            // printf("%s\n", buffer);
        }
        else if(state == 2)
        {
            read(fd,buffer,1024);
	        fprintf(stdout,"%s\n",buffer);
        }
        else if (state == 3)
            break;
    }
    free(buffer);
    close(fd);
    return 0;
    }
    
    
    
    
    else{
    
    
    
    
        char name[100] = {0};
    char path[100] = {0};
    size_t size = 100;
    strcpy(name,argv[1]);
	char command[100]={0};
	strcpy(command , argv[2]);
	
    sprintf(path,"/dev/%s", name);
    int fd = open(path, O_RDWR);
    char *buffer = malloc(1024);
    read(fd,buffer,1024);
	        fprintf(stdout,"%s\n",buffer);
    write(fd, command, 1024);
        printf("\n\n\n\n\n");
read(fd,buffer,1024);
	        fprintf(stdout,"%s\n",buffer);
    close(fd);
    return 0;
    }
}
