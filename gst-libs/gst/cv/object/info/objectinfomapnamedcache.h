/*
 * GStreamer Plugins CV
 * Copyright (C) 2020 Cesar Fabian Orccon Chipana <cfoch.fabian@gmail.com>
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
#ifndef __CV_OBJECT_INFO_MAP_NAMED_CACHE_H__
#define __CV_OBJECT_INFO_MAP_NAMED_CACHE_H__

#include <glib.h>
#include <gst/gst.h>
#include "objectinfomapcache.h"

G_BEGIN_DECLS

typedef struct _GstCVObjectInfoMapNamedCache GstCVObjectInfoMapNamedCache;


GstCVObjectInfoMapNamedCache * gst_cv_object_info_map_named_cache_new         (void);

gboolean                       gst_cv_object_info_map_named_cache_insert      (GstCVObjectInfoMapNamedCache * self,
                                                                               const gchar * cache_id,
                                                                               GstCVObjectInfoMapCache * cache);

GstCVObjectInfoMapCache *      gst_cv_object_info_map_named_cache_lookup      (GstCVObjectInfoMapNamedCache * self,
                                                                               const gchar * cache_id);

GstCVObjectInfoMapNamedCache * gst_cv_object_info_map_named_cache_get_default (void);

void                           gst_cv_object_info_map_named_cache_unref       (GstCVObjectInfoMapNamedCache * self);

G_END_DECLS

#endif /* __CV_OBJECT_INFO_MAP_NAMED_CACHE_H__ */