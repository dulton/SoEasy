#pragma once

#ifndef int64_t
typedef long long  int64_t;
#endif

///操作类型
typedef enum OperationType
{
	OPE_NONE = 0,						///无
	OPE_Audio = 0x0100,					///音频
	OPE_AudioEncode = 0x0101,			///音频编码
	OPE_AudioDecode = 0x0102,			///音频解码
	OPE_AudioDecodeFile = 0x0103,		///解码输出音频文件

	OPE_Video = 0x0200,					///视频
	OPE_VideoEncode = 0x0201,			///视频编码
	OPE_VideoDecode = 0x0202,			///视频解码
	OPE_VideoDecodeFile = 0x0203,		///解码输出视频文件
	
	OPE_AudioEncode_Hardware = 0x0301,	///音频编码 硬编
	OPE_AudioDecode_Hardware = 0x0302,	///音频解码 硬解
	OPE_VideoEncode_Hardware = 0x0401,	///视频编码 硬编
	OPE_VideoDecode_Hardware = 0x0402,	///视频解码 硬解


	OPE_AVMuxer = 0x300,				///音视频文件复用
	
	OPE_AVMemuxer = 0x310,				///音视频合并
	OPE_AuidoMemuxer = 0x311,			///音频复用合并
	OPE_VideoMemuxer = 0x312,			///视频复用合并
	OPE_AVDemuxer = 0x320,				///音视频分离
	OPE_AudioDemuxer = 0x321,			///音频分离
	OPE_VideoDemuxer = 0x322,			///视频分离
	OPE_AVRemuxer = 0x330,				///音视频转换
	OPE_AudioRemuxer = 0x331,			///音频转换
	OPE_VideoRemuxer = 0x332,			///视频转换
};
///设置音视频编解码类型
///现只有安卓支持硬编解码 其它平台软编解码
///默认都为软编解码
typedef enum ITAVEncodeDecodeType
{
	ITAV_ENCODE_Audio_Software,	///软编音频
	ITAV_ENCODE_Audio_Hardware,	///硬编音频

	ITAV_ENCODE_Video_Software,	///软编视频	
	ITAV_ENCODE_Video_Hardware,	///硬编视频

	ITAV_DECODE_Audio_Software,	///软解音频
	ITAV_DECODE_Audio_Hardware,	///硬解音频

	ITAV_DECODE_Video_Software,	///软解视频		
	ITAV_DECODE_Video_Hardware,	///硬解视频
};
///音视频合并 参数设置
///CodecType和CodecID 两种方法，传入一种即可
typedef struct ITAVCodecCtxFmt	
{
	///共用部分
	char strDstCodecType[100];	///编码格式字符串（传入".aac"等文件后缀名即可）
	char strSrcCodecType[100];	///原文件格式  
	int nSrcCodecID;			///编解码原格式 ITAVCodecID枚举值	
	int	nDstCodecID;			///编解码目标格式ITAVCodecID枚举值
	int nSrcBitRate;			///原码率	码率 音频来说：一秒播放数 例如 64000 128000  64kbp/s 128kbp/s
	int nDstBitRate;			///目标码率

	///音频部分		///编码时候需要原格式 如果目标没设置，则以原格式进行编码 不进行重采样 如果设置了目标格式，则会重采样
					//解码时候 原和目标都不需要设置，如果需要重采样，设置目标编码即可		
	int nSrcSampleRate;			///采样率 例如 44100
	int nSrcSampleNb;			///采样数 例如aac 1024 不用修改
	int nSrcChannels;			///通道数（类似左声道右声道立体声）
	int nSrcSampleFmt;			///采样格式 ITAVSampleFmt枚举值 

	int nDstSampleRate;			///采样率 例如 44100
	int nDstSampleNb;			///采样数 例如aac 1024 不用修改
	int nDstChannels;			///通道数（类似左声道右声道立体声）
	int nDstSampleFmt;			///采样格式 ITAVSampleFmt枚举值 


	///视频部分
	int nSrcWidth;				///原宽
	int nSrcHeight;				///原高
	int nDstWidth;				///目标宽
	int nDstHeight;				///目标高	
	int nGopSize;				///图片组大小 可以认为图片质量  不必配置	
	int nSrcTimeBaseNum;		///< numerator	通常为1 联合控制 把PTS转化为实际的时间（单位为秒s）
	int nSrcTimeBaseDen;		///< denominator 帧数 
	int nDstTimeBaseNum;		///< numerator	通常为1 联合控制 把PTS转化为实际的时间（单位为秒s）
	int nDstTimeBaseDen;		///< denominator 帧数 
	int nSrcPixelFmt;			///原像素		针对同一种类型 但存储方式不同而创建的项
	int nDstPixelFmt;			///目标像素格式
	int nSrcFrameRateNum;		
	int nSrcFrameRateDen;
	int nDstFrameRateNum;		///只有解码时有效？  不需要设置，设置timeBase部分还有点用，FrameRate部分无效果 不管编解码 
	int nDstFrameRateDen;		
	int64_t nCurDecodePts;		///当前解码后的pts

	///合并视频文件参数 解码文件也可用
	char strDstAVFile[256];		///目标AV 音视频合并文件
	char strDstAudioFile[256];	///目标音频文件
	char strDstVideoFile[256];	///目标视频文件
	char strSrcAudioFile[256];	///原音频文件
	char strSrcVideoFile[256];	///原视频文件
	char strSrcAVFile[256];		///原AV 音视频文件
}ITAVCODECCTXFMT;


#define MAXFrameSize 1000
typedef struct ITAVDecodeFrame
{
	ITAVCodecCtxFmt		struVideoFmt;			///参数结构体
	unsigned 	int 	nFrameSize;				//总结构体数
	unsigned	int		nDataSize[MAXFrameSize];///每个帧长度
	unsigned char*		strData[MAXFrameSize];	///帧数据
}ITAVDECODEFRAME;
///设置编解码目标类型  后缀名方式
typedef enum ITAVCodecID
{
	ITAV_CODEC_None=0,
	ITAV_CODEC_Audio=0x0100,
	ITAV_CODEC_AAC,
	ITAV_CODEC_MP2,
	ITAV_CODEC_MP3,
	ITAV_CODEC_WAV,
	ITAV_CODEC_PCM,	///分很多种  咱不用那么细 直接使用默认设置即可	
	ITAV_CODEC_Video=0x0200,
	ITAV_CODEC_H264,
	ITAV_CODEC_MP4,
	ITAV_CODEC_YUV,
	ITAV_CODEC_M2V,
	ITAV_CODEC_H265,
	ITAV_CODEC_HEVC,
	ITAV_CODEC_RM,
	ITAV_CODEC_AVI,	
	ITAV_CODEC_RGB,	
	ITAV_CODEC_FLV,
	ITAV_CODEC_MKV,
	ITAV_CODEC_RMVB,
	ITAV_CODEC_TS,
	ITAV_CODEC_MPEG4,
};
///视频相关  设置编解码目标类型  后缀名方式
//下面都是一些像素格式 
typedef enum ITAVPixelID
{
	ITAV_PIXEL_NONE,
	ITAV_PIXEL_GRAY8=0x0100,
	ITAV_PIXEL_YUV420P,
	ITAV_PIXEL_YUV420SP,		///NV12 NV21都属于YUV420SP  
	ITAV_PIXEL_YUV422P,
	ITAV_PIXEL_YUV444P,
	ITAV_PIXEL_YUYV422,
	ITAV_PIXEL_RGB24,	
	ITAV_PIXEL_RGB32,
	ITAV_PIXEL_BGR24,
	ITAV_PIXEL_NV12,///< planar YUV 4:2:0, 12bpp, 1 plane for Y and 1 plane for the UV components, which are interleaved (first byte U and the following byte V)
	ITAV_PIXEL_NV21,///< as above, but U and V bytes are swapped  和NV12一样，后面UV倒换	vu vu 	
};
///音频采样位格式设置 
//p的为平面格式
//8位采样还是16采样或者32 以及浮点数等 
typedef enum ITAVSampleFmt
{
	ITAV_SAMPLE_FMT_NONE=-1,	///空
	ITAV_SAMPLE_FMT_U8=1,          ///< unsigned 8 bits
	ITAV_SAMPLE_FMT_S16,         ///< signed 16 bits
	ITAV_SAMPLE_FMT_S32,         ///< signed 32 bits
	ITAV_SAMPLE_FMT_FLT,         ///< float
	ITAV_SAMPLE_FMT_DBL,         ///< double

	ITAV_SAMPLE_FMT_U8P,         ///< unsigned 8 bits, planar
	ITAV_SAMPLE_FMT_S16P,        ///< signed 16 bits, planar
	ITAV_SAMPLE_FMT_S32P,        ///< signed 32 bits, planar
	ITAV_SAMPLE_FMT_FLTP,        ///< float, planar
	ITAV_SAMPLE_FMT_DBLP,        ///< double, planar

	ITAV_SAMPLE_FMT_NB           ///< Number of sample formats. DO NOT USE if linking dynamically
};
//#define GETDEVSUPPERCLASS(n)	( n & 0xfffff000)			
//#define GETDEVCLASS(n)	( n & 0xffffff00)			
//#define GETDEVSUBCLASS(n)	( n & 0xfffffff0)	

#ifndef GETDEVSUBCLASS
#define GETDEVSUBCLASS(n) (n & 0xfffffff0)
#endif

#ifndef GETDEVCLASS
#define GETDEVCLASS(n)	( n & 0xffffff00)		
#endif

#ifndef GETDEVSUPPERCLASS
#define GETDEVSUPPERCLASS(n)	( n & 0xfffff000)
#endif

#ifdef OS_LINUX
#define BOOL int
#define TRUE 1
#define FALSE 0
#endif
