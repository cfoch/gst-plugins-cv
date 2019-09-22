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

#ifndef __GST_CV_DRAW_H__
#define __GST_CV_DRAW_H__

#include <gst/cv/object/info/objectinfomapmeta.h>
#include <gst/cv/object/info/objectinfodraw.h>
#include <gst/cv/object/info/objectinfoutils.h>
#include <gst/cv/object/info/objectinfocentroid.h>
#include <gst/cv/dupscalemeta.h>
#include <gst/opencv/gstopencvvideofilter.h>
#include <opencv2/core.hpp>
#include <graphene.h>

G_BEGIN_DECLS

#define GST_TYPE_CV_DRAW \
  (gst_cv_draw_get_type())
#define GST_CV_DRAW(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_CV_DRAW,GstCVDraw))
#define GST_CV_DRAW_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_CV_DRAW,GstCVDrawClass))
#define GST_IS_CV_DRAW(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_CV_DRAW))
#define GST_IS_CV_DRAW_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_CV_DRAW))
#define GST_CV_DRAW_CLASS_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS((obj),GST_TYPE_CV_DRAW,GstCVDrawClass))

typedef struct _GstCVDraw  GstCVDraw;
typedef struct _GstCVDrawClass GstCVDrawClass;
typedef struct _GstCVDrawPrivate GstCVDrawPrivate;

struct _GstCVDraw
{
  GstOpencvVideoFilter element;
  GstCVDrawPrivate *priv;
};

struct _GstCVDrawClass
{
  GstOpencvVideoFilterClass parent_class;
};

GType    gst_cv_draw_get_type    (void);

gboolean gst_cv_draw_plugin_init (GstPlugin * plugin);

G_END_DECLS

#endif /* __GST_CV_DRAW_H__ */