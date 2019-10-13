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

#ifndef __GST_CV_DLIB_OBJECT_DETECT_HOG_H__
#define __GST_CV_DLIB_OBJECT_DETECT_HOG_H__

#include <graphene.h>

#include <gst/cv/gstcvobjectdetect.h>
#include <gst/cv/utils.h>

#include <dlib/opencv/cv_image.h>
#include <dlib/image_processing.h>
#include <dlib/data_io.h>

G_BEGIN_DECLS

/* #defines don't like whitespacey bits */
#define GST_TYPE_CV_DLIB_OBJECT_DETECT_HOG \
  (gst_cv_dlib_object_detect_hog_get_type())
#define GST_CV_DLIB_OBJECT_DETECT_HOG(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_CV_DLIB_OBJECT_DETECT_HOG,GstCVDlibObjectDetectHOG))
#define GST_CV_DLIB_OBJECT_DETECT_HOG_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_CV_DLIB_OBJECT_DETECT_HOG,GstCVDlibObjectDetectHOGClass))
#define GST_IS_CV_DLIB_OBJECT_DETECT_HOG(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_CV_DLIB_OBJECT_DETECT_HOG))
#define GST_IS_CV_DLIB_OBJECT_DETECT_HOG_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_CV_DLIB_OBJECT_DETECT_HOG))
#define GST_CV_DLIB_OBJECT_DETECT_HOG_CLASS_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS((obj),GST_TYPE_CV_DLIB_OBJECT_DETECT_HOG,GstCVDlibObjectDetectHOGClass))

typedef struct _GstCVDlibObjectDetectHOG  GstCVDlibObjectDetectHOG;
typedef struct _GstCVDlibObjectDetectHOGClass GstCVDlibObjectDetectHOGClass;

struct _GstCVDlibObjectDetectHOG
{
  GstCVObjectDetect element;
  gchar *model_location;
};

struct _GstCVDlibObjectDetectHOGClass
{
  GstCVObjectDetectClass parent_class;
  GSList * (* detect) (GstCVDlibObjectDetectHOG * self, cv::Mat & img);
};

GType gst_cv_dlib_object_detect_hog_get_type (void);

G_END_DECLS

#endif /* __GST_CV_DLIB_OBJECT_DETECT_HOG_H__ */
