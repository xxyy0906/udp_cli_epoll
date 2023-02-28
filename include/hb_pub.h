#ifndef __HB_PUB_H__
#define  __HB_PUB_H__

#define G_SUCESSS                0
#define G_FAILED                 -1
#define HB_CON_LEN16             17
#define DEFAULT_PKTSIZE          1024
#define MAXEPOLLSIZE             10000

typedef enum {
    HB_CONNCETED_OK,
    HB_CONNCETED_FAILED,
    HB_CREATED_OK,
    HB_CREATED_FAILED,
    HB_BIND_FAILED,
    HB_BIND_OK,
    HB_SEND_OK,
    HB_SEND_FAILED,
    HB_ERROR_MAX,
} hb_error_num_t;
typedef void (*pf_hb_data_cb_t)(char* type, char *data, int data_len);

typedef struct cfg_udp_con
{
    char name[HB_CON_LEN16];
	char server_ip[HB_CON_LEN16];
	int  server_port;
	int  socket_fd;
    struct sockaddr_in *remote_addr;
    unsigned int remote_addr_len;
    char hb_msg[DEFAULT_PKTSIZE];
    int  hb_len;
    pf_hb_data_cb_t hb_data_ops_cb;

}cfg_udp_con_t;

typedef struct hb_op
{
    int epoll_fd;
    // cfg_udp_con_t msg;
    struct list* hb_cfg_list;
}hb_op_s;


extern hb_op_s *MASTER_HB_TableList;
extern int register_sub_hb_cfg(cfg_udp_con_t *ops);
extern int unregister_sub_hb_cfg(cfg_udp_con_t *ops);
extern hb_op_s* master_hb_tablelist_init(void);
extern int master_hb_tablelist_exit(void);
#endif
