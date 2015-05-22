/***************************************
* @file     VideoIO_Dynamic.h
* @brief    VideoIO的动态加载库头文件.与VideoIO.h相区别
* @details  当VideoIO使用mingw编译时，生成的.a,.dll文件给vc调用老是会出错，所以决定使用动态加载dll的方法给vc调用。
            调用方法，先调用iVideoIO::Init();然后就可以使用各接口了

* @author   phata, wqvbjhc@gmail.com
* @date     2014-8-8
* @mod      2014-08-11  phata  根据VideoIO3.0.6接口更新，添加encoderspeed参数
            2014-10-21  phata  在MFC工程中，对VideoIO_Dynamic.h包含顺序要求(所有第三方库最前或者最前)，不然会产生如下错误。
			                   wspiapi.h(48): error C2894: 模板不能声明为有“C”链接
			                   uv.h(761): error C2146: 语法错误: 缺少“;”(在标识符“func_connectex”的前面)
			                   uv.h(761): error C4430: 缺少类型说明符 - 假定为 int。注意: C++ 不支持默认 int
****************************************/
#ifndef VIDEOIO_DYNAMIC_H
#define VIDEOIO_DYNAMIC_H
#include <stdio.h>
#include "uv.h"

//#if (defined WIN32 || defined _WIN32 || defined WIN64 || defined _WIN64) && (defined _MSC_VER)
//#define VIDEOIO_DYNAMIC_STDCALL __stdcall
//#else
#define VIDEOIO_DYNAMIC_STDCALL __cdecl//GCC的默认调用方式
//#endif

enum {
	BGR24_ENCODER = 0,//读取视频编码成RGB24帧，默认类型
	H264_ENCODER  = 1,//读取视频编码成h264帧，适合于网络传输
	H265_ENCODER  = 2,//读取视频编码成h265帧，适合于网络传输
};

// 帧编码格式
#define NVD_T_YV12     3
#define NOD_T_JPG      4
#define NVD_T_RGB24    24
#define NVD_T_I420     420
#define NVD_T_YUV422   422

#ifndef VIDEO_FRAME_INFO_def
#define VIDEO_FRAME_INFO_def

#define NVD_T_YV12     3
#define NVD_T_RGB24    24
#define NVD_T_I420   420
#define NVD_T_YUV422   422
typedef struct _VIDEO_FRAME_INFO {
	_VIDEO_FRAME_INFO() {
		nFrameInfoSize=sizeof (_VIDEO_FRAME_INFO);
		pBuffer=NULL;
		nWidth=nHeight=nBuflen=0;
	}
	long nFrameInfoSize;	// 该结构体的大小
	long nWidth;			// 画面宽，单位像素
	long nHeight;			// 画面高，单位像素
	long nStamp;			// 时标信息，单位毫秒,未使用
	long nType;				// 帧编码格式，只支持T_RGB24， T_YV12两种格式
	long nFrameRate;		// 编码时产生的图像帧率帧号，未使用
	long nBuflen;			// 帧数据(pBuffer)长度
	unsigned long nFrameNum;// 帧数，未使用
	unsigned char *pBuffer; // 帧地址
} VIDEO_FRAME_INFO,*PVIDEO_FRAME_INFO;
#endif
#ifndef FrameCallBack_def
#define FrameCallBack_def
typedef void(__stdcall *FrameCallBack)(
	const VIDEO_FRAME_INFO& frameinfo,//帧数据信息
	void           *pUser//用户自定义数据
	);//开放给用户的帧数据回调函数
#endif


typedef const char*  (VIDEOIO_DYNAMIC_STDCALL *GetVer)(long& num);     //获取版本号
typedef long         (VIDEOIO_DYNAMIC_STDCALL *Init)(void);           //初始化DLL
typedef void	     (VIDEOIO_DYNAMIC_STDCALL *CleanUp)(void);        //释放DLL
typedef const char*  (VIDEOIO_DYNAMIC_STDCALL *GetLastErrorMsg)(void);//获取最后一次错误信息
typedef long	     (VIDEOIO_DYNAMIC_STDCALL *CreateVideoReader)(const char* videopath, int encodertype, int encoderspeed);//创建读视频id
typedef long         (VIDEOIO_DYNAMIC_STDCALL *IsReaderOpen)(const long& videoio_id);//判断是否已创建成功
typedef long         (VIDEOIO_DYNAMIC_STDCALL *GetVideoInfo)(const long& videoio_id,int *pwidth,int *pheight,int* pfps, long long *pfrmaecount);//得到视频信息
typedef long         (VIDEOIO_DYNAMIC_STDCALL *ReadVideo)(const long& videoio_id,unsigned char** videodata,int *pwidth,int *pheight);//读取一帧视频
typedef long         (VIDEOIO_DYNAMIC_STDCALL *GetPosition)(const long& videoio_id);//得到正在读取的帧数
typedef long         (VIDEOIO_DYNAMIC_STDCALL *SetPosition)(const long& videoio_id,int frames);//设置要读取的帧数(对实时流无效)
typedef void         (VIDEOIO_DYNAMIC_STDCALL *ReleaseVideoReader)(const long& videoio_id);//释放读视频id
typedef long         (VIDEOIO_DYNAMIC_STDCALL *CreateVideoWriter)(const char* videopath, int width, int height,int fourcc);//创建写视频id
typedef long	     (VIDEOIO_DYNAMIC_STDCALL *IsWriterOpen)(const long& videoio_id);//判断id是否创建成功
typedef long	     (VIDEOIO_DYNAMIC_STDCALL *WriteVideo)(const long& videoio_id,const unsigned char* videodata,const int width,const int height);//写视频文件
typedef void         (VIDEOIO_DYNAMIC_STDCALL *ReleaseVideoWriter)(const long& videoio_id);//释放写视频id
typedef long	     (VIDEOIO_DYNAMIC_STDCALL *CreateDecoderReader)(int coderid);//创建解码视频id
typedef long         (VIDEOIO_DYNAMIC_STDCALL *IsDecoderOpen)(const long& videoio_id);//判断是否已创建成功
typedef long         (VIDEOIO_DYNAMIC_STDCALL *DecoderVideoData)(const long& videoio_id,const unsigned char* data, int datasize);//解码视频数据
typedef long         (VIDEOIO_DYNAMIC_STDCALL *SetDecoderFrameCB)(const long& videoio_id,FrameCallBack pfun, void* userdata);//设置帧数据回调函数
typedef void         (VIDEOIO_DYNAMIC_STDCALL *ReleaseDecoderReader)(const long& videoio_id);//释放读视频id

extern GetVer VideoIO_GetVersion;
extern Init VideoIO_Init;
extern CleanUp VideoIO_CleanUp;
extern GetLastErrorMsg VideoIO_GetLastErrorMsg;
extern CreateVideoReader VideoIO_CreateVideoReader;
extern IsReaderOpen VideoIO_IsReaderOpen;
extern GetVideoInfo VideoIO_GetVideoInfo;
extern ReadVideo VideoIO_ReadVideo;
extern GetPosition VideoIO_GetPosition;
extern SetPosition VideoIO_SetPosition;
extern ReleaseVideoReader VideoIO_ReleaseVideoReader;
extern CreateVideoWriter VideoIO_CreateVideoWriter;
extern IsWriterOpen VideoIO_IsWriterOpen;
extern WriteVideo VideoIO_WriteVideo;
extern ReleaseVideoWriter VideoIO_ReleaseVideoWriter;
extern CreateDecoderReader VideoIO_CreateDecoderReader;
extern IsDecoderOpen VideoIO_IsDecoderOpen;
extern DecoderVideoData VideoIO_DecoderVideoData;
extern SetDecoderFrameCB VideoIO_SetDecoderFrameCB;
extern ReleaseDecoderReader VideoIO_ReleaseDecoderReader;

class iVideoIO
{
public:
    static void Init() {
        static iVideoIO init;
    }

private:
    uv_lib_t ivideio;

    ~iVideoIO() {
        uv_dlclose(&ivideio);
    }

    iVideoIO() {
        static const char* module_name = "VideoIO.dll";
		if(uv_dlopen(module_name,&ivideio) !=0) {
			fprintf(stdout,"Load %s error: %s\n",module_name,uv_dlerror(&ivideio));
			return;
		}
		if(uv_dlsym(&ivideio,"VideoIO_GetVersion",(void**)&VideoIO_GetVersion)!=0) {
			fprintf(stdout,"Get Fun VideoIO_Init error:%s\n",uv_dlerror(&ivideio));
		}
		if(uv_dlsym(&ivideio,"VideoIO_Init",(void**)&VideoIO_Init)!=0) {
			fprintf(stdout,"Get Fun VideoIO_Init error:%s\n",uv_dlerror(&ivideio));
		}
        if(uv_dlsym(&ivideio,"VideoIO_CleanUp",(void**)&VideoIO_CleanUp)!=0) {
            fprintf(stdout,"Get Fun VideoIO_CleanUp error:%s\n",uv_dlerror(&ivideio));
        }
        if(uv_dlsym(&ivideio,"VideoIO_GetLastErrorMsg",(void**)&VideoIO_GetLastErrorMsg)!=0) {
            fprintf(stdout,"Get Fun VideoIO_GetLastErrorMsg error:%s\n",uv_dlerror(&ivideio));
        }
        if(uv_dlsym(&ivideio,"VideoIO_CreateVideoReader",(void**)&VideoIO_CreateVideoReader)!=0) {
            fprintf(stdout,"Get Fun VideoIO_CreateVideoReader error:%s\n",uv_dlerror(&ivideio));
        }
        if(uv_dlsym(&ivideio,"VideoIO_IsReaderOpen",(void**)&VideoIO_IsReaderOpen)!=0) {
            fprintf(stdout,"Get Fun VideoIO_IsReaderOpen error:%s\n",uv_dlerror(&ivideio));
        }
        if(uv_dlsym(&ivideio,"VideoIO_GetVideoInfo",(void**)&VideoIO_GetVideoInfo)!=0) {
            fprintf(stdout,"Get Fun VideoIO_GetVideoInfo error:%s\n",uv_dlerror(&ivideio));
        }
        if(uv_dlsym(&ivideio,"VideoIO_ReadVideo",(void**)&VideoIO_ReadVideo)!=0) {
            fprintf(stdout,"Get Fun VideoIO_ReadVideo error:%s\n",uv_dlerror(&ivideio));
        }
        if(uv_dlsym(&ivideio,"VideoIO_GetPosition",(void**)&VideoIO_GetPosition)!=0) {
            fprintf(stdout,"Get Fun VideoIO_GetPosition error:%s\n",uv_dlerror(&ivideio));
        }
        if(uv_dlsym(&ivideio,"VideoIO_SetPosition",(void**)&VideoIO_SetPosition)!=0) {
            fprintf(stdout,"Get Fun VideoIO_SetPosition error:%s\n",uv_dlerror(&ivideio));
        }
        if(uv_dlsym(&ivideio,"VideoIO_ReleaseVideoReader",(void**)&VideoIO_ReleaseVideoReader)!=0) {
            fprintf(stdout,"Get Fun VideoIO_ReleaseVideoReader error:%s\n",uv_dlerror(&ivideio));
        }
        if(uv_dlsym(&ivideio,"VideoIO_CreateVideoWriter",(void**)&VideoIO_CreateVideoWriter)!=0) {
            fprintf(stdout,"Get Fun VideoIO_CreateVideoWriter error:%s\n",uv_dlerror(&ivideio));
        }
        if(uv_dlsym(&ivideio,"VideoIO_IsWriterOpen",(void**)&VideoIO_IsWriterOpen)!=0) {
            fprintf(stdout,"Get Fun VideoIO_IsWriterOpen error:%s\n",uv_dlerror(&ivideio));
        }
        if(uv_dlsym(&ivideio,"VideoIO_WriteVideo",(void**)&VideoIO_WriteVideo)!=0) {
            fprintf(stdout,"Get Fun VideoIO_WriteVideo error:%s\n",uv_dlerror(&ivideio));
        }
        if(uv_dlsym(&ivideio,"VideoIO_ReleaseVideoWriter",(void**)&VideoIO_ReleaseVideoWriter)!=0) {
            fprintf(stdout,"Get Fun VideoIO_ReleaseVideoWriter error:%s\n",uv_dlerror(&ivideio));
        }
        if(uv_dlsym(&ivideio,"VideoIO_CreateDecoderReader",(void**)&VideoIO_CreateDecoderReader)!=0) {
            fprintf(stdout,"Get Fun VideoIO_CreateDecoderReader error:%s\n",uv_dlerror(&ivideio));
        }
        if(uv_dlsym(&ivideio,"VideoIO_IsDecoderOpen",(void**)&VideoIO_IsDecoderOpen)!=0) {
            fprintf(stdout,"Get Fun VideoIO_IsDecoderOpen error:%s\n",uv_dlerror(&ivideio));
        }
        if(uv_dlsym(&ivideio,"VideoIO_DecoderVideoData",(void**)&VideoIO_DecoderVideoData)!=0) {
            fprintf(stdout,"Get Fun VideoIO_DecoderVideoData error:%s\n",uv_dlerror(&ivideio));
        }
        if(uv_dlsym(&ivideio,"VideoIO_SetDecoderFrameCB",(void**)&VideoIO_SetDecoderFrameCB)!=0) {
            fprintf(stdout,"Get Fun VideoIO_SetDecoderFrameCB error:%s\n",uv_dlerror(&ivideio));
        }
        if(uv_dlsym(&ivideio,"VideoIO_ReleaseDecoderReader",(void**)&VideoIO_ReleaseDecoderReader)!=0) {
            fprintf(stdout,"Get Fun VideoIO_ReleaseDecoderReader error:%s\n",uv_dlerror(&ivideio));
        }
    }
private://private中，禁止复制和赋值
	iVideoIO(const iVideoIO&);//不实现
	iVideoIO& operator =(const iVideoIO&);//不实现
};
#endif//VIDEOIO_DYNAMIC_H