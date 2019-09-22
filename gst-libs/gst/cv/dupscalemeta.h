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

#ifndef __GST_CV_DUP_SCALE_META_H__
#define __GST_CV_DUP_SCALE_META_H__

#include <gst/gst.h>
#include <opencv2/core.hpp>

G_BEGIN_DECLS

#define GST_CV_DUP_SCALE_META_API_TYPE (gst_cv_dup_scale_meta_api_get_type())
#define gst_buffer_get_cv_dup_scale_meta(b) \
    ((GstCVDupScaleMeta*)gst_buffer_get_meta ((b), GST_CV_DUP_SCALE_META_API_TYPE))
#define GST_CV_DUP_SCALE_META_INFO (gst_cv_dup_scale_meta_get_info())

typedef struct _GstCVDupScaleMeta GstCVDupScaleMeta;

/**
 * GstCVDupScaleMeta:
 * @meta: The parent #GstMeta.
 * @roi: A #cv::Mat containing the scaled frame.
 * @unscale_factor: The factor to use when mapping to the real size.
 *
 * Metadata type that describes a frame scaled to a different size.
 */
struct _GstCVDupScaleMeta {
  GstMeta meta;
  cv::Mat *scaled_frame;
  gdouble unscale_factor;
};


GType               gst_cv_dup_scale_meta_api_get_type       (void);

gboolean            gst_cv_dup_scale_meta_init               (GstMeta * meta,
                                                              gpointer  params,
                                                              GstBuffer * buffer);

void                gst_cv_dup_scale_meta_free               (GstMeta *   meta,
                                                              GstBuffer * buffer);

const GstMetaInfo * gst_cv_dup_scale_meta_get_info           (void);

gdouble             gst_cv_dup_scale_meta_get_unscale_factor (GstCVDupScaleMeta * self);

cv::Mat             gst_cv_dup_scale_meta_get_scaled_frame   (GstCVDupScaleMeta * self);

GstCVDupScaleMeta * gst_buffer_add_cv_dup_scale_meta         (GstBuffer * buffer,
                                                              cv::Mat &   original_frame,
                                                              cv::Mat &   scaled_frame);

G_END_DECLS

#endif /* __GST_CV_DUP_SCALE_META_H__ */
