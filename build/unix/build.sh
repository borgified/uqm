#!/bin/sh
# Build script
# Copyright (c) 2002 Serge van den Boom
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

# This file contains functions for the general building procedure.
# You shouldn't need to change this file if your project changes.

# Read in the functions we need
. build/unix/build_functions

if [ -z "$BUILD_WORK" ]; then
	BUILD_WORK=.
	export BUILD_WORK
fi

# Read in the config settings that affect the build, if present.
# Don't reread for every dir when recursing.
if [ -r "$BUILD_WORK/build.vars" -a -z "$BUILD_COMMAND" ]; then
	. "$BUILD_WORK/build.vars"
fi

# Read in the Makeinfo file for the dir currently being processed
. "${BUILD_REC_PATH:=./}Makeinfo"

# If we're recursing, go on.
if [ -n "$BUILD_COMMAND" ]; then
	$BUILD_COMMAND
	exit $?
fi

for VAR in TARGETS "$BUILD_PROJECT_NAME" "$BUILD_PROJECT_OBJS"; do
	eval VALUE="\$$VAR"
	if [ -z "$VALUE" ]; then
		echo "$VAR needs to be defined in the top Makeinfo file"
		exit 1
	fi
done

##############################################
### Everything below is parsing user input ###

TOPDIR="$PWD"
export TOPDIR

OSNAME=`uname -s`
export OSNAME

if [ $# -lt 1 ]; then
	usage 1>&2
	exit 1;
fi

case "$1" in
	cleanall)
		build_cleanall
		exit $?
		;;
	distclean)
		build_distclean
		exit $?
		;;
esac

unset TARGET
for TEMP in $TARGETS; do
	if [ "$1" = "$TEMP" ]; then
		TARGET="$1"
		break
	fi
done
if [ -z "$TARGET" ]; then
	echo "Invalid target; choose one from:"
	echo "    $TARGETS"
	exit 1
fi
BUILD_PROJECT="$TARGET"
export TARGET BUILD_PROJECT COMPILE MKDEPEND ECHON
export "${BUILD_PROJECT}_CFLAGS" "${BUILD_PROJECT}_LDFLAGS" 

# Add trailing / from objs dir
eval ${BUILD_PROJECT}_OBJS=\${${BUILD_PROJECT}_OBJS%/}/
export "${BUILD_PROJECT}_OBJS"

# Load the configuration functions
. build/unix/build.config

if [ $# -lt 2 ]; then
	build_check_config
	build_check_dependencies
	build_compile
	exit $?
fi

case "$2" in
	clean)
		build_clean
		;;
	config)
		build_config
		build_process_config
		;;
	reprocess_config)
		build_reconfig
		;;
	depend)
		build_check_config
		build_rec_dependancies
		;;
	install)
		build_check_config
		build_check_dependencies
		build_check_compile
		build_install
		;;
	*)
		usage 1>&2
		exit 1;
		;;
esac


