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
#include "objectinfomapnamedcache.h"

struct _GstCVObjectInfoMapNamedCache
{
  GstMiniObject mini_object;
  GHashTable *cache;
};

GST_DEFINE_MINI_OBJECT_TYPE (GstCVObjectInfoMapNamedCache,
    gst_cv_object_info_map_named_cache);

void
gst_cv_object_info_map_named_cache_free (GstCVObjectInfoMapNamedCache * self)
{
  g_hash_table_destroy (self->cache);
  g_slice_free (GstCVObjectInfoMapNamedCache, self);
}

GstCVObjectInfoMapNamedCache *
gst_cv_object_info_map_named_cache_new ()
{
  GstCVObjectInfoMapNamedCache *self;

  self = g_slice_new0 (GstCVObjectInfoMapNamedCache);

  gst_mini_object_init (GST_MINI_OBJECT_CAST (self),
      GST_MINI_OBJECT_FLAG_LOCKABLE,
      gst_cv_object_info_map_named_cache_get_type (),
      (GstMiniObjectCopyFunction) NULL, (GstMiniObjectDisposeFunction) NULL,
      (GstMiniObjectFreeFunction) gst_cv_object_info_map_named_cache_free);

  self->cache = g_hash_table_new_full (g_str_hash, g_str_equal, g_free,
      (GDestroyNotify) gst_cv_object_info_map_cache_destroy);

  return self;
}

GstCVObjectInfoMapCache *
gst_cv_object_info_map_named_cache_lookup (GstCVObjectInfoMapNamedCache * self,
    const gchar * cache_id)
{
  return (GstCVObjectInfoMapCache *) g_hash_table_lookup (self->cache,
      cache_id);
}

gboolean
gst_cv_object_info_map_named_cache_insert (GstCVObjectInfoMapNamedCache * self,
    const gchar * cache_id, GstCVObjectInfoMapCache * cache)
{
  return g_hash_table_insert (self->cache, g_strdup (cache_id), cache);
}

static GstCVObjectInfoMapNamedCache *_NAMED_CACHE = NULL;

GstCVObjectInfoMapNamedCache *
gst_cv_object_info_map_named_cache_get_default ()
{
  if (_NAMED_CACHE == NULL)
    _NAMED_CACHE = gst_cv_object_info_map_named_cache_new ();

  return _NAMED_CACHE;
}

void
gst_cv_object_info_map_named_cache_unref (GstCVObjectInfoMapNamedCache * self)
{
  gint prev_cache_refcount = GST_MINI_OBJECT_REFCOUNT_VALUE (_NAMED_CACHE);

  gst_mini_object_unref (GST_MINI_OBJECT_CAST (self));

  if (self == _NAMED_CACHE && prev_cache_refcount == 1)
    _NAMED_CACHE = NULL;
}
