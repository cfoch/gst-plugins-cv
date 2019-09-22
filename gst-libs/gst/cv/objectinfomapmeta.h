/* GStreamer
 * Copyright (C) <2015> British Broadcasting Corporation
 * Copyright (C) <2019> Cesar Fabian Orccon Chipana
 *   Author: Chris Bass <dash@rd.bbc.co.uk>
 *   Author: Cesar Fabian Orccon Chipana <cfoch.fabian@gmail.com>
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

#ifndef __GST_CV_OBJECT_INFO_MAP_META_H__
#define __GST_CV_OBJECT_INFO_MAP_META_H__

#include <gst/gst.h>
#include <graphene.h>
#include "objectinfomap.h"

G_BEGIN_DECLS

typedef struct _GstCVObjectInfoMapMeta GstCVObjectInfoMapMeta;

/**
 * GstCVObjectInfoMapMeta:
 * @meta: The parent #GstMeta.
 * @roi: A #graphene_rect_t representing a region of interest.
 *
 * Metadata type that describes a region of interest contained in a #GstBuffer.
 */
struct _GstCVObjectInfoMapMeta {
  GstMeta meta;

  GstCVObjectInfoMap *object_info_map;
};

GType gst_cv_object_info_map_meta_api_get_type (void);
#define GST_CV_OBJECT_INFO_MAP_META_API_TYPE (gst_cv_object_info_map_meta_api_get_type())

#define gst_buffer_get_cv_object_info_map_meta(b) \
    ((GstCVObjectInfoMapMeta*)gst_buffer_get_meta ((b), GST_CV_OBJECT_INFO_MAP_META_API_TYPE))

#define GST_CV_OBJECT_INFO_MAP_META_INFO (gst_cv_object_info_map_meta_get_info())

gboolean gst_cv_object_info_map_meta_init (GstMeta * meta, gpointer params,
    GstBuffer * buffer);

void gst_cv_object_info_map_meta_free (GstMeta * meta, GstBuffer * buffer);

const GstMetaInfo * gst_cv_object_info_map_meta_get_info (void);

GstCVObjectInfoMap * gst_cv_object_info_map_meta_get_object_info_map (GstCVObjectInfoMapMeta * self);

GstCVObjectInfoMapMeta * gst_buffer_add_cv_object_info_map_meta (GstBuffer * buffer);

G_END_DECLS

#endif /* __GST_CV_OBJECT_INFO_MAP_META_H__ */
