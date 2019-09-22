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

#include "objectinfocentroid.h"

graphene_point_t GST_CV_OBJECT_INFO_CENTROID_UNDEFINED =
GRAPHENE_POINT_INIT (-1.f, -1.f);

static void
_graphene_rect_get_centroid (GstCVObjectInfo * object_info,
    graphene_point_t * centroid)
{
  graphene_rect_t *r;

  r = (graphene_rect_t *) gst_cv_object_info_get_value_boxed (object_info);
  graphene_rect_get_center (r, centroid);
}

void
gst_cv_object_info_get_centroid (GstCVObjectInfo * object_info,
    graphene_point_t * centroid)
{
  GType type = gst_cv_object_info_get_value_type (object_info);

  *centroid = GST_CV_OBJECT_INFO_CENTROID_UNDEFINED;
  if (type == GRAPHENE_TYPE_RECT)
    _graphene_rect_get_centroid (object_info, centroid);
}
