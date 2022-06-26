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
#include "objectinfomap.h"

static void
gst_cv_combine_hash (guint * seed, const guint * hash)
{
  *seed ^= *hash + 0x9e3779b9 + (*seed << 6) + (*seed >> 2);
}

static gboolean
gst_cv_object_info_map_key_hash_foreach (GQuark field_id, const GValue * value,
    guint * hash)
{
  guint field_hash;
  guint value_hash;

  /* Field hash. */
  field_hash = g_str_hash (g_quark_to_string (field_id));

  /* Value hash. */
  switch (G_VALUE_TYPE (value)) {
    case G_TYPE_UINT:
    {
      gint int_value = g_value_get_uint (value);
      value_hash = g_direct_hash (GINT_TO_POINTER (int_value));
      break;
    }
      {
        gint int_value = g_value_get_int (value);
        value_hash = g_direct_hash (GINT_TO_POINTER (int_value));
        break;
      }
    case G_TYPE_INT64:
    {
      gint int_value = g_value_get_int64 (value);
      value_hash = g_int64_hash (GINT_TO_POINTER (int_value));
      break;
    }
    case G_TYPE_STRING:
    {
      const gchar *int_value = g_value_get_string (value);
      value_hash = g_int64_hash (GINT_TO_POINTER (int_value));
      break;
    }
    default:
      g_assert_not_reached ();
  }

  gst_cv_combine_hash (hash, &field_hash);
  gst_cv_combine_hash (hash, &value_hash);

  return TRUE;
}

guint
gst_cv_object_info_map_key_hash (gconstpointer key)
{
  guint hash = 0;
  GstStructure *st = GST_STRUCTURE (key);

  gst_structure_foreach (st,
      (GstStructureForeachFunc) gst_cv_object_info_map_key_hash_foreach, &hash);

  return hash;
}

gboolean
gst_cv_object_info_key_equal (gconstpointer k1, gconstpointer k2)
{
  return gst_structure_is_equal (GST_STRUCTURE (k1), GST_STRUCTURE (k2));
}

void
gst_cv_object_info_key_free (gpointer k)
{
  gst_structure_free (GST_STRUCTURE (k));
}

static gboolean
gst_cv_object_info_map_check_key_foreach_func (GQuark field_id,
    const GValue * value, gpointer user_data)
{
  GType type = G_VALUE_TYPE (value);

  return type == G_TYPE_STRING || type == G_TYPE_INT || type == G_TYPE_INT64
      || type == G_TYPE_UINT;
}

gboolean
gst_cv_object_info_map_check_key (GstStructure * key)
{
  return gst_structure_foreach (key,
      (GstStructureForeachFunc) gst_cv_object_info_map_check_key_foreach_func,
      NULL);
}

GstCVObjectInfoMap *
gst_cv_object_info_map_new_full (gboolean free_key_on_destroy,
    gboolean free_value_on_destroy)
{
  return g_hash_table_new_full (gst_cv_object_info_map_key_hash,
      gst_cv_object_info_key_equal,
      free_key_on_destroy ? gst_cv_object_info_key_free : NULL,
      free_value_on_destroy ? (GDestroyNotify) gst_cv_object_info_free : NULL);
}

GstCVObjectInfoMap *
gst_cv_object_info_map_new ()
{
  return gst_cv_object_info_map_new_full (TRUE, TRUE);
}

void
gst_cv_object_info_map_destroy (GstCVObjectInfoMap * self)
{
  g_hash_table_destroy (self);
}

static gboolean
gst_cv_object_info_map_insert_with_check (GstCVObjectInfoMap * self,
    GstStructure * key, GstCVObjectInfo * value, gboolean check)
{
  if (check) {
    g_return_val_if_fail (G_IS_VALUE (value), FALSE);
    g_return_val_if_fail (GST_IS_STRUCTURE (key), FALSE);
    g_return_val_if_fail (gst_cv_object_info_map_check_key (key), FALSE);
  }

  return g_hash_table_insert (self, key, value);
}

gboolean
gst_cv_object_info_map_insert (GstCVObjectInfoMap * self, GstStructure * key,
    GstCVObjectInfo * value)
{
  return gst_cv_object_info_map_insert_with_check (self, key, value, TRUE);
}

GstCVObjectInfo *
gst_cv_object_info_map_lookup (GstCVObjectInfoMap * self,
    const GstStructure * key)
{
  return g_hash_table_lookup (self, key);
}

gboolean
gst_cv_object_info_map_remove (GstCVObjectInfoMap * self,
    const GstStructure * key)
{
  return g_hash_table_remove (self, key);
}

void
gst_cv_object_info_map_iter_init (GstCVObjectInfoMapIter * iter,
    GstCVObjectInfoMap * map)
{
  g_hash_table_iter_init (&iter->iter, map);
  iter->sub_key = NULL;
}

gboolean
gst_cv_object_info_map_iter_next (GstCVObjectInfoMapIter * iter,
    GstStructure ** key, GstCVObjectInfo ** value)
{
  return g_hash_table_iter_next (&iter->iter, (gpointer *) key,
      (gpointer *) value);
}

void
gst_cv_object_info_map_iter_init_with_sub_key (GstCVObjectInfoMapIter * iter,
    GstCVObjectInfoMap * map, GstStructure * sub_key)
{
  g_hash_table_iter_init (&iter->iter, map);
  iter->sub_key = sub_key;
}

GstCVObjectInfoMap *
gst_cv_object_info_map_copy (GstCVObjectInfoMap * self)
{
  GstCVObjectInfoMap *copy;
  GstCVObjectInfoMapIter iter;
  GstStructure *key;
  GstCVObjectInfo *value;

  copy = gst_cv_object_info_map_new ();

  gst_cv_object_info_map_iter_init (&iter, self);
  while (gst_cv_object_info_map_iter_next (&iter, &key, &value)) {
    gst_cv_object_info_map_insert_with_check (copy,
        gst_structure_copy (key), gst_cv_object_info_copy (value), FALSE);
  }

  return copy;
}

GstCVObjectInfoMap *
gst_cv_object_info_map_copy_with_sub_key (GstCVObjectInfoMap * self,
    GstStructure * sub_key)
{
  GstCVObjectInfoMap *copy;
  GstCVObjectInfoMapIter iter;
  GstStructure *key;
  GstCVObjectInfo *value;

  copy = gst_cv_object_info_map_new ();

  gst_cv_object_info_map_iter_init_with_sub_key (&iter, self, sub_key);
  while (gst_cv_object_info_map_iter_next_with_sub_key (&iter, &key, &value)) {
    gst_cv_object_info_map_insert_with_check (copy,
        gst_structure_copy (key), gst_cv_object_info_copy (value), FALSE);
  }

  return copy;
}

/**
 * gst_cv_object_info_map_merge_with_sub_key:
 * @self: This #GstCVObjectInfoMap
 * @other: The target #GstCVObjectInfoMap to be inserted into @self
 *
 * Merges a copy of  @other #GstCVObjectInfoMap into @self. If both share keys,
 * the value of @other will replace the existing value in @self.
 *
 * Returns: FALSE if at least one value will be replaced. Otherwise, TRUE.
 */
gboolean
gst_cv_object_info_map_merge_with_sub_key (GstCVObjectInfoMap * self,
    GstCVObjectInfoMap * other, GstStructure * sub_key)
{
  GstCVObjectInfoMapIter iter;
  GstStructure *key;
  GstCVObjectInfo *value;
  gboolean ret = TRUE;

  gst_cv_object_info_map_iter_init_with_sub_key (&iter, other, sub_key);
  while (gst_cv_object_info_map_iter_next_with_sub_key (&iter, &key, &value)) {
    if (!gst_cv_object_info_map_insert_with_check (self,
            gst_structure_copy (key), gst_cv_object_info_copy (value), FALSE))
      ret = FALSE;
  }

  return ret;
}

gboolean
gst_cv_object_info_map_iter_next_with_sub_key (GstCVObjectInfoMapIter * iter,
    GstStructure ** key, GstCVObjectInfo ** value)
{
  g_return_val_if_fail (iter->sub_key != NULL, FALSE);

  while (gst_cv_object_info_map_iter_next (iter, key, value)) {
    if (gst_structure_is_subset (GST_STRUCTURE (*key), iter->sub_key))
      return TRUE;

    *key = NULL;
    *value = NULL;
  };
  return FALSE;
}

void
gst_cv_object_info_map_foreach (GstCVObjectInfoMap * self,
    GstCVObjectInfoMapFunc foreach_func, gpointer user_data)
{
  GstCVObjectInfoMapIter iter;
  GstStructure *key;
  GstCVObjectInfo *value;

  gst_cv_object_info_map_iter_init (&iter, self);
  while (gst_cv_object_info_map_iter_next (&iter, &key, &value))
    foreach_func (key, value, user_data);
}

void
gst_cv_object_info_map_foreach_with_sub_key (GstCVObjectInfoMap * self,
    GstStructure * sub_key, GstCVObjectInfoMapFunc func, gpointer user_data)
{
  GstCVObjectInfoMapIter iter;
  GstStructure *key;
  GstCVObjectInfo *value;

  gst_cv_object_info_map_iter_init_with_sub_key (&iter, self, sub_key);
  while (gst_cv_object_info_map_iter_next (&iter, &key, &value)) {
    if (!gst_structure_can_intersect (iter.sub_key, key))
      continue;

    func (key, value, user_data);
  }
}

GSList *
gst_cv_object_info_map_get_keys_with_sub_key (GstCVObjectInfoMap * self,
    GstStructure * sub_key)
{
  GSList *list = NULL;
  GstCVObjectInfoMapIter iter;
  GstStructure *key;
  GstCVObjectInfo *value;

  gst_cv_object_info_map_iter_init (&iter, self);
  while (gst_cv_object_info_map_iter_next (&iter, &key, &value)) {
    if (!gst_structure_is_subset (iter.sub_key, key))
      continue;

    list = g_slist_prepend (list, key);
  }

  return list;
}

guint
gst_cv_object_info_map_get_size (GstCVObjectInfoMap * self)
{
  return g_hash_table_size (self);
}
