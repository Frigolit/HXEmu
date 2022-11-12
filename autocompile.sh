#!/bin/bash
cd `dirname $0`
BASEDIR="`pwd`"

CXX="`(which g++ || which clang++) 2>/dev/null`"
PIKE="`(which pike || which ./.local/bin/pike) 2>/dev/null`"
PKGCONF="`(which pkg-config) 2>/dev/null`"
PKGMGR="`(which apt) 2>/dev/null`"

if [ "${CXX}" == "" ]; then
	echo "No C++ compiler found - Please install a toolchain!"
	if [ "${PKGMGR}" == "apt" ]; then
		echo "For example: sudo apt install build-essential m4"
	fi
	exit 1
fi

if [ "${PIKE}" == "" ]; then
	PIKE_BUILDDIR="${BASEDIR}/.pike-build"
	PREFIXDIR="${BASEDIR}/.local/"

	#PIKE_SNAPSHOT="https://pike.lysator.liu.se/pub/pike/latest-stable/Pike-v8.0.1738.tar.gz"
	#NETTLE_SNAPSHOT="https://ftp.gnu.org/gnu/nettle/nettle-3.8.tar.gz"

	PIKE_SNAPSHOT="https://static.frigolit.net/archive/pike/8.0/Pike-v8.0.1738.tar.gz"
	NETTLE_SNAPSHOT="httpS://static.frigolit.net/archive/nettle/nettle-3.8.tar.gz"

	${PKGCONF} --with-path=${PREFIXDIR}/lib64/pkgconfig --with-path=${PREFIXDIR}/lib/pkgconfig nettle
	HAS_NETTLE="$?"

	echo "No Pike runtime found - Downloading and installing locally..."
	echo "Pike tarball URL: ${PIKE_SNAPSHOT}"
	if [ "$HAS_NETTLE" != "0" ]; then
		echo "Nettle tarball URL: ${NETTLE_SNAPSHOT}"
	fi
	echo "Installation target: ${PREFIXDIR}"

	echo ""
	echo "Press ENTER to attempt to install Pike..."
	read WAIT_FOR_ENTER

	if [ ! -d "${PIKE_BUILDDIR}" ]; then
		mkdir "${PIKE_BUILDDIR}" || exit 1
	fi

	if [ "$HAS_NETTLE" != "0" ]; then
		if [ ! -e "${PIKE_BUILDDIR}/nettle.tar.gz" ]; then
			echo "Downloading Nettle tarball..."
			wget -O "${PIKE_BUILDDIR}/nettle.tar.gz" "${NETTLE_SNAPSHOT}"
		fi

		if [ -d "${PIKE_BUILDDIR}/nettle" ]; then
			rm -rf "${PIKE_BUILDDIR}/nettle"
		fi

		mkdir "${PIKE_BUILDDIR}/nettle" || exit 1
		(
			cd "${PIKE_BUILDDIR}/nettle" || exit 1
			tar xf ../nettle.tar.gz --strip 1 || exit 1
			./configure --prefix="${PREFIXDIR}" || exit 1
			make || exit 1
			make install || exit 1
		) || exit 1
	fi

	if [ ! -e "${PIKE_BUILDDIR}/pike.tar.gz" ]; then
		echo "Downloading Pike tarball..."
		wget -O "${PIKE_BUILDDIR}/pike.tar.gz..tmp" "${PIKE_SNAPSHOT}" || exit 1
		mv "${PIKE_BUILDDIR}/pike.tar.gz..tmp" "${PIKE_BUILDDIR}/pike.tar.gz" || exit 1
	fi

	if [ -d "${PIKE_BUILDDIR}/pike" ]; then
		echo "Cleaning up old build directory..."
		rm -rf "${PIKE_BUILDDIR}/pike"
	fi

	mkdir "${PIKE_BUILDDIR}/pike" || exit 1
	cd "${PIKE_BUILDDIR}/pike"

	tar xf ../pike.tar.gz --strip 1 || exit 1

	echo "Removing unnecessary modules..."
	(
		cd src || exit 1
		rm -rf modules/_Ffmpeg
		rm -rf modules/DVB
		rm -rf modules/FSEvents
		rm -rf modules/Fuse
		rm -rf modules/Gz
		rm -rf modules/HTTPLoop
		rm -rf modules/Image
		rm -rf modules/_Image*
		rm -rf modules/Java
		rm -rf modules/Msql
		rm -rf modules/Mysql
		rm -rf modules/Odbc
		rm -rf modules/Oracle
		rm -rf modules/PDF
		rm -rf modules/Postgres
		rm -rf modules/SANE
		rm -rf modules/Yp
		rm -rf post_modules/BSON
		rm -rf post_modules/Bz2
		rm -rf post_modules/COM
		rm -rf post_modules/CritBit
		rm -rf post_modules/GSSAPI
		rm -rf post_modules/GL
		rm -rf post_modules/GLUT
		rm -rf post_modules/GTK1
		rm -rf post_modules/GTK2
		rm -rf post_modules/_Image*
		rm -rf post_modules/Kerberos
		rm -rf post_modules/_Sass
		rm -rf post_modules/SDL
		rm -rf post_modules/SQLite
		rm -rf post_modules/VCDiff
		rm -rf post_modules/ZXID
	)

	make reconfigure CONFIGUREARGS="--prefix=\"${PREFIXDIR}\" --with-site-prefixes=\"${PREFIXDIR}\" --without-machine-code --without-gdbm --without-mysql --without-zlib" || exit 1
	make || { echo "ERROR: Pike failed to compile!"; exit 1; }
	make install || { echo "ERROR: Pike failed to install!"; exit 1; }

	cd ${BASEDIR}
	PIKE="`which ./.local/bin/pike`" || { echo "ERROR: Cannot locate local Pike after install!"; exit 1; }

	${PIKE} -e "exit(0);" || { echo "ERROR: Locally installed Pike not working!"; exit 1; }
fi

echo "Running: ${PIKE} ./compile"
echo ""

exec ${PIKE} ./compile
