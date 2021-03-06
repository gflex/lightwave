sbin_PROGRAMS = vmcad

idl_srcdir=$(top_srcdir)/vmca/idl

SUBDIRS = \
    common \
    config \
    plugin \
    rest-head \
    security

vmcad_SOURCES = \
    auth.c \
    dirsync.c \
    entrypoints.c \
    errormap.c \
    globals.c \
    init.c \
    main.c \
    rpc.c \
    rpcmemory.c \
    rpcserv.c \
    service.c \
    signal.c \
    state.c \
    thread.c \
    utils.c \
    vmcaservice.c \
    vmca_sstub.c

if REST_ENABLED

vmcad_SOURCES += \
    oidcutil.c \
    restauth.c \
    restnegauth.c \
    restserviceinit.c \
    restutil.c \
    vmcaHTTPCallBack.c \
    vmcaHTTPHandlers.c

endif

vmcad_CPPFLAGS = \
    -I$(top_srcdir)/vmca/include \
    -I$(top_srcdir)/vmca/include/public \
    -I$(top_srcdir)/vmca/service/include \
    -I$(top_srcdir)/vmca/service/common \
    -I$(top_srcdir)/vmca/service/config \
    -I$(top_srcdir)/vmafd/include/public \
    -I$(top_srcdir)/vmidentity/ssoclients/common/include/public \
    -I$(top_srcdir)/vmidentity/ssoclients/oidc/include/public \
    @DCERPC_INCLUDES@ \
    @LW_INCLUDES@ \
    @POPT_INCLUDES@ \
    @OPENSSL_INCLUDES@ \
    @JANSSON_INCLUDES@ \
    @COPENAPI_INCLUDES@ \
    @CRESTENGINE_INCLUDES@

vmcad_LDADD = \
    $(top_builddir)/vmca/service/common/libvmcasrvcommon.la \
    $(top_builddir)/vmca/service/config/libvmcasrvconfig.la \
    $(top_builddir)/vmca/service/rest-head/libvmcaresthead.la \
    $(top_builddir)/vmca/service/security/libvmcasecurity.la \
    $(top_builddir)/vmca/common/libcommon.la \
    $(top_builddir)/vmca/vmcadb/libvmcadb.la \
    $(top_builddir)/vmafd/client/libvmafdclient.la \
    $(top_builddir)/vmidentity/ssoclients/common/src/libssocommon.la \
    $(top_builddir)/vmidentity/ssoclients/oidc/src/libssooidc.la \
    @DCERPC_LIBS@ \
    @LWIO_LIBS@ \
    @SCHANNEL_LIBS@ \
    @LWMSG_LIBS@ \
    @LWRSUTILS_LIBS@ \
    @LWREG_LIBS@ \
    @LWBASE_LIBS@ \
    @CRYPTO_LIBS@ \
    @GSSAPI_LIBS@ \
    @LDAP_LIBS@ \
    @POPT_LIBS@ \
    @UUID_LIBS@ \
    @PTHREAD_LIBS@ \
    @JANSSON_LIBS@ \
    @COPENAPI_LIBS@ \
    @CRESTENGINE_LIBS@ \
    @DL_LIBS@

vmcad_LDFLAGS = \
    @OPENSSL_LDFLAGS@ \
    @LW_LDFLAGS@ \
    @POPT_LDFLAGS@ \
    @JANSSON_LDFLAGS@ \
    @COPENAPI_LDFLAGS@ \
    @CRESTENGINE_LDFLAGS@

CLEANFILES = \
    vmca_h.h \
    vmca_cstub.c \
    vmca_sstub.c

BUILT_SOURCES = vmca_h.h

vmca_h.h vmca_sstub.c: $(idl_srcdir)/vmca.idl
	$(IDL) $(IDLFLAGS) -keep c_source -header vmca_h.h -I$(idl_srcdir) $<
