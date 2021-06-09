/*
 * Copyright (C) 2020 C-SKY Microsystems Co., Ltd. All rights reserved.
 * 
 * Licensed under the Apache License, Version 2.0 (the 'License');
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an 'AS IS' BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions an
 * limitations under the License.
 */

#include <dma_test.h>

#define DMA_DATA_WIDTH_BYTE_1	1
#define DMA_DATA_WIDTH_BYTE_2	2
#define DMA_DATA_WIDTH_BYTE_4	4

volatile static uint8_t lock[16] = {0};
static void dma_callback(int32_t ch, dma_event_e event, void *arg)
{
	if (event == DMA_EVENT_TRANSFER_DONE)
		lock[ch] = 0;
}

static int data_init(void *addr_src, void *addr_dest, uint8_t size, uint32_t data_length)
{
	uint32_t i;

	if (size == DMA_DATA_WIDTH_BYTE_1)
	{
		for (i = 0; i < data_length; i++)
		{
			*(uint8_t *)((uint32_t)addr_src + i) = i;
			*(uint8_t *)((uint32_t)addr_dest + i) = 0;
		}
	}
	else if (size == DMA_DATA_WIDTH_BYTE_2)
	{
		for (i = 0; i < data_length / 2; i++)
		{
			*(uint16_t *)((uint32_t)addr_src + i*2) = i;
			*(uint16_t *)((uint32_t)addr_dest + i*2) = 0;
		}
	}
	else if (size == DMA_DATA_WIDTH_BYTE_4)
	{
		for (i = 0; i < data_length / 4; i++)
		{
			*(uint32_t *)((uint32_t)addr_src + i*4) = i;
			*(uint32_t *)((uint32_t)addr_dest + i*4) = 0;
		}
	}
	else
	{
		return -1;
	}

	return 0;
}

 int transfer_check(void *src, void *dest, test_dma_args_t *args)
{
	uint32_t i = 0;

	if ((args->src_addr_inc == args->dest_addr_inc) && (args->dest_addr_inc != DMA_ADDR_CONSTANT))
	{
		return memcmp(src, dest, args->transfer_length);
	}
	else if ((args->src_addr_inc == args->dest_addr_inc) && (args->dest_addr_inc == DMA_ADDR_CONSTANT))
	{
		if (args->data_width == DMA_DATA_WIDTH_BYTE_1)
		{
			if ((*(uint8_t *)src != *(uint8_t *)dest) || (*(uint8_t *)((uint32_t)dest + args->transfer_length - 1) == *(uint8_t *)((uint32_t)src + args->transfer_length - 1)))
				return -1;
		}

		if (args->data_width == DMA_DATA_WIDTH_BYTE_2)
		{
			if ((*(uint16_t *)src != *(uint16_t *)dest) || (*(uint16_t *)((uint32_t)dest + args->transfer_length - 2) == *(uint16_t *)((uint32_t)src + args->transfer_length - 2)))
				return -1;
		}

		if (args->data_width == DMA_DATA_WIDTH_BYTE_4)
		{
			if ((*(uint32_t *)src != *(uint32_t *)dest) || (*(uint32_t *)((uint32_t)dest + args->transfer_length - 4) == *(uint32_t *)((uint32_t)src + args->transfer_length - 4)))
				return -1;
		}
	}

	if ((args->src_addr_inc == DMA_ADDR_INC) && (args->dest_addr_inc == DMA_ADDR_DEC))
	{
		if (args->data_width == DMA_DATA_WIDTH_BYTE_1)
		{
			for (i = 0; i < args->transfer_length; i++)
			{
				if ((*(uint8_t *)((uint32_t)src+i)) != (*(uint8_t *)((uint32_t)dest+args->transfer_length-(i+1))))
					return -1;
			}
		}
		if (args->data_width == DMA_DATA_WIDTH_BYTE_2)
		{
			for (i = 0; i < args->transfer_length / 2; i++)
			{
				if ((*(uint16_t *)((uint32_t)src + 2*i)) != (*(uint16_t *)((uint32_t)dest + args->transfer_length - 2 * (i + 1))))
					return -1;
			}
		}
		if (args->data_width == DMA_DATA_WIDTH_BYTE_4)
		{
			for (i = 0; i < args->transfer_length / 4; i++)
			{
				if ((*(uint32_t *)((uint32_t)src + 4*i)) != (*(uint32_t *)((uint32_t)dest + args->transfer_length - 4 * (i + 1))))
					return -1;
			}
		}
	}

	if ((args->src_addr_inc == DMA_ADDR_DEC) && (args->dest_addr_inc == DMA_ADDR_INC))
	{
		if (args->data_width == DMA_DATA_WIDTH_BYTE_1)
		{
			for (i = 0; i < args->transfer_length; i++)
			{
				if ((*(uint8_t *)((uint32_t)dest + i)) != (*(uint8_t *)((uint32_t)src + args->transfer_length - (i + 1))))
					return -1;
			}
		}
		if (args->data_width == DMA_DATA_WIDTH_BYTE_2)
		{
			for (i = 0; i < args->transfer_length / 2; i++)
			{
				if ((*(uint16_t *)((uint32_t)dest + 2*i)) != (*(uint16_t *)((uint32_t)src + args->transfer_length - 2 * (i + 1))))
					return -1;
			}
		}
		if (args->data_width == DMA_DATA_WIDTH_BYTE_4)
		{
			for (i = 0; i < args->transfer_length / 4; i++)
			{
				if ((*(uint32_t *)((uint32_t)dest + 4*i)) != (*(uint32_t *)((uint32_t)src + args->transfer_length - 4 * (i + 1))))
					return -1;
			}
		}
	}

	if ((args->src_addr_inc == DMA_ADDR_INC) && (args->dest_addr_inc == DMA_ADDR_CONSTANT))
	{
		if (args->data_width == DMA_DATA_WIDTH_BYTE_1)
		{
			if (*(uint8_t *)((uint32_t)src + args->transfer_length - 1) != *(uint8_t *)dest)
				return -1;
		}
		if (args->data_width == DMA_DATA_WIDTH_BYTE_2)
		{
			if (*(uint16_t *)((uint32_t)src + args->transfer_length - 2) != *(uint16_t *)dest)
				return -1;
		}
		if (args->data_width == DMA_DATA_WIDTH_BYTE_4)
		{
			if (*(uint32_t *)((uint32_t)src + args->transfer_length - 4) != *(uint32_t *)dest)
				return -1;
		}
	}

	if ((args->src_addr_inc == DMA_ADDR_DEC) && (args->dest_addr_inc == DMA_ADDR_CONSTANT))
	{
		if (args->data_width == DMA_DATA_WIDTH_BYTE_1)
		{
			if (*(uint8_t *)src != *(uint8_t *)dest)
				return -1;
		}
		if (args->data_width == DMA_DATA_WIDTH_BYTE_2)
		{
			if (*(uint16_t *)src != *(uint16_t *)dest)
				return -1;
		}
		if (args->data_width == DMA_DATA_WIDTH_BYTE_4)
		{
			if (*(uint32_t *)src != *(uint32_t *)dest)
				return -1;
		}
	}

	if ((args->src_addr_inc == DMA_ADDR_CONSTANT) && (args->dest_addr_inc == DMA_ADDR_INC))
	{
		if (args->data_width == DMA_DATA_WIDTH_BYTE_1)
		{
			if (*(uint8_t *)src != *(uint8_t *)((uint32_t)dest + args->transfer_length - 1))
				return -1;
		}
		if (args->data_width == DMA_DATA_WIDTH_BYTE_2)
		{
			if (*(uint16_t *)src != *(uint16_t *)((uint32_t)dest + args->transfer_length - 2))
				return -1;
		}
		if (args->data_width == DMA_DATA_WIDTH_BYTE_4)
		{
			if (*(uint32_t *)src != *(uint32_t *)((uint32_t)dest + args->transfer_length - 4))
				return -1;
		}
	}

	if ((args->src_addr_inc == DMA_ADDR_CONSTANT) && (args->dest_addr_inc == DMA_ADDR_DEC))
	{
		if (args->data_width == DMA_DATA_WIDTH_BYTE_1)
		{
			if (*(uint8_t *)src != *(uint8_t *)dest)
				return -1;
		}
		if (args->data_width == DMA_DATA_WIDTH_BYTE_2)
		{
			if (*(uint16_t *)src != *(uint16_t *)dest)
				return -1;
		}
		if (args->data_width == DMA_DATA_WIDTH_BYTE_4)
		{
			if (*(uint32_t *)src != *(uint32_t *)dest)
				return -1;
		}
	}

	return 0;
}

int test_dma_memoryToMemory(char *args)
{
	int32_t ch_hd, alloc_ch[16];
	int32_t ret_sta;
	void *src = NULL, *dest = NULL;
	int ret;
	test_dma_args_t td;

	/*args get*/
	td.channel_idx = (uint32_t)*((uint32_t *)args);
	td.data_width = (uint32_t)*((uint32_t *)args+1);
	td.src_addr_inc = (uint32_t)*((uint32_t *)args+2);
	td.dest_addr_inc = (uint32_t)*((uint32_t *)args+3);
	td.group_length = (uint32_t)*((uint32_t *)args+4);
	td.transfer_length = (uint32_t)*((uint32_t *)args+5);	
	td.type = (uint8_t)*((uint8_t *)args+6);
	td.mode = (uint8_t)*((uint8_t *)args+7);
	td.ch_mode = (uint8_t)*((uint8_t *)args+8);

	TEST_CASE_TIPS("config data width is %d", td.data_width);
	TEST_CASE_TIPS("config source address increment is %d", td.src_addr_inc);
	TEST_CASE_TIPS("config destination address increment is %d", td.dest_addr_inc);
	TEST_CASE_TIPS("config group length is %d", td.group_length);
	TEST_CASE_TIPS("config transfer data length is %d bytes", td.transfer_length);
	TEST_CASE_TIPS("config type is %d", td.type);
	TEST_CASE_TIPS("config mode is %d", td.mode);
	TEST_CASE_TIPS("config ch_mode is %d", td.ch_mode);	

	src = (void *)malloc(td.transfer_length);
	dest = (void *)malloc(td.transfer_length);
	if ((src == NULL) || (dest == NULL))
	{
		free(src);
		free(dest);
		TEST_CASE_WARN_QUIT("malloc memory error");
	}

	ret = data_init(src, dest, td.data_width, td.transfer_length);
	if (ret != 0)
	{
		free(src);
		free(dest);
		TEST_CASE_WARN_QUIT("data init error");
	}


	int8_t i;
	for (i=0; i< 16;i++) {
		ch_hd = csi_dma_alloc_channel();
		if (ch_hd == -1) {
			free(src);
			free(dest);
			if (i != 0) {
				for (i--;i>=0;i--) {
					csi_dma_release_channel(alloc_ch[i]);
				}
			}
			TEST_CASE_WARN_QUIT("dma alloc channel error");
		}
		alloc_ch[i] = ch_hd;
		if (ch_hd == td.channel_idx) {
			break;
		}
	}

	if (i != 0) {
		for (i--;i>=0;i--) {
			csi_dma_release_channel(alloc_ch[i]);
		}
	}


	/*config channel*/
	dma_config_t config;
	config.src_inc = td.src_addr_inc;
	config.dst_inc = td.dest_addr_inc;
	config.src_tw = td.data_width;
	config.dst_tw = td.data_width;
	config.type = td.type;//0
	config.mode = td.mode;//2
	config.ch_mode = td.ch_mode;//1

//	config.type = DMA_MEM2MEM;
//	config.mode = DMA_BLOCK_TRIGGER;
//	config.ch_mode = DMA_MODE_SOFTWARE;
	config.group_len = td.group_length;

	ret_sta = csi_dma_config_channel(ch_hd, &config, dma_callback, NULL);
	if (ret_sta != 0)
	{
		csi_dma_release_channel(ch_hd);
		free(src);
		free(dest);
		TEST_CASE_WARN_QUIT("dma config channel error");
	}


	lock[ch_hd] = 1;
	if ((td.src_addr_inc == DMA_ADDR_INC) && (td.dest_addr_inc == DMA_ADDR_INC))
	{
		if (td.data_width == DMA_DATA_WIDTH_BYTE_1) {
			csi_dma_start(ch_hd, src, dest, td.transfer_length);
		}
		else if (td.data_width == DMA_DATA_WIDTH_BYTE_2) {
				csi_dma_start(ch_hd, src, dest, td.transfer_length/2);
		}
		else {
				csi_dma_start(ch_hd, src, dest, td.transfer_length/4);
		}
	}
	if ((td.src_addr_inc == DMA_ADDR_DEC) && (td.dest_addr_inc == DMA_ADDR_INC))
	{
		if (td.data_width == DMA_DATA_WIDTH_BYTE_1)
		{
			csi_dma_start(ch_hd, (void *)((uint32_t)src + td.transfer_length - 1), dest, td.transfer_length);
		}
		else if (td.data_width == DMA_DATA_WIDTH_BYTE_2)
		{
			csi_dma_start(ch_hd, (void *)((uint32_t)src + td.transfer_length - 2), dest, td.transfer_length/2);
		}
		else
		{
			csi_dma_start(ch_hd, (void *)((uint32_t)src + td.transfer_length - 4), dest, td.transfer_length/4);
		}
	}
	if ((td.src_addr_inc == DMA_ADDR_INC) && (td.dest_addr_inc == DMA_ADDR_DEC))
	{
		if (td.data_width == DMA_DATA_WIDTH_BYTE_1)
		{
			csi_dma_start(ch_hd, src, (void *)((uint32_t)dest + td.transfer_length - 1), td.transfer_length);
		}
		else if (td.data_width == DMA_DATA_WIDTH_BYTE_2)
		{
			csi_dma_start(ch_hd, src, (void *)((uint32_t)dest + td.transfer_length - 2), td.transfer_length/2);
		}
		else
		{
			csi_dma_start(ch_hd, src, (void *)((uint32_t)dest + td.transfer_length - 4), td.transfer_length/4);
		}
	}

	if ((td.src_addr_inc == DMA_ADDR_DEC) && (td.dest_addr_inc == DMA_ADDR_DEC))
	{
		if (td.data_width == DMA_DATA_WIDTH_BYTE_1)
		{
			csi_dma_start(ch_hd, (void *)((uint32_t)src + td.transfer_length - 1), (void *)((uint32_t)dest + td.transfer_length - 1), td.transfer_length);
		}
		else if (td.data_width == DMA_DATA_WIDTH_BYTE_2)
		{
			csi_dma_start(ch_hd, (void *)((uint32_t)src + td.transfer_length - 2), (void *)((uint32_t)dest + td.transfer_length - 2), td.transfer_length/2);
		}
		else
		{
			csi_dma_start(ch_hd, (void *)((uint32_t)src + td.transfer_length - 4), (void *)((uint32_t)dest + td.transfer_length - 4), td.transfer_length/4);
		}
	}

	if ((td.src_addr_inc == DMA_ADDR_CONSTANT) && (td.dest_addr_inc == DMA_ADDR_CONSTANT)) {
		if (td.data_width == DMA_DATA_WIDTH_BYTE_1){
			csi_dma_start(ch_hd, src, dest, td.transfer_length);
		}
		else if (td.data_width == DMA_DATA_WIDTH_BYTE_2) {
			csi_dma_start(ch_hd, src, dest, td.transfer_length/2);
		}
		else {
			csi_dma_start(ch_hd, src, dest, td.transfer_length/4);
		}
	}

	if ((td.src_addr_inc == DMA_ADDR_INC) && (td.dest_addr_inc == DMA_ADDR_CONSTANT)) {
		if (td.data_width == DMA_DATA_WIDTH_BYTE_1){
			csi_dma_start(ch_hd, src, dest, td.transfer_length);
		}
		else if (td.data_width == DMA_DATA_WIDTH_BYTE_2) {
			csi_dma_start(ch_hd, src, dest, td.transfer_length/2);
		}
		else {
			csi_dma_start(ch_hd, src, dest, td.transfer_length/4);
		}
	}

	if ((td.src_addr_inc == DMA_ADDR_CONSTANT) && (td.dest_addr_inc == DMA_ADDR_INC)) {
		if (td.data_width == DMA_DATA_WIDTH_BYTE_1){
			csi_dma_start(ch_hd, src, dest, td.transfer_length);
		}
		else if (td.data_width == DMA_DATA_WIDTH_BYTE_2) {
			csi_dma_start(ch_hd, src, dest, td.transfer_length/2);
		}
		else {
			csi_dma_start(ch_hd, src, dest, td.transfer_length/4);
		}
	}

	if ((td.src_addr_inc == DMA_ADDR_DEC) && (td.dest_addr_inc == DMA_ADDR_CONSTANT))
	{
		if (td.data_width == DMA_DATA_WIDTH_BYTE_1)
		{
			csi_dma_start(ch_hd, (void *)((uint32_t)src + td.transfer_length - 1), dest, td.transfer_length);
		}
		else if (td.data_width == DMA_DATA_WIDTH_BYTE_2)
		{
			csi_dma_start(ch_hd, (void *)((uint32_t)src + td.transfer_length - 2), dest, td.transfer_length/2);
		}
		else
		{
			csi_dma_start(ch_hd, (void *)((uint32_t)src + td.transfer_length - 4), dest, td.transfer_length/4);
		}
	}

	if ((td.src_addr_inc == DMA_ADDR_CONSTANT) && (td.dest_addr_inc == DMA_ADDR_DEC))
	{
		if (td.data_width == DMA_DATA_WIDTH_BYTE_1)
		{
			csi_dma_start(ch_hd, src, (void *)((uint32_t)dest + td.transfer_length - 1), td.transfer_length);
		}
		else if (td.data_width == DMA_DATA_WIDTH_BYTE_2)
		{
			csi_dma_start(ch_hd, src, (void *)((uint32_t)dest + td.transfer_length - 2), td.transfer_length/2);
		}
		else
		{
			csi_dma_start(ch_hd, src, (void *)((uint32_t)dest + td.transfer_length - 4), td.transfer_length/4);
		}
	}

	while (lock[ch_hd]) {
		;
	}
	csi_dma_stop(ch_hd);
	csi_dma_release_channel(ch_hd);

	ret = transfer_check(src, dest, &td);
	TEST_CASE_ASSERT(ret == 0, "data transfer error");
	
	/*uninit dma*/
	free(src);
	free(dest);

	return 0;
}
