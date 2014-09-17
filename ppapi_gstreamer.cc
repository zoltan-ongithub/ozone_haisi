// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <string.h>
#include <stdio.h>

#include <iostream>
#include <sstream>

#include "ppapi/c/pp_errors.h"
#include "ppapi/c/ppb_opengles2.h"
#include "ppapi/cpp/core.h"
#include "ppapi/cpp/fullscreen.h"
#include "ppapi/cpp/graphics_3d.h"
#include "ppapi/cpp/graphics_3d_client.h"
#include "ppapi/cpp/input_event.h"
#include "ppapi/cpp/instance.h"
#include "ppapi/cpp/module.h"
#include "ppapi/cpp/rect.h"
#include "ppapi/cpp/var.h"
#include "ppapi/lib/gl/include/GLES2/gl2.h"
#include "ppapi/utility/completion_callback_factory.h"

#include "gstreamer_player.h"

// Use assert as a poor-man's CHECK, even in non-debug mode.
// Since <assert.h> redefines assert on every inclusion (it doesn't use
// include-guards), make sure this is the last file #include'd in this file.
#undef NDEBUG
#include <assert.h>

// Assert |context_| isn't holding any GL Errors.  Done as a macro instead of a
// function to preserve line number information in the failure message.
#define assertNoGLError() \
  assert(!gles2_if_->GetError(context_->pp_resource()));

namespace {

class PPAPIGstreamerInstance : public pp::Instance,
                          public pp::Graphics3DClient {
 public:
  PPAPIGstreamerInstance(PP_Instance instance, pp::Module* module);
  virtual ~PPAPIGstreamerInstance();
  
  // pp::Instance implementation (see PPP_Instance).
  virtual bool Init(uint32_t argc, const char* argn[], const char* argv[]);
    
  virtual void DidChangeView(const pp::Rect& position,
                             const pp::Rect& clip_ignored);

  // pp::Graphics3DClient implementation.
  virtual void Graphics3DContextLost() {
    // TODO(jamesr): What's the state of context_? Should we delete the old one
    // or try to revive it somehow?
    // For now, just delete it and construct+bind a new context.
    delete context_;
    context_ = NULL;
    pp::CompletionCallback cb = callback_factory_.NewCallback(
        &PPAPIGstreamerInstance::InitGL);
    module_->core()->CallOnMainThread(0, cb, 0);
  }

  virtual bool HandleInputEvent(const pp::InputEvent& event) {
    if (event.GetType() == PP_INPUTEVENT_TYPE_MOUSEUP) {
      fullscreen_ = !fullscreen_;
      pp::Fullscreen(this).SetFullscreen(fullscreen_);
    }
    return true;
  }
  
   virtual void HandleMessage(const pp::Var& var_message);

 private:

  // GL-related functions.
  void InitGL(int32_t result);
  void FlickerAndPaint(int32_t result);
  bool StartPlay();

  pp::Size plugin_size_;
  pp::Rect windowrect;
  pp::CompletionCallbackFactory<PPAPIGstreamerInstance> callback_factory_;

  // Unowned pointers.
  const PPB_OpenGLES2* gles2_if_;
  pp::Module* module_;

  // Owned data.
  pp::Graphics3D* context_;
  bool fullscreen_;
  void *gstData_;
  std::string src_;
};

PPAPIGstreamerInstance::PPAPIGstreamerInstance(PP_Instance instance, pp::Module* module)
    : pp::Instance(instance), pp::Graphics3DClient(this),
      callback_factory_(this),
      gles2_if_(static_cast<const PPB_OpenGLES2*>(
          module->GetBrowserInterface(PPB_OPENGLES2_INTERFACE))),
      module_(module),
      context_(NULL),
      fullscreen_(false),
      gstData_(NULL) {
  assert(gles2_if_);
  RequestInputEvents(PP_INPUTEVENT_CLASS_MOUSE);
}

PPAPIGstreamerInstance::~PPAPIGstreamerInstance() {
  delete context_;
  gstPlayer_release(&gstData_);
}
bool PPAPIGstreamerInstance::StartPlay()
{
    if(gstData_!= NULL)
    {
        gstPlayer_release(&gstData_);
    }
    
    if(src_ !="")
    {
        gstData_ = gstPlayer_create();     
        gstPlayer_play(gstData_,src_.c_str());
        if( windowrect.width() != 0 && windowrect.height() != 0 )
        {
          gstPlayer_setwindow(gstData_, windowrect.x(), windowrect.y(), windowrect.width(), windowrect.height());
        }
        return true;
    }    
    return false;
}
bool PPAPIGstreamerInstance::Init(uint32_t argc, const char* argn[], const char* argv[]) {
    for (uint32_t i = 0; i < argc; i++) {
      printf("-------%s---%s\n",argn[i],argv[i]);
      if (strcmp("src", argn[i]) == 0) {
        src_ = argv[i];
      }
    }    
    return StartPlay();
  }
void PPAPIGstreamerInstance::DidChangeView(
    const pp::Rect& position, const pp::Rect& clip_ignored) {
  if (position.width() == 0 || position.height() == 0)
    return;
  plugin_size_ = position.size();
  printf("-----PPAPIGstreamerInstance::DidChangeView %d %d %d %d\n",
          position.x(), position.y(), position.width(),position.height());
  // set player video rect
  gstPlayer_setwindow(gstData_, position.x(), position.y(), position.width(), position.height());
  windowrect = position;
  // Initialize graphics.
  InitGL(0);
}

void PPAPIGstreamerInstance::HandleMessage(const pp::Var& var_message)
{
    if (!var_message.is_string())
      return;
    std::string message = var_message.AsString();  
    printf("-----HandleMessage %s \n",message.c_str());
    if(message == "playPause()")
    {
        if(!gstPlayer_isplay(gstData_))
        {
            StartPlay();
        }        
        gstPlayer_playPause(gstData_);
    }
    else if (message == "stop()")
    {
        gstPlayer_release(&gstData_);
    }
}

// This object is the global object representing this plugin library as long
// as it is loaded.
class PPAPIGstreamer : public pp::Module {
 public:
  PPAPIGstreamer() : pp::Module() {}
  virtual ~PPAPIGstreamer() {}

  virtual pp::Instance* CreateInstance(PP_Instance instance) {
    return new PPAPIGstreamerInstance(instance, this);
  }
};

void PPAPIGstreamerInstance::InitGL(int32_t result) {
  assert(plugin_size_.width() && plugin_size_.height());

  if (context_) {
    context_->ResizeBuffers(plugin_size_.width(), plugin_size_.height());
    return;
  }
  int32_t context_attributes[] = {
    PP_GRAPHICS3DATTRIB_ALPHA_SIZE, 8,
    PP_GRAPHICS3DATTRIB_BLUE_SIZE, 8,
    PP_GRAPHICS3DATTRIB_GREEN_SIZE, 8,
    PP_GRAPHICS3DATTRIB_RED_SIZE, 8,
    PP_GRAPHICS3DATTRIB_DEPTH_SIZE, 0,
    PP_GRAPHICS3DATTRIB_STENCIL_SIZE, 0,
    PP_GRAPHICS3DATTRIB_SAMPLES, 0,
    PP_GRAPHICS3DATTRIB_SAMPLE_BUFFERS, 0,
    PP_GRAPHICS3DATTRIB_WIDTH, plugin_size_.width(),
    PP_GRAPHICS3DATTRIB_HEIGHT, plugin_size_.height(),
    PP_GRAPHICS3DATTRIB_NONE,
  };
  context_ = new pp::Graphics3D(this, context_attributes);
  assert(!context_->is_null());
  assert(BindGraphics(*context_));

  // Clear color bit.
  gles2_if_->ClearColor(context_->pp_resource(), 0, 1, 0, 1);
  gles2_if_->Clear(context_->pp_resource(), GL_COLOR_BUFFER_BIT);

  assertNoGLError();

  FlickerAndPaint(0);
}

void PPAPIGstreamerInstance::FlickerAndPaint(int32_t result) {
  if (result != 0 || !context_)
    return;
  //colorkey is red
  float r = 1;
  float g = 0;
  float b = 0;
  float a = 1;
  //gles2_if_->Enable(context_->pp_resource(),GL_BLEND);
  //gles2_if_->BlendFunc(context_->pp_resource(),GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  gles2_if_->ClearColor(context_->pp_resource(), r, g, b, a);
  gles2_if_->Clear(context_->pp_resource(), GL_COLOR_BUFFER_BIT);
  assertNoGLError();

  pp::CompletionCallback cb = callback_factory_.NewCallback(
      &PPAPIGstreamerInstance::FlickerAndPaint);
  context_->SwapBuffers(cb);
  assertNoGLError();
}

}  // anonymous namespace

namespace pp {
// Factory function for your specialization of the Module object.
Module* CreateModule() {
  return new PPAPIGstreamer();
}
}  // namespace pp
