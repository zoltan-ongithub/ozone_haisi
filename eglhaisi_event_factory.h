// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_OZONE_PLATFORM_EGLHAISI_EGLHAISI_EVENT_FACTORY_H_
#define UI_OZONE_PLATFORM_EGLHAISI_EGLHAISI_EVENT_FACTORY_H_

#define EGLHAISI_USE_IR 1

#ifdef EGLHAISI_USE_IR

#include "base/memory/ref_counted.h"
#include "base/time/time.h"
#include "ui/ozone/public/event_factory_ozone.h"
#include "ui/events/platform/platform_event_source.h"
#include "ui/gfx/geometry/point_f.h"



namespace ui {


class EglhaisiEventFactory : public EventFactoryOzone,
                         public PlatformEventSource {
 public:
  EglhaisiEventFactory();
  virtual ~EglhaisiEventFactory();

  // ui::EventFactoryOzone:
  virtual void WarpCursorTo(gfx::AcceleratedWidget widget,
                            const gfx::PointF& location) OVERRIDE;

 private: 
   // PlatformEventSource:
  virtual void OnDispatcherListChanged() OVERRIDE;
  
  void ScheduleEventProcessing();

  void TryProcessingEvent();

  base::WeakPtrFactory<EglhaisiEventFactory> weak_ptr_factory_;

  // Delay between event polls.
  base::TimeDelta delay_;

  DISALLOW_COPY_AND_ASSIGN(EglhaisiEventFactory);
};

}  // namespace ui
#endif //EGLHAISI_USE_IR
#endif  // UI_OZONE_PLATFORM_EGLHAISI_EGLHAISI_EVENT_FACTORY_H_
