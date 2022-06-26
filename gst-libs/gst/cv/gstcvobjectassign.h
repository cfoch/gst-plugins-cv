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

#ifndef __GST_CV_ASSIGN_H__
#define __GST_CV_ASSIGN_H__

#include <gst/opencv/gstopencvvideofilter.h>
#include <gst/cv/cv-prelude.h>
#include <gst/cv/gstcvobjectdetect.h>
#include <gst/cv/dupscalemeta.h>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <graphene.h>
#include <graphene-gobject.h>
#include <gio/gio.h>

#include <gst/cv/object/info/objectinfocentroid.h>
#include <gst/cv/object/info/objectinfomapnamedcache.h>
#include <gst/cv/object/info/objectinfoutils.h>

G_BEGIN_DECLS

typedef struct _GstCVObjectAssignContext  GstCVObjectAssignContext;

#define GST_TYPE_CV_OBJECT_ASSIGN \
  (gst_cv_object_assign_get_type())
#define GST_CV_OBJECT_ASSIGN(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_CV_OBJECT_ASSIGN,GstCVObjectAssign))
#define GST_CV_OBJECT_ASSIGN_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_CV_OBJECT_ASSIGN,GstCVObjectAssignClass))
#define GST_IS_CV_ASSIGN_DETECT(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_CV_OBJECT_ASSIGN))
#define GST_IS_CV_ASSIGN_DETECT_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_CV_OBJECT_ASSIGN))
#define GST_CV_OBJECT_ASSIGN_CLASS_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS((obj),GST_TYPE_CV_OBJECT_ASSIGN,GstCVObjectAssignClass))

typedef struct _GstCVObjectAssign  GstCVObjectAssign;
typedef struct _GstCVObjectAssignClass GstCVObjectAssignClass;

typedef struct _GstCVObjectAssignPrivate GstCVObjectAssignPrivate;


struct _GstCVObjectAssign
{
  GstOpencvVideoFilter element;

  GstCVObjectAssignPrivate *priv;
};

struct _GstCVObjectAssignClass
{
  GstOpencvVideoFilterClass parent_class;

  void (* assign) (GstCVObjectAssign * self,
                   std::vector<std::vector<double>> cost_matrix,
                   std::vector<int> & assignment);
  double (* cost) (GstCVObjectAssign * self,
                   graphene_point_t current_centroid,
                   graphene_point_t previous_centroid);
};

GST_CV_API
GType    gst_cv_object_assign_get_type      (void);

GST_CV_API
gboolean gst_cv_object_assign_plugin_init   (GstPlugin * plugin);

G_END_DECLS

#endif /* __GST_CV_ASSIGN_H__ */
