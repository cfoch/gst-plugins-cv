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

typedef void (*GstCVObjectInfoMapCacheFunc) (guint64 key,
                                             GstCVObjectInfoMap * value,
                                             gpointer user_data);

GstCVObjectInfoMapCache * gst_cv_object_info_map_cache_new                       (guint max_size);

guint                     gst_cv_object_info_map_cache_get_size                  (GstCVObjectInfoMapCache * self);

guint                     gst_cv_object_info_map_cache_get_max_size              (GstCVObjectInfoMapCache * self);

void                      gst_cv_object_info_map_cache_destroy                   (GstCVObjectInfoMapCache * self);

gboolean                  gst_cv_object_info_map_cache_insert                    (GstCVObjectInfoMapCache * self,
                                                                                  guint64 frame_number,
                                                                                  GstCVObjectInfoMap * map);
                                                                                
gboolean                  gst_cv_object_info_map_cache_insert_with_sub_key       (GstCVObjectInfoMapCache * self,
                                                                                  guint64 frame_number,
                                                                                  GstCVObjectInfoMap * map,
                                                                                  GstStructure * sub_key);

GstCVObjectInfoMap *      gst_cv_object_info_map_cache_lookup                    (GstCVObjectInfoMapCache * self,
                                                                                  guint64 frame_number);

gboolean                  gst_cv_object_info_map_cache_extend_value_with_sub_key (GstCVObjectInfoMapCache * self,
                                                                                  guint64 frame_number,
                                                                                  GstCVObjectInfoMap * map,
                                                                                  GstStructure * sub_key);

void                      gst_cv_object_info_map_cache_foreach                   (GstCVObjectInfoMapCache * self,
                                                                                  GstCVObjectInfoMapCacheFunc func,
                                                                                  gpointer user_data);

G_END_DECLS

#endif /* __CV_OBJECT_INFO_MAP_CACHE_H__ */