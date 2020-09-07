#if defined(CONFIG_USING_TLS)
#include "http_tls.h"
#include "http_parser.h"

static int get_port(const char *url, struct http_parser_url *u)
{
    if (u->field_data[UF_PORT].len) {
        return strtol(&url[u->field_data[UF_PORT].off], NULL, 10);
    } else {
        if (strncasecmp(&url[u->field_data[UF_SCHEMA].off], "http", u->field_data[UF_SCHEMA].len) == 0) {
            return 80;
        } else if (strncasecmp(&url[u->field_data[UF_SCHEMA].off], "https", u->field_data[UF_SCHEMA].len) == 0) {
            return 443;
        }
    }
    return 0;
}

/**
 * @brief      Create a new TLS/SSL connection with a given "HTTP" url
 */
tls_t *tls_conn_http_new(const char *url, const tls_cfg_t *cfg)
{
    /* Parse URI */
    struct http_parser_url u;
    http_parser_url_init(&u);
    http_parser_parse_url(url, strlen(url), 0, &u);

    /* Connect to host */
    return tls_conn_new(&url[u.field_data[UF_HOST].off], u.field_data[UF_HOST].len,
			    get_port(url, &u), cfg);
}

/**
 * @brief      Create a new non-blocking TLS/SSL connection with a given "HTTP" url
 */
int tls_conn_http_new_async(const char *url, const tls_cfg_t *cfg, tls_t *tls)
{
    /* Parse URI */
    struct http_parser_url u;
    http_parser_url_init(&u);
    http_parser_parse_url(url, strlen(url), 0, &u);

    /* Connect to host */
    return tls_conn_new_async(&url[u.field_data[UF_HOST].off], u.field_data[UF_HOST].len,
			    get_port(url, &u), cfg, tls);
}
#endif