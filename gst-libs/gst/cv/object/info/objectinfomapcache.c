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
#include "objectinfomapcache.h"

struct _GstCVObjectInfoMapCache
{
  guint max_size;
  GstStructure *sub_key_filter;
  GHashTable *cache;
  GQueue insertion_order_queue;
};

static gpointer
gst_cv_object_info_map_cache_key_new (guint64 * key)
{
  guint64 *key_ptr = g_slice_new (guint64);
  *key_ptr = *key;
  return (gpointer) key_ptr;
}

static void
gst_cv_object_info_map_cache_key_free (gpointer k)
{
  g_slice_free (guint64, (guint64 *) k);
}

GstCVObjectInfoMapCache *
gst_cv_object_info_map_cache_new (guint max_size)
{
  GstCVObjectInfoMapCache *cache;

  cache = g_slice_new0 (GstCVObjectInfoMapCache);
  cache->max_size = max_size;
  cache->sub_key_filter = NULL;
  cache->cache = g_hash_table_new_full (g_int64_hash, g_int64_equal,
      gst_cv_object_info_map_cache_key_free,
      (GDestroyNotify) gst_cv_object_info_map_destroy);

  return cache;
}

GstCVObjectInfoMapCache *
gst_cv_object_info_map_cache_new_with_sub_key (guint max_size,
    GstStructure * info_map_sub_key_filter)
{
  GstCVObjectInfoMapCache *cache;

  g_return_val_if_fail (GST_IS_STRUCTURE (info_map_sub_key_filter) &&
      gst_cv_object_info_map_check_key (info_map_sub_key_filter), FALSE);

  cache = gst_cv_object_info_map_cache_new (max_size);
  cache->sub_key_filter = gst_structure_copy (info_map_sub_key_filter);

  return cache;
}

guint
gst_cv_object_info_map_cache_get_size (GstCVObjectInfoMapCache * self)
{
  return g_hash_table_size (self->cache);
}

guint
gst_cv_object_info_map_cache_get_max_size (GstCVObjectInfoMapCache * self)
{
  return self->max_size;
}

GstStructure *
gst_cv_object_info_map_cache_get_sub_key (GstCVObjectInfoMapCache * self)
{
  if (self->sub_key_filter == NULL)
    return NULL;
  return gst_structure_copy (self->sub_key_filter);
}

void
gst_cv_object_info_map_cache_destroy (GstCVObjectInfoMapCache * self)
{
  if (self->sub_key_filter)
    gst_structure_free (self->sub_key_filter);
  g_queue_free (&self->insertion_order_queue);
  g_hash_table_destroy (self->cache);
}

gboolean
gst_cv_object_info_map_cache_insert (GstCVObjectInfoMapCache * self,
    guint64 frame_number, GstCVObjectInfoMap * map)
{
  GstCVObjectInfoMap *map_copy;
  gpointer key;

  if (gst_cv_object_info_map_cache_get_size (self) == self->max_size) {
    gpointer popped_key;

    /* Remove the least recently inserted inserted item to the cache. */
    popped_key = g_queue_pop_head (&self->insertion_order_queue);
    g_assert (g_hash_table_remove (self->cache, popped_key));
  }

  key = gst_cv_object_info_map_cache_key_new (&frame_number);
  if (self->sub_key_filter)
    map_copy =
        gst_cv_object_info_map_copy_with_sub_key (map, self->sub_key_filter);
  else
    map_copy = gst_cv_object_info_map_copy (map);
  g_queue_push_tail (&self->insertion_order_queue, key);

  return g_hash_table_insert (self->cache, key, map_copy);
}

GstCVObjectInfo *
gst_cv_object_info_map_cache_lookup (GstCVObjectInfoMapCache * self,
    guint64 frame_number)
{
  gpointer lookup_result;

  lookup_result = g_hash_table_lookup (self->cache, &frame_number);

  return lookup_result;
}
