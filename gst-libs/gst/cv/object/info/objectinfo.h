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

#ifndef __CV_OBJECT_INFO_H__
#define __CV_OBJECT_INFO_H__

#include <glib.h>
#include <gst/gst.h>

G_BEGIN_DECLS

#define GST_CV_OBJECT_INFO_SUB_KEY_ROI  "roi"

typedef enum {
  GST_CV_OBJECT_INFO_TAG_UNDEFINED,
  GST_CV_OBJECT_INFO_TAG_ROI,
  /* GST_CV_OBJECT_INFO_TAG_LANDMARKS */
} GstCVObjectInfoTag;

struct _GstCVObjectInfo {
  GValue value;
  GstStructure *params;
};

typedef struct _GstCVObjectInfo GstCVObjectInfo;


GstCVObjectInfo * gst_cv_object_info_new             (const GValue * value);

GstCVObjectInfo * gst_cv_object_info_new_with_params (const GValue * value,
                                                      GstStructure * params);

const GValue *    gst_cv_object_info_get_value       (GstCVObjectInfo * self);

GType             gst_cv_object_info_get_value_type  (GstCVObjectInfo * self);

void              gst_cv_object_info_add_params      (GstCVObjectInfo * self,
                                                      GstStructure *    params);

GstStructure *    gst_cv_object_info_get_params      (GstCVObjectInfo * self);

GstCVObjectInfo * gst_cv_object_info_copy            (const GstCVObjectInfo * self);

void              gst_cv_object_info_free            (GstCVObjectInfo * self);

G_END_DECLS

#endif /* __CV_OBJECT_INFO_H__ */
