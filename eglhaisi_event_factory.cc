// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/ozone/platform/eglhaisi/eglhaisi_event_factory.h"
#include "base/bind.h"
#include "base/message_loop/message_loop.h"
#include "ui/events/event.h"
#include "ui/events/keycodes/keyboard_codes.h"
#include "hi_ir_remote.h"

#ifdef EGLHAISI_USE_IR

namespace ui {

namespace {
    
ui::KeyboardCode GetKeyboardCode(HI_S32 keyvalue) 
{ 
    switch(keyvalue)
    {
        case HI_ADP_KEY_1:
           return ui::VKEY_1;
        case HI_ADP_KEY_2:
           return ui::VKEY_2;
        case HI_ADP_KEY_3:
           return ui::VKEY_3;
        case HI_ADP_KEY_4:
           return ui::VKEY_4;
        case HI_ADP_KEY_5:
           return ui::VKEY_5;
        case HI_ADP_KEY_6:
           return ui::VKEY_6;
        case HI_ADP_KEY_7:
           return ui::VKEY_7;
        case HI_ADP_KEY_8:
           return ui::VKEY_8;
        case HI_ADP_KEY_9:
           return ui::VKEY_9;
        case HI_ADP_KEY_0:
           return ui::VKEY_0;
        case HI_ADP_KEY_DEL:
           return ui::VKEY_DELETE;
        case HI_ADP_KEY_NEXT:
           return ui::VKEY_NEXT;
        case HI_ADP_KEY_UP:
           return ui::VKEY_UP;
        case HI_ADP_KEY_DOWN:
           return ui::VKEY_DOWN;
        case HI_ADP_KEY_LEFT:
           return ui::VKEY_LEFT;
        case HI_ADP_KEY_RIGHT:
           return ui::VKEY_RIGHT;
        case HI_ADP_KEY_ENTER:
           return ui::VKEY_RETURN;
        case HI_ADP_KEY_EXIT:
           return ui::VKEY_ESCAPE;
        case HI_ADP_KEY_MENU:
           return ui::VKEY_TAB;
        case HI_ADP_KEY_BACK:
           return ui::VKEY_BACK;
        case HI_ADP_KEY_F1:
           return ui::VKEY_F1;
        case HI_ADP_KEY_F2:
           return ui::VKEY_F2;
        case HI_ADP_KEY_F3:
           return ui::VKEY_F3;
        case HI_ADP_KEY_F4:
           return ui::VKEY_F4;    
        case HI_ADP_KEY_POWER:
           return ui::VKEY_POWER;
        case HI_ADP_KEY_MUTE:
           return ui::VKEY_VOLUME_MUTE;
        case HI_ADP_KEY_VOLDOWN:
           return ui::VKEY_VOLUME_DOWN;
        case HI_ADP_KEY_VOLUP:
           return ui::VKEY_VOLUME_UP;
        case HI_ADP_KEY_SET://for test
           return ui::VKEY_SPACE;    
        default:
           return ui::VKEY_UNKNOWN;
    }  
}

}  // namespace

EglhaisiEventFactory::EglhaisiEventFactory():weak_ptr_factory_(this),
             delay_(base::TimeDelta::FromMilliseconds(10))
{

    HI_S32 s32RetVal = HI_RemoteDev_Initialize();
    if (HI_SUCCESS != s32RetVal)
    {
        printf(">>>> ir remote init failure >>>>> \n");
    }
    else
    {
        printf(">>>>>>>>>>>> ir  remote init success >>>>>>>>>\n");
    }    
   
}

EglhaisiEventFactory::~EglhaisiEventFactory()
{
    HI_S32 s32RetVal = HI_RemoteDev_Shutdown();
    if (HI_SUCCESS != s32RetVal)
    {
        printf(">>>> ir remote close failure >>>\n");
    }
    else
    {
        printf(">>>>>>>>>>>> ir  remote close success >>>>>>>>>\n");
    }    
}

void EglhaisiEventFactory::WarpCursorTo(gfx::AcceleratedWidget widget,
                            const gfx::PointF& location)
{
    NOTIMPLEMENTED();
}

void EglhaisiEventFactory::OnDispatcherListChanged() {
  ScheduleEventProcessing();
}

void EglhaisiEventFactory::ScheduleEventProcessing() {
  // Caca uses a poll based event retrieval. Since we don't want to block we'd
  // either need to spin up a new thread or just poll. For simplicity just poll
  // for a message every |delay_| time delta.
  base::MessageLoop::current()->PostDelayedTask(
      FROM_HERE,
      base::Bind(&EglhaisiEventFactory::TryProcessingEvent,
                 weak_ptr_factory_.GetWeakPtr()),
      delay_);
}

void EglhaisiEventFactory::TryProcessingEvent() {
    HI_S32 keyvalue = HI_RemoteDev_GetInput();     
    if (HI_FAILURE != keyvalue)
    {
        ui::KeyboardCode key = GetKeyboardCode(keyvalue);
        if(key != ui::VKEY_UNKNOWN)
        {
            printf("-----get keyvalue %d\n",keyvalue);
            ui::KeyEvent key_event(
                ui::ET_KEY_PRESSED, key, 0, false);
            DispatchEvent(&key_event);
        }
    }
    ScheduleEventProcessing();
}
}  // namespace ui

#endif //EGLHAISI_USE_IR
