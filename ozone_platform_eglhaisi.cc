// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/ozone/platform/eglhaisi/ozone_platform_eglhaisi.h"
#include "ui/ozone/platform/eglhaisi/eglhaisi_surface_factory.h"
#include "ui/ozone/platform/eglhaisi/eglhaisi_event_factory.h"

#include "ui/ozone/public/cursor_factory_ozone.h"
#include "ui/ozone/ozone_platform.h"
#include "ui/ozone/public/gpu_platform_support.h"
#include "ui/ozone/public/gpu_platform_support_host.h"

#ifndef EGLHAISI_USE_IR
#include "ui/events/ozone/device/device_manager.h"
#include "ui/events/ozone/evdev/event_factory_evdev.h"
#endif

#if defined(OS_CHROMEOS)
#include "ui/ozone/common/chromeos/native_display_delegate_ozone.h"
#endif


namespace ui {

namespace {

class OzonePlatformEglhaisi : public OzonePlatform {
 public:
  OzonePlatformEglhaisi()
  {
  }
  virtual ~OzonePlatformEglhaisi() { 
  }

  // OzonePlatform:
  virtual ui::SurfaceFactoryOzone* GetSurfaceFactoryOzone() OVERRIDE {
    return surface_factory_ozone_.get();
  }
  virtual EventFactoryOzone* GetEventFactoryOzone() OVERRIDE {
    return event_factory_ozone_.get();
  }
  virtual CursorFactoryOzone* GetCursorFactoryOzone() OVERRIDE {
    return cursor_factory_ozone_.get();
  }

  virtual GpuPlatformSupport* GetGpuPlatformSupport() OVERRIDE {
      return gpu_platform_support_.get();
  }
  virtual GpuPlatformSupportHost* GetGpuPlatformSupportHost() OVERRIDE {
      return gpu_platform_support_host_.get();
  }

#if defined(OS_CHROMEOS)
  virtual scoped_ptr<NativeDisplayDelegate> CreateNativeDisplayDelegate()
      OVERRIDE {
    return scoped_ptr<NativeDisplayDelegate>(new NativeDisplayDelegateOzone());
  }
#endif
  
  virtual void InitializeUI() OVERRIDE {
    printf("---------InitializeUI\n");
#ifdef EGLHAISI_USE_IR
   event_factory_ozone_.reset(new EglhaisiEventFactory());
#else
   device_manager_ = CreateDeviceManager();    
   event_factory_ozone_.reset(
        new EventFactoryEvdev(NULL, device_manager_.get()));
#endif 
    surface_factory_ozone_.reset(new SurfaceFactoryEglhaisi());
    cursor_factory_ozone_.reset(new CursorFactoryOzone());
    gpu_platform_support_host_.reset(CreateStubGpuPlatformSupportHost());
  }
  
  virtual void InitializeGPU() OVERRIDE {
    printf("---------InitializeGPU\n");
    if(!surface_factory_ozone_)
    {
        surface_factory_ozone_.reset(new SurfaceFactoryEglhaisi());
    }
    gpu_platform_support_.reset(CreateStubGpuPlatformSupport());
  }

 private:
  
#ifdef EGLHAISI_USE_IR  
  scoped_ptr<EglhaisiEventFactory> event_factory_ozone_;
#else
  scoped_ptr<DeviceManager> device_manager_;
  scoped_ptr<EventFactoryEvdev> event_factory_ozone_;
#endif
  scoped_ptr<SurfaceFactoryEglhaisi> surface_factory_ozone_;
  scoped_ptr<CursorFactoryOzone> cursor_factory_ozone_;
  scoped_ptr<GpuPlatformSupport> gpu_platform_support_;
  scoped_ptr<GpuPlatformSupportHost> gpu_platform_support_host_;

  DISALLOW_COPY_AND_ASSIGN(OzonePlatformEglhaisi);
};

}  // namespace

OzonePlatform* CreateOzonePlatformEglhaisi() {
  printf("-----------CreateOzonePlatformEglhaisi\n");
  OzonePlatformEglhaisi* platform = new OzonePlatformEglhaisi;
  return platform;
}

}  // namespace ui
