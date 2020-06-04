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

#ifndef __GST_CV_OBJECT_INFO_MAP_CACHE_META_H__
#define __GST_CV_OBJECT_INFO_MAP_CACHE_META_H__

#include <gst/gst.h>
#include <graphene.h>
#include "objectinfomapcache.h"

G_BEGIN_DECLS

#define GST_CV_OBJECT_INFO_MAP_CACHE_META_API_TYPE (gst_cv_object_info_map_cache_meta_api_get_type())
#define gst_buffer_get_cv_object_info_map_cache_meta(b) \
    ((GstCVObjectInfoMapCacheMeta*)gst_buffer_get_meta ((b), GST_CV_OBJECT_INFO_MAP_CACHE_META_API_TYPE))
#define GST_CV_OBJECT_INFO_MAP_CACHE_META_INFO (gst_cv_object_info_map_cache_meta_get_info())


typedef struct _GstCVObjectInfoMapCacheMeta GstCVObjectInfoMapCacheMeta;

struct _GstCVObjectInfoMapCacheMeta {
  GstMeta meta;

  gchar *element_name;
  GstCVObjectInfoMapCache *object_info_map_cache;
};


GType                         gst_cv_object_info_map_cache_meta_api_get_type              (void);

gboolean                      gst_cv_object_info_map_cache_meta_init                      (GstMeta   * meta,
                                                                                           gpointer    params,
                                                                                           GstBuffer * buffer);

void                          gst_cv_object_info_map_cache_meta_free                      (GstMeta   * meta,
                                                                                           GstBuffer * buffer);

const GstMetaInfo *           gst_cv_object_info_map_cache_meta_get_info                  (void);

GstCVObjectInfoMapCache *     gst_cv_object_info_map_cache_meta_get_object_info_map_cache (GstCVObjectInfoMapCacheMeta * self);

GstCVObjectInfoMapCacheMeta * gst_buffer_add_cv_object_info_map_cache_meta                (GstBuffer   * buffer,
                                                                                           guint         cache_size,
                                                                                           const gchar * element_name);

G_END_DECLS

#endif /* __GST_CV_OBJECT_INFO_MAP_CACHE_META_H__ */
