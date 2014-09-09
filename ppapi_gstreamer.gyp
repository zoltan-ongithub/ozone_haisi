{
    'variables': {
       'pkg-config': 'pkg-config',
    },
    
    'targets': [
      {
      'target_name': 'ppapi_gstreamer',
      'type': 'shared_library',
      'dependencies': [
        '<(DEPTH)/ppapi/ppapi.gyp:ppapi_cpp',
        '<(DEPTH)/ppapi/ppapi.gyp:ppapi_gles2',
      ],
      'include_dirs': [
        '<(DEPTH)/ppapi/lib/gl/include',
      ],
      'sources': [
        'ppapi_gstreamer.cc',
        'gstreamer_player.cc'
      ],
      'cflags': [
      '<!@(<(pkg-config) --cflags gstreamer-0.10)',
      ],
      'link_settings': {
            'ldflags': [
              '<!@(<(pkg-config) --libs-only-L --libs-only-other gstreamer-0.10)',
            ],
            'libraries': [
              '<!@(<(pkg-config) --libs-only-l gstreamer-0.10)',
            ],
       },
    },
      
    ],
  }
