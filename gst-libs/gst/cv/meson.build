cv_sources = [
  'gstcvobjectdetect.cpp',
  'gstcvobjectassign.cpp',
  'dupscalemeta.cpp',
  'utils.c',
  'object/info/objectinfo.c',
  'object/info/objectinfomap.c',
  'object/info/objectinfomapcache.c',
  'object/info/objectinfomapnamedcache.c',
  'object/info/objectinfomapmeta.c',
  'object/info/objectinfoutils.c',
  'object/info/objectinfodraw.cpp',
  'object/info/objectinfocentroid.c',
]

cv_headers = [
  'gstcvobjectdetect.h',
  'gstcvobjectassign.h',
  'dupscalemeta.h',
  'utils.h',
  'object/info/objectinfo.h',
  'object/info/objectinfomap.h',
  'object/info/objectinfomapcache.h',
  'object/info/objectinfomapnamedcache.h',
  'object/info/objectinfomapmeta.h',
  'object/info/objectinfoutils.h',
  'object/info/objectinfodraw.h',
  'object/info/objectinfocentroid.h',
]

if opencv_dep.found() and graphene_dep.found()
  gstcv = library('gstcv-' + api_version,
    cv_sources,
    link_args : ['-lgstopencv-1.0'],
    include_directories : [libsinc],
    version : libversion,
    soversion : soversion,
    install : true,
    dependencies : [
      gio_dep,
      gst_dep,
      gstbase_dep,
      graphene_dep,
      opencv_dep
    ]
  )

  gstcv_dep = declare_dependency(link_with: gstcv,
    include_directories : [libsinc],
    dependencies : [opencv_dep, gio_dep, gst_dep])

  install_headers(cv_headers, subdir : 'gstreamer-1.0/gst/cv')

  pkg = import('pkgconfig')
  pkg.generate(
    libraries: [gstcv],
    subdirs: 'gstreamer-1.0/gst/cv',
    version: meson.project_version(),
    name: 'GstCV',
    filebase: 'gstcv-@0@'.format(api_version),
    description: 'Base computer vision plugins.',
    requires: ['gstreamer-1.0']
  )
endif
