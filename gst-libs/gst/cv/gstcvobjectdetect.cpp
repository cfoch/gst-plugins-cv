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

struct _GstCVObjectDetectContext
{
  cv::Mat img;
};

static void
gst_cv_object_detect_context_init (GstCVObjectDetectContext *ctx,
    GstCVObjectDetect *filter, cv::Mat img)
{
  ctx->img = img;
}

typedef struct
{} GstCVObjectDetectPrivate;

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
  PROP_DRAW
};

#define parent_class gst_cv_object_detect_parent_class
G_DEFINE_ABSTRACT_TYPE_WITH_PRIVATE (GstCVObjectDetect, gst_cv_object_detect,
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

  g_object_class_install_property (gobject_class, PROP_DRAW,
      g_param_spec_boolean ("draw", "draw",
      "Wheter to draw the ROI of the detect objects.", DEFAULT_PROP_DRAW,
      (GParamFlags) (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS)));

  klass->detect = NULL;
}

static void
gst_cv_object_detect_init (GstCVObjectDetect *self)
{
  self->draw = DEFAULT_PROP_DRAW;
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
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
draw_bounding_box (cv::Mat &img, graphene_rect_t *r)
{
  cv::Rect box (r->origin.x, r->origin.y, r->size.width, r->size.height);
  cv::rectangle (img, box, DEFAULT_BOUNDING_BOX_COLOR);
}

void
gst_cv_object_detect_register_face (GstCVObjectDetect *self,
    graphene_rect_t *rectangle, GstCVObjectDetectContext *ctx)
{
  /* TODO: Communicate rectangle coordinates info. */
  if (self->draw)
    draw_bounding_box (ctx->img, rectangle);
}

static GstFlowReturn
gst_cv_object_detect_transform_ip (GstOpencvVideoFilter *base, GstBuffer *buf,
    cv::Mat img)
{
  GstCVObjectDetect *self = GST_CV_OBJECT_DETECT (base);
  GstCVObjectDetectClass *klass = GST_CV_OBJECT_DETECT_CLASS_GET_CLASS (self);

  g_return_val_if_fail (klass->detect != NULL, GST_FLOW_ERROR);

  if (self->ready) {
    GstCVObjectDetectContext ctx;

    gst_cv_object_detect_context_init (&ctx, self, img);
    klass->detect (self, img, &ctx);
  }

  return GST_FLOW_OK;
}