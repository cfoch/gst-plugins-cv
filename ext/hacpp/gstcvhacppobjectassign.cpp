/*
 * GStreamer Plugins CV
 * Copyright (C) 2021 Cesar Fabian Orccon Chipana <cfoch.fabian@gmail.com>
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
 * SECTION:element-cvhacppobjectassign
 *
 * cvhacppobjectassign detects objects following the method
 * Histograms of Oriented Gradients for Human Detection by
 * Navneet Dalal and Bill Triggs.
 *
 * <refsect2>
 * <title>Example launch line</title>
 * |[
 * gst-launch-1.0 v4l2src ! videoconvert ! cvhacppobjectassign model-location=model.dat ! videoconvert ! xvimagesink
 * ]|
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include "gstcvhacppobjectassign.h"

typedef struct
{
  /* nothing */
} GstCVHacppObjectAssignPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (GstCVHacppObjectAssign, gst_cv_hacpp_object_assign,
    GST_TYPE_CV_OBJECT_ASSIGN);

GST_DEBUG_CATEGORY_STATIC (gst_cv_hacpp_object_assign_debug);
#define GST_CAT_DEFAULT gst_cv_hacpp_object_assign_debug

#define DEFAULT_PYRAMID_DOWN    6

enum
{
  LAST_SIGNAL
};

enum
{
  PROP_0,
  PROP_MODEL_LOCATION
};

#define parent_class gst_cv_hacpp_object_assign_parent_class

static GstStaticPadTemplate sink_factory = GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK, GST_PAD_ALWAYS, GST_STATIC_CAPS (GST_VIDEO_CAPS_MAKE (
        "RGB")));

static GstStaticPadTemplate src_factory = GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC, GST_PAD_ALWAYS, GST_STATIC_CAPS (GST_VIDEO_CAPS_MAKE ("RGB")));

static void gst_cv_hacpp_object_assign_assign (GstCVObjectAssign *self,
    std::vector<std::vector<double> > cost_matrix,
    std::vector<int> &assignment);

static void
gst_cv_hacpp_object_assign_class_init (GstCVHacppObjectAssignClass *klass)
{
  GstCVObjectAssignClass *gstcvobjectassign_class;
  GstElementClass *element_class = GST_ELEMENT_CLASS (klass);

  GST_DEBUG_CATEGORY_INIT (gst_cv_hacpp_object_assign_debug,
      "cvhacppobjectassign", 0, "Template cvhacppobjectassign");

  gstcvobjectassign_class = (GstCVObjectAssignClass *) klass;
  gstcvobjectassign_class->assign = gst_cv_hacpp_object_assign_assign;

  gst_element_class_set_static_metadata (element_class, "cvhacppobjectassign",
      "Filter/Effect/Video", "Assign objects using the Hungarian method",
      "Cesar Fabian Orccon Chipana <cfoch.fabian@gmail.com>");

  gst_element_class_add_static_pad_template (element_class, &src_factory);
  gst_element_class_add_static_pad_template (element_class, &sink_factory);
}

static void
gst_cv_hacpp_object_assign_init (GstCVHacppObjectAssign *self)
{
  GstCVHacppObjectAssignPrivate *priv =
      (GstCVHacppObjectAssignPrivate *)
      gst_cv_hacpp_object_assign_get_instance_private (
      self);

  gst_opencv_video_filter_set_in_place (GST_OPENCV_VIDEO_FILTER_CAST (self),
      TRUE);
}

static void
gst_cv_hacpp_object_assign_assign (GstCVObjectAssign *self,
    std::vector<std::vector<double> > cost_matrix, std::vector<int> &assignment)
{
  HungarianAlgorithm hungarian;

  hungarian.Solve (cost_matrix, assignment);
}
