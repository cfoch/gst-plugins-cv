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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include "gstcvdraw.h"

struct _GstCVDrawPrivate
{
  GstPad *sinkpad;
  GstElement *peerelement;
  gdouble unscale_factor;
  cv::Mat *img;
};

GST_DEBUG_CATEGORY_STATIC (gst_cv_draw_debug);
#define GST_CAT_DEFAULT gst_cv_draw_debug

/* Default property values */
#define DEFAULT_ID_TAG              GST_CV_OBJECT_INFO_TAG_UNDEFINED
#define DEFAULT_COLOR               cv::Scalar (0, 255, 0)

/* Filter signals and args */
enum
{
  /* FILL ME */
  LAST_SIGNAL
};

enum
{
  PROP_0,
  PROP_ID_TAG
};

#define parent_class gst_cv_draw_parent_class
G_DEFINE_TYPE_WITH_PRIVATE (GstCVDraw, gst_cv_draw,
    GST_TYPE_OPENCV_VIDEO_FILTER);

static GstStaticPadTemplate sink_factory = GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK, GST_PAD_ALWAYS, GST_STATIC_CAPS (GST_VIDEO_CAPS_MAKE (
        "RGB")));

static GstStaticPadTemplate src_factory = GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC, GST_PAD_ALWAYS, GST_STATIC_CAPS (GST_VIDEO_CAPS_MAKE ("RGB")));

static void gst_cv_draw_set_property (GObject *object, guint prop_id, const
    GValue *value, GParamSpec *pspec);
static void gst_cv_draw_get_property (GObject *object, guint prop_id,
    GValue *value, GParamSpec *pspec);

static GstFlowReturn gst_cv_draw_transform_ip (GstOpencvVideoFilter *self,
    GstBuffer *buf, cv::Mat img);

static void
gst_cv_draw_finalize (GObject *obj)
{
  G_OBJECT_CLASS (parent_class)->finalize (obj);
}

static void
gst_cv_draw_class_init (GstCVDrawClass *klass)
{
  GObjectClass *gobject_class;
  GstOpencvVideoFilterClass *gstopencvbasefilter_class;
  GstElementClass *element_class = GST_ELEMENT_CLASS (klass);

  GST_DEBUG_CATEGORY_INIT (gst_cv_draw_debug, "cvdraw", 0, "cvdraw element");

  gobject_class = (GObjectClass *) klass;
  gobject_class->finalize = GST_DEBUG_FUNCPTR (gst_cv_draw_finalize);
  gstopencvbasefilter_class = (GstOpencvVideoFilterClass *) klass;

  gstopencvbasefilter_class->cv_trans_ip_func = gst_cv_draw_transform_ip;

  gobject_class->set_property = gst_cv_draw_set_property;
  gobject_class->get_property = gst_cv_draw_get_property;

  g_object_class_install_property (gobject_class, PROP_ID_TAG,
      g_param_spec_enum ("id-tag", "id-tag",
      "Reference tag to use as the position where to draw the objects ids.",
      GST_CV_OBJECT_INFO_TAG, DEFAULT_ID_TAG, (GParamFlags) (G_PARAM_READWRITE |
        G_PARAM_STATIC_STRINGS)));

  gst_element_class_set_static_metadata (element_class, "cvdraw",
      "Filter/Effect/Video",
      "Draws the info contained in the GstObjectInfoMapMeta structure.",
      "Cesar Fabian Orccon Chipana <cfoch.fabian@gmail.com>");

  gst_element_class_add_static_pad_template (element_class, &src_factory);
  gst_element_class_add_static_pad_template (element_class, &sink_factory);
}

static void
gst_cv_draw_init (GstCVDraw *self)
{
  self->priv = (GstCVDrawPrivate *) gst_cv_draw_get_instance_private (self);
  self->priv->sinkpad = gst_element_get_static_pad (GST_ELEMENT (self), "sink");
  self->priv->peerelement = NULL;
  gst_opencv_video_filter_set_in_place (GST_OPENCV_VIDEO_FILTER_CAST (self),
      TRUE);
}

static void
gst_cv_draw_set_property (GObject *object, guint prop_id, const GValue *value,
    GParamSpec *pspec)
{
  GstCVDraw *self = GST_CV_DRAW (object);

  switch (prop_id) {
    case PROP_ID_TAG:
      self->id_tag = (GstCVObjectInfoTag) g_value_get_enum (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gst_cv_draw_get_property (GObject *object, guint prop_id, GValue *value,
    GParamSpec *pspec)
{
  GstCVDraw *self = GST_CV_DRAW (object);

  switch (prop_id) {
    case PROP_ID_TAG:
      g_value_set_enum (value, self->id_tag);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gst_cv_draw_draw_object_info (GstCVDraw *self, GstCVObjectInfo *info)
{
  gst_cv_object_info_draw (info, self->priv->unscale_factor, *self->priv->img,
      DEFAULT_COLOR);
}

static void
gst_cv_draw_draw_id (GstCVDraw *self, GstCVObjectInfo *reference_object_info,
    guint index)
{
  graphene_point_t scaled_centroid;
  gdouble unscale_factor = self->priv->unscale_factor;

  gst_cv_object_info_get_centroid (reference_object_info, &scaled_centroid);
  if (GST_CV_OBJECT_INFO_CENTROID_IS_UNDEFINED (scaled_centroid))
    return;

  cv::Point unscaled_centroid = cv::Point (scaled_centroid.x,
      scaled_centroid.y) * unscale_factor;
  cv::putText (*self->priv->img, std::to_string (index), unscaled_centroid,
      cv::FONT_HERSHEY_SIMPLEX, 1.0, DEFAULT_COLOR);
}

static void
_draw (guint index, GstCVObjectInfo *info, GstCVDraw *self)
{
  if (g_strcmp0 (gst_cv_object_info_get_target_element_name (info),
      GST_ELEMENT_NAME (self->priv->peerelement)) != 0)
    return;

  if (self->id_tag == gst_cv_object_info_get_tag (info))
    gst_cv_draw_draw_id (self, info, index);
  gst_cv_draw_draw_object_info (self, info);
}

static void
_get_scaled_frame_and_unscale_factor (GstBuffer *buf, cv::Mat input_frame,
    cv::Mat &scaled_frame, gdouble *unscale_factor)
{
  GstCVDupScaleMeta *meta;

  meta = (GstCVDupScaleMeta *) gst_buffer_get_meta (buf,
      GST_CV_DUP_SCALE_META_API_TYPE);

  if (meta) {
    scaled_frame = gst_cv_dup_scale_meta_get_scaled_frame (meta);
    *unscale_factor = gst_cv_dup_scale_meta_get_unscale_factor (meta);
  } else {
    scaled_frame = input_frame;
    *unscale_factor = 1.0;
  }
}

static GstFlowReturn
gst_cv_draw_transform_ip (GstOpencvVideoFilter *base, GstBuffer *buf, cv::Mat
    img)
{
  GstCVDraw *self = GST_CV_DRAW (base);
  GstCVObjectInfoMapMeta *meta;
  GstCVObjectInfoMap *map;
  cv::Mat scaled_frame;
  GstPad *peerpad;

  self->priv->img = NULL;

  peerpad = gst_pad_get_peer (self->priv->sinkpad);
  if (!peerpad)
    return GST_FLOW_ERROR;
  self->priv->peerelement = GST_ELEMENT (gst_pad_get_parent (peerpad));
  if (!self->priv->peerelement)
    return GST_FLOW_ERROR;

  meta = (GstCVObjectInfoMapMeta *) (gst_buffer_get_meta (buf,
      GST_CV_OBJECT_INFO_MAP_META_API_TYPE));
  if (!meta)
    /* TODO: Warning. */
    return GST_FLOW_OK;

  self->priv->img = &img;
  _get_scaled_frame_and_unscale_factor (buf, img, scaled_frame,
      &self->priv->unscale_factor);

  map = gst_cv_object_info_map_meta_get_object_info_map (meta);
  gst_cv_object_info_map_foreach (map, (GstCVObjectInfoMapFunc) _draw, self);

  return GST_FLOW_OK;
}
