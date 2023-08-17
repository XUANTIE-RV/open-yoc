#include <aos/cli.h>
#include <aos/kernel.h>
#include <stdio.h>
#include <string.h>
#include <media_config.h>

#include <cx/faceai/face_ai_service.h>

#define IMG_PATH_SIZE (128)
extern cx_srv_hdl_t g_face_ai_handler;
extern int test_ai_vid_source(int shot_count);

void print_user_info(cx_face_user_info_t *user_info)
{
    aos_cli_printf("user_info: user_id: %d, user_name: %s, admin: %d\r\n", user_info->user_id, user_info->user_name, user_info->admin);
}

void faceai_start(char *buf, int32_t len, int32_t argc, char **argv)
{
    // faceai start facedet faceldmk quality living2d living3d facerecog
    aos_cli_printf("Start FaceAI service.\r\n");
    if (g_face_ai_handler != NULL) {
        aos_cli_printf("face ai g_face_ai_handler exists: %p\r\n", g_face_ai_handler);
        return;
    }
    if (argc == 2) {
#ifdef CONFIG_RGBIR_SENSOR_SWITCH
        MediaVideoIRInit();
#endif
        g_face_ai_handler = cx_face_start("faceai", NULL, NULL);
    }
    else {
        aos_cli_printf("Unsupport command\r\n");
        return;
    }

    aos_cli_printf("start face ai g_face_ai_handler: %p\r\n", g_face_ai_handler);

}

void faceai_stop(char *buf, int32_t len, int32_t argc, char **argv)
{
    // faceai stop
    aos_cli_printf("Stop FaceAI service.\r\n");
    if (g_face_ai_handler == NULL) {
        aos_cli_printf("Please start service.\r\n");
        return;
    }
    if (argc > 2) {
        aos_cli_printf("Unsupport command\r\n");
        return;
    }
#ifdef CONFIG_RGBIR_SENSOR_SWITCH
    MediaVideoIRDeInit();
#endif
    cx_face_stop(g_face_ai_handler);
    g_face_ai_handler = NULL;
    aos_cli_printf("Stop FaceAI service finish.\r\n");
}

void faceai_register(char *buf, int32_t len, int32_t argc, char **argv)
{
    // faceai register username admin(-1/0/1) direction(1-middle) left_img_path right_img_path
    aos_cli_printf("Register face.\r\n");
    if (g_face_ai_handler == NULL) {
        aos_cli_printf("Please start service.\r\n");
        return;
    }
    cx_face_user_info_t user_info;
    cx_face_dir_e face_dir = FACE_DIRECTION_MIDDLE;
    char left_img_path[IMG_PATH_SIZE];
    char right_img_path[IMG_PATH_SIZE];
    if (argc > 3 && argc < 5) {
        aos_cli_printf("Invalid param number, will use default param.\r\n");
    }
    if (argc >= 5) {
        // get user name 
        snprintf(user_info.user_name, 32, argv[2]);
        user_info.admin = atoi(argv[3]);
        face_dir = (cx_face_dir_e)atoi(argv[4]);
    }
    if (argc == 7) {
        snprintf(left_img_path, IMG_PATH_SIZE, argv[5]);
        snprintf(right_img_path, IMG_PATH_SIZE, argv[6]);
        cx_face_set_input_path(g_face_ai_handler, left_img_path, right_img_path);
        aos_cli_printf("Set image path\r\n");
    }
    if (argc > 7) {
        aos_cli_printf("Unsupport command\r\n");
        return;
    }
    
#ifdef CONFIG_RGBIR_SENSOR_SWITCH
    MediaVideoIRInit();
#endif
    int result = cx_face_enroll(g_face_ai_handler, &user_info, face_dir, -1);
    aos_cli_printf("cx_face_enroll result: %d\r\n", result, user_info.user_id);
}

void faceai_verify(char *buf, int32_t len, int32_t argc, char **argv)
{
    // faceai verify left_img_path right_img_path
    aos_cli_printf("Verify face.\r\n");
    if (g_face_ai_handler == NULL) {
        aos_cli_printf("Please start service.\r\n");
        return;
    }
    char left_img_path[IMG_PATH_SIZE];
    char right_img_path[IMG_PATH_SIZE];
    if (argc == 3) {
        aos_cli_printf("Invalid param number, will use default param.\r\n");
    }
    else if (argc == 4) {
        snprintf(left_img_path, IMG_PATH_SIZE, argv[2]);
        snprintf(right_img_path, IMG_PATH_SIZE, argv[3]);
        cx_face_set_input_path(g_face_ai_handler, left_img_path, right_img_path);
        aos_cli_printf("Set image path\r\n");
    }
    else if (argc > 4) {
        aos_cli_printf("Unsupport command\r\n");
        return;
    }

#ifdef CONFIG_RGBIR_SENSOR_SWITCH
    MediaVideoIRInit();
#endif
    int result = cx_face_verify(g_face_ai_handler, -1);
    aos_cli_printf("cx_face_verify result: %d\r\n", result);
}

void faceai_get_user_info(char *buf, int32_t len, int32_t argc, char **argv)
{
    // faceai get-user userid
    aos_cli_printf("Get userinfo.\r\n");
    if (g_face_ai_handler == NULL) {
        aos_cli_printf("Please start service.\r\n");
        return;
    }
    if (argc != 3) {
        aos_cli_printf("Unsupport command\r\n");
        return;
    }

    int user_id = atoi(argv[2]);
    cx_face_user_info_t user_info;
    int ret = cx_face_get_user_info(g_face_ai_handler, user_id, &user_info);
    aos_cli_printf("cx_face_get_user_info result: %d\r\n", ret);
    if(ret != 0) {
        return;
    }
    print_user_info(&user_info);
}

void faceai_delete_user(char *buf, int32_t len, int32_t argc, char **argv)
{
    // faceai delete-user userid
    aos_cli_printf("Delete user.\r\n");
    if (g_face_ai_handler == NULL) {
        aos_cli_printf("Please start service.\r\n");
        return;
    }
    if (argc != 3) {
        aos_cli_printf("Unsupport command\r\n");
        return;
    }

    int user_id = atoi(argv[2]);
    int ret = cx_face_delete_user(g_face_ai_handler, user_id);
    aos_cli_printf("cx_face_delete_user result: %d\r\n", ret);
    if(ret != 0) {
        return;
    }

    aos_cli_printf("Delete user_id: %d.\r\n", user_id);
}

void faceai_get_all_user_id(char *buf, int32_t len, int32_t argc, char **argv)
{
    // faceai get-id-list
    aos_cli_printf("Get all user.\r\n");
    if (g_face_ai_handler == NULL) {
        aos_cli_printf("Please start service.\r\n");
        return;
    }
    if (argc != 2) {
        aos_cli_printf("Unsupport command\r\n");
        return;
    }

    uint32_t user_id_list[50];
    int id_count = cx_face_get_all_user_id(g_face_ai_handler, user_id_list, 50);
    aos_cli_printf("user id list: ");
    for (int i = 0; i < id_count; i++)
    {
         aos_cli_printf("%d ", user_id_list[i]);
    }
    aos_cli_printf("\r\n"); 
}

void faceai_set_threshold(char *buf, int32_t len, int32_t argc, char **argv)
{
    // faceai get-id-list
    aos_cli_printf("Set threshold.\r\n");
    if (g_face_ai_handler == NULL) {
        aos_cli_printf("Please start service.\r\n");
        return;
    }
    if (argc != 8) {
        aos_cli_printf("Unsupport command\r\n");
        return;
    }
    cx_face_ai_threshold_t threshold;
    threshold.facedetect_thres = atoi(argv[2]);
    threshold.ptscore_thres = atoi(argv[3]);
    threshold.facequality_thres = atoi(argv[4]);
    threshold.living2d_thres = atoi(argv[5]);
    threshold.living3d_thres = atoi(argv[6]);
    threshold.face_reco_thres = atoi(argv[7]);
    int ret = cx_face_set_threshold(g_face_ai_handler, &threshold);
    aos_cli_printf("faceai_set_threshol result is %d.\r\n", ret);
}

void faceai_snapshot(char *buf, int32_t len, int32_t argc, char **argv)
{
    // faceai snapshot 2
    aos_cli_printf("Take snapshot begin.\r\n");
#ifdef CONFIG_RGBIR_SENSOR_SWITCH
    MediaVideoIRInit();
#endif
    if (argc == 2) {
        test_ai_vid_source(1);
    }
    else if (argc == 3) {
        int count = atoi(argv[2]);
        test_ai_vid_source(count);
    }
    else {
        aos_cli_printf("Invalid param number");
    }
    aos_cli_printf("Take snapshot end.\r\n");
}

void faceai_help()
{
    aos_cli_printf("faceai commands help:\r\n");
    aos_cli_printf("start: start service\r\n");
    aos_cli_printf("stop: stop service\r\n");
}

void faceai_cmd(char *buf, int32_t len, int32_t argc, char **argv)
{
    aos_cli_printf("This is FaceAI test command begin.\r\n");
    
    if (argc < 2) {
        aos_cli_printf("use 'debug_api help' for test\r\n");
        return;
    }

    if (strcmp(argv[1], "start") == 0) {
        faceai_start(buf, len, argc, argv);
    }
    else if (strcmp(argv[1], "stop") == 0) {
        faceai_stop(buf, len, argc, argv);
    }
    else if (strcmp(argv[1], "register") == 0) {
        faceai_register(buf, len, argc, argv);
    }
    else if (strcmp(argv[1], "verify") == 0) {
        faceai_verify(buf, len, argc, argv);
    }
    else if (strcmp(argv[1], "get-user") == 0) {
        faceai_get_user_info(buf, len, argc, argv);
    }
    else if (strcmp(argv[1], "delete-user") == 0) {
        faceai_delete_user(buf, len, argc, argv);
    }
    else if (strcmp(argv[1], "get-id-list") == 0) {
        faceai_get_all_user_id(buf, len, argc, argv);
    }
    else if (strcmp(argv[1], "set-thres") == 0) {
        faceai_set_threshold(buf, len, argc, argv);
    }
    else if (strcmp(argv[1], "snapshot") == 0) {
        faceai_snapshot(buf, len, argc, argv);
    }
    else if (strcmp(argv[1], "help") == 0) {
        faceai_help();
    }
    else {
        aos_cli_printf("Unsupport command\n");
    }

    aos_cli_printf("This is FaceAI test command end.\r\n");
}

const struct cli_command faceai_cli_cmds[] = {
    {"faceai", "FaceAI test", faceai_cmd},
};

void cli_reg_cmd_faceai(void)
{
    int32_t ret;
    
    ret = aos_cli_register_commands(faceai_cli_cmds, sizeof(faceai_cli_cmds) / sizeof(struct cli_command));
    if (ret) {
        aos_cli_printf("faceai cmds register fail\r\n");
    }
}