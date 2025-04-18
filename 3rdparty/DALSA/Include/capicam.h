//
// Coreco API - Camera control functions.
//

#ifndef _CAPICAM_H_
#define _CAPICAM_H_

#include <capidef.h>
#include <capiacqd.h>

/****************************************************************************
*	Camera Capabilities
*
*	Last capability #: 0
****************************************************************************/
#define CORCAM_CAP_FIRST				0
#define CORCAM_CAP( n, size)			((((n) + CORCAM_CAP_FIRST) << 16) | (size))

/****************************************************************************
*
*	Camera Parameters
*
*	Last Parameter #: 0x2a
****************************************************************************/
#define CORCAM_PRM_FIRST						CORACQ_PRM_CAM_FIRST
#define CORCAM_PRM( n, size)					( (((n) + CORCAM_PRM_FIRST) << 16) | (size))

#define CORCAM_PRM_CHANNEL						CORACQ_PRM_CHANNEL
#define CORCAM_PRM_FRAME						CORACQ_PRM_FRAME
#define CORCAM_PRM_INTERFACE					CORACQ_PRM_INTERFACE
#define CORCAM_PRM_SCAN							CORACQ_PRM_SCAN
#define CORCAM_PRM_SIGNAL						CORACQ_PRM_SIGNAL
#define CORCAM_PRM_VIDEO						CORACQ_PRM_VIDEO
#define CORCAM_PRM_PIXEL_DEPTH				CORACQ_PRM_PIXEL_DEPTH
#define CORCAM_PRM_VIDEO_STD					CORACQ_PRM_VIDEO_STD
#define CORCAM_PRM_RESERVED					CORACQ_PRM_RESERVED
#define CORCAM_PRM_RESERVED					CORACQ_PRM_RESERVED
#define CORCAM_PRM_FIELD_ORDER				CORACQ_PRM_FIELD_ORDER
#define CORCAM_PRM_HACTIVE						CORACQ_PRM_HACTIVE					
#define CORCAM_PRM_HSYNC						CORACQ_PRM_HSYNC
#define CORCAM_PRM_VACTIVE						CORACQ_PRM_VACTIVE
#define CORCAM_PRM_VSYNC						CORACQ_PRM_VSYNC
#define CORCAM_PRM_HFRONT_PORCH				CORACQ_PRM_HFRONT_PORCH
#define CORCAM_PRM_HBACK_PORCH				CORACQ_PRM_HBACK_PORCH
#define CORCAM_PRM_COUPLING					CORACQ_PRM_COUPLING
#define CORCAM_PRM_TRIGGER_EXP_SIGNAL		CORACQ_PRM_TRIGGER_EXP_SIGNAL
#define CORCAM_PRM_VFRONT_PORCH				CORACQ_PRM_VFRONT_PORCH
#define CORCAM_PRM_VBACK_PORCH				CORACQ_PRM_VBACK_PORCH
#define CORCAM_PRM_HFRONT_INVALID			CORACQ_PRM_HFRONT_INVALID
#define CORCAM_PRM_HBACK_INVALID				CORACQ_PRM_HBACK_INVALID
#define CORCAM_PRM_VFRONT_INVALID			CORACQ_PRM_VFRONT_INVALID
#define CORCAM_PRM_VBACK_INVALID				CORACQ_PRM_VBACK_INVALID
#define CORCAM_PRM_PIXEL_CLK_SRC				CORACQ_PRM_PIXEL_CLK_SRC
#define CORCAM_PRM_PIXEL_CLK_INT				CORACQ_PRM_PIXEL_CLK_INT
#define CORCAM_PRM_PIXEL_CLK_11				CORACQ_PRM_PIXEL_CLK_11
#define CORCAM_PRM_PIXEL_CLK_EXT				CORACQ_PRM_PIXEL_CLK_EXT
#define CORCAM_PRM_SYNC							CORACQ_PRM_SYNC
#define CORCAM_PRM_HSYNC_POLARITY			CORACQ_PRM_HSYNC_POLARITY
#define CORCAM_PRM_VSYNC_POLARITY			CORACQ_PRM_VSYNC_POLARITY
#define CORCAM_PRM_FRAME_INTEGRATE_METHOD	CORACQ_PRM_FRAME_INTEGRATE_METHOD
#define CORCAM_PRM_FRAME_INTEGRATE_POLARITY	CORACQ_PRM_FRAME_INTEGRATE_POLARITY
#define CORCAM_PRM_TIME_INTEGRATE_METHOD		CORACQ_PRM_TIME_INTEGRATE_METHOD
#define CORCAM_PRM_TIME_INTEGRATE_POLARITY	CORACQ_PRM_TIME_INTEGRATE_POLARITY
#define CORCAM_PRM_CAM_TRIGGER_METHOD			CORACQ_PRM_CAM_TRIGGER_METHOD
#define CORCAM_PRM_CAM_TRIGGER_POLARITY		CORACQ_PRM_CAM_TRIGGER_POLARITY
#define CORCAM_PRM_CAM_TRIGGER_DURATION		CORACQ_PRM_CAM_TRIGGER_DURATION
#define CORCAM_PRM_TRIGGER_EXP_SIGNAL			CORACQ_PRM_TRIGGER_EXP_SIGNAL
#define CORCAM_PRM_CAM_RESET_METHOD				CORACQ_PRM_CAM_RESET_METHOD
#define CORCAM_PRM_CAM_RESET_POLARITY			CORACQ_PRM_CAM_RESET_POLARITY
#define CORCAM_PRM_CAM_RESET_DURATION			CORACQ_PRM_CAM_RESET_DURATION
#define CORCAM_PRM_CAM_NAME						CORACQ_PRM_CAM_NAME
#define CORCAM_PRM_LINE_INTEGRATE_METHOD		CORACQ_PRM_LINE_INTEGRATE_METHOD
#define CORCAM_PRM_LINE_INTEGRATE_POLARITY	CORACQ_PRM_LINE_INTEGRATE_POLARITY
#define CORCAM_PRM_LINE_INTEGRATE_DELAY		CORACQ_PRM_LINE_INTEGRATE_DELAY
#define CORCAM_PRM_LINE_TRIGGER_METHOD			CORACQ_PRM_LINE_TRIGGER_METHOD
#define CORCAM_PRM_LINE_TRIGGER_POLARITY		CORACQ_PRM_LINE_TRIGGER_POLARITY
#define CORCAM_PRM_LINE_TRIGGER_DELAY			CORACQ_PRM_LINE_TRIGGER_DELAY
#define CORCAM_PRM_LINE_TRIGGER_DURATION		CORACQ_PRM_LINE_TRIGGER_DURATION
#define CORCAM_PRM_TAPS								CORACQ_PRM_TAPS
#define CORCAM_PRM_TAP_OUTPUT						CORACQ_PRM_TAP_OUTPUT
#define CORCAM_PRM_TAP_1_DIRECTION				CORACQ_PRM_TAP_1_DIRECTION
#define CORCAM_PRM_TAP_2_DIRECTION				CORACQ_PRM_TAP_2_DIRECTION
#define CORCAM_PRM_TAP_3_DIRECTION				CORACQ_PRM_TAP_3_DIRECTION
#define CORCAM_PRM_TAP_4_DIRECTION				CORACQ_PRM_TAP_4_DIRECTION
#define CORCAM_PRM_TAP_5_DIRECTION				CORACQ_PRM_TAP_5_DIRECTION
#define CORCAM_PRM_TAP_6_DIRECTION				CORACQ_PRM_TAP_6_DIRECTION
#define CORCAM_PRM_TAP_7_DIRECTION				CORACQ_PRM_TAP_7_DIRECTION
#define CORCAM_PRM_TAP_8_DIRECTION				CORACQ_PRM_TAP_8_DIRECTION
#define CORCAM_PRM_PIXEL_CLK_DETECTION			CORACQ_PRM_PIXEL_CLK_DETECTION
#define CORCAM_PRM_CHANNELS_ORDER				CORACQ_PRM_CHANNELS_ORDER
#define CORCAM_PRM_LINESCAN_DIRECTION			CORACQ_PRM_LINESCAN_DIRECTION
#define CORCAM_PRM_LINESCAN_DIRECTION_POLARITY	CORACQ_PRM_LINESCAN_DIRECTION_POLARITY
#define CORCAM_PRM_CAM_LINE_TRIGGER_FREQ_MIN	CORACQ_PRM_CAM_LINE_TRIGGER_FREQ_MIN
#define CORCAM_PRM_CAM_LINE_TRIGGER_FREQ_MAX	CORACQ_PRM_CAM_LINE_TRIGGER_FREQ_MAX
#define CORCAM_PRM_CAM_TIME_INTEGRATE_DURATION_MIN		CORACQ_PRM_CAM_TIME_INTEGRATE_DURATION_MIN	
#define CORCAM_PRM_CAM_TIME_INTEGRATE_DURATION_MAX		CORACQ_PRM_CAM_TIME_INTEGRATE_DURATION_MAX	
#define CORCAM_PRM_CONNECTOR_HD_INPUT					CORACQ_PRM_CONNECTOR_HD_INPUT
#define CORCAM_PRM_CONNECTOR_VD_INPUT					CORACQ_PRM_CONNECTOR_VD_INPUT
#define CORCAM_PRM_CONNECTOR_RESET_TRIGGER_INPUT		CORACQ_PRM_CONNECTOR_RESET_TRIGGER_INPUT
#define CORCAM_PRM_TIME_INTEGRATE_PULSE_POLARITY		CORACQ_PRM_TIME_INTEGRATE_PULSE_POLARITY
#define CORCAM_PRM_TIME_INTEGRATE_PULSE_DELAY			CORACQ_PRM_TIME_INTEGRATE_PULSE_DELAY
#define CORCAM_PRM_TIME_INTEGRATE_PULSE_DURATION		CORACQ_PRM_TIME_INTEGRATE_PULSE_DURATION
#define CORCAM_PRM_TIME_INTEGRATE_PULSE1_POLARITY		CORACQ_PRM_TIME_INTEGRATE_PULSE1_POLARITY
#define CORCAM_PRM_TIME_INTEGRATE_PULSE1_DELAY			CORACQ_PRM_TIME_INTEGRATE_PULSE1_DELAY
#define CORCAM_PRM_TIME_INTEGRATE_PULSE1_DURATION		CORACQ_PRM_TIME_INTEGRATE_PULSE1_DURATION
#define CORCAM_PRM_CAM_IO_CONTROL						CORACQ_PRM_CAM_IO_CONTROL
#define CORCAM_PRM_CONNECTOR_EXPOSURE_INPUT				CORACQ_PRM_CONNECTOR_EXPOSURE_INPUT
#define CORCAM_PRM_TIME_INTEGRATE_PULSE0_POLARITY		CORACQ_PRM_TIME_INTEGRATE_PULSE0_POLARITY
#define CORCAM_PRM_TIME_INTEGRATE_PULSE0_DELAY			CORACQ_PRM_TIME_INTEGRATE_PULSE0_DELAY
#define CORCAM_PRM_TIME_INTEGRATE_PULSE0_DURATION		CORACQ_PRM_TIME_INTEGRATE_PULSE0_DURATION
#define CORCAM_PRM_LINE_INTEGRATE_PULSE1_POLARITY		CORACQ_PRM_LINE_INTEGRATE_PULSE1_POLARITY
#define CORCAM_PRM_LINE_INTEGRATE_PULSE1_DELAY			CORACQ_PRM_LINE_INTEGRATE_PULSE1_DELAY
#define CORCAM_PRM_LINE_INTEGRATE_PULSE1_DURATION		CORACQ_PRM_LINE_INTEGRATE_PULSE1_DURATION
#define CORCAM_PRM_LINE_INTEGRATE_PULSE0_POLARITY		CORACQ_PRM_LINE_INTEGRATE_PULSE0_POLARITY
#define CORCAM_PRM_LINE_INTEGRATE_PULSE0_DELAY			CORACQ_PRM_LINE_INTEGRATE_PULSE0_DELAY
#define CORCAM_PRM_LINE_INTEGRATE_PULSE0_DURATION		CORACQ_PRM_LINE_INTEGRATE_PULSE0_DURATION
#define CORCAM_PRM_CAM_COMPANY_NAME						CORACQ_PRM_CAM_COMPANY_NAME
#define CORCAM_PRM_CAM_MODEL_NAME						CORACQ_PRM_CAM_MODEL_NAME
#define CORCAM_PRM_VIDEO_LEVEL_MIN						CORACQ_PRM_VIDEO_LEVEL_MIN
#define CORCAM_PRM_VIDEO_LEVEL_MAX						CORACQ_PRM_VIDEO_LEVEL_MAX
#define CORCAM_PRM_CONNECTOR_LINE_TRIGGER_INPUT       	CORACQ_PRM_CONNECTOR_LINE_TRIGGER_INPUT      
#define CORCAM_PRM_CONNECTOR_LINE_INTEGRATE_INPUT     	CORACQ_PRM_CONNECTOR_LINE_INTEGRATE_INPUT    
#define CORCAM_PRM_CONNECTOR_LINESCAN_DIRECTION_INPUT 	CORACQ_PRM_CONNECTOR_LINESCAN_DIRECTION_INPUT
#define CORCAM_PRM_CAMLINK_CONFIGURATION              	CORACQ_PRM_CAMLINK_CONFIGURATION             
#define CORCAM_PRM_SERIAL_PORT_BAUD_RATE   			CORACQ_PRM_SERIAL_PORT_BAUD_RATE   
#define CORCAM_PRM_SERIAL_PORT_DATA_BITS   			CORACQ_PRM_SERIAL_PORT_DATA_BITS   
#define CORCAM_PRM_SERIAL_PORT_PARITY 				   CORACQ_PRM_SERIAL_PORT_PARITY 
#define CORCAM_PRM_SERIAL_PORT_STOP_BITS   			CORACQ_PRM_SERIAL_PORT_STOP_BITS   
#define CORCAM_PRM_SERIAL_PORT_FLOW_CONTROL			CORACQ_PRM_SERIAL_PORT_FLOW_CONTROL
#define CORCAM_PRM_SERIAL_PORT_INIT_STRING         CORACQ_PRM_SERIAL_PORT_INIT_STRING 
#define CORCAM_PRM_DATA_VALID_ENABLE	 				CORACQ_PRM_DATA_VALID_ENABLE
#define CORCAM_PRM_DATA_VALID_POLARITY	 				CORACQ_PRM_DATA_VALID_POLARITY
#define CORCAM_PRM_CONNECTOR_PIXEL_CLK_OUTPUT      CORACQ_PRM_CONNECTOR_PIXEL_CLK_OUTPUT
#define CORCAM_PRM_CONNECTOR_WEN_OUTPUT            CORACQ_PRM_CONNECTOR_WEN_OUTPUT
#define CORCAM_PRM_WEN_POLARITY                    CORACQ_PRM_WEN_POLARITY
#define CORCAM_PRM_TAP_9_DIRECTION				CORACQ_PRM_TAP_9_DIRECTION
#define CORCAM_PRM_TAP_10_DIRECTION				CORACQ_PRM_TAP_10_DIRECTION
#define CORCAM_PRM_TAP_11_DIRECTION				CORACQ_PRM_TAP_11_DIRECTION
#define CORCAM_PRM_TAP_12_DIRECTION				CORACQ_PRM_TAP_12_DIRECTION
#define CORCAM_PRM_TAP_13_DIRECTION				CORACQ_PRM_TAP_13_DIRECTION
#define CORCAM_PRM_TAP_14_DIRECTION				CORACQ_PRM_TAP_14_DIRECTION
#define CORCAM_PRM_TAP_15_DIRECTION				CORACQ_PRM_TAP_15_DIRECTION
#define CORCAM_PRM_TAP_16_DIRECTION				CORACQ_PRM_TAP_16_DIRECTION
#define CORCAM_PRM_TIMESLOT                  CORACQ_PRM_TIMESLOT
#define CORCAM_PRM_BAYER_ALIGNMENT				CORACQ_PRM_BAYER_ALIGNMENT
#define CORCAM_PRM_CAM_CONTROL_DURING_READOUT   CORACQ_PRM_CAM_CONTROL_DURING_READOUT
#define CORCAM_PRM_CX4_CONFIGURATION            CORACQ_PRM_CX4_CONFIGURATION   
#define CORCAM_PRM_NB_INPUT_CONNECTORS			CORACQ_PRM_NB_INPUT_CONNECTORS 
#define CORCAM_PRM_CLHS_NB_CONNECTORS			CORACQ_PRM_CLHS_NB_CONNECTORS // Obsolete Name
#define CORCAM_PRM_CLHS_CONNECTOR_TYPE			CORACQ_PRM_CLHS_CONNECTOR_TYPE 
#define CORCAM_PRM_NB_DATA_LANES					CORACQ_PRM_NB_DATA_LANES 
#define CORCAM_PRM_CLHS_LANE						CORACQ_PRM_CLHS_LANE         // Obsolete Name
#define CORCAM_PRM_CLHS_COMMAND_CHANNEL		CORACQ_PRM_CLHS_COMMAND_CHANNEL 
#define CORCAM_PRM_BIT_TRANSFER_RATE		   CORACQ_PRM_BIT_TRANSFER_RATE 
#define CORCAM_PRM_CLHS_BIT_TRANSFER_RATE		CORACQ_PRM_CLHS_BIT_TRANSFER_RATE     // Obsolete Name
#define CORCAM_PRM_CXP_CONFIGURATION         CORACQ_PRM_CXP_CONFIGURATION
#define CORCAM_PRM_CLHS_CONFIGURATION        CORACQ_PRM_CLHS_CONFIGURATION   
#define CORCAM_PRM_CAM_CONTROL_READOUT_DELAY	CORACQ_PRM_CAM_CONTROL_READOUT_DELAY

#define CORCAM_PRM_LAST									CORACQ_PRM_CAM_LAST


/****************************************************************************
*
*	Camera Values
*
****************************************************************************/
#define CORCAM_VAL_BAYER_ALIGN_GB_RG            CORACQ_VAL_BAYER_ALIGN_GB_RG
#define CORCAM_VAL_BAYER_ALIGN_BG_GR            CORACQ_VAL_BAYER_ALIGN_BG_GR
#define CORCAM_VAL_BAYER_ALIGN_RG_GB            CORACQ_VAL_BAYER_ALIGN_RG_GB
#define CORCAM_VAL_BAYER_ALIGN_GR_BG            CORACQ_VAL_BAYER_ALIGN_GR_BG

#define CORCAM_VAL_BAYER_DECODER_METHOD_1       CORACQ_VAL_BAYER_DECODER_METHOD_1
#define CORCAM_VAL_BAYER_DECODER_METHOD_2       CORACQ_VAL_BAYER_DECODER_METHOD_2
#define CORCAM_VAL_BAYER_DECODER_METHOD_3       CORACQ_VAL_BAYER_DECODER_METHOD_3
#define CORCAM_VAL_BAYER_DECODER_METHOD_4       CORACQ_VAL_BAYER_DECODER_METHOD_4
#define CORCAM_VAL_BAYER_DECODER_METHOD_5       CORACQ_VAL_BAYER_DECODER_METHOD_5
#define CORCAM_VAL_BAYER_DECODER_METHOD_6       CORACQ_VAL_BAYER_DECODER_METHOD_6
#define CORCAM_VAL_BAYER_DECODER_METHOD_7       CORACQ_VAL_BAYER_DECODER_METHOD_7

#define CORCAM_VAL_CAM_IO_CONTROL_MAX			   CORACQ_VAL_CAM_IO_CONTROL_MAX

#define CORCAM_VAL_CAMLINK_CONFIGURATION_BASE	CORACQ_VAL_CAMLINK_CONFIGURATION_BASE	
#define CORCAM_VAL_CAMLINK_CONFIGURATION_MEDIUM	CORACQ_VAL_CAMLINK_CONFIGURATION_MEDIUM
#define CORCAM_VAL_CAMLINK_CONFIGURATION_FULL	CORACQ_VAL_CAMLINK_CONFIGURATION_FULL
#define CORCAM_VAL_CAMLINK_CONFIGURATION_2BASE	CORACQ_VAL_CAMLINK_CONFIGURATION_2BASE
#define CORCAM_VAL_CAMLINK_CONFIGURATION_10TAPS CORACQ_VAL_CAMLINK_CONFIGURATION_10TAPS
#define CORCAM_VAL_CAMLINK_CONFIGURATION_10TAPS_FORMAT1  CORACQ_VAL_CAMLINK_CONFIGURATION_10TAPS_FORMAT1
#define CORCAM_VAL_CAMLINK_CONFIGURATION_16TAPS				CORACQ_VAL_CAMLINK_CONFIGURATION_16TAPS
#define CORCAM_VAL_CAMLINK_CONFIGURATION_10TAPS_FORMAT2	CORACQ_VAL_CAMLINK_CONFIGURATION_10TAPS_FORMAT2

#define CORCAM_VAL_CHANNEL_SINGLE			CORACQ_VAL_CHANNEL_SINGLE
#define CORCAM_VAL_CHANNEL_DUAL				CORACQ_VAL_CHANNEL_DUAL

#define CORCAM_VAL_CHANNELS_ORDER_NORMAL	CORACQ_VAL_CHANNELS_ORDER_NORMAL
#define CORCAM_VAL_CHANNELS_ORDER_REVERSE CORACQ_VAL_CHANNELS_ORDER_REVERSE
#define CORCAM_VAL_CHANNELS_ORDER_DETECT  CORACQ_VAL_CHANNELS_ORDER_DETECT
#define CORCAM_VAL_CHANNELS_ORDER_SEGMENTED  CORACQ_VAL_CHANNELS_ORDER_SEGMENTED

#define CORCAM_VAL_CONNECTOR_PIN( value)		CORACQ_VAL_CONNECTOR_PIN( value)
#define CORCAM_VAL_CONNECTOR_TYPE( value)		CORACQ_VAL_CONNECTOR_TYPE( value)
#define CORCAM_VAL_CONNECTOR_NUMBER( value)	CORACQ_VAL_CONNECTOR_NUMBER( value)

#define CORCAM_VAL_CONNECTOR_TYPE_HIROSE		CORACQ_VAL_CONNECTOR_TYPE_HIROSE    // Obsolete. Use HIROSE12
#define CORCAM_VAL_CONNECTOR_TYPE_HIROSE12   CORACQ_VAL_CONNECTOR_TYPE_HIROSE12
#define CORCAM_VAL_CONNECTOR_TYPE_CAMLINK		CORACQ_VAL_CONNECTOR_TYPE_CAMLINK
#define CORCAM_VAL_CONNECTOR_TYPE_CAM_CONTROL   CORACQ_VAL_CONNECTOR_TYPE_CAM_CONTROL

#define CORCAM_VAL_CONNECTOR_TYPE_CAMLINK_MAX_PIN	CORACQ_VAL_CONNECTOR_TYPE_CAMLINK_MAX_PIN
#define CORCAM_VAL_CONNECTOR_TYPE_HIROSE_MAX_PIN	CORACQ_VAL_CONNECTOR_TYPE_HIROSE_MAX_PIN

#define CORCAM_VAL_COUPLING_AC				CORACQ_VAL_COUPLING_AC
#define CORCAM_VAL_COUPLING_DC				CORACQ_VAL_COUPLING_DC

#define CORCAM_VAL_DIR_INPUT		         CORACQ_VAL_DIR_INPUT
#define CORCAM_VAL_DIR_OUTPUT		         CORACQ_VAL_DIR_OUTPUT
#define CORCAM_VAL_DIR_TRISTATE	         CORACQ_VAL_DIR_TRISTATE

#define CORCAM_VAL_FIELD_ORDER_ODD_EVEN	CORACQ_VAL_FIELD_ORDER_ODD_EVEN
#define CORCAM_VAL_FIELD_ORDER_EVEN_ODD	CORACQ_VAL_FIELD_ORDER_EVEN_ODD
#define CORCAM_VAL_FIELD_ORDER_NEXT_FIELD	CORACQ_VAL_FIELD_ORDER_NEXT_FIELD
#define CORCAM_VAL_FIELD_ORDER_ODD_EVEN_PROGRESSIVE   CORACQ_VAL_FIELD_ORDER_ODD_EVEN_PROGRESSIVE

#define CORCAM_VAL_FRAME_INTERLACED			CORACQ_VAL_FRAME_INTERLACED
#define CORCAM_VAL_FRAME_NON_INTERLACED	CORACQ_VAL_FRAME_NON_INTERLACED
#define CORCAM_VAL_FRAME_PROGRESSIVE		CORACQ_VAL_FRAME_PROGRESSIVE

#define CORCAM_VAL_INTERFACE_ANALOG			CORACQ_VAL_INTERFACE_ANALOG
#define CORCAM_VAL_INTERFACE_DIGITAL		CORACQ_VAL_INTERFACE_DIGITAL

#define CORCAM_VAL_PIXEL_CLK_SRC_INT		CORACQ_VAL_PIXEL_CLK_SRC_INT
#define CORCAM_VAL_PIXEL_CLK_SRC_EXT		CORACQ_VAL_PIXEL_CLK_SRC_EXT
#define CORCAM_VAL_PIXEL_CLK_SRC_EXT_INT	CORACQ_VAL_PIXEL_CLK_SRC_EXT_INT

#define CORCAM_VAL_SCAN_AREA					CORACQ_VAL_SCAN_AREA
#define CORCAM_VAL_SCAN_LINE					CORACQ_VAL_SCAN_LINE

#define CORCAM_VAL_SIGNAL_SINGLE_ENDED		CORACQ_VAL_SIGNAL_SINGLE_ENDED
#define CORCAM_VAL_SIGNAL_DIFFERENTIAL		CORACQ_VAL_SIGNAL_DIFFERENTIAL

#define CORCAM_VAL_SIGNAL_NAME_NO_CONNECT       CORACQ_VAL_SIGNAL_NAME_NO_CONNECT
#define CORCAM_VAL_SIGNAL_NAME_HD               CORACQ_VAL_SIGNAL_NAME_HD
#define CORCAM_VAL_SIGNAL_NAME_VD               CORACQ_VAL_SIGNAL_NAME_VD
#define CORCAM_VAL_SIGNAL_NAME_PULSE0           CORACQ_VAL_SIGNAL_NAME_PULSE0
#define CORCAM_VAL_SIGNAL_NAME_PULSE1           CORACQ_VAL_SIGNAL_NAME_PULSE1
#define CORCAM_VAL_SIGNAL_NAME_PIXEL_CLOCK_IN   CORACQ_VAL_SIGNAL_NAME_PIXEL_CLOCK_IN
#define CORCAM_VAL_SIGNAL_NAME_PIXEL_CLOCK_OUT  CORACQ_VAL_SIGNAL_NAME_PIXEL_CLOCK_OUT
#define CORCAM_VAL_SIGNAL_NAME_LINESCAN_DIRECTION  CORACQ_VAL_SIGNAL_NAME_LINESCAN_DIRECTION
#define CORCAM_VAL_SIGNAL_NAME_WEN                 CORACQ_VAL_SIGNAL_NAME_WEN
#define CORCAM_VAL_SIGNAL_NAME_EXT_TRIGGER         CORACQ_VAL_SIGNAL_NAME_EXT_TRIGGER
#define CORCAM_VAL_SIGNAL_NAME_EXT_LINE_TRIGGER    CORACQ_VAL_SIGNAL_NAME_EXT_LINE_TRIGGER
#define CORCAM_VAL_SIGNAL_NAME_INT_FRAME_TRIGGER   CORACQ_VAL_SIGNAL_NAME_INT_FRAME_TRIGGER
#define CORCAM_VAL_SIGNAL_NAME_INT_LINE_TRIGGER    CORACQ_VAL_SIGNAL_NAME_INT_LINE_TRIGGER
#define CORCAM_VAL_SIGNAL_NAME_SOFTWARE_TRIGGER    CORACQ_VAL_SIGNAL_NAME_SOFTWARE_TRIGGER
#define CORCAM_VAL_SIGNAL_NAME_GND                 CORACQ_VAL_SIGNAL_NAME_GND
#define CORCAM_VAL_SIGNAL_NAME_POWER_12V           CORACQ_VAL_SIGNAL_NAME_POWER_12V
#define CORCAM_VAL_SIGNAL_NAME_VIDEO               CORACQ_VAL_SIGNAL_NAME_VIDEO
#define CORCAM_VAL_SIGNAL_NAME_VIDEO_GND           CORACQ_VAL_SIGNAL_NAME_VIDEO_GND

#define CORCAM_VAL_SYNC_COMP_VIDEO			CORACQ_VAL_SYNC_COMP_VIDEO
#define CORCAM_VAL_SYNC_COMP_SYNC			CORACQ_VAL_SYNC_COMP_SYNC
#define CORCAM_VAL_SYNC_SEP_SYNC				CORACQ_VAL_SYNC_SEP_SYNC
#define CORCAM_VAL_SYNC_INT_SYNC				CORACQ_VAL_SYNC_INT_SYNC
#define CORCAM_VAL_SYNC_RED					CORACQ_VAL_SYNC_RED
#define CORCAM_VAL_SYNC_GREEN					CORACQ_VAL_SYNC_GREEN
#define CORCAM_VAL_SYNC_BLUE					CORACQ_VAL_SYNC_BLUE

#define CORCAM_VAL_TAP_DIRECTION_LR			CORACQ_VAL_TAP_DIRECTION_LR
#define CORCAM_VAL_TAP_DIRECTION_RL			CORACQ_VAL_TAP_DIRECTION_RL
#define CORCAM_VAL_TAP_DIRECTION_UD			CORACQ_VAL_TAP_DIRECTION_UD
#define CORCAM_VAL_TAP_DIRECTION_DU		 	CORACQ_VAL_TAP_DIRECTION_DU
#define CORCAM_VAL_TAP_DIRECTION_FROM_TOP	CORACQ_VAL_TAP_DIRECTION_FROM_TOP
#define CORCAM_VAL_TAP_DIRECTION_FROM_MID CORACQ_VAL_TAP_DIRECTION_FROM_MID
#define CORCAM_VAL_TAP_DIRECTION_FROM_BOT CORACQ_VAL_TAP_DIRECTION_FROM_BOT

#define CORCAM_VAL_TAP_OUTPUT_ALTERNATE	CORACQ_VAL_TAP_OUTPUT_ALTERNATE
#define CORCAM_VAL_TAP_OUTPUT_SEGMENTED	CORACQ_VAL_TAP_OUTPUT_SEGMENTED
#define CORCAM_VAL_TAP_OUTPUT_PARALLEL	   CORACQ_VAL_TAP_OUTPUT_PARALLEL

#define CORCAM_VAL_TIMESLOT_1	            CORACQ_VAL_TIMESLOT_1
#define CORCAM_VAL_TIMESLOT_2	            CORACQ_VAL_TIMESLOT_2
#define CORCAM_VAL_TIMESLOT_3	            CORACQ_VAL_TIMESLOT_3
#define CORCAM_VAL_TIMESLOT_4	            CORACQ_VAL_TIMESLOT_4

#define CORCAM_VAL_TRIGGER_EXP_SIGNAL_SHARED	CORACQ_VAL_TRIGGER_EXP_SIGNAL_SHARED
#define CORCAM_VAL_TRIGGER_EXP_SIGNAL_SEPARATE	CORACQ_VAL_TRIGGER_EXP_SIGNAL_SEPARATE

#define CORCAM_VAL_VIDEO_MONO					CORACQ_VAL_VIDEO_MONO
#define CORCAM_VAL_VIDEO_COLOR				CORACQ_VAL_VIDEO_COLOR
#define CORCAM_VAL_VIDEO_YC					CORACQ_VAL_VIDEO_YC
#define CORCAM_VAL_VIDEO_RGB					CORACQ_VAL_VIDEO_RGB
#define CORCAM_VAL_VIDEO_BAYER            CORACQ_VAL_VIDEO_BAYER
#define CORCAM_VAL_VIDEO_BICOLOR          CORACQ_VAL_VIDEO_BICOLOR
#define CORCAM_VAL_VIDEO_RGBY             CORACQ_VAL_VIDEO_MULTI

#define CORCAM_VAL_VIDEO_STD_NON_STD		CORACQ_VAL_VIDEO_STD_NON_STD
#define CORCAM_VAL_VIDEO_STD_RS170_NTSC	CORACQ_VAL_VIDEO_STD_RS170_NTSC
#define CORCAM_VAL_VIDEO_STD_CCIR_PAL		CORACQ_VAL_VIDEO_STD_CCIR_PAL
#define CORCAM_VAL_VIDEO_STD_SECAM			CORACQ_VAL_VIDEO_STD_SECAM

#define CORCAM_VAL_ACTIVE_LOW				  	CORACQ_VAL_ACTIVE_LOW
#define CORCAM_VAL_ACTIVE_HIGH			  	CORACQ_VAL_ACTIVE_HIGH	
#define CORCAM_VAL_RISING_EDGE			  	CORACQ_VAL_RISING_EDGE	
#define CORCAM_VAL_FALLING_EDGE			  	CORACQ_VAL_FALLING_EDGE	

#define CORCAM_VAL_FRAME_INTEGRATE_METHOD_1		CORACQ_VAL_FRAME_INTEGRATE_METHOD_1
#define CORCAM_VAL_FRAME_INTEGRATE_METHOD_2		CORACQ_VAL_FRAME_INTEGRATE_METHOD_2

#define CORCAM_VAL_TIME_INTEGRATE_METHOD_1		CORACQ_VAL_TIME_INTEGRATE_METHOD_1
#define CORCAM_VAL_TIME_INTEGRATE_METHOD_2		CORACQ_VAL_TIME_INTEGRATE_METHOD_2
#define CORCAM_VAL_TIME_INTEGRATE_METHOD_3		CORACQ_VAL_TIME_INTEGRATE_METHOD_3
#define CORCAM_VAL_TIME_INTEGRATE_METHOD_4		CORACQ_VAL_TIME_INTEGRATE_METHOD_4
#define CORCAM_VAL_TIME_INTEGRATE_METHOD_5		CORACQ_VAL_TIME_INTEGRATE_METHOD_5
#define CORCAM_VAL_TIME_INTEGRATE_METHOD_6		CORACQ_VAL_TIME_INTEGRATE_METHOD_6
#define CORCAM_VAL_TIME_INTEGRATE_METHOD_7		CORACQ_VAL_TIME_INTEGRATE_METHOD_7

#define CORCAM_VAL_CAM_TRIGGER_METHOD_1			CORACQ_VAL_CAM_TRIGGER_METHOD_1

#define CORCAM_VAL_CAM_RESET_METHOD_1				CORACQ_VAL_CAM_RESET_METHOD_1

#define CORCAM_VAL_LINE_INTEGRATE_METHOD_1		CORACQ_VAL_LINE_INTEGRATE_METHOD_1
#define CORCAM_VAL_LINE_INTEGRATE_METHOD_2		CORACQ_VAL_LINE_INTEGRATE_METHOD_2
#define CORCAM_VAL_LINE_INTEGRATE_METHOD_3		CORACQ_VAL_LINE_INTEGRATE_METHOD_3
#define CORCAM_VAL_LINE_INTEGRATE_METHOD_4		CORACQ_VAL_LINE_INTEGRATE_METHOD_4
#define CORCAM_VAL_LINE_INTEGRATE_METHOD_5		CORACQ_VAL_LINE_INTEGRATE_METHOD_5
#define CORCAM_VAL_LINE_INTEGRATE_METHOD_6		CORACQ_VAL_LINE_INTEGRATE_METHOD_6
#define CORCAM_VAL_LINE_INTEGRATE_METHOD_7		CORACQ_VAL_LINE_INTEGRATE_METHOD_7

#define CORCAM_VAL_LINE_TRIGGER_METHOD_1  	 	CORACQ_VAL_LINE_TRIGGER_METHOD_1
#define CORCAM_VAL_LINE_TRIGGER_METHOD_2  	 	CORACQ_VAL_LINE_TRIGGER_METHOD_2
#define CORCAM_VAL_LINE_TRIGGER_METHOD_3  	 	CORACQ_VAL_LINE_TRIGGER_METHOD_3




#include "cpackd.h"
typedef CORACQ_CAM_IO_CONTROL	CORCAM_CAM_IO_CONTROL, *PCORCAM_CAM_IO_CONTROL;
#include "cunpackd.h"


#ifdef __cplusplus
extern "C" {
#endif

/* Application programming interface */

/* Object creation and destruction */
CORSTATUS CORAPIFUNC CorCamFree( CORCAM hCam);
CORSTATUS CORAPIFUNC CorCamNew ( CORSERVER hServer, CORCAM *pCam);

/* Load/Save from/to file */
CORSTATUS CORAPIFUNC CorCamLoad( CORCAM hCam, PCSTR filename);
CORSTATUS CORAPIFUNC CorCamSave( CORCAM hCam, PCSTR filename);

/* Modify Camera Parameters	*/
CORSTATUS CORAPIFUNC CorCamGetPrm  ( CORCAM hCam, UINT32 param, void *value); 
CORSTATUS CORAPIFUNC CorCamSetPrm  ( CORCAM hCam, UINT32 param, UINT32 value); 
CORSTATUS CORAPIFUNC CorCamSetPrmEx( CORCAM hCam, UINT32 prm, const void *value);
CORSTATUS CORAPIFUNC CorCamGetCap  ( CORCAM hCam, UINT32 cap, void *value);

#ifdef __cplusplus
}
#endif

#endif   // _CAPICAM_H_
