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

#ifndef __CV_OBJECT_INFO_UTILS_H__
#define __CV_OBJECT_INFO_UTILS_H__

#include <glib.h>

G_BEGIN_DECLS

/* Preset (sub) keys */
/* keys gtype */
#define GST_TYPE_CV_OBJECT_INFO_SUB_KEY_TYPE        G_TYPE_STRING
#define GST_TYPE_CV_OBJECT_INFO_SUB_KEY_LABEL       G_TYPE_STRING
#define GST_TYPE_CV_OBJECT_INFO_SUB_KEY_ELEMENT     G_TYPE_STRING
#define GST_TYPE_CV_OBJECT_INFO_SUB_KEY_INDEX       G_TYPE_UINT
/* keys */
#define GST_CV_OBJECT_INFO_SUB_KEY_TYPE             "type"
#define GST_CV_OBJECT_INFO_SUB_KEY_LABEL            "label"
#define GST_CV_OBJECT_INFO_SUB_KEY_ELEMENT          "element"
#define GST_CV_OBJECT_INFO_SUB_KEY_INDEX            "index"

/* values */
#define GST_CV_OBJECT_INFO_SUB_KEY_TYPE_ROI         "roi"

/* Preset (sub) params */
/* keys gtype */
#define GST_TYPE_CV_OBJECT_INFO_SUB_PARAM_INDEX     G_TYPE_UINT
/* keys */
#define GST_CV_OBJECT_INFO_SUB_PARAM_INDEX          "index"

guint gst_cv_object_info_generate_id ();

G_END_DECLS

#endif /* __CV_OBJECT_INFO_UTILS_H__ */
