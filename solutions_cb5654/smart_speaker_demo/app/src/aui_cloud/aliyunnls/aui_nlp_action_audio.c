/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include "aui_nlp.h"

#define TAG "nlpvol"

/**
 * 解析ASR&NLP信息
*/
int aui_nlp_action_set_volume(cJSON *js)
{
    int ret = -1;

    cJSON *action_params_name = cJSON_GetObjectItemByPath(js, "payload.action_params[0].name");
    if (json_string_eq(action_params_name, "sound")) {
        cJSON *action_params_value = cJSON_GetObjectItemByPath(js, "payload.action_params[0].value");

        if (json_string_eq(action_params_value, "up")) {
            app_volume_inc(0);
            ret = 0;
        } else if (json_string_eq(action_params_value, "down")) {
            app_volume_dec(0);
            ret = 0;
        } else if (json_string_eq(action_params_value, "max")) {
            app_volume_set(100, 0);
            ret = 0;
        } else if (json_string_eq(action_params_value, "min")) {
            app_volume_set(0, 0);
            ret = 0;
        } else if (json_string_eq(action_params_value, "mute")) {
            // app_volume_mute();
            ret = 0;
        } else {
            if (action_params_value) {
                char ch = action_params_value->valuestring[0];

                if (ch >= '0' && ch <= '9') {
                    int vol = atoi(action_params_value->valuestring);
                    app_volume_set(vol, 0);
                    ret = 0;
                }
            }
        }
    }

    return ret;
}

int aui_nlp_action_play_pause()
{
    app_player_pause();
    // local_audio_play(LOCAL_AUDIO_OK);

    return 0;
}

int aui_nlp_action_play_resume()
{
    app_player_resume();

    return 0;
}

/*
{
    "header": {
        "namespace": "VirtualAssistant",
        "name": "DialogResultGenerated",
        "status": 20000000,
        "message_id": "8d3a537a21ff4e01b40ab50b6e31fe07",
        "task_id": "00e04c87f1245d44556c718f1af48871",
        "status_text": "Gateway:SUCCESS:Success."
    },
    "payload": {
        "action": "Action://audio/set_volumn",
        "action_params": [
            {
                "name": "sound",
                "value": "up"
            }
        ],
        "semantics": [
            {
                "score": 1,
                "slots": {
                    "degree": [
                        {
                            "raw_value": "大一点",
                            "norm_value": "up"
                        }
                    ]
                },
                "domain": "general_command",
                "source": "jsgf",
                "intent": "set_volumn"
            }
        ]
    }
}

{
    "header": {
        "namespace": "VirtualAssistant",
        "name": "DialogResultGenerated",
        "status": 20000000,
        "message_id": "280db9c6cddc40478720e30eebf35155",
        "task_id": "00e04c87f124acf6ce2199233d31c879",
        "status_text": "Gateway:SUCCESS:Success."
    },
    "payload": {
        "action": "Action://audio/resume",
        "semantics": [
            {
                "score": 1,
                "slots": {},
                "domain": "general_command",
                "source": "jsgf",
                "intent": "play"
            }
        ]
    }
}



{
    "header": {
        "namespace": "VirtualAssistant",
        "name": "DialogResultGenerated",
        "status": 20000000,
        "message_id": "0fcc0e23d0d440ee91947aefa561c31f",
        "task_id": "00e04c87f124348d254e67ba5aa9bbb8",
        "status_text": "Gateway:SUCCESS:Success."
    },
    "payload": {
        "spoken_text": "",
        "action": "Action://audio/pasue",
        "semantics": [
            {
                "score": 1,
                "slots": {},
                "domain": "general_command",
                "source": "jsgf",
                "intent": "stop"
            }
        ]
    }
}
*/