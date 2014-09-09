// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_OZONE_PLATFORM_EGLHAISI_EGLHAISI_SURFACE_FACTORY_H_
#define UI_OZONE_PLATFORM_EGLHAISI_EGLHAISI_SURFACE_FACTORY_H_

#include "base/memory/scoped_ptr.h"
#include "ui/ozone/public/surface_factory_ozone.h"

namespace gfx {
class SurfaceOzone;
}

namespace ui {

class SurfaceFactoryEglhaisi : public ui::SurfaceFactoryOzone {
 public:
  SurfaceFactoryEglhaisi();
  virtual ~SurfaceFactoryEglhaisi();
  
  // SurfaceFactoryOzone:
  virtual HardwareState InitializeHardware() OVERRIDE;
  virtual void ShutdownHardware() OVERRIDE;
  virtual intptr_t GetNativeDisplay() OVERRIDE;
  virtual gfx::AcceleratedWidget GetAcceleratedWidget() OVERRIDE;
  virtual scoped_ptr<SurfaceOzoneEGL> CreateEGLSurfaceForWidget(
      gfx::AcceleratedWidget widget) OVERRIDE;
  virtual const int32* GetEGLSurfaceProperties(
      const int32* desired_list) OVERRIDE;
  virtual bool LoadEGLGLES2Bindings(
      AddGLLibraryCallback add_gl_library,
      SetGLGetProcAddressProcCallback setprocaddress) OVERRIDE;
 private:
    bool init_;
};

}  // namespace ui

#endif  // UI_OZONE_PLATFORM_EGLHAISI_EGLHAISI_SURFACE_FACTORY_H_
