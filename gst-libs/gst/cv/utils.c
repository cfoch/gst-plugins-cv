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

#include <gio/gio.h>
#include <glib.h>

typedef struct {
  GTask *task;
  void * buffer;
  gsize count;
  GByteArray * gbarray;
} TempBufferInfo;

TempBufferInfo *
temp_buffer_info_new (GTask * task, void * buffer, gsize count)
{
  TempBufferInfo *info = g_new0 (TempBufferInfo, 1);
  info->task = task;
  info->buffer = buffer;
  info->count = count;
  info->gbarray = g_byte_array_new ();

  return info;
}

static void
input_stream_read_async_cb (GObject * source_object, GAsyncResult * res,
    gpointer data)
{
  GInputStream * stream = G_INPUT_STREAM (source_object);
  TempBufferInfo *info = (TempBufferInfo *) data;
  GTask *task = info->task;
  g_autoptr(GError) error = NULL;
  guint count_read;

  count_read = g_input_stream_read_finish (stream, res, &error);

  if (count_read > 0) {
    g_byte_array_append (info->gbarray, (guint8 *) info->buffer, count_read);
    // Recursively read next bytes.
    g_input_stream_read_async (stream, info->buffer, info->count,
        G_PRIORITY_DEFAULT, NULL, input_stream_read_async_cb, info);
  } else if (count_read == 0) {
    g_task_return_pointer (task, info->gbarray, NULL);
    g_free (info);
  } else {
    g_byte_array_free (info->gbarray, FALSE);
    g_free (info);
    g_task_return_error (task, error);
  }
}

static void
file_read_async_cb (GObject * source_object, GAsyncResult * res, gpointer data)
{
  GFile *file = G_FILE (source_object);
  TempBufferInfo *info = (TempBufferInfo *) data;
  GTask *task = info->task;
  g_autoptr(GError) error = NULL;
  GFileInputStream *file_stream = g_file_read_finish (file, res, &error);

  if (error) {
    g_byte_array_free (info->gbarray, FALSE);
    g_free (info);
    g_task_return_error (task, error);
    return;
  }

  g_input_stream_read_async (G_INPUT_STREAM (file_stream), info->buffer,
      info->count, G_PRIORITY_DEFAULT, NULL, input_stream_read_async_cb, info);
}

void
g_file_read_to_byte_array_async (GFile * file, void * buffer, gsize count,
    GCancellable * cancellable, GAsyncReadyCallback callback,
    gpointer user_data)
{
  GTask *task;
  TempBufferInfo *info;

  task = g_task_new ((gpointer) file, cancellable, callback, user_data);
  info = temp_buffer_info_new (task, buffer, count);
  g_file_read_async (file, G_PRIORITY_DEFAULT, NULL, file_read_async_cb, info); 
}

GByteArray *
g_file_read_to_byte_array_finish (GFile * file, GAsyncResult * result,
    GError ** error)
{
  g_return_val_if_fail (g_task_is_valid (result, file), NULL);

  return g_task_propagate_pointer (G_TASK (result), error);
}
