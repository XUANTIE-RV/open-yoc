/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <aos/aos.h>

static char* g_url;

void test()
{
#define REQ_BUF_SIZE 2048
    static char req_buf[REQ_BUF_SIZE];
#define RSP_BUF_SIZE 2048
    static char rsp_buf[RSP_BUF_SIZE];
    httpclient_t client = { 0 };
    httpclient_data_t client_data = {0};
    int ret, total_len = 0;
    int recv_len = 0, recv_total_len = 0;
    char * customer_header = "Accept: */*\r\n";
    long long start = aos_now_ms();

    memset(req_buf, 0, sizeof(req_buf));
    client_data.header_buf = req_buf;
    client_data.header_buf_len = sizeof(req_buf);

    memset(rsp_buf, 0, sizeof(rsp_buf));
    client_data.response_buf = rsp_buf;
    client_data.response_buf_len = sizeof(rsp_buf);

    ret = httpclient_conn(&client, g_url);
    if(HTTP_SUCCESS != ret ) {
        LOGE(TAG, "http connect failed");
        return;
    }

    httpclient_set_custom_header(&client, customer_header);

    ret = httpclient_send(&client, g_url, HTTP_GET, &client_data);
    if(HTTP_SUCCESS != ret) {
        LOGE(TAG, "http send request failed");
        return;
    }

    while (total_len == 0 || recv_total_len < total_len) {
        ret = httpclient_recv(&client, &client_data);
        if (ret == HTTP_SUCCESS || ret == HTTP_EAGAIN) {
            recv_len = client_data.content_block_len;
            /*
             * Fetch data from client_data.response_buf
             * according to recv_len
             */
        } else {
            recv_len = 0;
        }

        if (ret < 0) {
            break;
        }

        /* first time */
        if (total_len <= 0) {
            int val_pos, val_len;
            if(0 == httpclient_get_response_header_value(client_data.header_buf, "Content-Length", (int *)&val_pos, (int *)&val_len)) {
                sscanf(client_data.header_buf + val_pos, "%d", &total_len);
            }
        }

        recv_total_len += recv_len;

        LOGI(TAG, "Total recv %d / %d \n", recv_total_len, total_len);
    }

    LOGI(TAG,"Finished. Duration %d ms\n", (int)(aos_now_ms() - start));

    httpclient_clse(&client);
    aos_freep(&g_url);
    return;
}

