# Copyright 2014 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'variables': {
    'internal_ozone_platform_deps': [
      'ozone_platform_eglhaisi',
    ],
    'internal_ozone_platforms': [
      'eglhaisi'
    ],
  },
  'targets': [
    {
      'target_name': 'ozone_platform_eglhaisi',
      'type': 'static_library',
      'defines': [
        'OZONE_IMPLEMENTATION',
      ],
      'dependencies': [
        '../../base/base.gyp:base',
        '../events/events.gyp:events',
        '../events/ozone/events_ozone.gyp:events_ozone_evdev',
        '../gfx/gfx.gyp:gfx',
      ],
      'sources': [
        'ozone_platform_eglhaisi.cc',
        'ozone_platform_eglhaisi.h',
        'eglhaisi_surface_factory.cc',
        'eglhaisi_surface_factory.h',
        'eglhaisi_event_factory.cc',
        'eglhaisi_event_factory.h',
        'hi_ir_remote.h',
        'hi_ir_remote.cc',
      ],
      'link_settings': {
            'libraries': [
              '-lhi_ir',
              '-lhi_common'
            ],
      },
    },
  ],
}
