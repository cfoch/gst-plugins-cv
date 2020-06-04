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
#ifndef __CV_OBJECT_INFO_MAP_CACHE_H__
#define __CV_OBJECT_INFO_MAP_CACHE_H__

#include <glib.h>
#include <gst/gst.h>
#include "objectinfomap.h"

G_BEGIN_DECLS

typedef struct _GstCVObjectInfoMapCache GstCVObjectInfoMapCache;


GstCVObjectInfoMapCache * gst_cv_object_info_map_cache_new              (guint max_size);

GstCVObjectInfoMapCache * gst_cv_object_info_map_cache_new_with_sub_key (guint max_size,
                                                                         GstStructure * info_map_sub_key_filter);

guint                     gst_cv_object_info_map_cache_get_size         (GstCVObjectInfoMapCache * self);

guint                     gst_cv_object_info_map_cache_get_max_size     (GstCVObjectInfoMapCache * self);

GstStructure *            gst_cv_object_info_map_cache_get_sub_key      (GstCVObjectInfoMapCache * self);

void                      gst_cv_object_info_map_cache_destroy          (GstCVObjectInfoMapCache * self);

gboolean                  gst_cv_object_info_map_cache_insert           (GstCVObjectInfoMapCache * self,
                                                                         guint64 frame_number,
                                                                         GstCVObjectInfoMap * map);

GstCVObjectInfo *         gst_cv_object_info_map_cache_lookup           (GstCVObjectInfoMapCache * self,
                                                                         guint64 frame_number);

G_END_DECLS

#endif /* __CV_OBJECT_INFO_MAP_CACHE_H__ */