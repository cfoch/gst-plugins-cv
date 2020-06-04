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

GstCVObjectInfo *
gst_cv_object_info_new (const GValue * value)
{
  GstCVObjectInfo *self;
  g_return_val_if_fail (G_IS_VALUE (value), NULL);

  /* new0 initializes the underlying GValue to 0 */
  self = g_slice_new0 (GstCVObjectInfo);

  g_value_init (&self->value, G_VALUE_TYPE (value));
  g_value_copy (value, &self->value);
  self->params = NULL;

  return self;
}

GstCVObjectInfo *
gst_cv_object_info_new_with_params (const GValue * value, GstStructure * params)
{
  GstCVObjectInfo *self;
  g_return_val_if_fail (G_IS_VALUE (value), NULL);
  g_return_val_if_fail (GST_IS_STRUCTURE (params), NULL);

  self = gst_cv_object_info_new (value);
  gst_cv_object_info_add_params (self, params);

  return self;
}

void
gst_cv_object_info_add_params (GstCVObjectInfo * self, GstStructure * params)
{
  g_return_if_fail (GST_IS_STRUCTURE (params));

  g_clear_pointer (&self->params, gst_structure_free);
  self->params = params;
}

GstStructure *
gst_cv_object_info_get_params (GstCVObjectInfo * self)
{
  return self->params;
}

const GValue *
gst_cv_object_info_get_value (GstCVObjectInfo * self)
{
  return &self->value;
}

GstCVObjectInfo *
gst_cv_object_info_copy (const GstCVObjectInfo * self)
{
  GstCVObjectInfo *copy;

  if (self->params) {
    copy = gst_cv_object_info_new_with_params (&self->value,
        gst_structure_copy (self->params));
  } else {
    copy = gst_cv_object_info_new (&self->value);
  }

  return copy;
}

void
gst_cv_object_info_free (GstCVObjectInfo * self)
{
  g_value_unset (&self->value);
  g_clear_pointer (&self->params, gst_structure_free);
  g_slice_free (GstCVObjectInfo, self);
}
