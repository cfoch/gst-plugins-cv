#include <gst/cv/object/info/objectinfo.h>

static void
test_new ()
{
  GstCVObjectInfo *info;
  const GValue *info_value;
  GValue value = G_VALUE_INIT;

  g_value_init (&value, G_TYPE_INT);
  g_value_set_int (&value, 100);

  info = gst_cv_object_info_new (&value);
  info_value = gst_cv_object_info_get_value (info);

  g_assert_false (info_value == &value);
  g_assert_cmpint (g_value_get_int (info_value), ==, g_value_get_int (&value));
  g_assert_null (gst_cv_object_info_get_params (info));

  gst_cv_object_info_free (info);
  g_value_unset (&value);
}

static void
test_new_invalid_args ()
{
  GstCVObjectInfo *info;
  GValue uninitialized_value;

  g_test_expect_message (G_LOG_DOMAIN, G_LOG_LEVEL_CRITICAL,
      "*assertion*G_IS_VALUE*failed");
  info = gst_cv_object_info_new (&uninitialized_value);
  g_assert_null (info);
}

static void
test_new_with_params ()
{
  GstCVObjectInfo *info;
  GValue value = G_VALUE_INIT;
  GstStructure *s;
  const GValue *info_value;

  g_value_init (&value, G_TYPE_INT);
  g_value_set_int (&value, 100);
  s = gst_structure_new_empty ("foo");

  info = gst_cv_object_info_new_with_params (&value, s);
  info_value = gst_cv_object_info_get_value (info);

  g_assert_false (info_value == &value);
  g_assert_cmpint (g_value_get_int (info_value), ==, g_value_get_int (&value));
  g_assert_true (gst_cv_object_info_get_params (info) == s);

  gst_cv_object_info_free (info);
  g_value_unset (&value);
}

static void
test_new_with_params_invalid_args ()
{
  GstCVObjectInfo *info;
  GValue uninitialized_value, good_value = G_VALUE_INIT;
  GstStructure *good_structure, *bad_structure;

  bad_structure = NULL;
  good_structure = gst_structure_new_empty ("foo");

  g_test_expect_message (G_LOG_DOMAIN, G_LOG_LEVEL_CRITICAL,
      "*assertion*G_IS_VALUE*failed");
  info = gst_cv_object_info_new_with_params (&uninitialized_value,
      good_structure);
  g_assert_null (info);

  gst_structure_free (good_structure);

  g_value_init (&good_value, G_TYPE_INT);
  g_value_set_int (&good_value, 100);
  g_test_expect_message (G_LOG_DOMAIN, G_LOG_LEVEL_CRITICAL,
      "*assertion*GST_IS_STRUCTURE*failed");
  info = gst_cv_object_info_new_with_params (&good_value, bad_structure);
  g_assert_null (info);

  g_value_unset (&good_value);
}

static void
test_add_params ()
{
  GstCVObjectInfo *info;
  GValue value = G_VALUE_INIT;
  GstStructure *st;

  g_value_init (&value, G_TYPE_INT);
  g_value_set_int (&value, 100);
  info = gst_cv_object_info_new (&value);

  st = gst_structure_new_empty ("foo");
  gst_cv_object_info_add_params (info, st);
  g_assert_true (st == gst_cv_object_info_get_params (info));

  g_test_expect_message (G_LOG_DOMAIN, G_LOG_LEVEL_CRITICAL,
      "*assertion*GST_IS_STRUCTURE*failed");
  gst_cv_object_info_add_params (info, NULL);
  g_assert_true (st == gst_cv_object_info_get_params (info));

  gst_cv_object_info_free (info);
  g_value_unset (&value);
}

static void
test_copy ()
{
  GstCVObjectInfo *info, *copy;
  GValue value = G_VALUE_INIT;
  const GValue *src_value, *copy_value;
  GstStructure *st, *st_copy;

  g_value_init (&value, G_TYPE_INT);
  g_value_set_int (&value, 100);
  st = gst_structure_new_empty ("foo");
  info = gst_cv_object_info_new_with_params (&value, st);
  copy = gst_cv_object_info_copy (info);

  src_value = gst_cv_object_info_get_value (info);
  copy_value = gst_cv_object_info_get_value (copy);
  g_assert_cmpint (G_VALUE_TYPE (src_value), ==, G_VALUE_TYPE (copy_value));
  g_assert_cmpint (g_value_get_int (src_value), ==,
      g_value_get_int (copy_value));

  st_copy = gst_cv_object_info_get_params (copy);
  g_assert_true (gst_structure_is_equal (st, st_copy));
}

int
main (int argc, char *argv[])
{
  g_test_init (&argc, &argv, NULL);

  g_test_add_func ("/cv/object/info/test_info_new", test_new);
  g_test_add_func ("/cv/object/info/test_info_new_invalid_args",
      test_new_invalid_args);
  g_test_add_func ("/cv/object/info/test_info_new_with_params",
      test_new_with_params);
  g_test_add_func ("/cv/object/info/test_info_new_with_params_invalid_args",
      test_new_with_params_invalid_args);
  g_test_add_func ("/cv/object/info/test_info_add_params", test_add_params);
  g_test_add_func ("/cv/object/info/test_info_copy", test_copy);

  return g_test_run ();
}
