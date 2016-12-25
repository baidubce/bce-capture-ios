Pod::Spec.new do |s|

  s.name         = 'BaiduBCECapture'
  s.version      = '2.1.3'
  s.summary      = 'Baidu BCE iOS Capture SDK'
  s.homepage     = 'https://cloud.baidu.com/doc/LSS/iOS-Capture-SDK.html'
  s.license      = {
    :type => 'Proprietary',
    :text => <<-LICENSE
         Copyright (c) 2016 Baidu.com, Inc. All Rights Reserved.
         LICENSE
  }
  s.description  = <<-DESC
    Baidu BCE iOS Capture SDK supoort iOS 8.0 and later,
  DESC
  s.author   =   { 'Baidu Cloud Multimedia Team' => 'bce-multimedia@baidu.com' }
  s.source   =   { :http => 'http://sdk.bce.baidu.com/media-sdk/Baidu-Capture-SDK-iOS-2.1.3.zip?responseContentDisposition=attachment',:sha1 => "066eeabf77ff02e01cb65a69f2de3c7cfa57514f"}

  s.prepare_command = <<-CMD
    VERSION="2.1.3"
    BASEPATH="${PWD}"
    SDKPATH="Baidu-Capture-SDK-iOS-${VERSION}"

    rm -rf "${BASEPATH}/sdk"
    mkdir -p "${BASEPATH}/sdk"

    # copy framework
    cp -R ${SDKPATH}/sdk/develop/VideoCore.framework "${BASEPATH}/sdk/"
  CMD

  s.ios.deployment_target = '8.0'
  s.requires_arc = true
  s.vendored_frameworks = 'sdk/VideoCore.framework'
end
