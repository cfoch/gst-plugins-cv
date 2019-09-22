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

#ifndef __GST_CV_OBJECT_DETECT_H__
#define __GST_CV_OBJECT_DETECT_H__

#include <gst/opencv/gstopencvvideofilter.h>
#include <gst/cv/cv-prelude.h>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <graphene.h>
#include <graphene-gobject.h>
#include <gio/gio.h>

G_BEGIN_DECLS

typedef struct _GstCVObjectDetectContext  GstCVObjectDetectContext;

#define GST_TYPE_CV_OBJECT_DETECT \
  (gst_cv_object_detect_get_type())
#define GST_CV_OBJECT_DETECT(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_CV_OBJECT_DETECT,GstCVObjectDetect))
#define GST_CV_OBJECT_DETECT_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_CV_OBJECT_DETECT,GstCVObjectDetectClass))
#define GST_IS_CV_OBJECT_DETECT(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_CV_OBJECT_DETECT))
#define GST_IS_CV_OBJECT_DETECT_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_CV_OBJECT_DETECT))
#define GST_CV_OBJECT_DETECT_CLASS_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS((obj),GST_TYPE_CV_OBJECT_DETECT,GstCVObjectDetectClass))

typedef struct _GstCVObjectDetect  GstCVObjectDetect;
typedef struct _GstCVObjectDetectClass GstCVObjectDetectClass;

struct _GstCVObjectDetect
{
  GstOpencvVideoFilter element;

  gboolean draw;
  gboolean ready;
};

struct _GstCVObjectDetectClass
{
  GstOpencvVideoFilterClass parent_class;
  void (* detect) (GstCVObjectDetect *        self,
                   cv::Mat &                  img,
                   GstCVObjectDetectContext * ctx);
};

GST_CV_API
GType    gst_cv_object_detect_get_type      (void);

GST_CV_API
gboolean gst_cv_object_detect_plugin_init   (GstPlugin * plugin);

GST_CV_API
void     gst_cv_object_detect_register_face (GstCVObjectDetect *       self,
                                             graphene_rect_t *         rectangle,
                                             GstCVObjectDetectContext *ctx);

G_END_DECLS

#endif /* __GST_CV_OBJECT_DETECT_H__ */
