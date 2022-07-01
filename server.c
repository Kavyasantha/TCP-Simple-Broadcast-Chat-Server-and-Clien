#include <stdio.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static const int JOIN=2;
static const int FWD=3;
static const int ACK=7;
static const int NAK=5;
static const int ONLINE=8;
static const int OFFLINE=6;
static const int SEND=4;

static const int MESSAGE=4;
static const int USERNAME=2;
static const int REASON=1;
static const int CLIENT_COUNT=3;

struct HEAD
{
	unsigned int vrsn : 9;
	unsigned int type : 7;
	int len;
};
struct ATT
{
	int type;
	int len;
	char payload[512];
};
struct MSG{
	struct HEAD h_e_a_d;
	struct ATT a_t_t[2];
};
struct INFO
{
	char name_user[16];
	int fd;
};



int t_o_t_cli = 0;
struct INFO *cli_list;

void sending(struct MSG sent_msg,fd_set rec_cli_list,int soc,int new, int next_cli)
{
	int j=0;
	while(j<=next_cli)
	{
		if (FD_ISSET(j, &rec_cli_list)) 
		{
			if(soc!=j && new!=j)
			{
				if ((write(j,(void *) &sent_msg,sizeof(sent_msg)))<0)
					printf("---error\n");
			}
		}
		j=j+1;
	}
	
}

int name_check(char z[], int cli_max)
{
	int j = 0;
	while(j<t_o_t_cli)
	{
		if(!strcmp(z,cli_list[j].name_user))
			return 2;
		if(t_o_t_cli == cli_max)
			return 1;
		j=j+1;
	}
	return 0;
}


int main(int argc, char const *argv[])
{
	struct MSG msg_send;
	struct ATT msg_send_a_t_t;
	struct MSG sent_msg;
	int new,next_cli,t,x,y,current,i_r,soc;
	int flag_user = 0;
	struct sockaddr_in cli;
	fd_set rec_cli_list;
	fd_set mon_list;

	int cli_max=0;

	struct sockaddr_in serv;
	struct hostent* hret;

	soc = socket(AF_INET,SOCK_STREAM,0);
	memset(&serv,0,sizeof(serv));
	
	serv.sin_family = AF_INET;
	hret = gethostbyname(argv[1]);
	memcpy(&serv.sin_addr.s_addr, hret->h_addr,hret->h_length);
	serv.sin_port = htons(atoi(argv[2]));
	if((bind(soc, (struct sockaddr*)&serv, sizeof(serv))==-1))
	{
		perror("----error binding");
		exit(1);
	}
	
	listen(soc, 99);
	printf("server listening\n");
	
	cli_max=atoi(argv[3]);
	cli_list= (struct INFO *) malloc(cli_max*sizeof(struct INFO));
	FD_SET(soc, &rec_cli_list);
	next_cli = soc;
	for(;;)
	{
		mon_list = rec_cli_list;
		select(next_cli+1, &mon_list, NULL, NULL, NULL);
		current=0;
		while(current<=next_cli)
		
		{
			if(FD_ISSET(current, &mon_list)) 
			{
				if(current == soc)
				{
					int clientSize = sizeof(cli);
					new = accept(soc,(struct sockaddr*)&cli,&clientSize);
					if(new ==-1)
					{
						printf("Server cant be accepted.\n");
					}
					else 
					{
						t = next_cli;
						FD_SET(new, &rec_cli_list); 
						if(new > next_cli)
						{
						next_cli = new;
						}
						struct MSG joinMsg;
						struct ATT joinMsga_t_t;
						char msg_index[16];
						read(new,(struct MSG *) &joinMsg,sizeof(joinMsg));
						joinMsga_t_t = joinMsg.a_t_t[0];
						strcpy(msg_index, joinMsga_t_t.payload);
						
						flag_user = name_check(msg_index, cli_max);
						if(flag_user == 0)
						{
							strcpy(cli_list[t_o_t_cli].name_user, msg_index);
							cli_list[t_o_t_cli].fd = new;
							
							t_o_t_cli=t_o_t_cli+1;
							struct MSG a_c_k;
							struct HEAD a_c_k_h_e_a_d;
							struct ATT a_c_k_a_t_t;
							int k = 0;
							char a[128];
							a[0] = (char)(((int)'0')+ t_o_t_cli);
							a[1] = ' ';
							a[2] = '\0';
							while(k<t_o_t_cli-1){
								strcat(a,cli_list[k].name_user);
								strcat(a, ",");
								k=k+1;
							}
							
							a_c_k_h_e_a_d.vrsn=FWD;
							a_c_k_h_e_a_d.type=ACK;
							a_c_k_a_t_t.type = MESSAGE;
							
							a_c_k_a_t_t.len = strlen(a)+1;
							strcpy(a_c_k_a_t_t.payload, a);
							a_c_k.h_e_a_d = a_c_k_h_e_a_d;
							a_c_k.a_t_t[0] = a_c_k_a_t_t;
							write(new,(void *) &a_c_k,sizeof(a_c_k));
							
							
							printf(" client (%s) has joined the chat room.\n",cli_list[t_o_t_cli-1].name_user);
							sent_msg.h_e_a_d.vrsn=3;
							sent_msg.h_e_a_d.type=ONLINE;
							sent_msg.a_t_t[0].type=USERNAME;
							strcpy(sent_msg.a_t_t[0].payload,cli_list[t_o_t_cli-1].name_user);
							sending(sent_msg,rec_cli_list,soc,new, next_cli);
						}
						
						else
						{
							if(flag_user == 1)
							{
								printf("The chatroom is full. client is not allowed. \n");
								
								struct MSG n_a_k;
								struct HEAD n_a_k_h_e_a_d;
								struct ATT n_a_k_a_t_t;
								char t[128];
								n_a_k_h_e_a_d.vrsn =FWD;
								n_a_k_h_e_a_d.type =NAK;
								n_a_k_a_t_t.type = REASON;
								
								strcpy(t,"max no. of clients reached. chatroom is full\n");
								
								
								n_a_k_a_t_t.len = strlen(t);
								strcpy(n_a_k_a_t_t.payload, t);
								n_a_k.h_e_a_d = n_a_k_h_e_a_d;
								n_a_k.a_t_t[0] = n_a_k_a_t_t;
								write(new,(void *) &n_a_k,sizeof(n_a_k));
								close(new);
								
							}
							else
							{
								printf("this name has already been taken. Connection denied. \n");
								
								struct MSG n_a_k;
								struct HEAD n_a_k_h_e_a_d;
								struct ATT n_a_k_a_t_t;
								char t[128];
								n_a_k_h_e_a_d.vrsn =FWD;
								n_a_k_h_e_a_d.type =NAK;
								n_a_k_a_t_t.type = REASON;
								
								strcpy(t,"use a  differnet username as the name is already taken\n");
										
								
								
								n_a_k_a_t_t.len = strlen(t);
								strcpy(n_a_k_a_t_t.payload, t);
								n_a_k.h_e_a_d = n_a_k_h_e_a_d;
								n_a_k.a_t_t[0] = n_a_k_a_t_t;
								write(new,(void *) &n_a_k,sizeof(n_a_k));
								close(new);
							}
							next_cli = t;
							FD_CLR(new, &rec_cli_list);
						}
					}
				}
		
				else
				{
					if ((i_r=read(current,(struct MSG *) &msg_send,sizeof(msg_send))) <= 0)
					{
						int tp=0;
						if (i_r == 0) 
						{
							y=0;
							while(y<t_o_t_cli+1)
							{
								if(cli_list[y].fd==current)
								{
									tp=y;
									sent_msg.a_t_t[0].type=2;
									strcpy(sent_msg.a_t_t[0].payload,cli_list[y].name_user);
									printf("Client (%s) exited the chatroom.\n", cli_list[y].name_user);
								}
								y=y+1;
							}
							
							sent_msg.h_e_a_d.vrsn=3;
							sent_msg.h_e_a_d.type=OFFLINE;
							sending(sent_msg,rec_cli_list,soc,current, next_cli);
						}
						else if(i_r < 0)
						{
							printf("---error reading the server \n");
						}
						close(current); 
						FD_CLR(current, &rec_cli_list);
						
						while(tp<t_o_t_cli)
						{
							cli_list[tp]=cli_list[tp+1];
							tp=tp+1;
						}
						t_o_t_cli=t_o_t_cli-1;
					}
					else
					{
						msg_send_a_t_t = msg_send.a_t_t[0];
						sent_msg=msg_send;
						sent_msg.h_e_a_d.type=FWD;
						sent_msg.a_t_t[1].type=USERNAME;
						int payloadLength=0;
						char t[128];
						payloadLength=strlen(msg_send_a_t_t.payload);
						strcpy(t,msg_send_a_t_t.payload);
						t[payloadLength]='\0';
						int z=0;
						while(z<t_o_t_cli)
						{
							if(cli_list[z].fd==current)
							{
								strcpy(sent_msg.a_t_t[1].payload,cli_list[z].name_user);
							}
							z=z+1;
						}
						sending(sent_msg,rec_cli_list,soc,current, next_cli);
					}
				}
			}
			current=current+1;
		}
	}
	close(soc);
	return 0;
}

