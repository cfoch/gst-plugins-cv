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

typedef struct _GstCVObjectInfoMapFuncData GstCVObjectInfoMapFuncData;
struct _GstCVObjectInfoMapFuncData
{
  GstCVObjectInfoMapFunc foreach_func;
  gpointer user_data;
  guint index;
};

struct _GstCVObjectInfoMap {
  GstMiniObject mini_object;
  /* < private > */
  GHashTable *table;
};

GST_DEFINE_MINI_OBJECT_TYPE (GstCVObjectInfoMap, gst_cv_object_info_map);

static void
_gst_cv_object_info_map_free_table_value (GList * list)
{
  g_list_free_full (list, (GDestroyNotify) gst_mini_object_unref);
}

static void
_gst_cv_object_info_map_free (GstCVObjectInfoMap * self)
{
  g_return_if_fail (self != NULL);
  g_hash_table_unref (self->table);
}

/**
 * gst_cv_object_info_map_new:
 *
 * Allocates a new #GstCVObjectInfoMap.
 *
 * Returns: (transfer full): A newly-allocated #GstCVObjectInfoMap. Unref
 * with gst_cv_object_info_map_unref () when no longer needed.
 */
GstCVObjectInfoMap *
gst_cv_object_info_map_new ()
{
  GstCVObjectInfoMap *self;
  self = g_slice_new0 (GstCVObjectInfoMap);
  gst_mini_object_init (GST_MINI_OBJECT_CAST (self),
      GST_MINI_OBJECT_FLAG_LOCKABLE, gst_cv_object_info_map_get_type (),
      // (GstMiniObjectCopyFunction) gst_cv_object_info_map_copy,
      (GstMiniObjectCopyFunction) NULL,
      (GstMiniObjectDisposeFunction) NULL,
      (GstMiniObjectFreeFunction) _gst_cv_object_info_map_free);
  self->table = g_hash_table_new_full (g_direct_hash, g_direct_equal, NULL,
      (GDestroyNotify) _gst_cv_object_info_map_free_table_value);
  return self;
}

static GList *
cv_object_info_map_get_object_info_list_at_index (GstCVObjectInfoMap * self,
    guint index)
{
  return (GList *) g_hash_table_lookup (self->table, GINT_TO_POINTER (index));
}

GstCVObjectInfo *
gst_cv_object_info_map_lookup_object_info (GstCVObjectInfoMap * self,
    guint index, const gchar * target_element_name, GstCVObjectInfoTag tag)
{
  GList *list, *l;

  list = cv_object_info_map_get_object_info_list_at_index (self, index);

  for (l = g_list_last (list); l != NULL; l = g_list_previous (l)) {
    GstCVObjectInfo *object_info;
    GstCVObjectInfoTag current_tag;
    gchar *current_target_element_name;

    object_info = (GstCVObjectInfo *) l->data;
    current_tag = gst_cv_object_info_get_tag (object_info);
    current_target_element_name =
        gst_cv_object_info_get_target_element_name (object_info);

    if (g_strcmp0 (target_element_name, current_target_element_name) == 0 &&
        tag == current_tag) {
      g_free (current_target_element_name);
      return object_info;
    }
  }
  return NULL;
}

void
gst_cv_object_info_map_insert_object_info_at_index (GstCVObjectInfoMap * self,
    guint index, GstCVObjectInfo * info)
{
  GList *original_list, *new_list;

  original_list = cv_object_info_map_get_object_info_list_at_index (self, index);
  new_list = g_list_append (original_list, info);

  if (original_list == NULL)
    g_hash_table_insert (self->table, GINT_TO_POINTER (index), new_list);
}


static void
_list_foreach (GstCVObjectInfo * object_info, GstCVObjectInfoMapFuncData *func_data)
{
  func_data->foreach_func (func_data->index, object_info, func_data->user_data);
}

static void
_table_foreach (guint index, GList * object_info_list, GstCVObjectInfoMapFuncData * func_data)
{
  func_data->index = index;
  g_list_foreach (object_info_list, (GFunc) _list_foreach, func_data);
}

void
gst_cv_object_info_map_foreach (GstCVObjectInfoMap * self, GstCVObjectInfoMapFunc func,
    gpointer user_data)
{
  GstCVObjectInfoMapFuncData data;

  data.foreach_func = func;
  data.user_data = user_data;

  g_hash_table_foreach (self->table, (GHFunc) _table_foreach, &data);
}