/**
 * @file record.h
 * @copyright Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef CX_RECORD_H
#define CX_RECORD_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define CXDVR_MAX_NAME_LEN      64  /* recorder file name max length */
#define CXDVR_MAX_PATH_LEN      128 /* recorder file path max length */

typedef enum {
    CXDVR_EVENT_FILE_GENERATED = 0, /* record finish, file generated event */
    CXDVR_EVENT_SNAP_GENERATED,     /* snapshot finish, file generated event */
    CXDVR_EVENT_FILE_ERROR,         /* record error event */
    CXDVR_EVENT_SNAP_ERROR,         /* snapshot error event */
} cx_dvr_event_t;

typedef enum {
    CXDVR_SNAP_SINGLE = 0,          /* snap type single */
    CXDVR_SNAP_BURST,               /* snap type burst */
} cx_dvr_snapshot_type_e;

typedef enum {
    CXDVR_VIDEO_IDLE        = 1 << 0,   /* recorder status idle */
    CXDVR_VIDEO_RECORDING   = 1 << 1,   /* recorder status recording video */
    CXDVR_VIDEO_SNAPSHOTING = 1 << 2,   /* recorder status snapshoting */
} cx_dvr_status_e;

typedef enum {
    CXDVR_FILE_TYPE_VIDEO       = 1,	/* file type video */
    CXDVR_FILE_TYPE_SNAPSHORT   = 2,	/* file type snapshot */
    CXDVR_FILE_TYPE_ALL			= 3	    /* file type all, CXDVR_FILE_TYPE_RECORD | CXDVR_FILE_TYPE_SNAPSHORT */
} cx_dvr_file_type_e;

typedef enum {
    CXDVR_SEARCH_RESULT_SUCCESS = 0,    /* search result success */
    CXDVR_SEARCH_RESULT_ERR_TIME,       /* search result error, time during error */
    CXDVR_SEARCH_RESULT_ERR,            /* search result error */
} cx_dvr_search_result_e;

typedef struct {
    cx_dvr_snapshot_type_e type;        /* snapshot type */
    uint32_t busrt_num;					/* snap nums in busrt mode, valid when type == CXDVR_SNAP_BURST */
    uint32_t burst_interval;			/* snap interval in busrt mode, valid when type == CXDVR_SNAP_BURST */
    int      quality;                   /* snap picture quality */
} cx_dvr_snapshort_config_t;

typedef struct {
    char  file_name[CXDVR_MAX_NAME_LEN];    /* file name */
    char  file_path[CXDVR_MAX_PATH_LEN];    /* file path */
    cx_dvr_file_type_e type;                /* file type */
    uint32_t  size;                         /* file size */
    uint32_t  begin_time;                   /* file begin time */
    uint32_t  end_time;                     /* file end time */
} cx_dvr_file_info_t;

/* recorder service handler */
typedef void * cx_dvr_hdl_t;

/* recorder service callback type */
typedef void (*cx_dvr_callback_t)(cx_dvr_event_t event, void *data);

/**
 * @description: chixiao recorder service start
 * @param [in] dvr_name:    recorder name, in the form "recorder#0"
 * @param [in] cb:          service callback
 * @return                  NULL: error, others: recorder handler.
 */
cx_dvr_hdl_t cx_dvr_start(const char *dvr_name, cx_dvr_callback_t cb);

/**
 * @description: chixiao recorder service stop
 * @param [in] hdl:         recorder service handler
 * @return                  0: Success, < 0: Error code.
 */
int cx_dvr_stop(cx_dvr_hdl_t hdl);

/**
 * @description: start video recording
 * @param [in] hdl:         recorder service handler
 * @param [in] stoptime_ms: time after which this record stops, -1 for no auto sotp
 * @return                  0: Success, < 0: Error code.
 */
int cx_dvr_record_start(const cx_dvr_hdl_t hdl, int stoptime_ms);

/**
 * @description: stop video recording
 * @param [in] hdl:         recorder service handler
 * @return                  0: Success, < 0: Error code.
 */
int cx_dvr_record_stop(const cx_dvr_hdl_t hdl);

/**
 * @description: trigger snapshot
 * @param [in] hdl:         recorder service handler
 * @param [in] config:      snapshot configuration
 * @return                  0: Success, < 0: Error code.
 */
int cx_dvr_trigger_snapshot(const cx_dvr_hdl_t hdl, const cx_dvr_snapshort_config_t *config);

/**
 * @description: get recorder current status
 * @param [in] hdl:         recorder service handler
 * @param [out] status:     current status
 * @return                  0: Success, < 0: Error code.
 */
int cx_dvr_get_status(const cx_dvr_hdl_t hdl, int *status);

/**
 * @description: search for video record and snapshot files during the given time
 * @param [in] hdl:         recorder service handler
 * @param [in] begin_time:  begin time for the search duration
 * @param [in] end_time:    end time for the search duration
 * @param [in] type:        file type for the search
 * @param [out] infos:      arrays for storing the file infos
 * @param [in] info_num:    arrays size of infos
 * @return                  0: Success, < 0: Error code.
 */
int cx_dvr_search(const cx_dvr_hdl_t hdl, uint32_t begin_time, uint32_t end_time, cx_dvr_file_type_e type, cx_dvr_file_info_t *infos, int info_num);

/**
 * @description: delete record or snapshot files
 * @param [in] hdl:         recorder service handler
 * @param [in] infos:       info arrays for the files to delete
 * @param [in] info_num:    arrays size of infos
 * @return                  0: Success, < 0: Error code.
 */
int cx_dvr_delete_record(const cx_dvr_hdl_t hdl, cx_dvr_file_info_t *infos, int num);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* CX_INIT_H */

