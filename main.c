#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "./include/hb_pub.h"
#include "./include/list.h"

#define SERVER_IP       "192.168.177.1"
#define SERVER_PORT     50018

void hb1_ops_cb(char *msg_type, char *data, int data_len)
{
    printf("=========1 this is hb1_ops_cb!!! data = %s\n",data);
    return;
}

void hb2_ops_cb(char *msg_type, char *data, int data_len)
{
    printf("=========2 this is hb2_ops_cb!!! data = %s\n",data);
    return;
}

int main()
{
    //while (1)
    {
        printf("=========== in\n");
        int ret = 0;
        int socket_fd1 = 0;
        int socket_fd2 = 0;
        struct sockaddr_in server_addr;
        char sendBuff1[] = "this is comfrom HB1 !!!\n";
        char sendBuff2[] = "this is comfrom HB2 !!!\n";

        hb_op_s *master_hb_list = NULL;
        cfg_udp_con_t *udp1;
        udp1 = (cfg_udp_con_t*)malloc(sizeof(cfg_udp_con_t));
        if(!udp1)
            printf("udp1 malloc failed !\n");
            
        memset(udp1,0,sizeof(cfg_udp_con_t));
        strcpy(udp1->server_ip,SERVER_IP);
        /*step 1*/
        strncpy(udp1->name,"udp111111111",HB_CON_LEN16);
        udp1->name[HB_CON_LEN16 -1] = '\0';
        udp1->server_port = SERVER_PORT;
        udp1->socket_fd = socket_fd1;
        udp1->remote_addr = &server_addr;
        udp1->remote_addr_len = sizeof(server_addr);
        strcpy(udp1->hb_msg,sendBuff1);
        udp1->hb_len = sizeof(sendBuff1);
        udp1->hb_data_ops_cb = hb1_ops_cb;

        master_hb_list = master_hb_tablelist_init();
        register_sub_hb_cfg(udp1);
        
        printf("count = %d\n",master_hb_list->hb_cfg_list->count);

        /*------------------------------------*/
        cfg_udp_con_t *udp2;
        udp2 = (cfg_udp_con_t*)malloc(sizeof(cfg_udp_con_t));
        if(!udp2)
            printf("udp2 malloc failed !\n");

        memset(udp2,0,sizeof(cfg_udp_con_t));
        strcpy(udp2->server_ip,SERVER_IP);
        /*step 1*/
        strncpy(udp2->name,"udp2222222222222",HB_CON_LEN16);
        udp2->name[HB_CON_LEN16 -1] = '\0';
        udp2->server_port = 50019;
        udp2->socket_fd = socket_fd2;
        udp2->remote_addr = &server_addr;
        udp2->remote_addr_len = sizeof(server_addr);
        strcpy(udp2->hb_msg,sendBuff2);
        udp2->hb_len = sizeof(sendBuff2);
        udp2->hb_data_ops_cb = hb2_ops_cb;

        master_hb_list = master_hb_tablelist_init();
        register_sub_hb_cfg(udp2);
        /*------------------------------------*/
        struct listnode *node;
        cfg_udp_con_t *data1 = NULL;
        void *data = NULL;
        for (ALL_LIST_ELEMENTS_RO(master_hb_list->hb_cfg_list, node, data))
        {
            data1 = (cfg_udp_con_t *)data;
            //printf("server_port = %d\n",data1->server_port);
        }
        printf("count1 = %d\n",master_hb_list->hb_cfg_list->count);

        printf("=========== out\n");
        
    }
    while(1);
    return 0;
}