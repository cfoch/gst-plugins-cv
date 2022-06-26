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
#ifndef __CV_OBJECT_INFO_MAP_H__
#define __CV_OBJECT_INFO_MAP_H__

#include <glib.h>
#include <gst/gst.h>
#include "objectinfo.h"

G_BEGIN_DECLS

#define GST_CV_OBJECT_INFO_KEY_NAME "cvobjectinfokey"

typedef GHashTable GstCVObjectInfoMap;

struct _GstCVObjectInfoMapIter {
  GHashTableIter iter;
  GstStructure *sub_key;
};

typedef struct _GstCVObjectInfoMapIter GstCVObjectInfoMapIter;


typedef void         (*GstCVObjectInfoMapFunc)                     (GstStructure *    key,
                                                                    GstCVObjectInfo * value,
                                                                    gpointer          user_data);

guint                gst_cv_object_info_map_key_hash               (gconstpointer key);

gboolean             gst_cv_object_info_key_equal                  (gconstpointer k1,
                                                                    gconstpointer k2);

void                 gst_cv_object_info_key_free                   (gpointer k);

gboolean             gst_cv_object_info_map_check_key              (GstStructure * key);

GstCVObjectInfoMap * gst_cv_object_info_map_new                    (void);

void                 gst_cv_object_info_map_destroy                (GstCVObjectInfoMap * self);

GstCVObjectInfoMap * gst_cv_object_info_map_copy                   (GstCVObjectInfoMap * self);

GstCVObjectInfoMap * gst_cv_object_info_map_copy_with_sub_key      (GstCVObjectInfoMap * self,
                                                                    GstStructure * sub_key);

gboolean             gst_cv_object_info_map_merge_with_sub_key     (GstCVObjectInfoMap * self,
                                                                    GstCVObjectInfoMap * other,
                                                                    GstStructure *       sub_key);

gboolean             gst_cv_object_info_map_insert                 (GstCVObjectInfoMap * self,
                                                                    GstStructure *       key,
                                                                    GstCVObjectInfo *    value);

GstCVObjectInfo *    gst_cv_object_info_map_lookup                 (GstCVObjectInfoMap * self,
                                                                    const GstStructure * key);

gboolean             gst_cv_object_info_map_remove                 (GstCVObjectInfoMap * self,
                                                                    const GstStructure * key);

void                 gst_cv_object_info_map_iter_init              (GstCVObjectInfoMapIter * iter,
                                                                    GstCVObjectInfoMap     * map);

gboolean             gst_cv_object_info_map_iter_next              (GstCVObjectInfoMapIter * iter,
                                                                    GstStructure **          key,
                                                                    GstCVObjectInfo **       value);

void                 gst_cv_object_info_map_iter_init_with_sub_key (GstCVObjectInfoMapIter * iter,
                                                                    GstCVObjectInfoMap *     map,
                                                                    GstStructure *           sub_key);

gboolean             gst_cv_object_info_map_iter_next_with_sub_key (GstCVObjectInfoMapIter * iter,
                                                                    GstStructure **          key,
                                                                    GstCVObjectInfo **       value);

void                 gst_cv_object_info_map_foreach                (GstCVObjectInfoMap *   self,
                                                                    GstCVObjectInfoMapFunc foreach_func,
                                                                    gpointer               user_data);

void                 gst_cv_object_info_map_foreach_with_sub_key   (GstCVObjectInfoMap *   self,
                                                                    GstStructure *         sub_key,
                                                                    GstCVObjectInfoMapFunc func,
                                                                    gpointer               user_data);

GSList *             gst_cv_object_info_map_get_keys_with_sub_key  (GstCVObjectInfoMap * self,
                                                                    GstStructure *       sub_key);

guint                gst_cv_object_info_map_get_size               (GstCVObjectInfoMap * self);

G_END_DECLS

#endif /* __CV_OBJECT_INFO_MAP_H__ */