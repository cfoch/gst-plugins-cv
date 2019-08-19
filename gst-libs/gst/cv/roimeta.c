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

/**
 * SECTION:gstcvroimeta
 * @title: GstCVROIMeta
 * @short_description: Metadata class for holding ROI info.
 *
 * The GstCVROIMeta class contains information of the region of interest of a
 * detected object.
 */

#include "roimeta.h"

GType
gst_cv_roi_meta_api_get_type (void)
{
  static volatile GType type;
  static const gchar *tags[] = { NULL };

  if (g_once_init_enter (&type)) {
    GType _type = gst_meta_api_type_register ("GstCVROIMetaAPI", tags);
    g_once_init_leave (&type, _type);
  }
  return type;
}

gboolean
gst_cv_roi_meta_init (GstMeta * meta, gpointer params, GstBuffer * buffer)
{
  return TRUE;
}

void
gst_cv_roi_meta_free (GstMeta * meta, GstBuffer * buffer)
{
  /* Nothing to do */
}

const GstMetaInfo *
gst_cv_roi_meta_get_info (void)
{
  static const GstMetaInfo *cv_roi_meta_info = NULL;

  if (g_once_init_enter (&cv_roi_meta_info)) {
    const GstMetaInfo *meta =
        gst_meta_register (GST_CV_ROI_META_API_TYPE, "GstCVROIMeta",
        sizeof (GstCVROIMeta), gst_cv_roi_meta_init,
        gst_cv_roi_meta_free, (GstMetaTransformFunction) NULL);
    g_once_init_leave (&cv_roi_meta_info, meta);
  }
  return cv_roi_meta_info;
}

/**
 * gst_buffer_add_cv_roi_meta:
 * @buffer: (transfer none): #GstBuffer to which a ROI metadata should be added.
 * @roi: a pointer to #graphene_rect_t representing a region of interest.
 *
 * Attaches a ROI metadata to a #GstBuffer.
 *
 * Returns: A pointer to the added #GstCVROIMeta if successful; %NULL if
 * unsuccessful.
 */
GstCVROIMeta *
gst_buffer_add_cv_roi_meta (GstBuffer * buffer, graphene_rect_t * roi)
{
  GstCVROIMeta *meta;

  g_return_val_if_fail (GST_IS_BUFFER (buffer), NULL);

  meta = (GstCVROIMeta *) gst_buffer_add_meta (buffer, GST_CV_ROI_META_INFO,
      NULL);
  meta->roi = *roi;
  return meta;
}
