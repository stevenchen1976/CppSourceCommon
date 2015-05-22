/***************************************
* @file     VideoIO_Dynamic.c
* @brief    VideoIO_Dynamic.h中函数变量的定义，防止重复定义
* @details  
* @author   phata, wqvbjhc@gmail.com
* @date     2014-8-8
****************************************/
#include "VideoIO_Dynamic.h"

GetVer VideoIO_GetVersion = 0;
Init VideoIO_Init = 0;
CleanUp VideoIO_CleanUp = 0;
GetLastErrorMsg VideoIO_GetLastErrorMsg = 0;
CreateVideoReader VideoIO_CreateVideoReader = 0;
IsReaderOpen VideoIO_IsReaderOpen = 0;
GetVideoInfo VideoIO_GetVideoInfo = 0;
ReadVideo VideoIO_ReadVideo = 0;
GetPosition VideoIO_GetPosition = 0;
SetPosition VideoIO_SetPosition = 0;
ReleaseVideoReader VideoIO_ReleaseVideoReader = 0;
CreateVideoWriter VideoIO_CreateVideoWriter = 0;
IsWriterOpen VideoIO_IsWriterOpen = 0;
WriteVideo VideoIO_WriteVideo = 0;
ReleaseVideoWriter VideoIO_ReleaseVideoWriter = 0;
CreateDecoderReader VideoIO_CreateDecoderReader = 0;
IsDecoderOpen VideoIO_IsDecoderOpen = 0;
DecoderVideoData VideoIO_DecoderVideoData = 0;
SetDecoderFrameCB VideoIO_SetDecoderFrameCB = 0;
ReleaseDecoderReader VideoIO_ReleaseDecoderReader = 0;
