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

/**
 * SECTION:gstcvobjectinfomapmeta
 * @title: GstCVObjectInfoMapMeta
 * @short_description: Metadata class for holding ROI info.
 *
 * The GstCVObjectInfoMapMeta class contains a #GstCVObjectInfoMap which holds
 * information about an object (i.e. the ROI, region-of-interest for a given
 * object).
 */

#include "objectinfomapmeta.h"

GType
gst_cv_object_info_map_meta_api_get_type (void)
{
  static volatile GType type;
  static const gchar *tags[] = { NULL };

  if (g_once_init_enter (&type)) {
    GType _type =
        gst_meta_api_type_register ("GstCVObjectInfoMapMetaAPI", tags);
    g_once_init_leave (&type, _type);
  }

  return type;
}

gboolean
gst_cv_object_info_map_meta_init (GstMeta * meta, gpointer params,
    GstBuffer * buffer)
{
  return TRUE;
}

void
gst_cv_object_info_map_meta_free (GstMeta * meta, GstBuffer * buffer)
{
  GstCVObjectInfoMapMeta *self = (GstCVObjectInfoMapMeta *) meta;
  gst_mini_object_unref (GST_MINI_OBJECT (self->object_info_map));
}

const GstMetaInfo *
gst_cv_object_info_map_meta_get_info (void)
{
  static const GstMetaInfo *cv_object_info_map_meta_info = NULL;

  if (g_once_init_enter (&cv_object_info_map_meta_info)) {
    const GstMetaInfo *meta =
        gst_meta_register (GST_CV_OBJECT_INFO_MAP_META_API_TYPE,
        "GstCVObjectInfoMapMeta",
        sizeof (GstCVObjectInfoMapMeta), gst_cv_object_info_map_meta_init,
        gst_cv_object_info_map_meta_free, (GstMetaTransformFunction) NULL);
    g_once_init_leave (&cv_object_info_map_meta_info, meta);
  }
  return cv_object_info_map_meta_info;
}

GstCVObjectInfoMap *
gst_cv_object_info_map_meta_get_object_info_map (GstCVObjectInfoMapMeta * self)
{
  return self->object_info_map;
}

/**
 * gst_buffer_add_cv_object_info_map_meta:
 * @buffer: (transfer none): #GstBuffer to which info about an object should be
 * added.
 *
 * Attaches a #GstCVObjectInfoMap to a #GstBuffer.
 *
 * Returns: A pointer to the added #GstCVObjectInfoMapMeta if successful; %NULL
 * if unsuccessful.
 */
GstCVObjectInfoMapMeta *
gst_buffer_add_cv_object_info_map_meta (GstBuffer * buffer)
{
  GstCVObjectInfoMapMeta *meta;

  g_return_val_if_fail (GST_IS_BUFFER (buffer), NULL);

  meta =
      (GstCVObjectInfoMapMeta *) gst_buffer_add_meta (buffer,
      GST_CV_OBJECT_INFO_MAP_META_INFO, NULL);

  meta->object_info_map = gst_cv_object_info_map_new ();
  return meta;
}
