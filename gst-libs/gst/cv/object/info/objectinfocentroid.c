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

// graphene_point_t GST_CV_OBJECT_INFO_CENTROID_UNDEFINED =
// GRAPHENE_POINT_INIT (-1.f, -1.f);

static gboolean
_graphene_rect_get_centroid (GstCVObjectInfo * object_info,
    graphene_point_t * centroid)
{
  const GValue *internal_value;
  graphene_rect_t *r;

  if (!centroid)
    return TRUE;

  internal_value = gst_cv_object_info_get_value (object_info);
  r = (graphene_rect_t *) g_value_get_boxed (internal_value);
  graphene_rect_get_center (r, centroid);

  return TRUE;
}

/**
 * gst_cv_object_info_get_centroid:
 * @object_info: This #GstCVObjectInfo
 * @centroid: (nullable): The #graphene_point_t where to set the values to.
 * 
 * If the centroid is not NULL, sets the centroid values for the given
 * @object_info, in the case it can be calculated.
 *
 * Returns: TRUE if the centroid is calculable, otherwise FALSE.
 */
gboolean
gst_cv_object_info_get_centroid (GstCVObjectInfo * object_info,
    graphene_point_t * centroid)
{
  GType type = gst_cv_object_info_get_value_type (object_info);

  // *centroid = GST_CV_OBJECT_INFO_CENTROID_UNDEFINED;
  if (type == GRAPHENE_TYPE_RECT)
    return _graphene_rect_get_centroid (object_info, centroid);
  return FALSE;
}
