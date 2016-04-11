#pragma once
typedef enum ITAVErrno
{
	ITAV_NoEro = 0,
	ITAV_ContextEro = 1,	///连接句柄错误
	ITAV_UnknownOpeType,	///未知操作类型 参考Gloabl_Define.h 枚举OperationType
	ITAV_DataIncomplete,	///数据不全 例如 不够1帧
	ITAV_EncodeEro,			///编码错误
	ITAV_DecodeEro,			///解码错误
	ITAV_EncodeInBuffFlow,	///编码缓存溢出
	ITAV_EncodeOutBuffFlow,	///编码输出缓存溢出 编码后数据存放的缓存 当前处理数据过大导致缓存buf上溢
	ITAV_DecodeInBuffFlow,	///解码缓存溢出
	ITAV_DecodeOutBuffFlow,	///解码输出缓存溢出 解码后数据存放的缓存 当前处理数据过大导致缓存buf上溢
	ITAV_CreateNewStreamEro,///创建输出流失败，可能输出context引起
	ITAV_FindEncoderEro,	///查找编码器失败
	ITAV_FindDecoderEro,	///查找解码器失败
	ITAV_OpenEncoderEro,	///打开编码器失败 查看参数设置
	ITAV_OpenDecoderEro,	///打开解码器失败
	ITAV_EncoderParamCfgEro,///编码参数设置错误
	ITAV_EncoderParamCfgOk,	///编码参数设置成功
	ITAV_DecoderParamCfgEro,///解码参数设置错误
	ITAV_DecoderParamCfgOk,	///解码参数设置成功	
	ITAV_EncodeOpenInEro,	///打开输入的编码数据失败
	ITAV_DecodeOpenInEro,	///打开输入的解码数据失败
	ITAV_EncodeFindStreamEro,///查找输入流信息失败
	ITAV_DecodeFindStreamEro,///查找输入流信息失败
	ITAV_DecodeFailBuffFlow,///解码失败数据缓存buf溢出
	ITAV_EncodeFailBuffFlow,///编码失败数据缓存buf溢出
	ITAV_DecodeAllocCtxFailed,	///分配解码内存失败
	ITAV_EncodeAllocCtxFailed,	///分配编码内存失败
	ITAV_DecodeAllocFrameFailed,///分配解码用帧内存失败
	ITAV_EncodeAllocFrameFailed,///分配解码用帧内存失败
	ITAV_DecodeFrameCfgFailed,///解码帧配置错误
	ITAV_EncodeFrameCfgFailed,///编码帧配置错误
	ITAV_DecodeCodecParserCtxFailed,///解码使用的解析上下文错误
	ITAV_EncodeCodecParserCtxFailed,///解码使用的解析上下文错误
	
	ITAV_ParamOpeTypeSetEro = 0x0100,///参数配置中，操作类型设置错误
	ITAV_ParamSetEro = 0x0100,		///参数设置错误

}ITAVErrno;
