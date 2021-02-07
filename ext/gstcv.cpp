/* GStreamer
 * Copyright (C) 2019 Fabian Orccon <cfoch.fabian@gmail.com>
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gst/gst.h>
#include "dlib/gstcvdlibobjectdetecthog.h"
#include "improc/gstcvscale.h"
#include "improc/gstcvdraw.h"
#include "utils/gstcvobjectcache.h"

static gboolean
cv_init (GstPlugin *plugin)
{
  gst_element_register (plugin, "cvdlibobjectdetecthog", GST_RANK_NONE,
      gst_cv_dlib_object_detect_hog_get_type ());

  gst_element_register (plugin, "cvdraw", GST_RANK_NONE,
      gst_cv_draw_get_type ());

  gst_element_register (plugin, "cvscale", GST_RANK_NONE,
      gst_cv_scale_get_type ());

  gst_element_register (plugin, "cvobjectcache", GST_RANK_NONE,
      gst_cv_object_cache_get_type ());

  return TRUE;
}

GST_PLUGIN_DEFINE (GST_VERSION_MAJOR, GST_VERSION_MINOR, cv,
    "GStreamer CV Plugins", cv_init, VERSION, "LGPL", GST_PACKAGE_NAME,
    GST_PACKAGE_ORIGIN)
