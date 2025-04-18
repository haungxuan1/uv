#pragma once

#define PQM_Ts_MSG (WM_USER + 10)
#define PQM_Ts_Image (PQM_Ts_MSG + 1)

// 相机信号 - 大恒
#define PQM_CAMERA_MSG	  (WM_USER + 200)
#define PQM_CAMERA_ARRIVAL (PQM_CAMERA_MSG + 1)
#define PQM_CAMERA_REMOVE  (PQM_CAMERA_MSG + 2)
#define PQM_CAMERA_CONNECT (PQM_CAMERA_MSG + 3)
#define PQM_CAMERA_DISCONNECT (PQM_CAMERA_MSG + 4)
#define PQM_CAMERA_GRAB (PQM_CAMERA_MSG + 5)
#define PQM_CAMERA_FREEZE (PQM_CAMERA_MSG + 6)
#define PQM_CAMERA_CAPTURE (PQM_CAMERA_MSG + 7)

// 匹配信号
#define PQM_MATCH_MSG	(WM_USER + 300)
#define PQM_MATCH_FRAME		(PQM_MATCH_MSG + 1)
#define PQM_MATCH_MATRIX	(PQM_MATCH_MSG + 2)
#define PQM_MATCH_CONVERT	(PQM_MATCH_MSG + 3)
#define PQM_MATCH_SENT		(PQM_MATCH_MSG + 4)
#define PQM_MATCH_SCHEDULE	(PQM_MATCH_MSG + 5)
#define PQM_MATCH_IM_TCP_FINISH (PQM_MATCH_MSG + 6)
#define PQM_MATCH_TRANSPARENT	(PQM_MATCH_MSG + 7)
#define PQM_MATCH_BACKGROUND	(PQM_MATCH_MSG + 8)

// 主菜单消息
#define PQM_MAIN_MSG			(WM_USER + 400)
#define PQM_MAIN_CAMERA_PARAM	(PQM_MAIN_MSG + 1)
#define PQM_MAIN_TEMP			(PQM_MAIN_MSG + 8)
#define PQM_MAIN_MAKE_CAMERA_PARAM	(PQM_MAIN_MSG + 9)
#define PQM_MAIN_UPDATE_CFG_PARAM	(PQM_MAIN_MSG + 10)

// 临时增加跳白消息
#define PQM_FLEECE_MSG		(WM_USER + 4500)
#define PQM_FLEECE_BEGIN	(PQM_FLEECE_MSG + 1)
#define PQM_FLEECE_SEND		(PQM_FLEECE_MSG + 2)
#define PQM_FLEECE_RECV		(PQM_FLEECE_MSG + 3)
#define PQM_FLEECE_PRT		(PQM_FLEECE_MSG + 4)
#define PQM_FLEECE_RESTRICT	(PQM_FLEECE_MSG + 5)
#define PQM_FRAME_RESET_IDX (PQM_FLEECE_MSG + 6)
#define PQM_FLEECE_END		(PQM_FLEECE_MSG + 7)
#define PQM_FLEECE_SEND_STOP		(PQM_FLEECE_MSG + 8)
#define PQM_FLEECE_SEND_STOP_END		(PQM_FLEECE_MSG + 9)
#define PQM_FEATURE_SEND_STOP		(PQM_FLEECE_MSG + 10)

// 校准消息
#define PQM_CALIBRATOR_MSG		(WM_USER + 500)

#define PQM_CALIBRATOR_HISTOGRAM		(PQM_CALIBRATOR_MSG + 3)
#define PQM_CALIBRATOR_FOREGROUND		(PQM_CALIBRATOR_MSG + 4)
#define PQM_CALIBRATOR_REFERENCE		(PQM_CALIBRATOR_MSG + 7)
#define PQM_CALIBRATOR_ALIGNING		(PQM_CALIBRATOR_MSG + 10)
#define PQM_CALIBRATOR_ALIGN		(PQM_CALIBRATOR_MSG + 11)
#define PQM_CALIBRATOR_MERGING		(PQM_CALIBRATOR_MSG + 14)
#define PQM_CALIBRATOR_MERGE		(PQM_CALIBRATOR_MSG + 15)
#define PQM_CALIBRATOR_ROI			(PQM_CALIBRATOR_MSG + 18)
#define PQM_CALIBRATOR_PERSPECTIVE		(PQM_CALIBRATOR_MSG + 21)
#define PQM_CALIBRATOR_TRANSLATING			(PQM_CALIBRATOR_MSG + 24)
#define PQM_CALIBRATOR_HOMOGRAPHY			(PQM_CALIBRATOR_MSG + 25)
#define PQM_CALIBRATOR_MERGING2		(PQM_CALIBRATOR_MSG + 28)
#define PQM_CALIBRATOR_MERGE2		(PQM_CALIBRATOR_MSG + 29)
#define PQM_CALIBRATOR_DPI			(PQM_CALIBRATOR_MSG + 32)
#define PQM_CALIBRATOR_CAPTURE		(PQM_CALIBRATOR_MSG + 33)
#define PQM_CALIBRATOR_QENCODE		(PQM_CALIBRATOR_MSG + 34)
#define PQM_CALIBRATOR_CHART		(PQM_CALIBRATOR_MSG + 35)
#define PQM_CALIBRATOR_SCAN			(PQM_CALIBRATOR_MSG + 36)
#define PQM_CALIBRATOR_PRINT		(PQM_CALIBRATOR_MSG + 37)
#define PQM_CALIBRATOR_LOAD_PARAM	(PQM_CALIBRATOR_MSG + 38)
#define PQM_CALIBRATOR_TYPE_CHANGED	(PQM_CALIBRATOR_MSG + 39)
#define PQM_CALIBRATOR_PARAM_BEGIN		(PQM_CALIBRATOR_MSG + 40)
#define PQM_CALIBRATOR_PRO_FOREGROUND	(PQM_CALIBRATOR_MSG + 41)
#define PQM_CALIBRATOR_PRO_REFERENCE	(PQM_CALIBRATOR_MSG + 42)
#define PQM_CALIBRATOR_PRO_ALIGN		(PQM_CALIBRATOR_MSG + 43)
#define PQM_CALIBRATOR_PRO_MERGE		(PQM_CALIBRATOR_MSG + 44)
#define PQM_CALIBRATOR_PRO_ROI			(PQM_CALIBRATOR_MSG + 45)
#define PQM_CALIBRATOR_PRO_PRESPECTIVE  (PQM_CALIBRATOR_MSG + 46)
#define PQM_CALIBRATOR_PRO_HOMOGRAPHY	(PQM_CALIBRATOR_MSG + 47)
#define PQM_CALIBRATOR_PRO_MERGE2		(PQM_CALIBRATOR_MSG + 50)
#define PQM_CALIBRATOR_PRO_DPI			(PQM_CALIBRATOR_MSG + 51)
#define PQM_CALIBRATOR_PARAM_END		(PQM_CALIBRATOR_MSG + 52)
#define PQM_CALIBRATOR_PRO_CRAB			(PQM_CALIBRATOR_MSG + 53)
#define PQM_CALIBRATOR_PRO_FREEZE		(PQM_CALIBRATOR_MSG + 54)
#define PQM_CALIBRATOR_DISABLE			(PQM_CALIBRATOR_MSG + 55)
#define PQM_CALIBRATOR_ENABLE			(PQM_CALIBRATOR_MSG + 56)
#define PQM_CALIBRATOR_PARAM_CHANGED	(PQM_CALIBRATOR_MSG + 57)


#define PQM_MOLD_PARAM	(WM_USER + 1020)
#define PQM_MOLD_MATRIX (WM_USER + 1021)

// 校准消息
#define PQM_FEATURE_MSG		(WM_USER + 600)
#define PQM_FEATURE_CONVERT	(PQM_FEATURE_MSG + 1)
#define PQM_FEATURE_Ts		(PQM_FEATURE_MSG + 2)

// 边框变化
#define PQM_FEATURE_FRAME_CHANGED	(WM_USER + 1030)
#define PQM_FEATURE_ZOOM			(WM_USER + 1031)
#define PQM_FEATURE_DEFAULT			(WM_USER + 1032)
#define PQM_FEATURE_RESET_PARAM		(WM_USER + 1033)
#define PQM_FEATURE_PARAM			(WM_USER + 1034)
#define PQM_FEATURE_SYNC_OS_PARAM	(WM_USER + 1035)
#define PQM_FEATURE_OK				(WM_USER + 1036)

#define PQM_TEMP_MATCHED (WM_USER + 1041)


#define PQM_PRINTER_MOSAIC (WM_USER + 1050)
#define PQM_PRINTER_POSITION (WM_USER + 1051)
#define PQM_PRINTER_PRT_MATRIX (WM_USER + 1052)
#define PQM_PRINTER_MAT2PRT (WM_USER + 1053)

#define PQM_CAMERA_CALIBRATION_VIEW	(WM_USER + 1060)
#define PQM_CAMERA_CALIBRATING		(WM_USER + 1061)
#define PQM_PASS_CALIBRATE		(WM_USER + 1062)
#define PQM_PASS_CALIBRATING	(WM_USER + 1063)
#define PQM_PASS_CALIBRATION	(WM_USER + 1064)
#define PQM_PASS_CALIBRATED		(WM_USER + 1065)





#define PQM_PADDING_POSITION		(WM_USER + 1103)


#define PQM_PADDING_ALIGNED		(WM_USER + 1105)

#define PQM_PADDING_MERGING		(WM_USER + 1106)
#define PQM_PADDING_MERGED		(WM_USER + 1107)


#define PQM_PADDING_ROI_END		(WM_USER + 1109)

#define PQM_PADDING_SLOPE		(WM_USER + 1110)



#define PQM_PADDING_HOMOGRAPHY		(WM_USER + 1112)
#define PQM_PADDING_HOMOGRAPHY_END		(WM_USER + 1113)

#define PQM_PADDING_DPI		(WM_USER + 1114)



// ImControl 控件专属消息
#define PQM_IM_CONTROM	(WM_USER + 2000)

#define PQM_IM_CTRL_MSG (PQM_IM_CONTROM)
#define PQM_IM_CTRL_CLICKED (PQM_IM_CTRL_MSG + 1)
#define PQM_IM_CTRL_CHANGED (PQM_IM_CTRL_MSG + 2)

#define PQM_IM_CENTER_MSG	(PQM_IM_CONTROM + 100)
#define PQM_IM_CENTER_CREATE	(PQM_IM_CENTER_MSG + 1)
#define PQM_IM_CENTER_DESTROY	(PQM_IM_CENTER_MSG + 2)
#define PQM_IM_CENTER_SYNC	(PQM_IM_CENTER_MSG + 4)


#define PQM_IM_LAYER_MSG (PQM_IM_CONTROM + 200)
#define PQM_IM_LAYER_CLICKED_ENABLE	(PQM_IM_LAYER_MSG + 1)
#define PQM_IM_LAYER_CLICKED_BACKGROUND	(PQM_IM_LAYER_MSG + 2)
#define PQM_IM_LAYER_NAME_CHANGED	(PQM_IM_LAYER_MSG + 3)
#define PQM_IM_LAYER_FOCUS_CHANGED		(PQM_IM_LAYER_MSG + 4)

#define PQM_IM_LAYER_CLEAR	(PQM_IM_LAYER_MSG + 5)
#define PQM_IM_LAYER_CREATE	(PQM_IM_LAYER_MSG + 6)
#define PQM_IM_LAYER_SYNC_LAYER_CTRL (PQM_IM_LAYER_MSG + 7)
#define PQM_IM_LAYER_SYNC_SCROLLBAR_CTRL (PQM_IM_LAYER_MSG + 8)


#define PQM_IM_LAYER_FOCUS (PQM_IM_LAYER_MSG + 1)	//	已废弃
#define PQM_IM_LAYER_ENABLE (PQM_IM_LAYER_MSG + 2)
#define PQM_IM_LAYER_PREVIEW	(PQM_IM_CONTROM + 3)
#define PQM_IM_LAYER_SCALING	(PQM_IM_CONTROM + 4)


#define PQM_IM_PARAM_MSG (PQM_IM_CONTROM + 300)
#define PQM_IM_PARAM_CLICKED_ASSOCIATED		(PQM_IM_PARAM_MSG + 1)
#define PQM_IM_PARAM_CLICKED_MOVING			(PQM_IM_PARAM_MSG + 2)
#define PQM_IM_PARAM_CLICKED_SCALING		(PQM_IM_PARAM_MSG + 3)
#define PQM_IM_PARAM_CLICKED_ROTATING		(PQM_IM_PARAM_MSG + 4)
#define PQM_IM_PARAM_CLICKED_SELECTING		(PQM_IM_PARAM_MSG + 5)
#define PQM_IM_PARAM_CLICKED_COPY			(PQM_IM_PARAM_MSG + 6)
#define PQM_IM_PARAM_CLICKED_SAVE			(PQM_IM_PARAM_MSG + 7)
#define PQM_IM_PARAM_LEFT_CHANGED				(PQM_IM_PARAM_MSG + 8)
#define PQM_IM_PARAM_TOP_CHANGED				(PQM_IM_PARAM_MSG + 9)
#define PQM_IM_PARAM_WIDTH_CHANGED				(PQM_IM_PARAM_MSG + 10)
#define PQM_IM_PARAM_HEIGHT_CHANGED				(PQM_IM_PARAM_MSG + 11)
#define PQM_IM_PARAM_OPACITY_CHANGED		(PQM_IM_PARAM_MSG + 12)
#define PQM_IM_PARAM_HOR_SCAL_CHANGED		(PQM_IM_PARAM_MSG + 13)
#define PQM_IM_PARAM_VER_SCAL_CHANGED		(PQM_IM_PARAM_MSG + 14)
#define PQM_IM_PARAM_ANGLE_CHANGED			(PQM_IM_PARAM_MSG + 15)
#define PQM_IM_PARAM_CLICKED_SELECTALL		(PQM_IM_PARAM_MSG + 16)


#define PQM_IM_VIEW_MSG (PQM_IM_CONTROM + 400)
#define PQM_IM_VIEW_MOVING	 	(PQM_IM_VIEW_MSG + 1)
#define PQM_IM_VIEW_MOVED	 	(PQM_IM_VIEW_MSG + 2)
#define PQM_IM_VIEW_SCALING		(PQM_IM_VIEW_MSG + 3)
#define PQM_IM_VIEW_SCALED		(PQM_IM_VIEW_MSG + 4)
#define PQM_IM_VIEW_SCALE		(PQM_IM_VIEW_MSG + 5) // 滚轮引起的缩放
#define PQM_IM_VIEW_ROTATING	 (PQM_IM_VIEW_MSG + 6)
#define PQM_IM_VIEW_ROTATED	 	(PQM_IM_VIEW_MSG + 7)
#define PQM_IM_VIEW_CENTER		(PQM_IM_VIEW_MSG + 8)
#define PQM_IM_VIEW_PT2PT		(PQM_IM_VIEW_MSG + 9)


#define PQM_IM_VIEW_RENDER (PQM_IM_VIEW_MSG + 10)
#define PQM_IM_VIEW_DRAGGING (PQM_IM_VIEW_MSG + 12)
#define PQM_IM_VIEW_DRAG	(PQM_IM_VIEW_MSG + 13)
#define PQM_IM_VIEW_CONTRAST	(PQM_IM_VIEW_MSG + 14)

#define PQM_IM_CONTROL_MSG (PQM_IM_CONTROM + 500)
#define PQM_IM_CONTROL_SYNC_RENDER_SIZE	(PQM_IM_CONTROL_MSG + 1)
#define PQM_IM_CONTROL_SYNC_SELECTED	(PQM_IM_CONTROL_MSG + 2)
#define PQM_IM_CONTROL_COPY_LAYER		(PQM_IM_CONTROL_MSG + 3)
#define PQM_IM_CONTROL_SAVE_LAYER		(PQM_IM_CONTROL_MSG + 4)
#define PQM_IM_CONTROL_SAVE_BACKGROUND		(PQM_IM_CONTROL_MSG + 5)
