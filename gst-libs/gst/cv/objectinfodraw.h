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

/* inclusion guard */
#ifndef __CV_OBJECT_INFO_DRAW_H__
#define __CV_OBJECT_INFO_DRAW_H__

#include <glib-object.h>
#include <graphene-gobject.h>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include "objectinfo.h"

G_BEGIN_DECLS

void
gst_cv_object_info_draw (GstCVObjectInfo * object_info, gdouble unscale_factor,
	cv::Mat img, cv::Scalar color);

G_END_DECLS

#endif /* __CV_OBJECT_INFO_DRAW_H__ */

