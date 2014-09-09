#include "base/files/file_path.h"
#include "base/native_library.h"
#include "ui/ozone/platform/eglhaisi/eglhaisi_surface_factory.h"
#include "ui/ozone/public/surface_ozone_egl.h"
#include "ui/ozone/public/surface_factory_ozone.h"
#include "ui/gfx/vsync_provider.h"

#include <EGL/egl.h>
#include "EGL/fbdev_window.h"
#include "hifb.h"
#include <sys/ioctl.h>
#include <fcntl.h>

#define EGLHAISI_WINDOW_WIDTH 1280
#define EGLHAISI_WINDOW_HEIGTH 720

namespace ui {

namespace {

class SurfaceOzoneEglhaisi : public SurfaceOzoneEGL {
 public:
  SurfaceOzoneEglhaisi(gfx::AcceleratedWidget window_id){
    native_window_ = (fbdev_window *) malloc( sizeof(fbdev_window));
    if (NULL != native_window_)
    {
        native_window_->width  = EGLHAISI_WINDOW_WIDTH;
        native_window_->height = EGLHAISI_WINDOW_HEIGTH;
    }   
  }
  virtual ~SurfaceOzoneEglhaisi() {
    if(native_window_ != NULL)
    {
       free(native_window_);
       native_window_ = NULL;
    }
  }

  virtual intptr_t GetNativeWindow() OVERRIDE 
  { 
    return (intptr_t)native_window_; 
  }

  virtual bool OnSwapBuffers() OVERRIDE 
  { 
    return true; 
  }

  virtual bool ResizeNativeWindow(const gfx::Size& viewport_size) OVERRIDE {
    printf("-----------SurfaceOzoneEglhaisi::ResizeNativeWindow %d %d\n",viewport_size.width(),viewport_size.height());
    if(native_window_ != NULL)
    {
       free(native_window_);
       native_window_ = NULL;
    }
    native_window_ = (fbdev_window *) malloc( sizeof(fbdev_window));
    if (NULL != native_window_)
    {
        native_window_->width  = viewport_size.width();
        native_window_->height = viewport_size.height();
    }
    return true;
  }

  virtual scoped_ptr<gfx::VSyncProvider> CreateVSyncProvider() OVERRIDE {
    return scoped_ptr<gfx::VSyncProvider>();
  }

 private:
  fbdev_window * native_window_;
};

}  // namespace

SurfaceFactoryEglhaisi::SurfaceFactoryEglhaisi():console_fd_(0)
{
}
SurfaceFactoryEglhaisi::~SurfaceFactoryEglhaisi() 
{ 
}

void SurfaceFactoryEglhaisi::setColorKey(int color)
{
   
    HI_S32 s32Ret;
    HIFB_COLORKEY_S struColorKey;    
    
    if(color==0)
    {
        struColorKey.bKeyEnable  = HI_FALSE;
        struColorKey.u32Key = 0;
    }
    else
    {
        struct fb_var_screeninfo vinfo;
        if ((s32Ret = ioctl(console_fd_, FBIOGET_VSCREENINFO, &vinfo)) < 0)
        {
            printf("Unable to FBIOGET_VSCREENINFO!, err=0x%x\n", s32Ret);
            return;
        }
        
        HI_S32 key = hifb_color2key(&vinfo, color);    
        struColorKey.bKeyEnable  = HI_TRUE;
        struColorKey.u32Key = key; 
    }

    s32Ret = ioctl(console_fd_, FBIOPUT_COLORKEY_HIFB, &struColorKey);
    if (s32Ret < 0)
    {
        printf("set colorkey failed , err=0x%x\n", s32Ret);
        return;
    }
    printf("set colorkey ok\n");
}

SurfaceFactoryEglhaisi::HardwareState
SurfaceFactoryEglhaisi::InitializeHardware() {
    printf("-----SurfaceFactoryEglhaisi::InitializeHardware\n");   
    console_fd_ = open("/dev/fb0", O_RDWR, 0);
    //colorkey is red
    setColorKey(0xFFFF0000);
    return INITIALIZED;
}

void SurfaceFactoryEglhaisi::ShutdownHardware() {
    printf("-----SurfaceFactoryEglhaisi::ShutdownHardware\n");
    if (console_fd_ > 0)
    {
        setColorKey(0);
        close(console_fd_);
    }
}

intptr_t SurfaceFactoryEglhaisi::GetNativeDisplay() {
  return (intptr_t)EGL_DEFAULT_DISPLAY;
}

gfx::AcceleratedWidget SurfaceFactoryEglhaisi::GetAcceleratedWidget() {
  static int opaque_handle = 0;
  opaque_handle++;
  printf("-----------SurfaceFactoryEglhaisi::GetAcceleratedWidget %d\n",opaque_handle);
  return (gfx::AcceleratedWidget)opaque_handle;
}

scoped_ptr<SurfaceOzoneEGL>
SurfaceFactoryEglhaisi::CreateEGLSurfaceForWidget(
    gfx::AcceleratedWidget widget) {
  return make_scoped_ptr<SurfaceOzoneEGL>(
      new SurfaceOzoneEglhaisi(widget));
}

bool SurfaceFactoryEglhaisi::LoadEGLGLES2Bindings(
    AddGLLibraryCallback add_gl_library,
    SetGLGetProcAddressProcCallback setprocaddress) {  
  base::NativeLibraryLoadError error;
  base::NativeLibrary gles_library = base::LoadNativeLibrary(
    base::FilePath("libGLESv2.so.2"), &error);

  if (!gles_library) {
    LOG(WARNING) << "Failed to load GLES library: " << error.ToString();
    return false;
  }

  base::NativeLibrary egl_library = base::LoadNativeLibrary(
    base::FilePath("libEGL.so.1"), &error);

  if (!egl_library) {
    LOG(WARNING) << "Failed to load EGL library: " << error.ToString();
    base::UnloadNativeLibrary(gles_library);
    return false;
  }

  GLGetProcAddressProc get_proc_address =
      reinterpret_cast<GLGetProcAddressProc>(
          base::GetFunctionPointerFromNativeLibrary(
              egl_library, "eglGetProcAddress"));

  if (!get_proc_address) {
    LOG(ERROR) << "eglGetProcAddress not found.";
    base::UnloadNativeLibrary(egl_library);
    base::UnloadNativeLibrary(gles_library);
    return false;
  }

  setprocaddress.Run(get_proc_address);
  add_gl_library.Run(egl_library);
  add_gl_library.Run(gles_library);
  return true;
}

const int32* SurfaceFactoryEglhaisi::GetEGLSurfaceProperties(
    const int32* desired_list) {  
  static const EGLint kConfigAttribs[] = {
    EGL_BUFFER_SIZE, 32,
    EGL_ALPHA_SIZE, 8,
    EGL_BLUE_SIZE, 8,
    EGL_GREEN_SIZE, 8,
    EGL_RED_SIZE, 8,
    EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
    EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
    EGL_NONE
  };

  return kConfigAttribs;
}

}  // namespace ui
