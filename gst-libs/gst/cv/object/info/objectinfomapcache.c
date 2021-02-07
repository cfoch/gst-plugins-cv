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
  GstMiniObject mini_object;

  guint max_size;
  GHashTable *cache;
  GQueue insertion_order_queue;
};

GST_DEFINE_MINI_OBJECT_TYPE (GstCVObjectInfoMapCache,
    gst_cv_object_info_map_cache);

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

  gst_mini_object_init (GST_MINI_OBJECT_CAST (cache),
      GST_MINI_OBJECT_FLAG_LOCKABLE, gst_cv_object_info_map_cache_get_type (),
      (GstMiniObjectCopyFunction) NULL,
      (GstMiniObjectDisposeFunction) NULL,
      (GstMiniObjectFreeFunction) gst_cv_object_info_map_cache_destroy);

  cache->max_size = max_size;
  cache->cache = g_hash_table_new_full (g_int64_hash, g_int64_equal,
      gst_cv_object_info_map_cache_key_free,
      (GDestroyNotify) gst_cv_object_info_map_destroy);

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

void
gst_cv_object_info_map_cache_destroy (GstCVObjectInfoMapCache * self)
{
  g_queue_free (&self->insertion_order_queue);
  g_hash_table_destroy (self->cache);
  g_slice_free (GstCVObjectInfoMapCache, self);
}

gboolean
gst_cv_object_info_map_cache_insert (GstCVObjectInfoMapCache * self,
    guint64 frame_number, GstCVObjectInfoMap * map)
{
  return gst_cv_object_info_map_cache_insert_with_sub_key (self, frame_number,
      map, NULL);
}

gboolean
gst_cv_object_info_map_cache_insert_with_sub_key (GstCVObjectInfoMapCache *
    self, guint64 frame_number, GstCVObjectInfoMap * map,
    GstStructure * sub_key)
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
  if (sub_key)
    map_copy = gst_cv_object_info_map_copy_with_sub_key (map, sub_key);
  else
    map_copy = gst_cv_object_info_map_copy (map);
  g_queue_push_tail (&self->insertion_order_queue, key);

  return g_hash_table_insert (self->cache, key, map_copy);
}

GstCVObjectInfoMap *
gst_cv_object_info_map_cache_lookup (GstCVObjectInfoMapCache * self,
    guint64 frame_number)
{
  gpointer lookup_result;

  lookup_result = g_hash_table_lookup (self->cache, &frame_number);

  return lookup_result;
}

/**
 * gst_cv_object_info_map_cache_extend_value_with_sub_key:
 * @self: This #GstCVObjectInfoMapCache
 * @frame_number: The frame number
 * @map: The #GstCVObjectInfoMap to insert
 * @sub_key: A #GstStructure representing a sub key of the map key.
 * 
 * Inserts a copy of @map (filtered by its @sub_key) into a
 * #GstCVObjectInfoMapCache mapping it by the @frame_number. If a
 * @frame_number already exists, the existing mapped map will be merged
 * with @map.
 * 
 * Returns: FALSE if at least one value will be replaced. Otherwise, TRUE.
 */
gboolean
gst_cv_object_info_map_cache_extend_value_with_sub_key (GstCVObjectInfoMapCache
    * self, guint64 frame_number, GstCVObjectInfoMap * map,
    GstStructure * sub_key)
{
  GstCVObjectInfoMap *this_map;

  this_map = gst_cv_object_info_map_cache_lookup (self, frame_number);

  if (this_map == NULL)
    return gst_cv_object_info_map_cache_insert_with_sub_key (self,
        frame_number, map, sub_key);

  return gst_cv_object_info_map_merge_with_sub_key (this_map, map, sub_key);
}

void
gst_cv_object_info_map_cache_foreach (GstCVObjectInfoMapCache * self,
    GstCVObjectInfoMapCacheFunc func, gpointer user_data)
{
  GHashTableIter iter;
  guint64 *key;
  GstCVObjectInfoMap *value;

  /* TODO: Iter in order of sorted queue */
  g_hash_table_iter_init (&iter, self->cache);
  while (g_hash_table_iter_next (&iter, (gpointer *) & key,
          (gpointer *) & value))
    func (*key, value, user_data);
}
