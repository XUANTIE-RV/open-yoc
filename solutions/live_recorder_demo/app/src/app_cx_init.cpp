/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <string>
#include <cx/init.h>
#include <cv_config.h>
#include <yoc/partition.h>
#include <ulog/ulog.h>
#include <aos/aos.h>
#include <cJSON.h>
#if defined(CONFIG_OTA_AB) && (CONFIG_OTA_AB > 0)
#include <yoc/ota_ab.h>
#endif

#define TAG "app"

#ifdef __cplusplus
extern "C" {
#endif

int app_cx_hw_init();
int app_cx_svr_init();

#ifdef __cplusplus
}
#endif

typedef struct {
	char 	 *name;
	uint32_t offset;
	uint32_t size;
} weight_info_t;

char g_alg_weight[1024 * 8704];  // 8.5M
static int g_weight_info_len = 0;
static weight_info_t *g_weight_info = NULL;
static char part_name[MTB_IMAGE_NAME_SIZE];

const static char *g_cx_memconfig = R"({	
	"flash2mem": 	
    [	
        {	
            "name": "model_facedet"	
        },
        {	
            "name": "model_faceldmk"
        },
        {	
            "name": "model_quality"
        },
        {	
            "name": "model_single_ir"
        },
        {	
            "name": "model_facerecog"
        }	
    ]	
})";


/*
* weight bin format as follows:
*|--4 bytes--|-- 16 bytes --|-- 4 bytes --|-- x bytes --|--512*M-x-4 bytes--|-- N1 bytes --|-- N2 bytes --|   .....  |-- Nm bytes --|
*|-- magic --|--md5 check --| header size |    header   |      paddings     |    weight1   |    weight1   |   .....  |    weight1   |
* 
* magic: 0x57454754 - 'WEGT'
* md5 check: md5 checksum from header size till end of image
* header size: size of header and paddings
* header: in json format, e.g.:
*               {'model_facedet.params': [512, 169340], 'model_single_ir.params': [169852, 158112]}
*
*/
static int parse_weight_bin(weight_info_t **weight_info)
{
	int ret = 0;
	partition_t handle;
	uint32_t header_size;
	char *header_bytes = NULL;
	cJSON *root = NULL;
	char magic_md5_hsize[24];

	strlcpy(part_name, "weight", sizeof(part_name));
#if defined(CONFIG_OTA_AB) && (CONFIG_OTA_AB > 0)
	snprintf(part_name, sizeof(part_name), "weight%s", otaab_get_current_ab());
	handle = partition_open(part_name);
#else
	handle = partition_open(part_name);
#endif
	if (handle < 0) {
		LOGE(TAG, "open %s partition failed", part_name);
		return -1;
	}
	LOGD(TAG, "open %s partition ok.", part_name);

	ret = partition_read(handle, 0, magic_md5_hsize, sizeof(magic_md5_hsize));
	if (ret < 0) {
		ret = -1;
		goto END;
	}

	header_size = *(uint32_t *)(magic_md5_hsize + 20);

	if (strncmp(magic_md5_hsize, "WEGT", 4) != 0) {
		LOGE(TAG, "weight magic check failed");
		ret = -1;
	} else if (header_size == 0 || header_size > 4096) {
		LOGE(TAG,"header size oversize %u", header_size);
		ret = -1;
	} else {
		LOGD(TAG, "weight header len=%u\n", header_size);

		header_bytes = (char *)aos_malloc(header_size + 1);
		partition_read(handle, sizeof(magic_md5_hsize), header_bytes, header_size);
		header_bytes[header_size] = 0;
				
		cJSON *item;
		int	item_num = 0;
		root = cJSON_Parse(header_bytes);
		if (!root) {
			LOGW(TAG, "not json: %s\n", header_bytes);
			goto END;
		}		

		/* validate the json format, and get the item number */
		item = root->child;
		while (item) {
			if (item->type != cJSON_Array) {
				LOGW(TAG, "item not array\n");
				ret = -1;
				goto END;
			}

			/* should have only two points: offset and size */
			if (cJSON_GetArraySize(item) != 2) {
				LOGW(TAG, "item array size err\n");
				ret = -1;
				goto END;
			}

			item = item->next;
			item_num ++;
		}

		if (item_num <= 0) {
			LOGW(TAG, "no items\n");
			goto END;
		} 

		/* extract the json content */
		int item_i = 0;
		*weight_info = (weight_info_t *)aos_malloc(sizeof(weight_info_t) * item_num);
		item = root->child;
		while (item) {
			(*weight_info)[item_i].name 	= strdup(item->string);
			(*weight_info)[item_i].offset 	= (uint32_t)(cJSON_GetArrayItem(item, 0)->valuedouble);
			(*weight_info)[item_i].size 	= (uint32_t)(cJSON_GetArrayItem(item, 1)->valuedouble);

			item = item->next;
			item_i ++;
		}

		ret = item_num;
	}

END:
	free(header_bytes);
	cJSON_Delete(root);
	partition_close(handle);
	return ret;
}

int mem_load_cb(const char *weight_name, uint64_t flash_addr, uint64_t ram_addr, uint32_t size)
{
	LOGD(TAG, "mem_load_cb name=%s flash_addr=%u ram_addr=%u size=%u\n", weight_name, flash_addr, ram_addr, size);

	(void)flash_addr;
	(void)size;

 	if (!g_weight_info || g_weight_info_len <= 0) {
		LOGW(TAG, "weight info get error\n");
		return -1;
	}

	int weight_index = -1;
	for (int i = 0; i < g_weight_info_len; i++) {
		if (strcmp(g_weight_info[i].name, weight_name) == 0) {
			weight_index = i;
			break;
		}
	}

	if (weight_index < 0) {
		LOGW(TAG, "weight %s not found\n", weight_name);
		return -1;
	}
	
	partition_t handle = partition_open(part_name);
	if (handle < 0) {
		LOGW(TAG, "open weight partition failed\n");
		return -1;
	}

	partition_read(handle, g_weight_info[weight_index].offset, (void *)ram_addr, g_weight_info[weight_index].size);
	
	partition_close(handle);
	
	return 0;
}

static char *load_weight_config(const char *mem_config, weight_info_t *infos, int weight_len)
{
	char *config = NULL;
	int config_num;
	cJSON *root = cJSON_Parse(mem_config);
	cJSON *js_memconfig = cJSON_GetObjectItem(root, "flash2mem");
	if (root == NULL || js_memconfig == NULL || !cJSON_IsArray(js_memconfig)) {
		LOGE(TAG, "mem_config parse failed\n");
		goto END;
	}
	
	config_num = cJSON_GetArraySize(js_memconfig);
	for (int i = 0; i < weight_len; i++) {
		cJSON *model = NULL;
		bool found = false;

		for (int j = 0; j < config_num; j++) {
			model = cJSON_GetArrayItem(js_memconfig, j);
			cJSON *mname = cJSON_GetObjectItem(model, "name");
			if (mname == NULL) goto END;

			if (strcmp(mname->valuestring, infos[i].name) == 0) {
				found = true;
				break;
			}
		}

		if (!found) {
			LOGE(TAG, "model not found\n");
			goto END;
		}

		/* put all weights into g_alg_weight, aligned the same as in the flash, just eliminate the header */
		cJSON_AddNumberToObject(model, "ram_addr", 	(infos[i].offset - infos[0].offset) + (uint64_t)&g_alg_weight);
		cJSON_AddNumberToObject(model, "size", 		infos[i].size);

		LOGD(TAG, "cx weight name=%s offset=%u size=%u\n", infos[i].name, infos[i].offset, infos[i].size);
	}

	config = cJSON_PrintUnformatted(root);
	
END:
	if (config == NULL) {
		LOGW(TAG, "weight load failed\n");
	}
	cJSON_Delete(root);
	
	return config;
}

int app_cx_hw_init()
{
	int ret;

    cv_params_init();

	/* parse weight partition header, to get the model address/size info */
	g_weight_info_len = parse_weight_bin(&g_weight_info);
	if (g_weight_info == NULL || g_weight_info_len <= 0) {
		LOGW(TAG, "parse weight header failed\n");
		// return -1;
	}

	char *mem_config = load_weight_config(g_cx_memconfig, g_weight_info, g_weight_info_len);
	if (mem_config == NULL) {
		LOGW(TAG, "get cx mem config failed\n");
		return -1;
	} else {
		LOGD(TAG, "cx mem config: %s\n", mem_config);
	}

	cx_mem_config_t cx_mem_config = {mem_config, mem_load_cb};
	// extern const char *g_cx_boardconfig;
	ret = cx_hardware_init(get_cx_board_config(), &cx_mem_config);
	free(mem_config);

	return ret;
}

int app_cx_svr_init()
{
	return cx_service_init(get_cx_service_config(), get_cx_input_channel_config());
}

