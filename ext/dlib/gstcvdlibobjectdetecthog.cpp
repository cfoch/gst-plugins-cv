/*
 * GStreamer
 * Copyright (C) 2005 Thomas Vander Stichele <thomas@apestaart.org>
 * Copyright (C) 2005 Ronald S. Bultje <rbultje@ronald.bitfreak.net>
 * Copyright (C) 2010 Sreerenj Balachandran <bsreerenj@gmail.com>
 * Copyright (C) 2019 Cesar Fabian Orccon Chipana <cfoch.fabian@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

/**
 * SECTION:element-cvdlibobjectdetecthog
 *
 * cvdlibobjectdetecthog detects objects following the method
 * Histograms of Oriented Gradients for Human Detection by
 * Navneet Dalal and Bill Triggs.
 *
 * <refsect2>
 * <title>Example launch line</title>
 * |[
 * gst-launch-1.0 videotestsrc ! videoconvert ! cvdlibobjectdetecthog ! videoconvert ! xvimagesink
 * ]|
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include "gstcvdlibobjectdetecthog.h"

#include <dlib/opencv/cv_image.h>

#include <dlib/svm_threaded.h>
#include <dlib/gui_widgets.h>
#include <dlib/image_processing.h>
#include <dlib/data_io.h>
#include <string>
#include <iostream>

using namespace dlib;

typedef object_detector<scan_fhog_pyramid<pyramid_down<6> > > GstCVDlibDetector;

typedef struct
{
  GstCVDlibDetector *detector;
  // gboolean loading_model;
} GstCVDlibObjectDetectHOGPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (GstCVDlibObjectDetectHOG, gst_cv_dlib_object_detect_hog,
    GST_TYPE_CV_OBJECT_DETECT);

static GstFlowReturn gst_cv_dlib_object_detect_hog_transform_ip (
    GstOpencvVideoFilter * base, GstBuffer * buf, cv::Mat img);
void gst_cv_dlib_object_detect_hog_detect (GstCVObjectDetect * self, cv::Mat & img,
    GstCVObjectDetectContext * ctx);
static void gst_cv_dlib_object_detect_hog_load_model (
    GstCVDlibObjectDetectHOG * self, const gchar * location);

GST_DEBUG_CATEGORY_STATIC (gst_cv_dlib_object_detect_hog_debug);
#define GST_CAT_DEFAULT gst_cv_dlib_object_detect_hog_debug

#define DEFAULT_PYRAMID_DOWN 	6

enum
{
  LAST_SIGNAL
};

enum
{
  PROP_0,
  PROP_MODEL_LOCATION
};

#define parent_class gst_cv_dlib_object_detect_hog_parent_class

static GstStaticPadTemplate sink_factory = GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS (GST_VIDEO_CAPS_MAKE ("RGB"))
    );

static GstStaticPadTemplate src_factory = GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS (GST_VIDEO_CAPS_MAKE ("RGB"))
    );

static void gst_cv_dlib_object_detect_hog_set_property (GObject * object,
    guint prop_id, const GValue * value, GParamSpec * pspec);
static void gst_cv_dlib_object_detect_hog_get_property (GObject * object,
    guint prop_id, GValue * value, GParamSpec * pspec);

static GstFlowReturn gst_cv_dlib_object_detect_hog_transform_ip (
    GstOpencvVideoFilter * self, GstBuffer * buf, cv::Mat img);

/* Clean up */
static void
gst_cv_dlib_object_detect_hog_finalize (GObject * obj)
{
  // GstCVDlibObjectDetectHOG *self = GST_CV_DLIB_OBJECT_DETECT_HOG (obj);

  G_OBJECT_CLASS (parent_class)->finalize (obj);
}

/* initialize the cvdlibobjectdetecthog's class */
static void
gst_cv_dlib_object_detect_hog_class_init (GstCVDlibObjectDetectHOGClass * klass)
{
  GObjectClass *gobject_class;
  GstOpencvVideoFilterClass *gstopencvbasefilter_class;
  GstCVObjectDetectClass *gstcvobjectdetect_class;
  GstElementClass *element_class = GST_ELEMENT_CLASS (klass);

  GST_DEBUG_CATEGORY_INIT (gst_cv_dlib_object_detect_hog_debug,
      "cvdlibobjectdetecthog", 0, "Template cvdlibobjectdetecthog");

  gobject_class = (GObjectClass *) klass;
  gobject_class->finalize =
      GST_DEBUG_FUNCPTR (gst_cv_dlib_object_detect_hog_finalize);

  gstopencvbasefilter_class = (GstOpencvVideoFilterClass *) klass;
  gstopencvbasefilter_class->cv_trans_ip_func =
      gst_cv_dlib_object_detect_hog_transform_ip;

  gstcvobjectdetect_class = (GstCVObjectDetectClass *) klass;
  gstcvobjectdetect_class->detect = gst_cv_dlib_object_detect_hog_detect;

  gobject_class->set_property = gst_cv_dlib_object_detect_hog_set_property;
  gobject_class->get_property = gst_cv_dlib_object_detect_hog_get_property;

  g_object_class_install_property (gobject_class, PROP_MODEL_LOCATION,
      g_param_spec_string ("model-location", "model-location",
          "The location of the serialized model to classify against.",
          NULL,
          (GParamFlags) (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS)));

  gst_element_class_set_static_metadata (element_class,
      "cvdlibobjectdetecthog",
      "Filter/Effect/Video",
      "Detect objects using a HOG-based detector",
      "Cesar Fabian Orccon Chipana <cfoch.fabian@gmail.com>");

  gst_element_class_add_static_pad_template (element_class, &src_factory);
  gst_element_class_add_static_pad_template (element_class, &sink_factory);

}

static void
gst_cv_dlib_object_detect_hog_init (GstCVDlibObjectDetectHOG * self)
{
  GstCVDlibObjectDetectHOGPrivate *priv = (GstCVDlibObjectDetectHOGPrivate *)
      gst_cv_dlib_object_detect_hog_get_instance_private (self);

  self->model_location = NULL;
  priv->detector = new GstCVDlibDetector();

  gst_opencv_video_filter_set_in_place (GST_OPENCV_VIDEO_FILTER_CAST (self),
      TRUE);
}

static void
gst_cv_dlib_object_detect_hog_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec)
{
  GstCVDlibObjectDetectHOG *self = GST_CV_DLIB_OBJECT_DETECT_HOG (object);

  switch (prop_id) {
    case PROP_MODEL_LOCATION:
      if (self->model_location)
        g_free (self->model_location);
      self->model_location = g_value_dup_string (value);
      gst_cv_dlib_object_detect_hog_load_model (self, self->model_location);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gst_cv_dlib_object_detect_hog_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec)
{
  GstCVDlibObjectDetectHOG *self = GST_CV_DLIB_OBJECT_DETECT_HOG (object);

  switch (prop_id) {
    case PROP_MODEL_LOCATION:
      g_value_set_string (value, self->model_location);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

void
gst_cv_dlib_object_detect_hog_detect (GstCVObjectDetect * filter, cv::Mat & img,
    GstCVObjectDetectContext *ctx)
{
  GstCVDlibObjectDetectHOG *self = GST_CV_DLIB_OBJECT_DETECT_HOG (filter);
  GstCVDlibObjectDetectHOGPrivate *priv = (GstCVDlibObjectDetectHOGPrivate *)
      gst_cv_dlib_object_detect_hog_get_instance_private (self);
  dlib::array2d<bgr_pixel> dlib_img;
  dlib::assign_image(dlib_img, dlib::cv_image<bgr_pixel>(img));
  guint i;

  std::vector<rectangle> dets = (*priv->detector)(dlib_img);

  for (i = 0; i < dets.size(); i++) {
    dlib::rectangle &rectangle = dets[i];
    graphene_rect_t rect;
    graphene_rect_init (&rect, rectangle.left (), rectangle.top (),
        rectangle.width (), rectangle.height ());

    gst_cv_object_detect_register_face (GST_CV_OBJECT_DETECT (self), &rect,
        ctx);
  }
}

static void
read_model_cb (GObject * source_object, GAsyncResult * res, gpointer user_data)
{
  GstCVDlibObjectDetectHOG *self = GST_CV_DLIB_OBJECT_DETECT_HOG (user_data);
  GstCVDlibObjectDetectHOGPrivate *priv = (GstCVDlibObjectDetectHOGPrivate *)
      gst_cv_dlib_object_detect_hog_get_instance_private (self);
  g_autoptr(GError) error = NULL;
  GByteArray *gbarray;
  GBytes *bytes;
  gconstpointer data;
  gsize data_size;

  gbarray =
      g_file_read_to_byte_array_finish (G_FILE (source_object), res, &error);

  if (error != NULL) {
    GST_ELEMENT_ERROR (self, RESOURCE, READ,
        ("Error while reading from file \"%s\".", self->model_location),
        ("%s", error->message));
    return;
  }

  bytes = g_byte_array_free_to_bytes (gbarray);
  data = g_bytes_get_data (bytes, &data_size);

  std::string buffer((const char *) data, (size_t) data_size);
  std::istringstream sin(buffer);

  try {
    deserialize (*priv->detector, sin);
    GST_CV_OBJECT_DETECT (self)->ready = TRUE;

  } catch (serialization_error &e) {
    std::cout << e.info;
  }
}

static void
gst_cv_dlib_object_detect_hog_load_model (GstCVDlibObjectDetectHOG * self,
    const gchar * location)
{
  GFile *file;
  char buffer[4096];
  g_return_if_fail (self->model_location != NULL);

  file = g_file_new_for_path (self->model_location);
  g_file_read_to_byte_array_async (file, buffer, sizeof (buffer),
      NULL, read_model_cb, self);
}

static GstFlowReturn
gst_cv_dlib_object_detect_hog_transform_ip (GstOpencvVideoFilter * base,
    GstBuffer * buf, cv::Mat img)
{
  GstFlowReturn ret;

  ret = GST_OPENCV_VIDEO_FILTER_CLASS (parent_class)->cv_trans_ip_func (base,
      buf, img);

  return ret;
}
