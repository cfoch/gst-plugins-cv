/* GStreamer OpenCV Library
 * Copyright (C) 2018 GStreamer developers
 * Copyright (C) 2019 Cesar Fabian Orccon Chipana <cfoch.fabian@gmail.com>
 *
 * opencv-prelude.h: prelude include header for gst-opencv library
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

#ifndef __GST_CV_PRELUDE_H__
#define __GST_CV_PRELUDE_H__

#include <gst/gst.h>

#ifndef GST_CV_API
# ifdef BUILDING_GST_CV
#  define GST_CV_API GST_API_EXPORT         /* from config.h */
# else
#  define GST_CV_API GST_API_IMPORT
# endif
#endif

#endif /* __GST_CV_PRELUDE_H__ */
