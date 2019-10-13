#!/bin/sh
# 
# Check that the code follows a consistant code style
#
# Check for existence of indent, and error out if not present.
# On some *bsd systems the binary seems to be called gnunindent,
# so check for that first.
#
# Based on https://gitlab.freedesktop.org/gstreamer/common/
#

SCRIPT=$(realpath $0)
ROOT_DIR="$(git rev-parse --show-toplevel)"

check_command() {
  local program=$1
  if command -v $program &>/dev/null; then
    return 0
  fi
  return 1
}

check_toolbox() {
  return $(check_command toolbox)
}

check_uncrustify() {
  return $(check_command uncrustify)
}

check_gnu_indent() {
  local program=$1
  local version=`$program --version 2>/dev/null`

  if test "x$version" = "x"; then
    return 1
  fi
  return 0
}


is_flatpak() {
  test -f "/.flatpak-info"
  return $?
}

run_in_flatpak() {
  flatpak-spawn --host bash $@
}

if is_flatpak; then
  echo "Running outside flatpak. Please, ensure you run flatpak with --socket=session-bus."
  run_in_flatpak "$SCRIPT $@"
  exit $?
fi

INDENT_BIN_NAMES=("gnuindent" "gindent" "indent")
INDENT=
for prog_name in "${INDENT_BIN_NAMES[@]}"
do
  if check_gnu_indent $prog_name; then
    INDENT=$prog_name
    break
  fi
done

UNCRUSTIFY_BIN_NAME="uncrustify"
UNCRUSTIFY=
if check_uncrustify; then
  UNCRUSTIFY=$UNCRUSTIFY_BIN_NAME
fi

if test "x$INDENT" = "x" || test "x$UNCRUSTIFY" = "x"; then
  if check_toolbox; then
    echo "Running script in toolbox."
    toolbox run bash -c "$SCRIPT $@"
    exit $?
  fi

  echo "GstCV git pre-commit hook:"
  echo "Did not find the following tools:"
  if test "x$INDENT" = "x"; then
    echo " - gnu-indent."
  fi
  if test "x$UNCRUSTIFY" = "x"; then
    echo " - uncrustify."
  fi
  echo "Please install it before continuing."
  exit 1
fi


INDENT_PARAMETERS="--braces-on-if-line \
  --case-brace-indentation0 \
  --case-indentation2 \
  --braces-after-struct-decl-line \
  --line-length80 \
  --no-tabs \
  --cuddle-else \
  --dont-line-up-parentheses \
  --continuation-indentation4 \
  --honour-newlines \
  --tab-size8 \
  --indent-level2 \
  --leave-preprocessor-space"

UNCRUSTIFY_CONFIG_FILE="$ROOT_DIR/scripts/data/uncrustify.cfg"

run_indent() {
  local nf=$1
  local newfile=$2

  $INDENT ${INDENT_PARAMETERS} \
$nf -o $newfile 2>> /dev/null
  # FIXME: Call indent twice as it tends to do line-breaks
  # different for every second call.
  $INDENT ${INDENT_PARAMETERS} \
      $newfile 2>> /dev/null
}

run_uncrustify() {
  local nf=$1
  local newfile=$2
  $UNCRUSTIFY -c $UNCRUSTIFY_CONFIG_FILE -f $nf -o $newfile 2>> /dev/null
}

check_coding_style() {
  local nf=$1
  local newfile=$2

  if [[ ${nf: -2} == ".c" ]]; then
    run_indent $nf $newfile
  else
    run_uncrustify $nf $newfile
  fi
}

