#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "hb_pub.h"
#include "list.h"

hb_op_s *MASTER_HB_TableList = NULL;

extern int epoll_setup(cfg_udp_con_t *cfg);
#if 1
/*  todo ...
    add fds epoll
    ctl_fuc
    wait_fuc
    del_fds
    event???
    
*/
int send_heartbeat_msg(cfg_udp_con_t *cfg) 
{
	printf("send_heartbeat_msg_ start ...\n");
	int send_len = 0;
	send_len = sendto(cfg->socket_fd, cfg->hb_msg, cfg->hb_len, 0,
				(struct sockaddr *) cfg->remote_addr, cfg->remote_addr_len);
    if (send_len > 0){
		/*connect ok*/
		printf("send_heartbeat_msg_ ok !!!\n");
		printf("succeed transfer data to %s:%u\n",
				inet_ntoa(cfg->remote_addr->sin_addr), ntohs(cfg->remote_addr->sin_port));
	} else{
        printf("send_heartbeat_msg_ failed !!!\n");
        //show_socket_error_reason(cfg->socket_fd);
		close(cfg->socket_fd);
        return HB_SEND_FAILED;
	}
	return HB_SEND_OK;
}

int recv_heartbeat_msg(cfg_udp_con_t *cfg,char* data,int len)
{   
    int ret = 0;
    len = recvfrom(cfg->socket_fd, data, sizeof(data), 0,
				(struct sockaddr *) cfg->remote_addr, &(cfg->remote_addr_len));
    if(len < 0)
    {
        printf("recvfrom error,len = %d\n",len);
        //show_socket_error_reason(cfg->socket_fd);
		close(cfg->socket_fd);
        return -1;
    }
    
    return ret;
}

int heartbeat_bind_local(cfg_udp_con_t *cfg)
{
	struct sockaddr_in local_addr;
	local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    local_addr.sin_port = htons(cfg->server_port);
	if(bind(cfg->socket_fd, (struct sockaddr *) &local_addr, sizeof(local_addr)) < 0) {
        printf("bind local address error\n");
        close(cfg->socket_fd);
		return HB_BIND_FAILED;
    }
    /*add epoll_fd*/
    // kdpfd = epoll_create(MAXEPOLLSIZE);
	return HB_BIND_OK;
}

int create_udp_socket(cfg_udp_con_t *cfg)
{
	printf("create_udp_client\n");
	printf("connecting to %s:%d\n",cfg->server_ip, cfg->server_port);
	int ret = 0;

    cfg->remote_addr->sin_family = AF_INET;
	cfg->remote_addr->sin_port = htons(cfg->server_port);
	cfg->remote_addr->sin_addr.s_addr = inet_addr(cfg->server_ip);

	cfg->socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
	if (cfg->socket_fd < 0){
		//show_socket_error_reason(cfg->socket_fd); todo
		return HB_CREATED_FAILED;
	}

	//bind local port
	if((ret=heartbeat_bind_local(cfg)) == HB_BIND_FAILED)
		return HB_BIND_FAILED;
    
    //epoll setup
    if(ret = epoll_setup(cfg) != 0)
        return G_FAILED;
    
	return HB_CREATED_OK;
}

int heartbeat_create_client(cfg_udp_con_t *cfg)
{
    int ret = 0;

    ret = create_udp_socket(cfg);
    if (ret != HB_CREATED_OK){
		printf("client create socket error!!! \n");
        return ret;
	}
	printf("client create socket Succeed !!! \n");

	return HB_CREATED_OK;
}

/*****************fd ops todo***************************/
int hb_epoll_create()
{
    if(MASTER_HB_TableList->epoll_fd == 0)
        MASTER_HB_TableList->epoll_fd = epoll_create(MAXEPOLLSIZE);
    printf("epoll_fd = %d\n",MASTER_HB_TableList->epoll_fd);
    return 0;
}

int hb_epoll_ctl(int set_fd)
{
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = set_fd;
    if (epoll_ctl(MASTER_HB_TableList->epoll_fd, EPOLL_CTL_ADD, set_fd, &ev) < 0)
    {
        fprintf(stderr, "epoll set insertion error: fd=%d\n", set_fd);
        return -1;       
    }
    return 0;
}

int all_list_get_fd()
{
    struct listnode *node;
    cfg_udp_con_t *data1 = NULL;
    void *data = NULL;
    for (ALL_LIST_ELEMENTS_RO(MASTER_HB_TableList->hb_cfg_list, node, data))
    {
        data1 = (cfg_udp_con_t *)data;
        return data1->socket_fd;
    }
    return -1;
}

void* hb_epoll_wait()
{
    int n = 0;
    int nfds,recv_buf_len;
    // void *data = NULL;
    // struct listnode *node;
    // cfg_udp_con_t * cfg_data = NULL;
    char recv_buf[DEFAULT_PKTSIZE];
    struct epoll_event events[MAXEPOLLSIZE];
    
    printf("---------------- hb_epoll_wait start !\n");
    while(1) 
    {
        nfds = epoll_wait(MASTER_HB_TableList->epoll_fd, events, MAXEPOLLSIZE, -1);
        if (nfds == -1)
        {
            printf("epoll_wait\n");
            continue;
        }
        for (n = 0; n < nfds; ++n)
        {
            /*判断链表是否有此fd，则回调 or*/
            
            void *data = NULL;
            struct listnode *node;
            cfg_udp_con_t * cfg_data = NULL;

            // data = (cfg_udp_con_t*)malloc(sizeof(cfg_udp_con_t));
            // node = (struct listnode*)malloc(sizeof(struct listnode));
            // cfg_data = (cfg_udp_con_t*)malloc(sizeof(cfg_udp_con_t));
            /*为什么memset会挂
                因为在memset之前需要对结构体指针申请空间
            */
            // memset((cfg_udp_con_t*)data,0,sizeof(cfg_udp_con_t));
            // memset(node,0,sizeof(struct listnode));
            // memset(cfg_data,0,sizeof(cfg_udp_con_t));

            
            
            for (ALL_LIST_ELEMENTS_RO(MASTER_HB_TableList->hb_cfg_list, node, data))
            {
                cfg_data = (cfg_udp_con_t *)data;
                if (events[n].data.fd == cfg_data->socket_fd)
                {
                    printf("----------------------epoll wait ... fd = %d,name = %s\n", cfg_data->socket_fd,cfg_data->name);
                    recv_buf_len =0;
                    memset(recv_buf,0,sizeof(recv_buf));
                    recv_buf_len = recv_heartbeat_msg(cfg_data,recv_buf,recv_buf_len);
                    cfg_data->hb_data_ops_cb(0,recv_buf,recv_buf_len);
                }
            }
        }
      }

    return NULL;
}

int epoll_setup(cfg_udp_con_t *cfg)
{
    int ret = 0;
    ret = hb_epoll_create();
    ret |= hb_epoll_ctl(cfg->socket_fd);
    if(ret)
    {
        printf("epoll_setup_ failed!\n");
        return -1;
    }
    return 0;
}
/*********************************************/
#endif
/*MASTER_HB_TableList list node del*/
void cfg_hb_cfg_free(cfg_udp_con_t *re_node)
{
    free(re_node);
}

/*MASTER_HB_TableList list and node del*/
void cfg_hb_cfg_delete(cfg_udp_con_t *re_node)
{
    listnode_delete(MASTER_HB_TableList->hb_cfg_list,re_node);
    cfg_hb_cfg_free(re_node);
}

/*create MASTER_HB_TableList*/
hb_op_s* hb_master_create()
{
    if(MASTER_HB_TableList != NULL)
        return MASTER_HB_TableList;

    MASTER_HB_TableList = (hb_op_s*)malloc(sizeof(hb_op_s));
    if (!MASTER_HB_TableList)
    {
        printf("not enough memory for create MASTER_HB_TableList\n");
        return NULL;
    }
    memset(MASTER_HB_TableList,0,sizeof(hb_op_s));

    MASTER_HB_TableList->hb_cfg_list = list_new();
    MASTER_HB_TableList->hb_cfg_list->del = (void (*) (void *))cfg_hb_cfg_free;
    MASTER_HB_TableList->epoll_fd = 0;

    return MASTER_HB_TableList;
}
/*********************************/
int hb_send(cfg_udp_con_t *ops)
{   
    int ret = 0;
    ret = send_heartbeat_msg(ops);
    if (HB_SEND_FAILED == ret)
        return G_FAILED;
    return G_SUCESSS;
}

int heartbeat_recv(cfg_udp_con_t *ops)
{
    int ret = 0;
    /*1.创建一个线程 做recv动作*/
    /*2.添加定时器,超时就返回？还是超时继续等待？或者是重连？*/
    

    return 0;
}

int hb_epoll_recv(cfg_udp_con_t *ops)
{
    int ret = 0;
    hb_epoll_wait();

    return 0;
}

int hb_deal(cfg_udp_con_t *ops)
{
    int ret = 0;
    pthread_t tidp;
    /*需要一个进程去运行?*/
    printf("----------count = %d\n",MASTER_HB_TableList->hb_cfg_list->count);
    if(MASTER_HB_TableList->hb_cfg_list->count == 1)
    {
        printf("pthread_create start!\n");
        if ((pthread_create(&tidp, NULL, hb_epoll_wait, NULL)) == -1)
        {
            printf("create error!\n");
            return -1;
        }
    }
    //hb_epoll_wait();

    return 0;
}

int hb_cfg_set_up(cfg_udp_con_t *ops)
{
    /*create and bind*/
    int ret = 0;
    ret = heartbeat_create_client(ops);
    if(ret != HB_CREATED_OK)
        return G_FAILED;
     return G_SUCESSS;
}
/*********************************/

/*在MASTER_HB_TableList上注册cfg_udp_con_t的参数*/
int register_sub_hb_cfg(cfg_udp_con_t *ops)
{
    int ret = 0;
    listnode_add_sort(MASTER_HB_TableList->hb_cfg_list, ops);
    /*1.set_up cfg 连接 bind 创建进程*/
    ret = hb_cfg_set_up(ops);
    /*connet and send*/
    ret |= hb_send(ops);
    /*recv*/
    //ret |= heartbeat_recv(ops);
    
    //ret |= hb_epoll_recv(ops);
    //printf("-------------------deal 1\n");
    ret |= hb_deal(ops);
    //printf("-------------------deal 2\n");

    return 0;
}

int unregister_sub_hb_cfg(cfg_udp_con_t *ops)
{
    cfg_hb_cfg_free(ops);
    return 0;
}

hb_op_s* master_hb_tablelist_init()
{
    MASTER_HB_TableList = hb_master_create();
    if (!MASTER_HB_TableList)
    {
        printf("register_sub_hb not enough memory for create MASTER_HB_TableList\n");
        return NULL;
    }
    return MASTER_HB_TableList;
}

int master_hb_tablelist_exit()
{
    //free
    //pthread_join(tidp, NULL)
    return 0;
}

/*以下方案无法共用一个epoll_fd*/
// int register_sub_hb(struct hb_op *ops)
// {
//     listnode_add_sort(HB_TableList, ops);
//     return 0;
// }

// int unregister_sub_hb(struct hb_op *ops)
// {
//     listnode_delete(HB_TableList,ops);
//     return 0;
// }

// int hb_tablelist_init()
// {
//     HB_TableList = list_new();
// 	if(HB_TableList == NULL)
// 	    return -1;

// 	HB_TableList->cmp = NULL;
//     return 0;
// }