# $NetBSD: Makefile,v 1.9 2021/03/07 13:45:08 taca Exp $

MODNAME=		json
CATEGORIES+=		textproc
COMMENT=		PHP extension for JSON serialization support

USE_LANGUAGES+=		c c++
USE_TOOLS+=		yacc

BUILD_DEPENDS+=		re2c>=0.13.0.4:../../devel/re2c

CONFIGURE_ARGS+=	--enable-${MODNAME}=shared

PHP_VERSIONS_ACCEPTED=	56 73 74

.include "../../lang/php/ext.mk"

MAKE_ENV+=		RE2C=re2c

.include "../../mk/bsd.pkg.mk"
