/*
 * GStreamer Plugins CV
 * Copyright (C) 2005 Thomas Vander Stichele <thomas@apestaart.org>
 * Copyright (C) 2005 Ronald S. Bultje <rbultje@ronald.bitfreak.net>
 * Copyright (C) 2010 Sreerenj Balachandran <bsreerenj@gmail.com>
 * Copyright (C) 2019 Cesar Fabian Orccon Chipana <cfoch.fabian@gmail.com>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * Alternatively, the contents of this file may be used under the
 * GNU Lesser General Public License Version 2.1 (the "LGPL"), in
 * which case the following provisions apply instead of the ones
 * mentioned above:
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
#include "objectinfomapmeta.h"


struct _GstCVObjectDetectContext
{
  gint index;
  cv::Mat img;
  GstBuffer *buf;
  gdouble unscale_factor;
};

static void
gst_cv_object_detect_context_init (GstCVObjectDetectContext * ctx,
    GstCVObjectDetect * filter, cv::Mat img, GstBuffer * buf,
    gdouble unscale_factor)
{
  ctx->index = 0;
  ctx->img = img;
  ctx->buf = buf;
  ctx->unscale_factor = unscale_factor;
}

typedef struct
{
} GstCVObjectDetectPrivate;

GST_DEBUG_CATEGORY_STATIC (gst_cv_object_detect_debug);
#define GST_CAT_DEFAULT gst_cv_object_detect_debug

/* Default property values */
#define DEFAULT_PROP_ENABLED        TRUE
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
  PROP_ENABLED,
  PROP_DRAW
};

#define parent_class gst_cv_object_detect_parent_class
G_DEFINE_ABSTRACT_TYPE_WITH_PRIVATE (GstCVObjectDetect, gst_cv_object_detect,
    GST_TYPE_OPENCV_VIDEO_FILTER);

static void gst_cv_object_detect_set_property (GObject * object,
    guint prop_id, const GValue * value, GParamSpec * pspec);
static void gst_cv_object_detect_get_property (GObject * object,
    guint prop_id, GValue * value, GParamSpec * pspec);

static GstFlowReturn gst_cv_object_detect_transform_ip (GstOpencvVideoFilter
    * self, GstBuffer * buf, cv::Mat img);

static void
gst_cv_object_detect_finalize (GObject * obj)
{
  G_OBJECT_CLASS (parent_class)->finalize (obj);
}

static void
gst_cv_object_detect_class_init (GstCVObjectDetectClass * klass)
{
  GObjectClass *gobject_class;
  GstOpencvVideoFilterClass *gstopencvbasefilter_class;

  GST_DEBUG_CATEGORY_INIT (gst_cv_object_detect_debug,
      "cvobjectdetect", 0, "cvobjectdetect element");

  gobject_class = (GObjectClass *) klass;
  gobject_class->finalize =
      GST_DEBUG_FUNCPTR (gst_cv_object_detect_finalize);
  gstopencvbasefilter_class = (GstOpencvVideoFilterClass *) klass;

  gstopencvbasefilter_class->cv_trans_ip_func =
      gst_cv_object_detect_transform_ip;

  gobject_class->set_property = gst_cv_object_detect_set_property;
  gobject_class->get_property = gst_cv_object_detect_get_property;

  g_object_class_install_property (gobject_class, PROP_ENABLED,
      g_param_spec_boolean ("enabled", "enabled",
          "Whether detection is enabled.", DEFAULT_PROP_ENABLED,
          (GParamFlags) (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS)));

  g_object_class_install_property (gobject_class, PROP_DRAW,
      g_param_spec_boolean ("draw", "draw",
          "Wheter to draw the ROI of the detect objects.", DEFAULT_PROP_DRAW,
          (GParamFlags) (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS)));

  klass->detect = NULL;
}

static void
gst_cv_object_detect_init (GstCVObjectDetect * self)
{
  self->enabled = DEFAULT_PROP_ENABLED;
  self->draw = DEFAULT_PROP_DRAW;
  self->ready = FALSE;

  gst_opencv_video_filter_set_in_place (GST_OPENCV_VIDEO_FILTER_CAST (self),
      TRUE);
}

static void
gst_cv_object_detect_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec)
{
  GstCVObjectDetect *self = GST_CV_OBJECT_DETECT (object);

  switch (prop_id) {
    case PROP_ENABLED:
      self->enabled = g_value_get_boolean (value);
      break;
    case PROP_DRAW:
      self->draw = g_value_get_boolean (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gst_cv_object_detect_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec)
{
  GstCVObjectDetect *self = GST_CV_OBJECT_DETECT (object);

  switch (prop_id) {
    case PROP_ENABLED:
      g_value_set_boolean (value, self->enabled);
      break;
    case PROP_DRAW:
      g_value_set_boolean (value, self->draw);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
draw_bounding_box (cv::Mat & img, graphene_rect_t * r, gdouble & unscale_factor)
{
  cv::Rect box (r->origin.x * unscale_factor, r->origin.y * unscale_factor,
      r->size.width * unscale_factor, r->size.height * unscale_factor);
  cv::rectangle (img, box, DEFAULT_BOUNDING_BOX_COLOR);
}

void
gst_cv_object_detect_register_face (GstCVObjectDetect * self,
    graphene_rect_t * rectangle, gpointer user_data)
{
  GstCVObjectDetectContext *ctx = (GstCVObjectDetectContext *) user_data;
  GstCVObjectInfoMapMeta *meta;
  GstCVObjectInfoMap *map;
  GstCVObjectInfo *info;

  meta = (GstCVObjectInfoMapMeta *) (gst_buffer_get_meta (ctx->buf,
      GST_CV_OBJECT_INFO_MAP_META_API_TYPE));
  if (!meta)
    meta = gst_buffer_add_cv_object_info_map_meta (ctx->buf);

  map = gst_cv_object_info_map_meta_get_object_info_map (meta);
  info = gst_cv_object_info_new (GST_ELEMENT (self), GST_CV_OBJECT_INFO_TAG_ROI,
      GRAPHENE_TYPE_RECT, rectangle);

  gst_cv_object_info_map_insert_object_info_at_index (map, ctx->index, info);

  if (self->draw)
    draw_bounding_box (ctx->img, rectangle, ctx->unscale_factor);

  ctx->index++;
}

void
gst_cv_object_detect_default (GstCVObjectDetect * self, GstBuffer * buf,
    cv::Mat & img)
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
gst_cv_object_detect_transform_ip (GstOpencvVideoFilter * base,
    GstBuffer * buf, cv::Mat img)
{
  GstCVObjectDetect *self = GST_CV_OBJECT_DETECT (base);
  GstCVObjectDetectClass *klass = GST_CV_OBJECT_DETECT_CLASS_GET_CLASS (self);

  g_return_val_if_fail (klass->detect != NULL, GST_FLOW_ERROR);

  if (!self->enabled)
      return GST_FLOW_OK;

  if (self->ready)
    gst_cv_object_detect_default (self, buf, img);

  return GST_FLOW_OK;
}