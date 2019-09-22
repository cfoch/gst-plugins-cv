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

/* inclusion guard */
#ifndef __CV_OBJECT_INFO_H__
#define __CV_OBJECT_INFO_H__

#include <glib.h>
#include <gst/gst.h>
#include <gst/gstminiobject.h>

G_BEGIN_DECLS

typedef enum {
  GST_CV_OBJECT_INFO_TAG_UNDEFINED,
  GST_CV_OBJECT_INFO_TAG_ROI,
  /* GST_CV_OBJECT_INFO_TAG_LANDMARKS */
} GstCVObjectInfoTag;

GType gst_cv_object_info_tag_get_type (void);
#define GST_CV_OBJECT_INFO_TAG (gst_cv_object_info_tag_get_type ())

typedef struct _GstCVObjectInfo GstCVObjectInfo;
GType gst_cv_object_info_get_type ();

GstCVObjectInfo * gst_cv_object_info_new (GstElement * element,
    GstCVObjectInfoTag tag, GType v_type, gconstpointer v_boxed);

GstCVObjectInfoTag gst_cv_object_info_get_tag (GstCVObjectInfo * self);
gchar * gst_cv_object_info_get_target_element_name (GstCVObjectInfo * self);
GType gst_cv_object_info_get_value_type (GstCVObjectInfo * self);
gpointer gst_cv_object_info_get_value_boxed (GstCVObjectInfo * self);

G_END_DECLS

#endif /* __CV_OBJECT_INFO_H__ */