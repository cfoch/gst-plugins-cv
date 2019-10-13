/*
 * GStreamer Plugins CV Utils
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
#ifndef __GST_CV_UTILS_H__
#define __GST_CV_UTILS_H__

G_BEGIN_DECLS

GST_CV_API
void 		 g_file_read_to_byte_array_async  (GFile *             file,
											   void *  			   buffer,
											   gsize 			   count,
    								 		   GCancellable * 	   cancellable,
    								 		   GAsyncReadyCallback callback,
    										   gpointer            user_data);

GST_CV_API
GByteArray * g_file_read_to_byte_array_finish (GFile *        file,
    										   GAsyncResult * result,
    										   GError **      error);

G_END_DECLS

#endif /* __GST_CV_UTILS_H__ */
