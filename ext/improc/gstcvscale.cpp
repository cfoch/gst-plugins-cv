/*
 * GStreamer Plugins CV
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
 * SECTION:element-cvscale
 *
 * cvscale duplicates and resizes an input frame an puts it into the
 * buffer metadata, so other filters can work on the resized frame
 * instead of the original one.
 *
 * <refsect2>
 * <title>Example launch line</title>
 * |[
 * gst-launch-1.0 v4l2src ! videoconvert ! cvscale scale-factor=0.6 ! \
 *    cvdlibobjectdetecthog model-location=model.dat ! videoconvert ! xvimagesink
 * ]|
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include "gstcvscale.h"

GST_DEBUG_CATEGORY_STATIC (gst_cv_scale_debug);
#define GST_CAT_DEFAULT gst_cv_scale_debug

/* Default property values */
#define DEFAULT_ENABLED        TRUE
#define DEFAULT_SCALE_FACTOR   1

/* Filter signals and args */
enum
{
  /* FILL ME */
  LAST_SIGNAL
};

enum
{
  PROP_0,
  PROP_ENABLED,
  PROP_SCALE_FACTOR
};

#define parent_class gst_cv_scale_parent_class
G_DEFINE_TYPE (GstCVScale, gst_cv_scale, GST_TYPE_OPENCV_VIDEO_FILTER);

static GstStaticPadTemplate sink_factory = GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK, GST_PAD_ALWAYS, GST_STATIC_CAPS (GST_VIDEO_CAPS_MAKE (
        "RGB")));

static GstStaticPadTemplate src_factory = GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC, GST_PAD_ALWAYS, GST_STATIC_CAPS (GST_VIDEO_CAPS_MAKE ("RGB")));

static void gst_cv_scale_set_property (GObject *object, guint prop_id, const
    GValue *value, GParamSpec *pspec);
static void gst_cv_scale_get_property (GObject *object, guint prop_id,
    GValue *value, GParamSpec *pspec);
static GstFlowReturn gst_cv_scale_transform_ip (GstOpencvVideoFilter *self,
    GstBuffer *buf, cv::Mat img);

static void
gst_cv_scale_finalize (GObject *obj)
{
  G_OBJECT_CLASS (parent_class)->finalize (obj);
}

static void
gst_cv_scale_class_init (GstCVScaleClass *klass)
{
  GObjectClass *gobject_class;
  GstOpencvVideoFilterClass *gstopencvbasefilter_class;
  GstElementClass *element_class = GST_ELEMENT_CLASS (klass);

  GST_DEBUG_CATEGORY_INIT (gst_cv_scale_debug, "cvscale", 0, "cvscale element");

  gobject_class = (GObjectClass *) klass;
  gobject_class->finalize = GST_DEBUG_FUNCPTR (gst_cv_scale_finalize);
  gstopencvbasefilter_class = (GstOpencvVideoFilterClass *) klass;

  gstopencvbasefilter_class->cv_trans_ip_func = gst_cv_scale_transform_ip;

  gobject_class->set_property = gst_cv_scale_set_property;
  gobject_class->get_property = gst_cv_scale_get_property;

  g_object_class_install_property (gobject_class, PROP_SCALE_FACTOR,
      g_param_spec_double ("scale-factor", "scale-factor",
      "Scale factor to apply to input the frame.", 0, G_MAXDOUBLE,
      DEFAULT_SCALE_FACTOR, (GParamFlags) (G_PARAM_READWRITE |
        G_PARAM_STATIC_STRINGS)));

  gst_element_class_set_static_metadata (element_class, "cvscale",
      "Filter/Effect/Video",
      "Creates a duplicated scaled frame to be added to the buffer metadata.",
      "Cesar Fabian Orccon Chipana <cfoch.fabian@gmail.com>");

  gst_element_class_add_static_pad_template (element_class, &src_factory);
  gst_element_class_add_static_pad_template (element_class, &sink_factory);
}

static void
gst_cv_scale_init (GstCVScale *self)
{
  self->scale_factor = DEFAULT_SCALE_FACTOR;

  gst_opencv_video_filter_set_in_place (GST_OPENCV_VIDEO_FILTER_CAST (self),
      TRUE);
}

static void
gst_cv_scale_set_property (GObject *object, guint prop_id, const GValue *value,
    GParamSpec *pspec)
{
  GstCVScale *self = GST_CV_SCALE (object);

  switch (prop_id) {
    case PROP_SCALE_FACTOR:
      self->scale_factor = g_value_get_double (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gst_cv_scale_get_property (GObject *object, guint prop_id, GValue *value,
    GParamSpec *pspec)
{
  GstCVScale *self = GST_CV_SCALE (object);

  switch (prop_id) {
    case PROP_SCALE_FACTOR:
      g_value_set_double (value, self->scale_factor);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static GstFlowReturn
gst_cv_scale_transform_ip (GstOpencvVideoFilter *base, GstBuffer *buf, cv::Mat
    img)
{
  GstCVScale *self = GST_CV_SCALE (base);
  GstCVDupScaleMeta *old_meta;
  cv::Mat scaled_copy;

  /* Only one meta of this type is allowed. */
  old_meta = (GstCVDupScaleMeta *) gst_buffer_get_meta (buf,
      GST_CV_DUP_SCALE_META_API_TYPE);
  if (old_meta != NULL)
    gst_buffer_remove_meta (buf, (GstMeta *) old_meta);

  if (self->scale_factor != 1.0) {
    cv::Size size (img.cols * self->scale_factor, img.rows *self->scale_factor);

    cv::resize (img, scaled_copy, size);
  } else
    scaled_copy = img;

  gst_buffer_add_cv_dup_scale_meta (buf, img, scaled_copy);

  return GST_FLOW_OK;
}
