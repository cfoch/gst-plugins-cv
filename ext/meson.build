hungariandir = join_paths(meson.source_root(), 'hungarian-algorithm-cpp')

gstcv_sources = [
  join_paths(hungariandir, 'Hungarian.cpp'),
  join_paths('hacpp', 'gstcvhacppobjectassign.cpp'),
  join_paths('dlib', 'gstcvdlibobjectdetecthog.cpp'),
  join_paths('improc', 'gstcvscale.cpp'),
  join_paths('improc', 'gstcvdraw.cpp'),
  join_paths('utils', 'gstcvobjectcache.cpp'),
  'gstcv.cpp'
]

if dlib_dep.found() and graphene_dep.found()
  gstcv = library('gstcv',
    gstcv_sources,
    cpp_args : gst_plugins_bad_args,
    link_args : ['-lgstopencv-1.0', '-lgslcblas'],
    include_directories : [configinc, hungarianinc],
    dependencies : [gst_dep, gstbase_dep, gstcv_dep, graphene_dep, dlib_dep],
    install : true,
    install_dir : plugins_install_dir,
  )
endif
