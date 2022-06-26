project('gst-plugins-cv', 'c', 'cpp',
        version : '1.16.0.0',
        meson_version : '>= 0.40.1')

gst_version = meson.project_version()
version_arr = gst_version.split('.')
gst_version_major = version_arr[0]
gst_version_minor = version_arr[1]
gst_version_micro = version_arr[2]
if version_arr.length() == 4
  gst_version_nano = version_arr[3]
else
  gst_version_nano = 0
endif

plugins_install_dir = '@0@/gstreamer-1.0'.format(get_option('libdir'))

cxx = meson.get_compiler('cpp')
cdata = configuration_data()
cdata.set('GST_LICENSE', '"LGPL"')
cdata.set('GST_PACKAGE_ORIGIN', '"c"')
cdata.set('GST_PACKAGE_NAME', '"Gstreamer Plugins CV"')
cdata.set('PACKAGE', '"gst-plugins-cv"')
cdata.set('VERSION', '"@0@"'.format(gst_version))
configure_file(output : 'config.h', configuration : cdata)

gst_plugins_bad_args = ['-DHAVE_CONFIG_H']
configinc = include_directories('.')
libsinc = include_directories('gst-libs')

gst_req = '>= @0@.@1@.0'.format(gst_version_major, gst_version_minor)
api_version = '1.0'
soversion = 0
libversion = '@0@.@1@.0'.format(soversion, gst_version_minor.to_int() * 100 + gst_version_micro.to_int())

# GStreamer dependencies
gst_dep = dependency('gstreamer-1.0', version : gst_req,
  fallback : ['gstreamer', 'gst_dep'])
gstbase_dep = dependency('gstreamer-base-1.0', version : gst_req,
  fallback : ['gstreamer', 'gst_base_dep'])

gio_dep = dependency('gio-2.0')
opencv_dep = dependency('opencv', version : '>= 3.0.0', required: false)
if not opencv_dep.found()
  opencv_dep = dependency('opencv4', version : '>= 4.0.0', required: true)
endif
dlib_dep = dependency('dlib-1', required : false)
graphene_dep = dependency('graphene-1.0', version : ['>= 1.7.0'],
  required : true)

hungarianinc = include_directories('hungarian-algorithm-cpp')

subdir('gst-libs')
subdir('ext')
subdir('tests')

python3 = import('python').find_installation()

run_command(python3, '-c', 'import shutil; shutil.copy("hooks/pre-commit.hook", ".git/hooks/pre-commit")')
