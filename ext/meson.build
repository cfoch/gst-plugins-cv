gstcv_sources = [
  join_paths('dlib', 'gstcvdlibobjectdetecthog.cpp'),
  join_paths('improc', 'gstcvscale.cpp'),
  'gstcv.cpp'
]

if dlib_dep.found() and graphene_dep.found()
  gstcv = library('gstcv',
    gstcv_sources,
    cpp_args : gst_plugins_bad_args,
    cflags : [ '-g' ],
    link_args : ['-lgstopencv-1.0', '-lgslcblas'],
    include_directories : [configinc],
    dependencies : [gst_dep, gstcv_dep, graphene_dep, gio_dep, dlib_dep],
    install : true,
    install_dir : plugins_install_dir,
  )
endif
