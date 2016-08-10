Pod::Spec.new do |s|

  s.name         = 'BaiduBCECapture'
  s.version      = '2.0.1'
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
  s.author   =   { 'Andy Young' => 'yanganning@baidu.com' }
  s.source   =   { :git => 'https://github.com/baidubce/bce-capture-ios.git',:tag=>s.version.to_s}
  s.ios.deployment_target = '8.0'
  s.requires_arc = true
  s.source_files = 'include/*.h'
  s.vendored_libraries = 'lib/librtmp.a', 'lib/libVideoCore.a', 'lib/libVisionin.a'
  s.frameworks = 'Foundation', 'AudioToolbox', 'VideoToolbox', 'AVFoundation', 'CoreMedia'
  s.dependency 'OpenSSL', '~> 1.0'
  s.libraries = 'z', 'c++'
end
