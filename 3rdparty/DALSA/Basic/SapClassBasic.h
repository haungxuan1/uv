#ifndef _SAPCLASSBASIC_H_
#define _SAPCLASSBASIC_H_

//	SapClassBasic.h : header file
//

// If compiling with Common Language Runtime support with C++/CLI, switch to unmanaged code
#ifdef __cplusplus_cli
#pragma unmanaged
#endif

// For Visual Studio 2005, disable Unicode support, as Sapera++ headers generate compiler errors
#if defined(_MSC_VER) && _MSC_VER >= 1400
   #ifdef UNICODE
      #undef UNICODE
      #define SAP_NEED_UNICODE
   #endif
#endif

// Disable some Visual Studio level 4 warnings
//    4239 -> nonstandard extension used : 'token' : conversion from 'type' to 'type'
//    4251 -> 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
//    4514 -> 'function' : unreferenced inline function has been removed
#ifdef _MSC_VER
#pragma warning(disable: 4239 4514 4251)
#endif


/////////////////////////////////////////////////////////////////////////////
// Standard API header files
#include "corapi.h"


/////////////////////////////////////////////////////////////////////////////
// Class header files

// Sapera .NET Components
#ifdef SAPERA_DOT_NET

#include "SapClassBasicDef.h"
#include "SapData.h"
#include "SapManager.h"
#include "SapLut.h"
#include "SapXferNode.h"
#include "SapAcquisition.h"
#include "SapAcqDevice.h"
#include "SapFeature.h"
#include "SapBuffer.h"
#include "SapBufferWithTrash.h"
#include "SapBufferRoi.h"
#include "SapRTPro.h"
#include "SapTransfer.h"
#include "SapTransferEx.h"
#include "SapDisplay.h"
#include "SapProcessing.h"
#include "SapPerformance.h"
#include "SapView.h"
#include "SapGraphic.h"
#include "SapGio.h"
#include "SapBayer.h"	// obsolete
#include "SapColorConversion.h"
#include "SapFlatField.h"
#include "SapDynamicLut.h"
#include "SapMetadata.h"

// 64-bit Windows
#elif COR_WIN64

#include "SapClassBasicDef.h"
#include "SapData.h"
#include "SapManager.h"
#include "SapLut.h"
#include "SapXferNode.h"
#include "SapAcquisition.h"
#include "SapAcqDevice.h"
#include "SapFeature.h"
#include "SapBuffer.h"
#include "SapBufferWithTrash.h"
#include "SapBufferRoi.h"
#include "SapPixPro.h"
#include "SapTransfer.h"
#include "SapXferMultiNode.h"
#include "SapRTPro.h"
#include "SapRTProDesign.h"
#include "SapTransferEx.h"
#include "SapDisplay.h"
#include "SapProcessing.h"
#include "SapPerformance.h"
#include "SapView.h"
#include "SapGraphic.h"
#include "SapGio.h"
#include "SapBayer.h"	// obsolete
#include "SapColorConversion.h"
#include "SapFlatField.h"
#include "SapDynamicLut.h"
#include "SapMetadata.h"

// 32-bit Windows
#elif COR_WIN32

#include "SapClassBasicDef.h"
#include "SapData.h"
#include "SapManager.h"
#include "SapLut.h"
#include "SapXferNode.h"
#include "SapAcquisition.h"
#include "SapAcqDevice.h"
#include "SapBuffer.h"
#include "SapBufferWithTrash.h"
#include "SapBufferRoi.h"
#include "SapFeature.h"
#include "SapPixPro.h"
#include "SapTransfer.h"
#include "SapXferMultiNode.h"
#include "SapRTPro.h"
#include "SapRTProDesign.h"
#include "SapTransferEx.h"
#include "SapDisplay.h"
#include "SapProcessing.h"
#include "SapPerformance.h"
#include "SapView.h"
#include "SapGraphic.h"
#include "SapGio.h"
#include "SapBayer.h"	// obsolete
#include "SapColorConversion.h"
#include "SapFlatField.h"
#include "SapDynamicLut.h"
#include "SapMetadata.h"
#include "SapClassBasicObsolete.h"

// Linux (i386 or x86_64)
#elif COR_LINUX

#include "SapClassBasicDef.h"
#include "SapData.h"
#include "SapManager.h"
#include "SapLut.h"
#include "SapXferNode.h"
#include "SapAcquisition.h"
#include "SapAcqDevice.h"
#include "SapBuffer.h"
#include "SapBufferWithTrash.h"
#include "SapBufferRoi.h"
#include "SapFeature.h"
#include "SapTransfer.h"
#include "SapXferMultiNode.h"
#include "SapRTPro.h"
#include "SapRTProDesign.h"
#include "SapTransferEx.h"
#include "SapProcessing.h"
#include "SapPerformance.h"
#include "SapGio.h"
#include "SapBayer.h"	// obsolete
#include "SapColorConversion.h"
#include "SapFlatField.h"
#include "SapDynamicLut.h"
#include "SapMetadata.h"
#include "SapClassBasicObsolete.h"

#else
#error "Only 32/64-bit Windows, C++/CLI, and Linux are supported"
#endif

// For Visual Studio 2005, restore Unicode support if it was previously disabled
#if defined(_MSC_VER) && _MSC_VER >= 1400
   #ifdef SAP_NEED_UNICODE
      #define UNICODE
      #undef SAP_NEED_UNICODE
   #endif
#endif

// If compiling with Common Language Runtime support with C++/CLI, switch back to managed code
#ifdef __cplusplus_cli
#pragma managed
#endif

#endif	// _SAPCLASSBASIC_H_

