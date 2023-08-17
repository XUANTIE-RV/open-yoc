/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     gcc_csky.ld
 * @brief    csky linker file
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/
MEMORY
{
    DRAM   : ORIGIN = 0x80040000, LENGTH = 0x3FC0000   /* DRAM 64M-256K */
}

__min_heap_size = 0x200;
PROVIDE (__ram_end  = ORIGIN(DRAM) + LENGTH(DRAM));
PROVIDE (__heap_end = __ram_end);

REGION_ALIAS("REGION_TEXT",         DRAM);
REGION_ALIAS("REGION_RODATA",       DRAM);
REGION_ALIAS("REGION_DATA",         DRAM);
REGION_ALIAS("REGION_POST_TEXT",    DRAM);
REGION_ALIAS("REGION_POST_RODATA",  DRAM);
REGION_ALIAS("REGION_POST_DATA",    DRAM);
REGION_ALIAS("REGION_BSS",          DRAM);


#define POST_LIBCVI_OSDRV_OBJS *libcvi_osdrv.a:main_helper.o *libcvi_osdrv.a:main_enc_cvitest.o *libcvi_osdrv.a:coda9.o *libcvi_osdrv.a:wave4.o *libcvi_osdrv.a:vdi.o *libcvi_osdrv.a:vo_sdk_layer.o *libcvi_osdrv.a:cvi_enc_internal.o *libcvi_osdrv.a:cvi_jpg_enc_internal.o *libcvi_osdrv.a:common.o *libcvi_osdrv.a:vpuapi.o *libcvi_osdrv.a:cvi_rc_kernel.o *libcvi_osdrv.a:vi_raw_dump.o *libcvi_osdrv.a:cvi_soft_float.o *libcvi_osdrv.a:vpuhelper.o *libcvi_osdrv.a:cvi_jpg_dec_internal.o *libcvi_osdrv.a:cvi_jpg_interface.o *libcvi_osdrv.a:cvi_vb_proc.o *libcvi_osdrv.a:jpuhelper.o *libcvi_osdrv.a:cvi_sys_proc.o *libcvi_osdrv.a:vpss_rgn_ctrl.o *libcvi_osdrv.a:cnm_video_helper.o *libcvi_osdrv.a:cvi_tpu_interface.o *libcvi_osdrv.a:pbu.o *libcvi_osdrv.a:mixer.o *libcvi_osdrv.a:cfgParser.o *libcvi_osdrv.a:cvi_vc_ctrl.o *libcvi_osdrv.a:tpu_platform.o
#define POST_LIBRTL8723DS_OBJS *librtl8723ds.a:Hal8723d_fw.o *librtl8723ds.a:rtw_mlme_ext.o *librtl8723ds.a:customer_rtos_ioctl.o *librtl8723ds.a:halbtc8723d1ant.o *librtl8723ds.a:rom_aes.o *librtl8723ds.a:rtl8723d_hal_ops.o *librtl8723ds.a:hal_com.o *librtl8723ds.a:phydm_debug.o *librtl8723ds.a:rtw_psk.o *librtl8723ds.a:halhwimg8723d_rf.o *librtl8723ds.a:halrf_8723d.o *librtl8723ds.a:rtw_recv.o *librtl8723ds.a:rtw_ap.o *librtl8723ds.a:rtw_xmit.o *librtl8723ds.a:rtw_wlan_util.o *librtl8723ds.a:rtw_mlme.o *librtl8723ds.a:rtw_pwrctrl.o *librtl8723ds.a:rtw_cmd.o *librtl8723ds.a:phydm.o *librtl8723ds.a:rom_rtw_security.o *librtl8723ds.a:rtl8723d_hal_init.o *librtl8723ds.a:phydm_phystatus.o *librtl8723ds.a:halhwimg8723d_bb.o *librtl8723ds.a:hal_com_phycfg.o *librtl8723ds.a:halrf_kfree.o *librtl8723ds.a:sdio_intf.o *librtl8723ds.a:customer_rtos_intfs.o *librtl8723ds.a:phydm_api.o *librtl8723ds.a:rtw_promisc.o *librtl8723ds.a:hal_intf.o *librtl8723ds.a:rtw_mp.o *librtl8723ds.a:halrf_powertracking_ce.o *librtl8723ds.a:phydm_dig.o *librtl8723ds.a:sdio_drvio.o *librtl8723ds.a:phydm_auto_dbg.o *librtl8723ds.a:halrf.o *librtl8723ds.a:rom_rtw_ieee80211.o *librtl8723ds.a:rtw_suspend.o *librtl8723ds.a:phydm_adaptivity.o *librtl8723ds.a:phydm_psd.o *librtl8723ds.a:rtl8723d_cmd.o *librtl8723ds.a:rtw_sta_mgt.o *librtl8723ds.a:rtw_security.o *librtl8723ds.a:phydm_cfotracking.o *librtl8723ds.a:customer_rtos_skbuff.o *librtl8723ds.a:rtw_ioctl_set.o *librtl8723ds.a:rtl8723d_recv.o *librtl8723ds.a:halphyrf_ce.o *librtl8723ds.a:rtl8723d_txdesc.o *librtl8723ds.a:customer_rtos_recv.o *librtl8723ds.a:rtl8723d_dm.o *librtl8723ds.a:Hal8723DPwrSeq.o *librtl8723ds.a:rtw_intfs.o *librtl8723ds.a:rom_arc4.o *librtl8723ds.a:rtw_ieee80211.o *librtl8723ds.a:phydm_math_lib.o *librtl8723ds.a:rom_sha1.o *librtl8723ds.a:rtw_btcoex.o *librtl8723ds.a:rtl8723d_rxdesc.o *librtl8723ds.a:rom_rtw_psk.o *librtl8723ds.a:rtl8723d_isr.o *librtl8723ds.a:hal_dm.o *librtl8723ds.a:phydm_noisemonitor.o *librtl8723ds.a:netdev.o *librtl8723ds.a:rtw_odm.o *librtl8723ds.a:sdio_isr.o *librtl8723ds.a:HalPwrSeqCmd.o *librtl8723ds.a:rtl8723d_rf6052.o *librtl8723ds.a:rtw_debug.o *librtl8723ds.a:phydm_rssi_monitor.o *librtl8723ds.a:phydm_hwconfig.o *librtl8723ds.a:customer_rtos_xmit.o *librtl8723ds.a:rtw_io.o *librtl8723ds.a:phydm_regconfig8723d.o *librtl8723ds.a:phydm_rtl8723d.o *librtl8723ds.a:hci_intfs.o *librtl8723ds.a:phydm_dfs.o *librtl8723ds.a:halrf_powertracking.o *librtl8723ds.a:rtl8723d_xmit.o *librtl8723ds.a:phydm_soml.o
#define POST_LIBLVGL_OBJS *liblvgl.a:lv_obj_style.o *liblvgl.a:lv_font_montserrat_24.o *liblvgl.a:lv_theme_default.o *liblvgl.a:lv_font_montserrat_18.o *liblvgl.a:lv_obj_scroll.o *liblvgl.a:lv_textarea.o *liblvgl.a:lv_draw_sw_blend.o *liblvgl.a:lv_btnmatrix.o *liblvgl.a:lv_bidi.o *liblvgl.a:lv_draw_sw_rect.o *liblvgl.a:lv_chart.o *liblvgl.a:lv_spinbox.o *liblvgl.a:lv_draw_mask.o *liblvgl.a:lv_label.o *liblvgl.a:lv_refr.o *liblvgl.a:lv_colorwheel.o *liblvgl.a:lv_printf.o *liblvgl.a:lv_roller.o *liblvgl.a:lv_dropdown.o *liblvgl.a:lv_obj_tree.o *liblvgl.a:lv_group.o *liblvgl.a:lv_draw_sw_arc.o *liblvgl.a:lv_area.o *liblvgl.a:lv_txt.o *liblvgl.a:lv_table.o *liblvgl.a:lv_style.o *liblvgl.a:lv_flex.o *liblvgl.a:lv_mem.o *liblvgl.a:lv_keyboard.o *liblvgl.a:lv_calendar.o *liblvgl.a:lv_img.o *liblvgl.a:lv_img_decoder.o *liblvgl.a:lv_meter.o *liblvgl.a:lv_slider.o *liblvgl.a:lv_obj_draw.o *liblvgl.a:lv_draw_sw_transform.o *liblvgl.a:lv_anim.o *liblvgl.a:lv_draw_label.o *liblvgl.a:lv_draw_sw_line.o *liblvgl.a:lv_tabview.o *liblvgl.a:lv_menu.o *liblvgl.a:lv_led.o *liblvgl.a:lv_font_fmt_txt.o *liblvgl.a:lv_color.o *liblvgl.a:lv_draw_arc.o *liblvgl.a:lv_draw_sw_letter.o *liblvgl.a:lv_event.o *liblvgl.a:lv_fs.o *liblvgl.a:lv_calendar_header_dropdown.o *liblvgl.a:lv_draw_sw_gradient.o *liblvgl.a:lv_checkbox.o *liblvgl.a:lv_switch.o *liblvgl.a:lv_obj_class.o *liblvgl.a:lv_timer.o *liblvgl.a:lv_math.o *liblvgl.a:lv_calendar_header_arrow.o *liblvgl.a:lv_draw_sw_polygon.o *liblvgl.a:lv_fs_stdio.o *liblvgl.a:lv_hal_disp.o *liblvgl.a:lv_port_disp_yoc.o *liblvgl.a:lv_style_gen.o *liblvgl.a:lv_tileview.o *liblvgl.a:lv_draw_sw_layer.o *liblvgl.a:lv_indev.o *liblvgl.a:lv_theme.o *liblvgl.a:lv_font.o *liblvgl.a:lv_draw_sw.o *liblvgl.a:lv_img_buf.o *liblvgl.a:lv_draw_layer.o *liblvgl.a:lv_disp.o *liblvgl.a:lv_spinner.o *liblvgl.a:lv_win.o *liblvgl.a:lv_draw_rect.o *liblvgl.a:lv_list.o *liblvgl.a:lv_img_cache.o *liblvgl.a:lv_msgbox.o *liblvgl.a:lv_draw_line.o *liblvgl.a:lv_indev_scroll.o *liblvgl.a:lv_hal_tick.o *liblvgl.a:lv_hal_indev.o *liblvgl.a:lv_draw_transform.o *liblvgl.a:lv_extra.o *liblvgl.a:lv_draw.o *liblvgl.a:lv_gc.o
#define POST_LIBSHL_C906_RTOS_OBJS *libshl_c906_rtos.a:maxpool.c.o *libshl_c906_rtos.a:avgpool.c.o *libshl_c906_rtos.a:gemm_fp32.c.o *libshl_c906_rtos.a:subgraph.c.o *libshl_c906_rtos.a:convolution_3x3_fp16.c.o *libshl_c906_rtos.a:convolution_3x3_fp32.c.o *libshl_c906_rtos.a:convolution_channel.c.o *libshl_c906_rtos.a:fullyconnected.c.o *libshl_c906_rtos.a:cache_matmul.c.o *libshl_c906_rtos.a:proposal.c.o *libshl_c906_rtos.a:deconvolution.c.o *libshl_c906_rtos.a:matmul.c.o *libshl_c906_rtos.a:split.c.o *libshl_c906_rtos.a:cache_conv1d.c.o *libshl_c906_rtos.a:gather.c.o *libshl_c906_rtos.a:depthwise_convolution_5x5_fp32.c.o *libshl_c906_rtos.a:segment_mean.c.o *libshl_c906_rtos.a:resize.c.o *libshl_c906_rtos.a:psroipooling.c.o *libshl_c906_rtos.a:maxpool2d_locat.c.o *libshl_c906_rtos.a:sub.c.o *libshl_c906_rtos.a:segment_sum.c.o *libshl_c906_rtos.a:minimum.c.o *libshl_c906_rtos.a:roipool.c.o *libshl_c906_rtos.a:segment_min.c.o *libshl_c906_rtos.a:segment_max.c.o *libshl_c906_rtos.a:segment_prod.c.o *libshl_c906_rtos.a:roialign.c.o *libshl_c906_rtos.a:gather_nd.c.o *libshl_c906_rtos.a:batch_normalization.c.o *libshl_c906_rtos.a:depthwise_convolution_3x3_fp32.c.o *libshl_c906_rtos.a:lrn.c.o *libshl_c906_rtos.a:depth_to_space.c.o *libshl_c906_rtos.a:convolution_sgemm_fp32.c.o *libshl_c906_rtos.a:scatter.c.o *libshl_c906_rtos.a:im2col.c.o *libshl_c906_rtos.a:stack.c.o *libshl_c906_rtos.a:deconvolution3d.c.o *libshl_c906_rtos.a:prelu.c.o *libshl_c906_rtos.a:and.c.o *libshl_c906_rtos.a:or.c.o *libshl_c906_rtos.a:xor.c.o *libshl_c906_rtos.a:sum.c.o *libshl_c906_rtos.a:relu6.c.o *libshl_c906_rtos.a:topk.c.o *libshl_c906_rtos.a:not.c.o *libshl_c906_rtos.a:strided_slice.c.o *libshl_c906_rtos.a:maxpool3d.c.o *libshl_c906_rtos.a:reduce_logsumexp.c.o *libshl_c906_rtos.a:convolution3d.c.o *libshl_c906_rtos.a:averagepool3d.c.o *libshl_c906_rtos.a:reverse.c.o *libshl_c906_rtos.a:fullyconnected_int8.c.o *libshl_c906_rtos.a:layer_norm.c.o *libshl_c906_rtos.a:reduce_max.c.o *libshl_c906_rtos.a:reduce_min.c.o *libshl_c906_rtos.a:argmin.c.o *libshl_c906_rtos.a:argmax.c.o *libshl_c906_rtos.a:batch_to_space.c.o *libshl_c906_rtos.a:relu1.c.o *libshl_c906_rtos.a:fullyconnected_fp32.c.o *libshl_c906_rtos.a:space_to_batch.c.o *libshl_c906_rtos.a:reduce_prod.c.o *libshl_c906_rtos.a:log_softmax.c.o *libshl_c906_rtos.a:reduce_mean.c.o *libshl_c906_rtos.a:reduce_sum.c.o *libshl_c906_rtos.a:leaky_relu.c.o *libshl_c906_rtos.a:non_max_suppression.c.o *libshl_c906_rtos.a:space_to_depth.c.o *libshl_c906_rtos.a:depthwise_convolution_fp16.c.o *libshl_c906_rtos.a:l2pool.c.o *libshl_c906_rtos.a:col2im.c.o *libshl_c906_rtos.a:slice.c.o *libshl_c906_rtos.a:sigmoid.c.o *libshl_c906_rtos.a:cumprod.c.o *libshl_c906_rtos.a:cumsum.c.o *libshl_c906_rtos.a:convolution_1x1_fp32.c.o *libshl_c906_rtos.a:abs.c.o *libshl_c906_rtos.a:shape.c.o *libshl_c906_rtos.a:l2_normalization.c.o *libshl_c906_rtos.a:shuffle_channel.c.o *libshl_c906_rtos.a:softrelu.c.o *libshl_c906_rtos.a:mean.c.o *libshl_c906_rtos.a:global_maxpool.c.o *libshl_c906_rtos.a:floor_mod.c.o *libshl_c906_rtos.a:arange.c.o *libshl_c906_rtos.a:div.c.o *libshl_c906_rtos.a:softplus.c.o *libshl_c906_rtos.a:max.c.o *libshl_c906_rtos.a:min.c.o *libshl_c906_rtos.a:hard_sigmoid.c.o *libshl_c906_rtos.a:expm1.c.o *libshl_c906_rtos.a:log1p.c.o *libshl_c906_rtos.a:prod.c.o *libshl_c906_rtos.a:convolution_relu.c.o *libshl_c906_rtos.a:elu.c.o *libshl_c906_rtos.a:maximum.c.o *libshl_c906_rtos.a:yuv_rgb_scale.c.o *libshl_c906_rtos.a:floor.c.o *libshl_c906_rtos.a:round.c.o *libshl_c906_rtos.a:exp.c.o *libshl_c906_rtos.a:relun.c.o *libshl_c906_rtos.a:ceil.c.o *libshl_c906_rtos.a:convolution_relu6.c.o *libshl_c906_rtos.a:sqrt.c.o *libshl_c906_rtos.a:tan.c.o *libshl_c906_rtos.a:trunc.c.o *libshl_c906_rtos.a:erf.c.o *libshl_c906_rtos.a:logical_or.c.o *libshl_c906_rtos.a:equal.c.o *libshl_c906_rtos.a:logical_and.c.o *libshl_c906_rtos.a:sign.c.o *libshl_c906_rtos.a:greater_equal.c.o *libshl_c906_rtos.a:less.c.o *libshl_c906_rtos.a:logical_not.c.o *libshl_c906_rtos.a:greater.c.o *libshl_c906_rtos.a:threshold_relu.c.o *libshl_c906_rtos.a:logical_xor.c.o *libshl_c906_rtos.a:negative.c.o *libshl_c906_rtos.a:square.c.o *libshl_c906_rtos.a:isnan.c.o *libshl_c906_rtos.a:expand_dims.c.o *libshl_c906_rtos.a:global_avgpool.c.o *libshl_c906_rtos.a:flatten.c.o *libshl_c906_rtos.a:mod.c.o *libshl_c906_rtos.a:not_equal.c.o *libshl_c906_rtos.a:ndarray_size.c.o *libshl_c906_rtos.a:floor_divide.c.o *libshl_c906_rtos.a:sin.c.o *libshl_c906_rtos.a:cos.c.o *libshl_c906_rtos.a:tanh.c.o *libshl_c906_rtos.a:atan.c.o *libshl_c906_rtos.a:acos.c.o *libshl_c906_rtos.a:cosh.c.o *libshl_c906_rtos.a:asin.c.o *libshl_c906_rtos.a:acosh.c.o *libshl_c906_rtos.a:atanh.c.o *libshl_c906_rtos.a:power.c.o *libshl_c906_rtos.a:asinh.c.o *libshl_c906_rtos.a:global_averagepool.c.o *libshl_c906_rtos.a:squeeze.c.o *libshl_c906_rtos.a:bn.c.o *libshl_c906_rtos.a:broadcast_to.c.o *libshl_c906_rtos.a:unstack.c.o *libshl_c906_rtos.a:where.c.o *libshl_c906_rtos.a:all.c.o *libshl_c906_rtos.a:avgpool3d.c.o *libshl_c906_rtos.a:space_to_batch_nd.c.o *libshl_c906_rtos.a:batch_to_space_nd.c.o *libshl_c906_rtos.a:any.c.o *libshl_c906_rtos.a:sequence_mask.c.o
#define POST_LIBLINK_VISUAL_OBJS *liblink_visual.a:rpc_session.cpp.o *liblink_visual.a:lv_xml.cpp.o *liblink_visual.a:intelligent_alarm_data.cpp.o *liblink_visual.a:rtmp-invoke-handler.cpp.o *liblink_visual.a:intelligent_alarm_event_session.cpp.o *liblink_visual.a:intelligent_alarm_task.cpp.o *liblink_visual.a:dns_service.cpp.o *liblink_visual.a:alarm_event_service.cpp.o *liblink_visual.a:lv_sha1.cpp.o *liblink_visual.a:sls_client.cpp.o *liblink_visual.a:rtmp-client.cpp.o *liblink_visual.a:requestParse.cpp.o *liblink_visual.a:device_dispatch.cpp.o *liblink_visual.a:http_protocol.cpp.o *liblink_visual.a:net_socket.cpp.o *liblink_visual.a:sls_identify.cpp.o *liblink_visual.a:stream_check.cpp.o *liblink_visual.a:lv_md5.cpp.o *liblink_visual.a:flv_tag_body.cpp.o *liblink_visual.a:trigger_picture_service.cpp.o *liblink_visual.a:rtmp_client.cpp.o *liblink_visual.a:net_engine.cpp.o *liblink_visual.a:lv_aes.cpp.o *liblink_visual.a:task_queue.cpp.o *liblink_visual.a:ping_client.cpp.o *liblink_visual.a:event_limit.cpp.o *liblink_visual.a:stream_save.cpp.o *liblink_visual.a:file_transform_service.cpp.o *liblink_visual.a:sls_service.cpp.o *liblink_visual.a:rtmp-chunk-read.cpp.o *liblink_visual.a:port_detect.cpp.o *liblink_visual.a:picture_store.cpp.o *liblink_visual.a:net_url_parse.cpp.o *liblink_visual.a:picture_session.cpp.o *liblink_visual.a:trans_quality.cpp.o *liblink_visual.a:lv_time.cpp.o *liblink_visual.a:delay_detect.cpp.o *liblink_visual.a:lv_hmac.cpp.o *liblink_visual.a:lv_sha256.cpp.o *liblink_visual.a:mtu_detect.cpp.o *liblink_visual.a:oss_ca.cpp.o *liblink_visual.a:sls_ca.cpp.o *liblink_visual.a:http_client.cpp.o *liblink_visual.a:io_multiplexing.cpp.o *liblink_visual.a:rtmp-netstream.cpp.o *liblink_visual.a:dns_protocol.cpp.o *liblink_visual.a:lv_thread.cpp.o *liblink_visual.a:rtmp-chunk-write.cpp.o *liblink_visual.a:video_check.cpp.o *liblink_visual.a:bitrate_check.cpp.o *liblink_visual.a:async_process.cpp.o *liblink_visual.a:auto_grow_buffer.cpp.o *liblink_visual.a:net_connector_dns.cpp.o *liblink_visual.a:rtmp-handshake.cpp.o *liblink_visual.a:lv_base64.cpp.o *liblink_visual.a:rtmp-control-handler.cpp.o *liblink_visual.a:rtmp-netconnection.cpp.o *liblink_visual.a:device_env.cpp.o *liblink_visual.a:logger.cpp.o *liblink_visual.a:net_transmission.cpp.o *liblink_visual.a:icmp_protocol.cpp.o *liblink_visual.a:lv_sstream.cpp.o *liblink_visual.a:http_protocol_assistant.cpp.o *liblink_visual.a:rtmp-handler.cpp.o *liblink_visual.a:rtmp-event.cpp.o *liblink_visual.a:lv_base_c.c.o *liblink_visual.a:audio_check.cpp.o *liblink_visual.a:das_service.cpp.o *liblink_visual.a:lv_hex.cpp.o *liblink_visual.a:lv_sum.cpp.o *liblink_visual.a:rtmp-control-message.cpp.o *liblink_visual.a:lv_mutex.cpp.o *liblink_visual.a:sls_sds.cpp.o *liblink_visual.a:lv_endian.cpp.o
#define POST_LIBIOT_SDK_OBJS *libiot_sdk.a:mqtt_client.o *libiot_sdk.a:dm_message.o *libiot_sdk.a:impl_linkkit.o *libiot_sdk.a:iotx_ota.o *libiot_sdk.a:awss_registrar.o *libiot_sdk.a:dm_manager.o *libiot_sdk.a:utils_httpc.o *libiot_sdk.a:CoAPMessage.o *libiot_sdk.a:dm_msg_process.o *libiot_sdk.a:awss_wifimgr.o *libiot_sdk.a:utils_sha1.o *libiot_sdk.a:awss_notify.o *libiot_sdk.a:zconfig_vendor_common.o *libiot_sdk.a:utils_md5.o *libiot_sdk.a:dm_client.o *libiot_sdk.a:zconfig_ieee80211.o *libiot_sdk.a:iotx_cm_mqtt.o *libiot_sdk.a:awss_report.o *libiot_sdk.a:dm_utils.o *libiot_sdk.a:awss_dev_ap.o *libiot_sdk.a:impl_dynreg.o *libiot_sdk.a:ota_lib.o *libiot_sdk.a:CoAPServer.o *libiot_sdk.a:awss_wps.o *libiot_sdk.a:utils_hmac.o *libiot_sdk.a:iotx_log.o *libiot_sdk.a:CoAPResource.o *libiot_sdk.a:iotx_cm.o *libiot_sdk.a:sdk-impl.o *libiot_sdk.a:zconfig_protocol.o *libiot_sdk.a:awss_reset.o *libiot_sdk.a:awss_packet.o *libiot_sdk.a:utils_sha256.o *libiot_sdk.a:ca.o *libiot_sdk.a:dm_fota.o *libiot_sdk.a:awss_cmp_coap.o *libiot_sdk.a:dm_cota.o *libiot_sdk.a:json_parser.o *libiot_sdk.a:event.o *libiot_sdk.a:awss_adha.o *libiot_sdk.a:utils_base64.o *libiot_sdk.a:string_utils.o *libiot_sdk.a:CoAPSerialize.o *libiot_sdk.a:awss_main.o *libiot_sdk.a:MQTTConnectClient.o *libiot_sdk.a:passwd.o *libiot_sdk.a:MQTTPacket.o *libiot_sdk.a:utils_net.o *libiot_sdk.a:CoAPDeserialize.o *libiot_sdk.a:awss_crypt.o *libiot_sdk.a:zconfig_utils.o *libiot_sdk.a:awss_cmp_mqtt.o *libiot_sdk.a:dm_client_adapter.o *libiot_sdk.a:os_misc.o *libiot_sdk.a:ota_fetch.o *libiot_sdk.a:connect_ap.o *libiot_sdk.a:mem_stats.o *libiot_sdk.a:MQTTSubscribeClient.o *libiot_sdk.a:MQTTSerializePublish.o *libiot_sdk.a:CoAPPlatform.o *libiot_sdk.a:MQTTUnsubscribeClient.o *libiot_sdk.a:MQTTDeserializePublish.o *libiot_sdk.a:awss_bind.o *libiot_sdk.a:device.o *libiot_sdk.a:utils_timer.o *libiot_sdk.a:aos_support.o *libiot_sdk.a:dm_ota.o *libiot_sdk.a:dm_opt.o *libiot_sdk.a:awss_event.o *libiot_sdk.a:awss_timer.o
#define POST_LIBSTDCPP_OBJS *libstdc++.a:cp-demangle.o *libstdc++.a:cxx11-shim_facets.o *libstdc++.a:fstream-inst.o *libstdc++.a:lt1-codecvt.o *libstdc++.a:cow-wstring-inst.o *libstdc++.a:sstream-inst.o *libstdc++.a:wstring-inst.o *libstdc++.a:eh_personality.o *libstdc++.a:cxx11-ios_failure.o *libstdc++.a:basic_file.o *libstdc++.a:iostream-inst.o *libstdc++.a:functexcept.o *libstdc++.a:snprintf_lite.o *libstdc++.a:pbase_type_info.o *libstdc++.a:codecvt_members.o *libstdc++.a:vterminate.o *libstdc++.a:class_type_info.o *libstdc++.a:thread.o *libstdc++.a:ios_failure.o *libstdc++.a:eh_throw.o *libstdc++.a:pointer_type_info.o *libstdc++.a:eh_exception.o *libstdc++.a:eh_catch.o *libstdc++.a:cow-stdexcept.o *libstdc++.a:functional.o *libstdc++.a:function_type_info.o *libstdc++.a:bad_array_new.o *libstdc++.a:enum_type_info.o *libstdc++.a:guard_error.o *libstdc++.a:cxx11-stdexcept.o *libstdc++.a:bad_typeid.o *libstdc++.a:bad_cast.o *libstdc++.a:eh_aux_runtime.o *libstdc++.a:list.o *libstdc++.a:condition_variable.o *libstdc++.a:tinfo.o *libstdc++.a:collate_members.o *libstdc++.a:collate_members_cow.o *libstdc++.a:pmem_type_info.o *libstdc++.a:chrono.o *libstdc++.a:eh_call.o *libstdc++.a:messages_members_cow.o *libstdc++.a:messages_members.o *libstdc++.a:misc-inst.o *libstdc++.a:eh_type.o *libstdc++.a:new_handler.o *libstdc++.a:del_opv.o *libstdc++.a:istream-string.o *libstdc++.a:eh_unex_handler.o *libstdc++.a:del_ops.o *libstdc++.a:eh_term_handler.o *libstdc++.a:new_opv.o *libstdc++.a:globals_io.o
#define POST_LIBSAILFACE_NANO_OBJS *libsailface-nano.a:FaceNetReco.c.obj *libsailface-nano.a:sail_stereo_estimator.cpp.obj *libsailface-nano.a:sail_stereo_bm.cpp.obj *libsailface-nano.a:FaceNetLandmark.c.obj *libsailface-nano.a:FaceNetAntispoofingLocal.c.obj *libsailface-nano.a:FaceNetQuality.c.obj *libsailface-nano.a:align_face.c.obj *libsailface-nano.a:faceLandmarkModule.c.obj *libsailface-nano.a:faceAntispoofingDepthModule.c.obj *libsailface-nano.a:faceAntispoofingCommon.c.obj *libsailface-nano.a:faceQualityModule.c.obj *libsailface-nano.a:faceAntispoofingLocalModule.c.obj *libsailface-nano.a:faceRecogModule.c.obj *libsailface-nano.a:faceAntispoofingGlobalModule.c.obj *libsailface-nano.a:sail_stereo_estimator_wrapper.cpp.obj
#define POST_LIBC_OBJS *libc.a:lib_a-vfscanf.o *libc.a:lib_a-svfwprintf.o *libc.a:lib_a-strtodg.o *libc.a:lib_a-wcsftime.o *libc.a:lib_a-strftime.o *libc.a:lib_a-strerror.o *libc.a:lib_a-getopt.o *libc.a:lib_a-gdtoa-gethex.o *libc.a:lib_a-mktime.o *libc.a:lib_a-tzset_r.o *libc.a:lib_a-fseeko.o *libc.a:lib_a-timelocal.o *libc.a:lib_a-ungetc.o *libc.a:lib_a-fread.o *libc.a:lib_a-fgetwc.o *libc.a:lib_a-fputwc.o *libc.a:lib_a-wcstoul.o *libc.a:lib_a-strtorx.o *libc.a:lib_a-refill.o *libc.a:lib_a-gmtime_r.o *libc.a:lib_a-tzcalc_limits.o *libc.a:lib_a-putc.o *libc.a:lib_a-fclose.o *libc.a:lib_a-sniprintf.o *libc.a:lib_a-getc.o *libc.a:lib_a-fopen.o *libc.a:lib_a-ungetwc.o *libc.a:lib_a-fwalk.o *libc.a:lib_a-wbuf.o *libc.a:lib_a-swprintf.o *libc.a:lib_a-fwrite.o *libc.a:lib_a-siscanf.o *libc.a:lib_a-fputs.o *libc.a:lib_a-getenv_r.o *libc.a:lib_a-mbrtowc.o *libc.a:lib_a-fileno.o *libc.a:lib_a-asprintf.o *libc.a:lib_a-wcrtomb.o *libc.a:lib_a-sccl.o *libc.a:lib_a-assert.o *libc.a:lib_a-rget.o *libc.a:lib_a-vasprintf.o *libc.a:lib_a-flags.o *libc.a:lib_a-strtold.o *libc.a:lib_a-fprintf.o *libc.a:lib_a-fiprintf.o *libc.a:lib_a-strncasecmp.o *libc.a:lib_a-gettzinfo.o *libc.a:lib_a-rewind.o *libc.a:lib_a-month_lengths.o *libc.a:lib_a-strcasecmp.o *libc.a:lib_a-scanf.o *libc.a:lib_a-strpbrk.o *libc.a:lib_a-wcslcpy.o *libc.a:lib_a-sysopen.o *libc.a:lib_a-div.o *libc.a:lib_a-strxfrm.o *libc.a:lib_a-fini.o *libc.a:lib_a-vprintf.o *libc.a:lib_a-tzset.o *libc.a:lib_a-iswgraph_l.o *libc.a:lib_a-towlower.o *libc.a:lib_a-strnlen.o *libc.a:lib_a-mbstowcs.o *libc.a:lib_a-towupper.o *libc.a:lib_a-iswupper_l.o *libc.a:lib_a-iswblank_l.o *libc.a:lib_a-iswlower_l.o *libc.a:lib_a-fseek.o *libc.a:lib_a-ftell.o *libc.a:lib_a-tzlock.o *libc.a:lib_a-atoll.o *libc.a:lib_a-envlock.o *libc.a:lib_a-strtok.o *libc.a:lib_a-iswalpha_l.o *libc.a:lib_a-syswrite.o *libc.a:lib_a-iswalnum_l.o *libc.a:lib_a-realloc.o *libc.a:lib_a-syslseek.o *libc.a:lib_a-iswpunct_l.o *libc.a:lib_a-wcscpy.o *libc.a:lib_a-sysfstat.o *libc.a:lib_a-sysread.o *libc.a:lib_a-sysstat.o *libc.a:lib_a-wmemset.o *libc.a:lib_a-wcscmp.o *libc.a:lib_a-iswprint_l.o *libc.a:lib_a-iswxdigit.o *libc.a:lib_a-wmemchr.o *libc.a:lib_a-wcslen.o *libc.a:lib_a-gmtime.o *libc.a:lib_a-strndup.o *libc.a:lib_a-putwc.o *libc.a:lib_a-getwc.o *libc.a:lib_a-tzvars.o *libc.a:lib_a-isalpha.o *libc.a:lib_a-atexit.o *libc.a:lib_a-iswdigit.o *libc.a:lib_a-strcoll.o *libc.a:lib_a-cfreer.o *libc.a:lib_a-abs.o *libc.a:lib_a-sysunlink.o *libc.a:lib_a-isspace.o *libc.a:lib_a-u_strerr.o *libc.a:lib_a-wmemmove.o *libc.a:lib_a-wmemcpy.o *libc.a:lib_a-errno.o *libc.a:lib_a-labs.o *libc.a:lib_a-sysclose.o *libc.a:lib_a-wcscoll.o *libc.a:lib_a-iswgraph.o *libc.a:lib_a-iswblank.o *libc.a:lib_a-iswcntrl.o *libc.a:lib_a-iswpunct.o *libc.a:lib_a-iswalpha.o *libc.a:lib_a-iswspace.o *libc.a:lib_a-iswlower.o *libc.a:lib_a-iswupper.o *libc.a:lib_a-environ.o *libc.a:lib_a-reent.o
#define POST_LIBCVI_KERNEL_OBJS *libcvi_kernel.a:tdma.o *libcvi_kernel.a:cvkmars.o *libcvi_kernel.a:tiu_xor.o *libcvi_kernel.a:tiu_and.o *libcvi_kernel.a:tiu_or.o *libcvi_kernel.a:tiu_add.o *libcvi_kernel.a:tiu_sub.o *libcvi_kernel.a:tiu_pt_depthwise_convolution.o *libcvi_kernel.a:tiu_pt_convolution.o *libcvi_kernel.a:tiu_convolution.o *libcvi_kernel.a:tiu_matrix_multiplication.o *libcvi_kernel.a:tiu_shift.o *libcvi_kernel.a:tiu_mul_qm.o *libcvi_kernel.a:tiu_mul.o *libcvi_kernel.a:engine_conductor.o *libcvi_kernel.a:tiu_average_pooling.o *libcvi_kernel.a:tiu_lookup_table.o *libcvi_kernel.a:tiu_max.o *libcvi_kernel.a:tiu_ge.o *libcvi_kernel.a:tiu_max_pooling.o *libcvi_kernel.a:tiu_min_pooling.o *libcvi_kernel.a:tiu_copy.o *libcvi_kernel.a:mode_manager.o *libcvi_kernel.a:tiu_matrix_multiplication_qm.o *libcvi_kernel.a:engine_state.o *libcvi_kernel.a:serial_mode.o *libcvi_kernel.a:cvikernel.o *libcvi_kernel.a:parallel_mode.o *libcvi_kernel.a:stream_mode.o
#define POST_LIBCVI_MW_VENC_OBJS *libcvi_mw_venc.a:cvi_venc.o *libcvi_mw_venc.a:enc_ctx.o
#define POST_LIBLWIP_OBJS *liblwip.a:tcp_in.o *liblwip.a:cli_iperf.o *liblwip.a:dhcp.o *liblwip.a:api_msg.o *liblwip.a:tcp_out.o *liblwip.a:dhcps.o *liblwip.a:dns.o *liblwip.a:pbuf.o *liblwip.a:etharp.o *liblwip.a:ip4_frag.o *liblwip.a:api_lib.o *liblwip.a:udp.o *liblwip.a:ip4.o *liblwip.a:igmp.o *liblwip.a:memp.o *liblwip.a:mem.o *liblwip.a:raw.o *liblwip.a:tcpip.o *liblwip.a:ping_v4.o *liblwip.a:timeouts.o *liblwip.a:ip4_addr.o *liblwip.a:icmp.o *liblwip.a:netdb.o *liblwip.a:inet_chksum.o *liblwip.a:ethernet.o *liblwip.a:netbuf.o *liblwip.a:cli_ping.o *liblwip.a:err.o *liblwip.a:def.o *liblwip.a:netifapi.o *liblwip.a:init.o *liblwip.a:stats.o *liblwip.a:ip.o
#define POST_LIBMBEDTLS_OBJS *libmbedtls.a:ssl_tls.o *libmbedtls.a:bignum.o *libmbedtls.a:sha1.o *libmbedtls.a:oid.o *libmbedtls.a:rsa.o *libmbedtls.a:aes.o *libmbedtls.a:x509_crt.o *libmbedtls.a:sha256.o *libmbedtls.a:md5.o *libmbedtls.a:sha512.o *libmbedtls.a:ssl_cli.o *libmbedtls.a:rsa_internal.o *libmbedtls.a:pem.o *libmbedtls.a:x509.o *libmbedtls.a:cipher.o *libmbedtls.a:ctr_drbg.o *libmbedtls.a:cipher_wrap.o *libmbedtls.a:asn1parse.o *libmbedtls.a:base64.o *libmbedtls.a:net_sockets.o *libmbedtls.a:md.o *libmbedtls.a:pk_wrap.o *libmbedtls.a:pk.o *libmbedtls.a:entropy_poll.o *libmbedtls.a:platform.o
#define POST_LIBCVI_SENSOR_OBJS *libcvi_sensor.a:gc02m1_cmos.o *libcvi_sensor.a:gc02m1_slave_cmos.o *libcvi_sensor.a:gc02m1_sensor_ctl.o *libcvi_sensor.a:gc2053_1l_cmos.o *libcvi_sensor.a:gc2053_1l_sensor_ctl.o
#define POST_LIBTMEDIA_BACKEND_CVITEK_OBJS *libtmedia_backend_cvitek.a:isp_rgb_color_mode_param.o
#define POST_LIBCX_LITE_BASE_OBJS *libcx_lite_base.a:cx_storage_nv_yoc.o
#define POST_LIBCVI_MW_VPU_OBJS *libcvi_mw_vpu.a:gdc_mesh.o *libcvi_mw_vpu.a:cvi_vo.o *libcvi_mw_vpu.a:cvi_gdc.o *libcvi_mw_vpu.a:cvi_region.o *libcvi_mw_vpu.a:vo_ioctl.o *libcvi_mw_vpu.a:rgn_ioctl.o
#define POST_LIBCX_LITE_LINKVISUAL_OBJS *libcx_lite_linkvisual.a:linkkit_client.o *libcx_lite_linkvisual.a:linkvisual_client.o *libcx_lite_linkvisual.a:HAL_OS_yoc.o *libcx_lite_linkvisual.a:property_parser.o *libcx_lite_linkvisual.a:link_visual_hal.o *libcx_lite_linkvisual.a:HAL_UDP_yoc.o *libcx_lite_linkvisual.a:sdk_assistant.o *libcx_lite_linkvisual.a:picture_parser.o *libcx_lite_linkvisual.a:link_visual_mbedtls.o *libcx_lite_linkvisual.a:thread_entry.o *libcx_lite_linkvisual.a:HAL_Crypt_yoc.o *libcx_lite_linkvisual.a:local_time.o
#define POST_LIBCVI_MW_ISP_ALGO_OBJS *libcvi_mw_isp_algo.a:isp_algo_dpc.o *libcvi_mw_isp_algo.a:isp_algo_rgbcac.o *libcvi_mw_isp_algo.a:isp_algo_cac.o
#define POST_LIBCSI_CV_OBJS *libcsi_cv.a:remap.o *libcsi_cv.a:affine.o *libcsi_cv.a:cos_sim.o *libcsi_cv.a:decomp_matrix.o
#define POST_LIBCVIAI_OBJS *libcviai.a:anchor_generator.o *libcviai.a:anchors.o *libcviai.a:vpss_engine.o *libcviai.a:object_utils.o
#define POST_LIBTMEDIA_CORE_OBJS *libtmedia_core.a:error.o
#define POST_LIBCVI_MW_AUDIO_OBJS *libcvi_mw_audio.a:fftcore.o *libcvi_mw_audio.a:cvi_ssp_interface.o *libcvi_mw_audio.a:lpaec.o *libcvi_mw_audio.a:nlpaes.o *libcvi_mw_audio.a:mmse.o *libcvi_mw_audio.a:nlpaes_subfun.o *libcvi_mw_audio.a:cviaudio_algo_interface.o *libcvi_mw_audio.a:bessel.o *libcvi_mw_audio.a:agc_init.o *libcvi_mw_audio.a:notch.o *libcvi_mw_audio.a:dg.o *libcvi_mw_audio.a:dr_detection.o *libcvi_mw_audio.a:dc.o *libcvi_mw_audio.a:delay.o *libcvi_mw_audio.a:slope_detection.o *libcvi_mw_audio.a:packfft.o *libcvi_mw_audio.a:mmse_rtwutil.o *libcvi_mw_audio.a:agc_subfun.o *libcvi_mw_audio.a:abs.o *libcvi_mw_audio.a:log.o *libcvi_mw_audio.a:sum.o
#define POST_LIBCVI_RUNTIME_OBJS *libcvi_runtime.a:cvi_device_mem.o *libcvi_runtime.a:bm_dmabuf.o *libcvi_runtime.a:alloc.o
#define POST_LIBM_OBJS *libm.a:lib_a-s_erf.o *libm.a:lib_a-k_rem_pio2.o *libm.a:lib_a-ef_pow.o *libm.a:lib_a-e_rem_pio2.o *libm.a:lib_a-s_expm1.o *libm.a:lib_a-e_acos.o *libm.a:lib_a-s_log1p.o *libm.a:lib_a-s_atan.o *libm.a:lib_a-e_asin.o *libm.a:lib_a-ef_log.o *libm.a:lib_a-e_atan2.o *libm.a:lib_a-e_sinh.o *libm.a:lib_a-s_rint.o *libm.a:lib_a-wf_pow.o *libm.a:lib_a-e_cosh.o *libm.a:lib_a-math_err.o *libm.a:lib_a-s_scalbn.o *libm.a:lib_a-e_acosh.o *libm.a:lib_a-sf_scalbn.o *libm.a:lib_a-s_asinh.o *libm.a:lib_a-e_atanh.o *libm.a:lib_a-sf_rint.o *libm.a:lib_a-ef_sqrt.o *libm.a:lib_a-s_modf.o *libm.a:lib_a-w_sinh.o *libm.a:lib_a-w_cosh.o *libm.a:lib_a-s_trunc.o *libm.a:lib_a-w_acos.o *libm.a:lib_a-w_asin.o *libm.a:lib_a-s_frexp.o *libm.a:lib_a-wf_log.o *libm.a:lib_a-w_sqrt.o *libm.a:lib_a-wf_sqrt.o *libm.a:lib_a-s_tan.o *libm.a:lib_a-s_fmin.o *libm.a:lib_a-sf_fmin.o *libm.a:lib_a-sf_fpclassify.o *libm.a:lib_a-wf_log2.o *libm.a:lib_a-sf_nan.o *libm.a:lib_a-sf_fabs.o *libm.a:lib_a-w_atan2.o *libm.a:lib_a-s_lib_ver.o
#define POST_LIBSEC_CRYPTO_OBJS *libsec_crypto.a:sha1.o *libsec_crypto.a:aes.o *libsec_crypto.a:softse_aes.o
#define POST_LIBTMEDIA_BACKEND_SENO_OBJS *libtmedia_backend_seno.a:bs_read.o
#define POST_LIBDRV_WIFI_RTL8723DS_OBJS *libdrv_wifi_rtl8723ds.a:rtl8723ds_devops.o *libdrv_wifi_rtl8723ds.a:wifi_util.o *libdrv_wifi_rtl8723ds.a:customer_aos_service.o *libdrv_wifi_rtl8723ds.a:osdep_service.o *libdrv_wifi_rtl8723ds.a:net_stack_intf.o *libdrv_wifi_rtl8723ds.a:ethernetif.o *libdrv_wifi_rtl8723ds.a:wifi_ind.o *libdrv_wifi_rtl8723ds.a:device_lock.o *libdrv_wifi_rtl8723ds.a:wifi_promisc.o *libdrv_wifi_rtl8723ds.a:rtwlan_bsp.o
#define POST_LIBGCC_OBJS *libgcc.a:unwind-dw2.o *libgcc.a:addtf3.o *libgcc.a:subtf3.o *libgcc.a:divtf3.o *libgcc.a:atomic.o *libgcc.a:letf2.o *libgcc.a:getf2.o
#define POST_LIBFATFS_OBJS *libfatfs.a:ff.o *libfatfs.a:fatfs_vfs.o *libfatfs.a:ffunicode.o *libfatfs.a:sd_disk.o *libfatfs.a:ffsystem.o *libfatfs.a:fattime.o *libfatfs.a:diskio.o
#define POST_LIBHTTPCLIENT_OBJS *libhttpclient.a:http_auth.o *libhttpclient.a:http_header.o *libhttpclient.a:http_utils.o
#define POST_LIBFLASH_DOORBELL_OBJS *libflash_doorbell.a:cli_faceai.o *libflash_doorbell.a:app_sys_except.o *libflash_doorbell.a:app_sys.o *libflash_doorbell.a:app_event.o *libflash_doorbell.a:app_gui.o *libflash_doorbell.a:cli_cmd.o *libflash_doorbell.a:bengine_test.o
#define POST_LIBCX_LITE_FACEAI_OBJS *libcx_lite_faceai.a:cx_faceai_user_manager.o *libcx_lite_faceai.a:cx_faceai_face_manager.o *libcx_lite_faceai.a:cx_faceai_crypto_test.o *libcx_lite_faceai.a:cx_faceai_crypto.o
#define POST_LIBMPEGTS_OBJS *libmpegts.a:bytes_io.o *libmpegts.a:ts_format.o *libmpegts.a:ts_misc.o
#define POST_LIBTRANSPORT_OBJS *libtransport.a:tls.o *libtransport.a:transport_ssl.o *libtransport.a:transport_tcp.o *libtransport.a:transport.o *libtransport.a:transport_utils.o
#define POST_LIBCVI_MW_SYS_OBJS *libcvi_mw_sys.a:cvi_misc.o *libcvi_mw_sys.a:cvi_log_proc.o
#define POST_LIBWIFI_PROVISIONING_OBJS *libwifi_provisioning.a:softap_prov.o *libwifi_provisioning.a:wifi_provisioning.o
#define POST_LIBCVI_PLATFORM_OBJS *libcvi_platform.a:disk_sd.o *libcvi_platform.a:media_nightvision.o
#define POST_LIBMINIALSA_OBJS *libminialsa.a:pcm.o
#define POST_LIBCHERRYUSB_OBJS *libcherryusb.a:usbh_core.o *libcherryusb.a:usbh_hub.o
#define POST_LIBNTP_OBJS *libntp.a:ntp.o
#define POST_LIBG711_OBJS *libg711.a:g711.o
#define POST_LIBKEY_MGR_OBJS *libkey_mgr.a:key_mgr.o *libkey_mgr.a:key_mgr_parser.o
#define POST_CONTENT  POST_LIBCVI_OSDRV_OBJS POST_LIBRTL8723DS_OBJS POST_LIBLVGL_OBJS POST_LIBSHL_C906_RTOS_OBJS POST_LIBLINK_VISUAL_OBJS POST_LIBIOT_SDK_OBJS POST_LIBSTDCPP_OBJS POST_LIBSAILFACE_NANO_OBJS POST_LIBC_OBJS POST_LIBCVI_KERNEL_OBJS POST_LIBCVI_MW_VENC_OBJS POST_LIBLWIP_OBJS POST_LIBMBEDTLS_OBJS POST_LIBCVI_SENSOR_OBJS POST_LIBTMEDIA_BACKEND_CVITEK_OBJS POST_LIBCX_LITE_BASE_OBJS POST_LIBCVI_MW_VPU_OBJS POST_LIBCX_LITE_LINKVISUAL_OBJS POST_LIBCVI_MW_ISP_ALGO_OBJS POST_LIBCSI_CV_OBJS POST_LIBCVIAI_OBJS POST_LIBTMEDIA_CORE_OBJS POST_LIBCVI_MW_AUDIO_OBJS POST_LIBCVI_RUNTIME_OBJS POST_LIBM_OBJS POST_LIBSEC_CRYPTO_OBJS POST_LIBTMEDIA_BACKEND_SENO_OBJS POST_LIBDRV_WIFI_RTL8723DS_OBJS POST_LIBGCC_OBJS POST_LIBFATFS_OBJS POST_LIBHTTPCLIENT_OBJS POST_LIBFLASH_DOORBELL_OBJS POST_LIBCX_LITE_FACEAI_OBJS POST_LIBMPEGTS_OBJS POST_LIBTRANSPORT_OBJS POST_LIBCVI_MW_SYS_OBJS POST_LIBWIFI_PROVISIONING_OBJS POST_LIBCVI_PLATFORM_OBJS POST_LIBMINIALSA_OBJS POST_LIBCHERRYUSB_OBJS POST_LIBNTP_OBJS POST_LIBG711_OBJS POST_LIBKEY_MGR_OBJS


ENTRY(Reset_Handler)
SECTIONS
{
 .text : {
  . = ALIGN(0x4) ;
  __text_start = .;
  __stext = . ;
  KEEP(*startup.o(*.text))
  KEEP(*startup.o(*.vectors))
  KEEP(*vectors.o(*.text))
  *(EXCLUDE_FILE(POST_CONTENT) .text*)
  *(.gnu.warning)
  *(.stub)
  *(.gnu.linkonce.t*)
  *(.glue_7t)
  *(.glue_7)
  *(.jcr)
  KEEP (*(.init))
  KEEP (*(.fini))
  . = ALIGN(0x8) ;
  PROVIDE(__ctbp = .);
  *(.call_table_data)
  *(.call_table_text)
  __etext = . ;
  __text_end = .;
 } > REGION_TEXT
 .rodata ALIGN(0x1000): {
  __rodata_start = .;
  __srodata = .;
  *(.rdata*)
  *(EXCLUDE_FILE(POST_CONTENT) .rodata*)
  *(.srodata*)
  . = ALIGN(0x8) ;

   __init_array_start = .;
   __ctors_start__ = .;
   KEEP (*(SORT(.init_array.*)))
   KEEP (*(.init_array))
   __init_array_end = .;
   __ctors_end__ = .;

   __fini_array_start = .;
   __dtors_start__ = .;
   KEEP (*(SORT(.fini_array.*)))
   KEEP (*(.fini_array))
   __fini_array_end = .;
   __dtors_end__ = .;

  __ctor_start__ = .;
  KEEP (*(SORT(.ctors.*)))
  KEEP (*(.ctors))
  __ctor_end__ = .;
  KEEP (*(SORT(.dtors.*)))
  KEEP (*(.dtors))
  __dtor_end__ = .;
  . = ALIGN(0x8) ;

    /************** added drivers **************/
    _cli_region_begin = .;
    KEEP(*(CliRegion))
    . = ALIGN(8);
    _cli_region_end = .;

    __core_driver_start__ = .;
    KEEP(*(.core_driver_entry))
    . = ALIGN(8);
    __core_driver_end__ = .;

    __bus_driver_start__ = .;
    KEEP(*(*.bus_driver_entry))
    __bus_driver_end__ = .;

    __early_driver_start__ = .;
    KEEP(*(*.early_driver_entry))
    __early_driver_end__ = .;

    __vfs_driver_start__ = .;
    KEEP(*(*.vfs_driver_entry))
    __vfs_driver_end__ = .;

    __level0_driver_start__ = .;
    KEEP(*(*.level0_driver_entry))
    __level0_driver_end__ = .;

    __level1_driver_start__ = .;
    KEEP(*(*.level1_driver_entry))
    __level1_driver_end__ = .;

    __level2_driver_start__ = .;
    KEEP(*(*.level2_driver_entry))
    __level2_driver_end__ = .;

    __level3_driver_start__ = .;
    KEEP(*(*.level3_driver_entry))
    __level3_driver_end__ = .;

    __post_driver_start__ = .;
    KEEP(*(*.post_driver_entry))
    __post_driver_end__ = .;
/************** end of drivers *********/
  . = ALIGN(0x8);
  __erodata = .;
  __rodata_end__ = .;
  __rodata_end = .;
 } > REGION_RODATA
 .data ALIGN(0x1000): {
  __data_start = .;
  __sdata = . ;
  __data_start__ = . ;
  data_start = . ;
  KEEP(*startup.o(*.vectors*))
  *(.got.plt)
  *(.got)
  *(.gnu.linkonce.r*)
  *(EXCLUDE_FILE(POST_CONTENT) .data*)
  *(.gnu.linkonce.d*)
  *(.gcc_except_table)
  *(.gcc_except_table*)
  __start_init_call = .;
  *(.initcall.init)
  __stop_init_call = .;
  __start_cmd = .;
  *(.bootloaddata.cmd)
  . = ALIGN(8) ;
  __stop_cmd = .;
  __global_pointer$ = .;
  *(.sdata)
  *(.sdata.*)
  *(.sdata2.*)
  *(.gnu.linkonce.s.*)
  *(__libc_atexit)
  *(__libc_subinit)
  *(__libc_subfreeres)
  *(.note.ABI-tag)
 . = ALIGN(0x8);
  __edata = .;
  __data_end__ = .;
  __data_end = .;
 } > REGION_DATA

 __post_load_start = .;
 .post_text ALIGN(0x1000):
 {
  __post_text_start = .;
     *(.text*)
  . = ALIGN(0x8);
  __post_text_end = .;
 } > REGION_POST_TEXT
 .post_rodata ALIGN(0x1000):
 {
  __post_rodata_start = .;
     *(.rodata*)
  . = ALIGN(0x8);
  __post_rodata_end = .;
 } > REGION_POST_RODATA
 .post_data ALIGN(0x1000):
 {
  __post_data_start = .;
     *(.data*)
  . = ALIGN(0x8);
  __post_data_end = .;
 } > REGION_POST_DATA
 __post_load_end = .;
 .bss ALIGN(0x1000): {
  __bss_start = .;
  __sbss = ALIGN(0x8) ;
  __bss_start__ = . ;
  *(.dynsbss)
  *(.sbss)
  *(.sbss.*)
  *(.scommon)
  *(.dynbss)
  *(.bss)
  *(.bss.*)
  *(COMMON)
  . = ALIGN(0x8) ;
  __ebss = . ;
  __bss_end__ = .;
  __end = . ;
  end = . ;
 __bss_end = .;
 } > REGION_BSS
 ._user_heap ALIGN(0x1000) (NOLOAD) : {
  __heap_start0 = .;
  *(.stack*)
  . = ALIGN(0x8) ;
  __heap_start = .;
  . = ALIGN(0x8) ;
  . += __min_heap_size;
  . = ALIGN(0x8) ;
 } > REGION_BSS
 . = ALIGN(16);
 _mmu_text_start        = __text_start;
 _mmu_text_end          = __text_end;
 _mmu_text_len          = _mmu_text_end - _mmu_text_start;

 _mmu_rodata_start      = __rodata_start;
 _mmu_rodata_end        = __rodata_end;
 _mmu_rodata_len        = _mmu_rodata_end - _mmu_rodata_start;

 _mmu_data_start        = __data_start;
 _mmu_data_end          = __data_end;
 _mmu_data_len          = _mmu_data_end - _mmu_data_start;

 _mmu_post_text_start   = __post_text_start;
 _mmu_post_text_end     = __post_text_end;
 _mmu_post_text_len     = _mmu_post_text_end - _mmu_post_text_start;

 _mmu_post_rodata_start = __post_rodata_start;
 _mmu_post_rodata_end   = __post_rodata_end;
 _mmu_post_rodata_len   = _mmu_post_rodata_end - _mmu_post_rodata_start;

 _mmu_post_data_start   = __post_data_start;
 _mmu_post_data_end     = __post_data_end;
 _mmu_post_data_len     = _mmu_post_data_end - _mmu_post_data_start;

 _mmu_bss_start         = __bss_start;
 _mmu_bss_end           = __bss_end;
 _mmu_bss_len           = _mmu_bss_end - _mmu_bss_start;

 _mmu_heap_start        = __heap_start0;
 _mmu_heap_end          = __heap_end;
 _mmu_heap_len          = _mmu_heap_end - _mmu_heap_start;
}
