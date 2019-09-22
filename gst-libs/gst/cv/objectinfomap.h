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
#ifndef __CV_OBJECT_INFO_MAP_H__
#define __CV_OBJECT_INFO_MAP_H__

#include <glib.h>
#include <graphene.h>
#include <gst/gst.h>
#include <gst/gstminiobject.h>
#include "objectinfo.h"

G_BEGIN_DECLS
typedef void (* GstCVObjectInfoMapFunc) (guint index, GstCVObjectInfo * info, gpointer user_data);

typedef struct _GstCVObjectInfoMap GstCVObjectInfoMap;

GType gst_cv_object_info_map_get_type ();

GstCVObjectInfoMap * gst_cv_object_info_map_new ();


GstCVObjectInfo * gst_cv_object_info_map_lookup_object_info (
	GstCVObjectInfoMap * self, guint index, const gchar * target_element_name,
	GstCVObjectInfoTag tag);
void gst_cv_object_info_map_insert_object_info_at_index (GstCVObjectInfoMap * self,
	guint index, GstCVObjectInfo * info);
void gst_cv_object_info_map_foreach (GstCVObjectInfoMap * self, GstCVObjectInfoMapFunc func,
    gpointer user_data);


G_END_DECLS

#endif /* __CV_OBJECT_INFO_MAP_H__ */