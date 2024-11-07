#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>

char *get_filename(char *buff)
{
	if (*buff ==  '/')
	{
		char *p;
		char *fname;

		buff++;
		p = strchr(buff, ' ');
		if (p == NULL)
		{
			printf("Illegal format\n");
			exit(1);
		}
		*p = '\0';
		if (p == buff)
			return ("index.html");
		fname = malloc(strlen(buff) + 1);
		strcpy(fname, buff);
		return (fname);
	}
	else
	{
		printf("Unknown header %s\n", buff);
		exit(1);
	}
}
int	main()
{
	struct	sockaddr_in serv_addr;
	int	sockfd;
	int	check_fd;
	int	new_sockfd;
	char	buff[1024];
	FILE 	*istream;
	int	count;
	//char *filename;

	count = 0;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		perror("Error socket: ");
		return (1);
	}
	if (memset(&serv_addr, 0, sizeof(struct sockaddr_in)) == NULL)
	{
		write(2, "memset\n", 7);
		return (1);
	}
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(80);
	check_fd = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &check_fd, sizeof(int)) < 0)
	{
		perror ("Error setsockopt: ");
		return (1);
	}
	if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr_in)) < 0)
	{
		perror("Error bind: ");
		return (1);
	}
	if (listen(sockfd, 5) < 0)
	{
		perror("Error listen: ");
		return(1);
	}
	while(1)
	{
		new_sockfd = accept(sockfd, NULL, NULL);
		if (new_sockfd < 0)
		{
			perror("Error accept: ");
			return(1);
		}
		istream = fdopen(new_sockfd, "r+");
		if (istream == NULL)
		{
			perror("Error fdopen: ");
			return (1);
		}
		if(setvbuf(istream, NULL, _IONBF, 0) != 0)
		{
			perror("Error setvbuf: ");
			return(1);
		}
		count = 0;
		char *filename = NULL;
		while(1)
		{
			char *p;
			if(fgets(buff, 1024, istream) == 0)
				break ;
			p = strchr(buff, '/');
			if (count == 0 && p != NULL && *(p + 1) != ' ')
				filename = get_filename(p);	
			printf("%s", buff);
			if (strcmp(buff, "\r\n") == 0)
				break ;
			count++;
		}
		if (filename == NULL)
		{

			fprintf(istream, "HTTP/1.1 200 OK \r\n");
			fprintf(istream, "Content-Type: text/html \r\n");
			fprintf(istream, "\r\n");
			fprintf(istream, "Hello\r\n");
		}
		else
		{
			FILE *istream2;

			istream2 = fopen(filename, "r");
			if (istream2 == NULL)
			{
				perror ("Error fopen: ");
				return(1);
			}
			while (1)
			{
				char *check;
				if(fgets(buff, 1024, istream2) == 0)
					break ;
				if(buff[0] == '\0')
				{
					fprintf(istream, "\r\n");
					break;
				}
				check = strchr(buff, '\n');
				if (check  != NULL)
					*check =  '\0';
				fprintf(istream, "%s\r\n", buff);

			}
			free(filename);
			if (fclose(istream2) < 0)
			{
				perror("Error fclose2: ");
				return (1);
			}
		}
		//printf("test\n");
		if(fclose(istream) != 0)
		{
			perror("Error fclose: ");
			return (1);
		}
		//if (close(new_sockfd) < 0)
		//{
		//	perror ("Error close_new_sock: ");
		//	return (1);
		//}
	}
	if (close(sockfd) < 0)
	{
		perror ("Error close: ");
		return(1);
	}
}




	

