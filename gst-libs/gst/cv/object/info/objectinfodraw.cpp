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

#include "objectinfodraw.h"

static void
_draw_graphene_rect (GstCVObjectInfo *object_info, gdouble unscale_factor,
    cv::Mat img, cv::Scalar color)
{
  const GValue *internal_value;
  graphene_rect_t *r;

  internal_value = gst_cv_object_info_get_value (object_info);
  r = (graphene_rect_t *) g_value_get_boxed (internal_value);

  cv::Rect box (r->origin.x * unscale_factor, r->origin.y * unscale_factor,
      r->size.width * unscale_factor, r->size.height * unscale_factor);
  cv::rectangle (img, box, color);
}

void
gst_cv_object_info_draw (GstCVObjectInfo *object_info, gdouble unscale_factor,
    cv::Mat img, cv::Scalar color)
{
  GType type = gst_cv_object_info_get_value_type (object_info);

  if (type == GRAPHENE_TYPE_RECT)
    _draw_graphene_rect (object_info, unscale_factor, img, color);
}
