LOCAL_PATH := $(call my-dir)

# Setup KnShim related stuff
include $(CLEAR_VARS)
LOCAL_MODULE := yip-prebuilt
LOCAL_SRC_FILES := yip/$(TARGET_ARCH_ABI)/libYipLoader.so
include $(PREBUILT_SHARED_LIBRARY)

# This is the setup for YOUR project!
include $(CLEAR_VARS)

LOCAL_ARM_MODE  := arm

# Your module's name
LOCAL_MODULE    := Katie.smashhit

# The source files for your module. Don't remove mod_info.c; it's required!
LOCAL_SRC_FILES := KatieMod/mod_info.c \
	KatieMod/main.c \
	KatieMod/util.c \
	KatieMod/knshim.c \
	KatieMod/builtin/antitamper.c \
	KatieMod/builtin/asset.c \
	KatieMod/builtin/script.c \
	KatieMod/builtin/log.c \
	KatieMod/builtin/patching.c \
	KatieMod/builtin/http.c \
	KatieMod/builtin/string.c \
	KatieMod/builtin/system.c \
	KatieMod/builtin/reg.c \
	KatieMod/builtin/files.c \
	KatieMod/builtin/gamectl_smashhit.c \
	KatieMod/builtin/overlay.c \
	KatieMod/builtin/shaders.c \
	KatieMod/builtin/input.c \
	KatieMod/builtin/draw.c \
	KatieMod/builtin/pack.c \
	KatieMod/builtin/isc.c \
	KatieMod/builtin/shutdown.c \
	KatieMod/lua/loader.c \
	KatieMod/extern/miniz.c

# Link against any extra libraries you might need here
LOCAL_LDLIBS     := -llog -landroid -lGLESv2

# Link against YipLoader itself
LOCAL_SHARED_LIBRARIES := yip-prebuilt mbedtls

# Include YipLoader's headers
LOCAL_C_INCLUDES := shim/extern jni/mbedtls/mbedtls
LOCAL_C_INCLUDES += $(LOCAL_PATH)/yip

# Consider providing C flags
LOCAL_CFLAGS     := -DHTTP_ENABLE_MBEDTLS

include $(BUILD_SHARED_LIBRARY)

## 
## Build MbedTLS
## 

include $(CLEAR_VARS)

LOCAL_ARM_MODE  := arm
LOCAL_MODULE    := mbedtls
LOCAL_SRC_FILES += mbedtls/mbedtls/library/block_cipher.c \
	mbedtls/mbedtls/library/psa_crypto.c \
	mbedtls/mbedtls/library/pkcs7.c \
	mbedtls/mbedtls/library/pk_ecc.c \
	mbedtls/mbedtls/library/psa_crypto_mac.c \
	mbedtls/mbedtls/library/des.c \
	mbedtls/mbedtls/library/pkwrite.c \
	mbedtls/mbedtls/library/md.c \
	mbedtls/mbedtls/library/pkparse.c \
	mbedtls/mbedtls/library/x509write_crt.c \
	mbedtls/mbedtls/library/sha1.c \
	mbedtls/mbedtls/library/bignum_core.c \
	mbedtls/mbedtls/library/ssl_tls12_client.c \
	mbedtls/mbedtls/library/ssl_ciphersuites.c \
	mbedtls/mbedtls/library/ctr_drbg.c \
	mbedtls/mbedtls/library/entropy_poll.c \
	mbedtls/mbedtls/library/dhm.c \
	mbedtls/mbedtls/library/ccm.c \
	mbedtls/mbedtls/library/padlock.c \
	mbedtls/mbedtls/library/aesce.c \
	mbedtls/mbedtls/library/ssl_client.c \
	mbedtls/mbedtls/library/hkdf.c \
	mbedtls/mbedtls/library/x509_create.c \
	mbedtls/mbedtls/library/ssl_cache.c \
	mbedtls/mbedtls/library/psa_crypto_ffdh.c \
	mbedtls/mbedtls/library/mps_reader.c \
	mbedtls/mbedtls/library/gcm.c \
	mbedtls/mbedtls/library/bignum_mod_raw.c \
	mbedtls/mbedtls/library/psa_crypto_driver_wrappers_no_static.c \
	mbedtls/mbedtls/library/ecp.c \
	mbedtls/mbedtls/library/lms.c \
	mbedtls/mbedtls/library/rsa.c \
	mbedtls/mbedtls/library/version_features.c \
	mbedtls/mbedtls/library/x509.c \
	mbedtls/mbedtls/library/asn1parse.c \
	mbedtls/mbedtls/library/ssl_tls13_client.c \
	mbedtls/mbedtls/library/ssl_tls13_keys.c \
	mbedtls/mbedtls/library/sha3.c \
	mbedtls/mbedtls/library/entropy.c \
	mbedtls/mbedtls/library/oid.c \
	mbedtls/mbedtls/library/nist_kw.c \
	mbedtls/mbedtls/library/pem.c \
	mbedtls/mbedtls/library/bignum.c \
	mbedtls/mbedtls/library/bignum_mod.c \
	mbedtls/mbedtls/library/ssl_tls13_generic.c \
	mbedtls/mbedtls/library/ssl_msg.c \
	mbedtls/mbedtls/library/psa_crypto_rsa.c \
	mbedtls/mbedtls/library/psa_crypto_hash.c \
	mbedtls/mbedtls/library/sha512.c \
	mbedtls/mbedtls/library/aria.c \
	mbedtls/mbedtls/library/constant_time.c \
	mbedtls/mbedtls/library/chachapoly.c \
	mbedtls/mbedtls/library/asn1write.c \
	mbedtls/mbedtls/library/ssl_ticket.c \
	mbedtls/mbedtls/library/psa_crypto_client.c \
	mbedtls/mbedtls/library/x509_csr.c \
	mbedtls/mbedtls/library/net_sockets.c \
	mbedtls/mbedtls/library/memory_buffer_alloc.c \
	mbedtls/mbedtls/library/cmac.c \
	mbedtls/mbedtls/library/psa_crypto_slot_management.c \
	mbedtls/mbedtls/library/threading.c \
	mbedtls/mbedtls/library/cipher.c \
	mbedtls/mbedtls/library/ssl_tls13_server.c \
	mbedtls/mbedtls/library/lmots.c \
	mbedtls/mbedtls/library/aes.c \
	mbedtls/mbedtls/library/chacha20.c \
	mbedtls/mbedtls/library/psa_crypto_se.c \
	mbedtls/mbedtls/library/psa_util.c \
	mbedtls/mbedtls/library/ssl_tls12_server.c \
	mbedtls/mbedtls/library/ecdh.c \
	mbedtls/mbedtls/library/psa_crypto_aead.c \
	mbedtls/mbedtls/library/base64.c \
	mbedtls/mbedtls/library/ssl_tls.c \
	mbedtls/mbedtls/library/x509_crl.c \
	mbedtls/mbedtls/library/md5.c \
	mbedtls/mbedtls/library/mps_trace.c \
	mbedtls/mbedtls/library/ecdsa.c \
	mbedtls/mbedtls/library/ripemd160.c \
	mbedtls/mbedtls/library/pk_wrap.c \
	mbedtls/mbedtls/library/psa_crypto_storage.c \
	mbedtls/mbedtls/library/camellia.c \
	mbedtls/mbedtls/library/poly1305.c \
	mbedtls/mbedtls/library/pkcs12.c \
	mbedtls/mbedtls/library/ecp_curves_new.c \
	mbedtls/mbedtls/library/pk.c \
	mbedtls/mbedtls/library/psa_crypto_ecp.c \
	mbedtls/mbedtls/library/x509write.c \
	mbedtls/mbedtls/library/aesni.c \
	mbedtls/mbedtls/library/error.c \
	mbedtls/mbedtls/library/psa_crypto_cipher.c \
	mbedtls/mbedtls/library/psa_crypto_pake.c \
	mbedtls/mbedtls/library/psa_its_file.c \
	mbedtls/mbedtls/library/cipher_wrap.c \
	mbedtls/mbedtls/library/sha256.c \
	mbedtls/mbedtls/library/ecp_curves.c \
	mbedtls/mbedtls/library/debug.c \
	mbedtls/mbedtls/library/timing.c \
	mbedtls/mbedtls/library/ssl_debug_helpers_generated.c \
	mbedtls/mbedtls/library/x509write_csr.c \
	mbedtls/mbedtls/library/x509_crt.c \
	mbedtls/mbedtls/library/version.c \
	mbedtls/mbedtls/library/platform.c \
	mbedtls/mbedtls/library/platform_util.c \
	mbedtls/mbedtls/library/pkcs5.c \
	mbedtls/mbedtls/library/ssl_cookie.c \
	mbedtls/mbedtls/library/rsa_alt_helpers.c \
	mbedtls/mbedtls/library/hmac_drbg.c \
	mbedtls/mbedtls/library/ecjpake.c
LOCAL_C_INCLUDES += jni/mbedtls/mbedtls

include $(BUILD_SHARED_LIBRARY)

# $(call import-module,android/native_app_glue)
