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

#include "objectinfo.h"

struct _GstCVObjectInfo {
  GstMiniObject mini_object;
  /* < private > */
  GstCVObjectInfoTag tag;
  GstElement *target_element;
  GValue value;
  // GstCVObjectInfoDrawFunc draw;
  gpointer _gst_reserved[GST_PADDING];
};

GST_DEFINE_MINI_OBJECT_TYPE (GstCVObjectInfo, gst_cv_object_info);

static void
_gst_cv_object_info_free (GstCVObjectInfo * self)
{
  /* Nothing? */
}

/**
 * gst_cv_object_info_new:
 *
 * Allocates a new #GstCVObjectInfo.
 *
 * Returns: (transfer full): A newly-allocated #GstCVObjectInfo. Unref
 * with gst_cv_object_info_unref () when no longer needed.
 */
GstCVObjectInfo *
gst_cv_object_info_new (GstElement * element, GstCVObjectInfoTag tag,
    GType v_type, gconstpointer v_boxed)
{
  GstCVObjectInfo *self;
  self = g_slice_new0 (GstCVObjectInfo);
  gst_mini_object_init (GST_MINI_OBJECT_CAST (self),
      GST_MINI_OBJECT_FLAG_LOCKABLE, gst_cv_object_info_get_type (),
      // (GstMiniObjectCopyFunction) gst_cv_object_info_copy,
      (GstMiniObjectCopyFunction) NULL,
      (GstMiniObjectDisposeFunction) NULL,
      (GstMiniObjectFreeFunction) _gst_cv_object_info_free);

  self->target_element = element;
  g_value_unset (&self->value);
  g_value_init (&self->value, v_type);
  g_value_set_boxed (&self->value, &v_boxed);

  return self;
}

GstCVObjectInfoTag
gst_cv_object_info_get_tag (GstCVObjectInfo * self)
{
  return self->tag;
}

gchar *
gst_cv_object_info_get_target_element_name (GstCVObjectInfo * self)
{
  return gst_element_get_name (self->target_element);
}

GType
gst_cv_object_info_get_value_type (GstCVObjectInfo * self)
{
  return G_VALUE_TYPE (&self->value);
}

gpointer
gst_cv_object_info_get_value_boxed (GstCVObjectInfo * self)
{
  return g_value_get_boxed (&self->value);
}
