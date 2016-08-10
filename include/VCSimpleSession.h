/*

 Video Core
 Copyright (c) 2015 Andy Young @ Baidu.com Inc.

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.

 */

/*!
 *  A simple Objective-C Session API that will create an RTMP session using the
 *  device's camera(s) and microphone.
 *
 */

#import <Foundation/Foundation.h>
#import <CoreGraphics/CoreGraphics.h>
#import <AVFoundation/AVFoundation.h>
#import <UIKit/UIKit.h>

// 以下宏用于iOS中的NSNotificationCenter消息广播
#define RTMP_Started    @"RtmpStarted" // name为RTMP_Started的消息表明推流已经开始
#define RTMP_Error      @"RtmpError" // name为kRTMP_Error的消息表明推流sdk出错了

@class VCSimpleSession;

// 推流sdk运行过程中所处的状态
typedef NS_ENUM(NSInteger, VCSessionState)
{
    VCSessionStateNone, // 推流sdk的初始状态
    VCSessionStatePreviewStarted, // 开始出现预览画面，收到此状态回调后方可设置美颜参数
    VCSessionStateStarting, // 正在连接服务器或创建码流传输通道
    VCSessionStateStarted, // 已经建立连接，并已经开始推流
    VCSessionStateEnded, // 推流已经结束
    VCSessionStateError // 推流sdk运行过程中出错

};

// 摄像头类型（在推流过程中可调用switchCamera进行切换）
typedef NS_ENUM(NSInteger, VCCameraState)
{
    VCCameraStateFront, // 前置摄像头
    VCCameraStateBack // 后置摄像头
};

// 视频画面的填充方式
typedef NS_ENUM(NSInteger, VCAspectMode)
{
    VCAspectModeFit, // 画面保持比例，但是高宽自适应视频窗口，1.3.0版本及以后失效
    VCAspectModeFill, // 画面保持比例，画面拉伸填满视频窗口，超出部分将被裁剪
    VCAspectModeScale // 不保持画面比例，画面拉伸填满视频窗口，1.3.0版本及以后失效
};

// 实时滤镜类型
// With new filters should add an enum here
typedef NS_ENUM(NSInteger, VCFilter) {
    VCFilterNormal, // 正常颜色滤镜
    VCFilterGray, // 灰度处理滤镜
    VCFilterInvertColors, // 反色处理滤镜
    VCFilterSepia, // 棕色滤镜
    VCFilterFisheye, // 鱼眼特效滤镜
    VCFilterGlow, // 放光效果滤镜
    VCFilterGussBlur // 高斯模糊滤镜
};

@protocol VCSessionDelegate <NSObject>
@required
// 当推流sdk状态发生改变时，该接口会被调用，参数sessionState即为当前推流sdk所处的状态
- (void) connectionStatusChanged: (VCSessionState) sessionState;
@optional
// 当推流sdk创建CameraSource（即相机被占用）以后，该接口会被调用，参数session为VCSimpleSession的对象
- (void) didAddCameraSource:(VCSimpleSession*)session;
@end

@interface VCSimpleSession : NSObject

@property (nonatomic, readonly) VCSessionState rtmpSessionState;
@property (nonatomic, strong, readonly) UIView* previewView;

/*! Setters / Getters for session properties */
@property (nonatomic, assign) CGSize            videoSize; // 该属性为推流过程中视频的分辨率，在初始化时被确定，推流开始后不可更改
@property (nonatomic, assign) int               bitrate; // 该属性为推流过程中视频的编码码率，在初始化时被确定，推流开始后不可更改
@property (nonatomic, assign) int               fps; // 该属性为推流过程中视频的帧率，在初始化时被确定，推流开始后不可更改
@property (nonatomic, assign, readonly) BOOL    useInterfaceOrientation; // 该属性表示是否使用应用的竖直方向作为视频的竖直方向，在初始化时被确定，推流开始后不可更改
@property (nonatomic, assign) VCCameraState cameraState; // 该属性表示采集视频时使用何种摄像头，初始化时可设置，推流过程中可通过switchCamera接口进行切换摄像头
@property (nonatomic, assign) BOOL          orientationLocked; // 该属性表示推流过程中视频的竖直方向是否锁定，在初始化时被确定，推流开始后不可更改
@property (nonatomic, assign) BOOL          torch; // 该属性表示是否开启相机的闪光灯，初始化时默认为不开启，推流开始后可以修改
@property (nonatomic, assign) float         videoZoomFactor; // 该属性表示对原始视频进行缩放的比例，初始化时默认为1，推流开始后可以修改，其取值范围为(0, 1]
@property (nonatomic, assign) int           audioChannelCount; // 该属性为音频采集编码过程中所使用的声道数，初始化时默认为2，推流开始后不可修改，其值只能为1或者2
@property (nonatomic, assign) float         audioSampleRate; // 该属性为音频采集编码过程中所使用的采样率，初始化时默认为44100.0，推流开始后不可修改，其值只能为44100.0、22050，不建议对该值进行修改
@property (nonatomic, assign) float         micGain; // 该属性表示麦克风音量增益因子，初始化时默认为1.0，推流开始后可以修改，其取值范围为[0, 1]
@property (nonatomic, assign) CGPoint       focusPointOfInterest; // 该属性表示自动对焦时摄像头的对焦焦点，初始化时默认值为视频中心，推流开始后可以修改，其取值范围为[(0, 0), (1, 1)]，(0, 0)表示左上角，(1, 1)表示右下角
@property (nonatomic, assign) CGPoint       exposurePointOfInterest; // 该属性表示摄像头的测光中心点，初始化时默认值为视频中心，推流开始后可以修改，其取值范围为[(0, 0), (1, 1)]，(0, 0)表示左上角，(1, 1)表示右下角
@property (nonatomic, assign) BOOL          continuousAutofocus; // 该属性表示是否开启摄像头实时自动对焦功能，初始化时默认为开启，推流开始后可以修改
@property (nonatomic, assign) BOOL          continuousExposure; // 该属性表示是否开启摄像头动态测光功能，初始化时默认为开启，推流开始后可以修改
@property (nonatomic, assign) BOOL          useAdaptiveBitrate; // 该属性表示视频编码过程中是否开启动态码率选项，初始化时默认为关闭，推流开始后不可修改（不建议使用动态码率）
@property (nonatomic, readonly) int         estimatedThroughput; // 该属性为只读，表示推流过程中与服务器协商后的网络带宽
@property (nonatomic, assign) VCAspectMode  aspectMode; // 该属性表示视频画面对窗口的填充方式，初始化时默认为VCAspectModeFit，推流开始后可以修改
@property (nonatomic, assign) VCFilter      filter; // 该属性表示预览和推流过程中所使用的实时滤镜效果，初始化时默认为VCFilterNormal，推流开始后可以修改，1.3.0版本及以后失效
@property (nonatomic, assign) id<VCSessionDelegate> delegate;

// -----------------------------------------------------------------------------
- (instancetype) initWithVideoSize:(CGSize)videoSize
                         frameRate:(int)fps
                           bitrate:(int)bps;

// -----------------------------------------------------------------------------
- (instancetype) initWithVideoSize:(CGSize)videoSize
                         frameRate:(int)fps
                           bitrate:(int)bps
           useInterfaceOrientation:(BOOL)useInterfaceOrientation;

// -----------------------------------------------------------------------------
- (instancetype) initWithVideoSize:(CGSize)videoSize
                         frameRate:(int)fps
                           bitrate:(int)bps
           useInterfaceOrientation:(BOOL)useInterfaceOrientation
                       cameraState:(VCCameraState) cameraState;

// -----------------------------------------------------------------------------
- (instancetype) initWithVideoSize:(CGSize)videoSize
                         frameRate:(int)fps
                           bitrate:(int)bps
           useInterfaceOrientation:(BOOL)useInterfaceOrientation
                       cameraState:(VCCameraState) cameraState
                        aspectMode:(VCAspectMode) aspectMode;

// -----------------------------------------------------------------------------

- (void) startRtmpSessionWithURL:(NSString*) streamUrl;

- (void) startRtmpSessionWithURL:(NSString*) rtmpUrl
                    andStreamKey:(NSString*) streamKey;

- (void) switchCamera;

// 获取当前相机最大光学变焦放大倍数
- (float) getMaxZoomLevel;

/**
 * 光学变焦接口，可实现放大缩小功能
 * zoomFactor的取值范围为[1.0, MaxZoomLevel]，其中MaxZoomLevel为getMaxZoomLevel函数的返回值
 * center 始终为屏幕中心点像素坐标
 */
- (void) zoomVideo:(float) zoomFactor
        withCenter:(CGPoint) center;

- (void) endRtmpSession;

// 通过该接口可以获取相机采集到的原始画面
- (void) getCameraPreviewLayer: (AVCaptureVideoPreviewLayer**) previewLayer;

// 获取当前上传的带宽（单位：KBps）
- (double) getCurrentUploadBandwidthKbps;

// 设置是否启用音频推流，参数为YES表示启用音频推流，参数为NO表示停止音频推流，推流过程中可多次设置
- (void) setAudioEnabled:(BOOL) isEnable;
// 设置是否启用视频推流，参数为YES表示启用视频推流，参数为NO表示停止视频推流，推流过程中可多次设置
- (void) setVideoEnabled:(BOOL) isEnable;

// 开启或关闭美颜效果
- (void) enableBeautyEffect:(BOOL) isEnable;

/**
 * 设置美颜参数
 * bright 美白参数，取值范围[0, 1]
 * smooth 磨皮参数，取值范围[0, 1]
 * pink   粉嫩参数，取值范围[0, 1]
 */
- (void) setBeatyEffect:(float) bright
             withSmooth:(float) smooth
               withPink:(float) pink;

// 获取当前画面截图
- (UIImage*) getScreenShot;

/*!
 *  注意：
 *  rect的origin为水印图片的中心在视频画面中所处的目标坐标位置
 *  rect的size为水印图片在视频中的实际大小，此宽高值必须为4的整数倍
 */
- (void) addPixelBufferSource: (UIImage*) image
                     withRect: (CGRect) rect;

@end
