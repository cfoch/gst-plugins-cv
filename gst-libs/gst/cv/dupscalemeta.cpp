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
 * SECTION:gstcvdupscalemeta
 * @title: GstCVDupScaleMeta
 * @short_description: Metadata class for holding ROI info.
 *
 * The GstCVDupScaleMeta class contains information of the region of interest of a
 * detected object.
 */

#include "dupscalemeta.h"

GType
gst_cv_dup_scale_meta_api_get_type (void)
{
  static volatile GType type;
  static const gchar *tags[] =
  {
    NULL
  };

  if (g_once_init_enter (&type)) {
    GType _type = gst_meta_api_type_register ("GstCVDupScaleMetaAPI", tags);
    g_once_init_leave (&type, _type);
  }
  return type;
}

gboolean
gst_cv_dup_scale_meta_init (GstMeta *meta, gpointer params, GstBuffer *buffer)
{
  GstCVDupScaleMeta *dup_scale_meta_meta = (GstCVDupScaleMeta *) meta;
  dup_scale_meta_meta->scaled_frame = new cv::Mat;

  return TRUE;
}

void
gst_cv_dup_scale_meta_free (GstMeta *meta, GstBuffer *buffer)
{
  GstCVDupScaleMeta *dup_scale_meta_meta = (GstCVDupScaleMeta *) meta;
  delete dup_scale_meta_meta->scaled_frame;
}

const GstMetaInfo *
gst_cv_dup_scale_meta_get_info (void)
{
  static const GstMetaInfo *cv_dup_scale_meta_info = NULL;

  if (g_once_init_enter (&cv_dup_scale_meta_info)) {
    const GstMetaInfo *meta = gst_meta_register (
        GST_CV_DUP_SCALE_META_API_TYPE, "GstCVDupScaleMeta",
        sizeof (GstCVDupScaleMeta), gst_cv_dup_scale_meta_init,
        gst_cv_dup_scale_meta_free, (GstMetaTransformFunction) NULL);
    g_once_init_leave (&cv_dup_scale_meta_info, meta);
  }
  return cv_dup_scale_meta_info;
}

gdouble
gst_cv_dup_scale_meta_get_unscale_factor (GstCVDupScaleMeta *self)
{
  return self->unscale_factor;
}

cv::Mat
gst_cv_dup_scale_meta_get_scaled_frame (GstCVDupScaleMeta *self)
{
  return *self->scaled_frame;
}

/**
 * gst_buffer_add_cv_dup_scale:
 * @buffer: (transfer none): #GstBuffer to which the scaled frame should be set.
 * @scaled_frame: the OpenCV image containing the scald frame.
 * @unscale_factor: the factor to multiply against when mapping to the original frame.
 *
 * Attaches a scaled frame to the metadata of a #GstBuffer.
 *
 * Returns: A pointer to the added #GstCVDupScaleMeta if successful; %NULL if
 * unsuccessful.
 */
GstCVDupScaleMeta *
gst_buffer_add_cv_dup_scale_meta (GstBuffer *buffer, cv::Mat &original_frame,
    cv::Mat &scaled_frame)
{
  GstCVDupScaleMeta *meta;
  g_return_val_if_fail (GST_IS_BUFFER (buffer), NULL);

  meta = (GstCVDupScaleMeta *) gst_buffer_add_meta (buffer,
      GST_CV_DUP_SCALE_META_INFO, NULL);

  (*meta->scaled_frame) = scaled_frame;
  meta->unscale_factor = original_frame.cols / (gdouble) scaled_frame.cols;

  return meta;
}
