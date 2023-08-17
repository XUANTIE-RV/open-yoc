#include "JSHttpImpl.h"
#include "port/jquick_time.h"
#include <http_client.h>

#include <unistd.h>
#include <stdio.h>

namespace aiot {

static char ca_crt_rsa[] = {
"-----BEGIN CERTIFICATE-----\r\n"
"MIIEdTCCA12gAwIBAgIJAKcOSkw0grd/MA0GCSqGSIb3DQEBCwUAMGgxCzAJBgNV\r\n"
"BAYTAlVTMSUwIwYDVQQKExxTdGFyZmllbGQgVGVjaG5vbG9naWVzLCBJbmMuMTIw\r\n"
"MAYDVQQLEylTdGFyZmllbGQgQ2xhc3MgMiBDZXJ0aWZpY2F0aW9uIEF1dGhvcml0\r\n"
"eTAeFw0wOTA5MDIwMDAwMDBaFw0zNDA2MjgxNzM5MTZaMIGYMQswCQYDVQQGEwJV\r\n"
"UzEQMA4GA1UECBMHQXJpem9uYTETMBEGA1UEBxMKU2NvdHRzZGFsZTElMCMGA1UE\r\n"
"ChMcU3RhcmZpZWxkIFRlY2hub2xvZ2llcywgSW5jLjE7MDkGA1UEAxMyU3RhcmZp\r\n"
"ZWxkIFNlcnZpY2VzIFJvb3QgQ2VydGlmaWNhdGUgQXV0aG9yaXR5IC0gRzIwggEi\r\n"
"MA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDVDDrEKvlO4vW+GZdfjohTsR8/\r\n"
"y8+fIBNtKTrID30892t2OGPZNmCom15cAICyL1l/9of5JUOG52kbUpqQ4XHj2C0N\r\n"
"Tm/2yEnZtvMaVq4rtnQU68/7JuMauh2WLmo7WJSJR1b/JaCTcFOD2oR0FMNnngRo\r\n"
"Ot+OQFodSk7PQ5E751bWAHDLUu57fa4657wx+UX2wmDPE1kCK4DMNEffud6QZW0C\r\n"
"zyyRpqbn3oUYSXxmTqM6bam17jQuug0DuDPfR+uxa40l2ZvOgdFFRjKWcIfeAg5J\r\n"
"Q4W2bHO7ZOphQazJ1FTfhy/HIrImzJ9ZVGif/L4qL8RVHHVAYBeFAlU5i38FAgMB\r\n"
"AAGjgfAwge0wDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMCAYYwHQYDVR0O\r\n"
"BBYEFJxfAN+qAdcwKziIorhtSpzyEZGDMB8GA1UdIwQYMBaAFL9ft9HO3R+G9FtV\r\n"
"rNzXEMIOqYjnME8GCCsGAQUFBwEBBEMwQTAcBggrBgEFBQcwAYYQaHR0cDovL28u\r\n"
"c3MyLnVzLzAhBggrBgEFBQcwAoYVaHR0cDovL3guc3MyLnVzL3guY2VyMCYGA1Ud\r\n"
"HwQfMB0wG6AZoBeGFWh0dHA6Ly9zLnNzMi51cy9yLmNybDARBgNVHSAECjAIMAYG\r\n"
"BFUdIAAwDQYJKoZIhvcNAQELBQADggEBACMd44pXyn3pF3lM8R5V/cxTbj5HD9/G\r\n"
"VfKyBDbtgB9TxF00KGu+x1X8Z+rLP3+QsjPNG1gQggL4+C/1E2DUBc7xgQjB3ad1\r\n"
"l08YuW3e95ORCLp+QCztweq7dp4zBncdDQh/U90bZKuCJ/Fp1U1ervShw3WnWEQt\r\n"
"8jxwmKy6abaVd38PMV4s/KCHOkdp8Hlf9BRUpJVeEXgSYCfOn8J3/yNTd126/+pZ\r\n"
"59vPr5KW7ySaNRB6nJHGDn2Z9j8Z3/VyVOEVqQdZe4O/Ui5GjLIAZHYcSNPYeehu\r\n"
"VsyuLAOQ1xk4meTKCRlb/weWsKh/NEnfVqn3sF/tM+2MR7cwA130A4w=\r\n"
"-----END CERTIFICATE-----\r\n"
" "
};

class ImplCtx: public JQuick::REF_BASE {
public:
    ImplCtx() {
        contentLength = 0;
        progressLastTime = 0;
        progressLastSize = 0;
        fp = NULL;
    }
    JQuick::sp<Request> req;
    JQuick::sp<Response> rep;

    // JQuick::sp<ImplThreadSingleton> threadRef;
    JQuick::sp<JQuick::Handler> handler;

    long contentLength;
    long progressLastTime;
    long progressLastSize;
    FILE *fp;
};

int _http_event_handler(http_client_event_t *evt)
{
    ImplCtx* ctx = (ImplCtx*)evt->user_data;
    switch(evt->event_id) {
        case HTTP_EVENT_ON_HEADER:
            printf("HTTP_EVENT_ON_HEADER, key=%s, value=%s\n", evt->header_key, evt->header_value);
            if (ctx != NULL) {
                ctx->rep->headers[std::string(evt->header_key)] = std::string(evt->header_value);
                if (strcmp("content-length", evt->header_key) == 0) {
                    ctx->contentLength = atoi(evt->header_value);
                }
            }
            break;
        case HTTP_EVENT_ON_DATA:
            //printf("HTTP_EVENT_ON_DATA, len=%d\n", evt->data_len);
            if (!http_client_is_chunked_response(evt->client)) {
                // Write out data
                // printf("%.*s", evt->data_len, (char*)evt->data);
                if (ctx != NULL) {
                    if (ctx->fp != NULL) {
                        int s = 0;
                        if ((s = fwrite(evt->data, 1, evt->data_len, ctx->fp)) != evt->data_len) {
                            printf("http download write to file failed, data_len=%d, write=%d\n", evt->data_len, s);
                            return HTTP_CLI_FAIL;
                        }
                        ctx->progressLastSize += s;
                        if (ctx->req->progressCb) {
                            int64_t n = jquick_get_current_time();
                            if (ctx->progressLastTime + ctx->req->progressInterval <= n || ctx->progressLastSize >= ctx->contentLength) {
                                ctx->req->progressCb(0, ctx->progressLastSize, ctx->contentLength);
                                ctx->progressLastTime = n;
                            }
                        }
                    } else {
                        int idx = ctx->rep->body.size();
                        ctx->rep->body.resize(idx + evt->data_len);
                        memcpy((uint8_t*)ctx->rep->body.data() + idx, evt->data, evt->data_len);
                    }
                }
            }
            break;
        default:
            printf("http client unhandle event %d\n", evt->event_id);
            break;
    }
    return HTTP_CLI_OK;
}

void JSHttpImpl::request(JQuick::sp<Request> req)
{
    // TODO: 自定义 ImplCtx(可选) 调用一系列下载接口设定下载参数，发起下载，并将返回数据和状态设置到 req中
    //  最后调用 req.finishCb，如果是流式下载可调用 req.progressCb 通知JS层下载进度
    JQuick::sp<ImplCtx> implCtx = new ImplCtx();

    JQuick::sp<Response> rep = new Response();
    // request 持有了请求参数
    implCtx->req = std::move(req);
    // response 将会持有一些请求返回的数据和状态
    implCtx->rep = std::move(rep);
    int timeout_ms = req->timeout > 0 ? req->timeout : 10000;
    if (!req->outPath.empty()) {
        // download to file
        timeout_ms = req->timeout > 0 ? req->timeout : 0;
        // TODO mkdirs
        implCtx->fp = fopen(req->outPath.c_str(), "wb");
        if (implCtx->fp == NULL) {
            rep->ret = HTTP_CLI_FAIL;
            printf("HTTP download failed: unable to create file\n");
            if (req->finishCb) {
                req->finishCb(req, rep);
            }
            return;
        }
    }
    http_client_config_t config = {
        .url = req->url.c_str(),
        .cert_pem = ca_crt_rsa,
        .timeout_ms = timeout_ms,
        .event_handler = _http_event_handler,
        .user_data = implCtx.get(),
    };
    http_client_handle_t client = http_client_init(&config);
    http_errors_t err;
    for (auto h : req->headers) {
        http_client_set_header(client, h.first.c_str(), h.second.c_str());
    }
    bool set_post_data = false;
    if (req->method == "POST") {
        http_client_set_method(client, HTTP_METHOD_POST);
        set_post_data = true;
    } else if (req->method == "PUT") {
        http_client_set_method(client, HTTP_METHOD_PUT);
        set_post_data = true;
    } else if (req->method == "PATCH") {
        http_client_set_method(client, HTTP_METHOD_PATCH);
        set_post_data = true;
    } else if (req->method == "DELETE") {
        http_client_set_method(client, HTTP_METHOD_DELETE);
        set_post_data = true;
    } else if (req->method == "HEAD") {
        http_client_set_method(client, HTTP_METHOD_HEAD);
    } else if (req->method == "OPTIONS") {
        http_client_set_method(client, HTTP_METHOD_OPTIONS);
    } else if (req->method == "GET") {
        http_client_set_method(client, HTTP_METHOD_GET);
    }
    if (set_post_data && !req->body.empty()) {
        http_client_set_post_field(client, req->body.c_str(), req->body.size());
    }
    
    err = http_client_perform(client);
    if (err == HTTP_CLI_OK) {
        rep->status = http_client_get_status_code(client);
        printf("HTTP request Status = %d, content_length = %d\n",
                http_client_get_status_code(client),
                http_client_get_content_length(client));
        printf("++++++++++++++ HTTP request OK\n");
    } else {
        if (!req->outPath.empty()) {
            remove(req->outPath.c_str());
        }
        rep->ret = err;
        printf("HTTP request failed: 0x%x @#@@@@@@\n", (err));
        if (req->progressCb) {
            req->progressCb(1, 0, 0);
        }
    }
    http_client_cleanup(client);
    if (implCtx->fp != NULL) {
        fclose(implCtx->fp);
        implCtx->fp = NULL;
    }
    if (req->finishCb) {
        req->finishCb(req, rep);
    }
}

}  // namespace aiot
