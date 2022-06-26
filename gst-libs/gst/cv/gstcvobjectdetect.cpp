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

#include "gstcvobjectdetect.h"
#include "object/info/objectinfomapmeta.h"
#include "object/info/objectinfoutils.h"

struct _GstCVObjectDetectContext
{
  guint index;
  cv::Mat img;
  GstBuffer *buf;
  gdouble unscale_factor;
};

static void
gst_cv_object_detect_context_init (GstCVObjectDetectContext *ctx,
    GstCVObjectDetect *filter, cv::Mat img, GstBuffer *buf, gdouble
    unscale_factor)
{
  ctx->index = gst_cv_object_info_generate_id ();
  ctx->img = img;
  ctx->buf = buf;
  ctx->unscale_factor = unscale_factor;
}

GST_DEBUG_CATEGORY_STATIC (gst_cv_object_detect_debug);
#define GST_CAT_DEFAULT gst_cv_object_detect_debug

/* Default property values */
#define DEFAULT_PROP_DRAW           FALSE
#define DEFAULT_BOUNDING_BOX_COLOR  cv::Scalar (0, 255, 0)

/* Filter signals and args */
enum
{
  /* FILL ME */
  LAST_SIGNAL
};

enum
{
  PROP_0,
  PROP_DRAW,
  PROP_SUB_KEY,
};

#define parent_class gst_cv_object_detect_parent_class
G_DEFINE_ABSTRACT_TYPE (GstCVObjectDetect, gst_cv_object_detect,
    GST_TYPE_OPENCV_VIDEO_FILTER);

static void gst_cv_object_detect_set_property (GObject *object, guint prop_id,
    const GValue *value, GParamSpec *pspec);
static void gst_cv_object_detect_get_property (GObject *object, guint prop_id,
    GValue *value, GParamSpec *pspec);

static GstFlowReturn gst_cv_object_detect_transform_ip (
    GstOpencvVideoFilter *self, GstBuffer *buf, cv::Mat img);

static void
gst_cv_object_detect_finalize (GObject *obj)
{
  G_OBJECT_CLASS (parent_class)->finalize (obj);
}

static void
gst_cv_object_detect_class_init (GstCVObjectDetectClass *klass)
{
  GObjectClass *gobject_class;
  GstOpencvVideoFilterClass *gstopencvbasefilter_class;

  GST_DEBUG_CATEGORY_INIT (gst_cv_object_detect_debug, "cvobjectdetect", 0,
      "cvobjectdetect element");

  gobject_class = (GObjectClass *) klass;
  gobject_class->finalize = GST_DEBUG_FUNCPTR (gst_cv_object_detect_finalize);
  gstopencvbasefilter_class = (GstOpencvVideoFilterClass *) klass;

  gstopencvbasefilter_class->cv_trans_ip_func =
      gst_cv_object_detect_transform_ip;

  gobject_class->set_property = gst_cv_object_detect_set_property;
  gobject_class->get_property = gst_cv_object_detect_get_property;

  g_object_class_install_property (gobject_class, PROP_SUB_KEY,
      g_param_spec_boxed ("sub-key", "Sub-key", "A structured key useful to "
      "look up the output object info. This key will be extended with the "
      "structure 'type=roi,index=%d'.", GST_TYPE_STRUCTURE,
      (GParamFlags) (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS)));

  klass->detect = NULL;
}

static void
gst_cv_object_detect_init (GstCVObjectDetect *self)
{
  self->ready = FALSE;

  gst_opencv_video_filter_set_in_place (GST_OPENCV_VIDEO_FILTER_CAST (self),
      TRUE);
}

static void
gst_cv_object_detect_set_property (GObject *object, guint prop_id, const
    GValue *value, GParamSpec *pspec)
{
  GstCVObjectDetect *self = GST_CV_OBJECT_DETECT (object);

  switch (prop_id) {
    case PROP_DRAW:
      self->draw = g_value_get_boolean (value);
      break;
    case PROP_SUB_KEY:
      if (self->sub_key)
        gst_structure_free (self->sub_key);
      self->sub_key = GST_STRUCTURE (g_value_dup_boxed (value));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gst_cv_object_detect_get_property (GObject *object, guint prop_id,
    GValue *value, GParamSpec *pspec)
{
  GstCVObjectDetect *self = GST_CV_OBJECT_DETECT (object);

  switch (prop_id) {
    case PROP_DRAW:
      g_value_set_boolean (value, self->draw);
      break;
    case PROP_SUB_KEY:
      g_value_set_boxed (value, self->sub_key);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

GstStructure *
gst_cv_object_detect_new_key (GstCVObjectDetect *self,
    GstStructure *base_sub_key, guint index)
{
  GstStructure *sub_key;
  gchar *element_name;

  element_name = gst_element_get_name (self);

  sub_key = gst_structure_copy (base_sub_key);
  gst_structure_set (sub_key, GST_CV_OBJECT_INFO_SUB_KEY_ELEMENT,
      GST_TYPE_CV_OBJECT_INFO_SUB_KEY_ELEMENT, element_name,
      GST_CV_OBJECT_INFO_SUB_KEY_TYPE, GST_TYPE_CV_OBJECT_INFO_SUB_KEY_TYPE,
      GST_CV_OBJECT_INFO_SUB_KEY_TYPE_ROI, GST_CV_OBJECT_INFO_SUB_KEY_INDEX,
      GST_TYPE_CV_OBJECT_INFO_SUB_KEY_INDEX, index, NULL);

  g_free (element_name);

  return sub_key;
}

static GstCVObjectInfo *
new_object_info (graphene_rect_t *rectangle, guint index)
{
  GstStructure *params;
  GValue rectangle_value = G_VALUE_INIT;

  params = gst_structure_new ("cv", GST_CV_OBJECT_INFO_SUB_PARAM_INDEX,
      GST_TYPE_CV_OBJECT_INFO_SUB_PARAM_INDEX, index, NULL);

  g_value_init (&rectangle_value, GRAPHENE_TYPE_RECT);
  g_value_set_boxed (&rectangle_value, rectangle);

  return gst_cv_object_info_new_with_params (&rectangle_value, params);
}

void
gst_cv_object_detect_register_face (GstCVObjectDetect *self,
    graphene_rect_t *rectangle, gpointer user_data)
{
  GstCVObjectDetectContext *ctx = (GstCVObjectDetectContext *) user_data;
  GstCVObjectInfoMapMeta *meta;
  GstCVObjectInfoMap *map;
  GstCVObjectInfo *object_info;
  GstStructure *key;

  meta = (GstCVObjectInfoMapMeta *) (gst_buffer_get_meta (ctx->buf,
      GST_CV_OBJECT_INFO_MAP_META_API_TYPE));
  if (!meta)
    meta = gst_buffer_add_cv_object_info_map_meta (ctx->buf);

  map = gst_cv_object_info_map_meta_get_object_info_map (meta);

  key = gst_cv_object_detect_new_key (self, self->sub_key, ctx->index);

  object_info = new_object_info (rectangle, ctx->index);

  if (!gst_cv_object_info_map_insert (map, key, object_info)) {
    gst_cv_object_info_free (object_info);
    GST_ERROR_OBJECT (self, "Cannot add object %d information to buffer meta.",
        ctx->index);
  }

  ctx->index = gst_cv_object_info_generate_id ();
}

static void
gst_cv_object_detect_default (GstCVObjectDetect *self, GstBuffer *buf,
    cv::Mat &img)
{
  GstCVObjectDetectClass *klass = GST_CV_OBJECT_DETECT_CLASS_GET_CLASS (self);
  GstCVDupScaleMeta *meta;
  cv::Mat scaled_frame;
  GstCVObjectDetectContext ctx;
  gdouble unscale_factor;

  meta = (GstCVDupScaleMeta *) gst_buffer_get_meta (buf,
      GST_CV_DUP_SCALE_META_API_TYPE);

  if (meta) {
    scaled_frame = gst_cv_dup_scale_meta_get_scaled_frame (meta);
    unscale_factor = gst_cv_dup_scale_meta_get_unscale_factor (meta);
  } else {
    scaled_frame = img;
    unscale_factor = 1.0;
  }

  gst_cv_object_detect_context_init (&ctx, self, img, buf, unscale_factor);
  klass->detect (self, scaled_frame, &ctx);
}

static GstFlowReturn
gst_cv_object_detect_transform_ip (GstOpencvVideoFilter *base, GstBuffer *buf,
    cv::Mat img)
{
  GstCVObjectDetect *self = GST_CV_OBJECT_DETECT (base);
  GstCVObjectDetectClass *klass = GST_CV_OBJECT_DETECT_CLASS_GET_CLASS (self);

  g_return_val_if_fail (klass->detect != NULL, GST_FLOW_ERROR);

  if (self->ready)
    gst_cv_object_detect_default (self, buf, img);

  return GST_FLOW_OK;
}
