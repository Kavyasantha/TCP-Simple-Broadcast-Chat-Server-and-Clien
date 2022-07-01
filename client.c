#include <stdio.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/errno.h>

static const int JOIN=2;
static const int SEND=4;
static const int FWD=3;
static const int ACK=7;
static const int NAK=5;
static const int ONLINE=8;
static const int OFFLINE=6;

static const int USERNAME=2;
static const int MESSAGE=4;
static const int REASON=1;
static const int CLIENT_COUNT=3;

struct HEAD{
	unsigned int vrsn : 9;
	unsigned int type : 7;
	int len;
};
struct ATT{
	int type;
	int len;
	char payload[512];
};
struct MSG{
	struct HEAD h_e_a_d;
	struct ATT a_t_t[2];
};
struct INFO{
	char name_user[16];
	int fd;
};



int main(int argc, char const *argv[])
{
	
	
		int soc;
		struct sockaddr_in serv;
		struct hostent* hret;
		fd_set master;
		fd_set read_fd;
		FD_ZERO(&read_fd);
		FD_ZERO(&master);
		soc = socket(AF_INET, SOCK_STREAM, 0);
		if(soc<0)
		{
			perror("---error creating socket\n");
			exit(1);
		}
		printf("socket successfully created!\n");

		memset(&serv, 0, sizeof(serv));
		
		serv.sin_family = AF_INET;
		hret = gethostbyname(argv[2]);
		memcpy(&serv.sin_addr.s_addr, hret->h_addr, hret->h_length);
		serv.sin_port = htons(atoi(argv[3]));
		
		if (connect(soc,(struct sockaddr *)&serv, sizeof(serv))<0)
		{
			printf("---error connection\n");
			exit(0);
		}
		else
		{
			printf("connected to the server!\n");
			struct HEAD h_e_a_d;
			struct ATT a_t_t_r;
			struct MSG msg_s;
			
			h_e_a_d.vrsn = '3';
			h_e_a_d.type = '2';
			a_t_t_r.type = USERNAME;
			a_t_t_r.len = strlen(argv[1]) + 1;
			strcpy(a_t_t_r.payload,argv[1]);
			msg_s.h_e_a_d = h_e_a_d;
			msg_s.a_t_t[0] = a_t_t_r;
			write(soc,(void *) &msg_s, sizeof(msg_s));
			
			sleep(1);
			struct MSG msg_serv;
	
			read(soc, (struct MSG *) &msg_serv, sizeof(msg_serv));
			if (msg_serv.h_e_a_d.type == FWD)	
				printf("FWD msg from %s: %s", msg_serv.a_t_t[1].payload, msg_serv.a_t_t[0].payload);
			
			if (msg_serv.h_e_a_d.type == NAK)
			{
				printf("NAK msg from server: %s", msg_serv.a_t_t[0].payload);
				close(soc);	
			}

			if (msg_serv.h_e_a_d.type == ACK)
				printf("ACK msg from server: %s", msg_serv.a_t_t[0].payload);

			if (msg_serv.h_e_a_d.type == ONLINE)
				printf("ONLINE msg: %s is now online.", msg_serv.a_t_t[0].payload);

			FD_SET(soc, &master);
			FD_SET(STDIN_FILENO, &master);
			
			
			while(1)
			{
				read_fd = master;
				printf("\n");
				if (select(soc+1, &read_fd, NULL, NULL, NULL) ==-1)
				{
					perror("---select error");
					exit(4);
				}
				if (FD_ISSET(soc, &read_fd))
				{
					struct MSG msg_serv;
	
					read(soc, (struct MSG *) &msg_serv, sizeof(msg_serv));
					if (msg_serv.h_e_a_d.type == FWD)				
						printf("FWD msg from %s: %s", msg_serv.a_t_t[1].payload, msg_serv.a_t_t[0].payload);
						
					if (msg_serv.h_e_a_d.type == NAK)
					{
						printf("NAK msg from server: %s", msg_serv.a_t_t[0].payload);
						close(soc);
					}

					if (msg_serv.h_e_a_d.type == OFFLINE)
						printf("OFFLINE msg: %s is now offline.", msg_serv.a_t_t[0].payload);


					if (msg_serv.h_e_a_d.type == ACK)
						printf("ACK msg from server: %s", msg_serv.a_t_t[0].payload);

					if (msg_serv.h_e_a_d.type == ONLINE)
						printf("ONLINE msg: %s is now online.", msg_serv.a_t_t[0].payload);

				}
				if (FD_ISSET(STDIN_FILENO, &read_fd))
				{
					struct ATT cli_a_t_t;
					struct MSG msg_s;
					
					char t_p[512];
					int serv_read = 0;
					
					struct timeval tv;
					fd_set readfds;
					tv.tv_sec = 2;
					tv.tv_usec = 0;

					FD_ZERO(&readfds);
					FD_SET(STDIN_FILENO, &readfds);
					select(STDIN_FILENO+1, &readfds, NULL, NULL, &tv);

					if (FD_ISSET(STDIN_FILENO, &readfds))
					{
						serv_read = read(STDIN_FILENO, t_p, sizeof(t_p));
					if (serv_read > 0)
					{
						t_p[serv_read] = '\0';
					}
					cli_a_t_t.type = MESSAGE;
					strcpy(cli_a_t_t.payload, t_p);
					msg_s.a_t_t[0] = cli_a_t_t;
					write(soc, (void *) &msg_s, sizeof(msg_s));
					}
					else
					{
						printf("Timed out!\n");
					}
				}
			}
		
		}
	
	return 0;

}
