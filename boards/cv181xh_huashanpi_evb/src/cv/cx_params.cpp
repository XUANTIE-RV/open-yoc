/*
 * Copyright (C) 2023 Alibaba Group Holding Limited
 */

#include "cv_config.h"

/* board (sensors) configuration for chixiao */
const static char *g_cx_boardconfig = R"({	
"sensors": [	
	{	
		"tag": "ir0",	
		"width": 1920,
		"height": 1080,
		"fps": 25
	},	
	{	
		"tag": "rgb1",	
		"width": 1920,
		"height": 1080,
		"fps": 25
	}
]	
})";

/* chixiao service configuration */
const static char *g_cx_serviceconfig =	R"({	
"recorder#0": {	
	"video_input": [	
		{	
			"channel": 0,
			"encoder": "H264"
		},	
		{	
			"channel": 0,
			"encoder": "JPEG"
		}	
	],	
	"audio_input": [	
		{	
			"sample_rate": 8000,	
			"sample_channels": 1,	
			"sample_bits": 16,	
			"encoder": "G711A"	
		}	
	],	
	"storage_space": 1024000,	
	"segment_size": 20,	
	"root_path": "/mnt/sd/record"	
}, 
"link_visual#0": {	
	"video_input": [	
		{	
			"channel": 0,
			"target_rate": 1024,
			"encoder": "H265"
		},	
		{	
			"channel": 1,
			"target_rate": 512,
			"encoder": "H265"
		},	
		{	
			"channel": 2,
			"target_rate": 256,
			"encoder": "H265"
		}	
	],	
	"audio_input": [	
		{	
			"sample_rate": 8000,	
			"sample_channels": 1,	
			"sample_bits": 16,	
			"encoder": "G711A"	
		}	
	]	
},
"preview#0": {	
	"video_input": [	
		{	
			"channel": 3
		}
	]
},
"faceai#0": {	
	"video_input": [	
		{	
			"channel": 4
		},
		{	
			"channel": 5
		}
	],
	"register_mode": "single",
	"face_det_thres": 450,
	"face_landmark_thres": 145,
	"quality_thres": 400,
	"living2d_thres": 830,
	"living3d_thres": 900,
	"face_recog_thres": 710
	}
})";

/* chixiao video input channel configuration */
const static char *g_input_channel_config = R"({
	"video_input#0": [
		{
			"element": "vi",
			"sensor": "rgb1"
		},
		{
			"element": "vpss",
			"label": "0x010100",
			"width": 720,
			"height": 1280,
			"rotate": 90,
			"crop": false
		}
	],
	"video_input#1": [
		{
			"element": "vi",
			"sensor": "rgb1"	
		},
		{
			"element": "vpss",
			"label": "0x010100",
			"width": 720,
			"height": 1280,
			"rotate": 90,
			"crop": false
		},
		{
			"element": "vpss",
			"label": "0x000200",
			"width": 480,
			"height": 640,
			"fps": 25,	
			"rotate": 0,
			"crop": false
		}
	],
	"video_input#2": [
		{
			"element": "vi",
			"sensor": "rgb1"
		},
		{
			"element": "vpss",
			"label": "0x010100",
			"width": 720,
			"height": 1280,
			"rotate": 90,
			"crop": false
		},
		{
			"element": "vpss",
			"label": "0x000300",
			"width": 360,
			"height": 640,
			"fps": 25,	
			"rotate": 0,
			"crop": false
		}
	],
	"video_input#3": [
		{
			"element": "vi",
			"sensor": "rgb1"
		},
		{
			"element": "vpss",
			"label": "0x010100",
			"width": 720,
			"height": 1280,
			"rotate": 90,
			"crop": false
		},
		{
			"element": "vpss",
			"label": "0x000400",
			"width": 480,
			"height": 480,
			"rotate": 0,
			"crop": true
		}
	],
	"video_input#4": [
		{
			"element": "vi",
			"sensor": "ir0"
		},
		{
			"element": "vpss",
			"label": "0x010000",
			"width": 480,
			"height": 640,
			"rotate": 90,
			"crop": false,
			"start_frame": 6
		}
	],
	"video_input#5": [
		{
			"element": "vi",
			"sensor": "rgb1"
		},
		{
			"element": "vpss",
			"label": "0x010100",
			"width": 720,
			"height": 1280,
			"rotate": 90,
			"crop": false
		},
		{
			"element": "vpss",
			"label": "0x000500",
			"width": 480,
			"height": 640,
			"rotate": 0,
			"crop": false,
			"start_frame": 6
		}
	]
})";

const char *get_cx_board_config()
{
    return g_cx_boardconfig;
}

const char *get_cx_service_config()
{
    return g_cx_serviceconfig;
}

const char *get_cx_input_channel_config()
{
    return g_input_channel_config;
}