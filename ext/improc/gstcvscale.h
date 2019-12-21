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

#ifndef __GST_CV_SCALE_H__
#define __GST_CV_SCALE_H__

#include <gst/opencv/gstopencvvideofilter.h>
#include <gst/cv/dupscalemeta.h>
#include <opencv2/imgproc.hpp>

G_BEGIN_DECLS

#define GST_TYPE_CV_SCALE \
  (gst_cv_scale_get_type())
#define GST_CV_SCALE(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_CV_SCALE,GstCVScale))
#define GST_CV_SCALE_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_CV_SCALE,GstCVScaleClass))
#define GST_IS_CV_SCALE(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_CV_SCALE))
#define GST_IS_CV_SCALE_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_CV_SCALE))
#define GST_CV_SCALE_CLASS_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS((obj),GST_TYPE_CV_SCALE,GstCVScaleClass))

typedef struct _GstCVScale  GstCVScale;
typedef struct _GstCVScaleClass GstCVScaleClass;

struct _GstCVScale
{
  GstOpencvVideoFilter element;

  gdouble scale_factor;
};

struct _GstCVScaleClass
{
  GstOpencvVideoFilterClass parent_class;
};

GType    gst_cv_scale_get_type    (void);

gboolean gst_cv_scale_plugin_init (GstPlugin * plugin);

G_END_DECLS

#endif /* __GST_CV_SCALE_H__ */
