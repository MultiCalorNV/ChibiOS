# List of the required PolarSSL files.
POLARSSL = 	${CHIBIOS}/os/ext/PolarSSL

SSLSRC = \
		 ${POLARSSL}/library/memory_buffer_alloc.c \
         ${POLARSSL}/library/net.c \
		 ${POLARSSL}/library/timing.c \
		 ${POLARSSL}/library/x509parse.c \
		 ${POLARSSL}/library/asn1parse.c \
		 ${POLARSSL}/library/certs.c \
		 ${POLARSSL}/library/rsa.c \
		 ${POLARSSL}/library/bignum.c \
		 ${POLARSSL}/library/pem.c \
		 ${POLARSSL}/library/md5.c \
		 ${POLARSSL}/library/des.c \
		 ${POLARSSL}/library/aes.c \
		 ${POLARSSL}/library/base64.c \
		 ${POLARSSL}/library/pkcs5.c \
		 ${POLARSSL}/library/pkcs12.c \
		 ${POLARSSL}/library/md.c \
		 ${POLARSSL}/library/cipher.c \
		 ${POLARSSL}/library/arc4.c \
		 ${POLARSSL}/library/sha1.c \
		 ${POLARSSL}/library/camellia.c \
		 ${POLARSSL}/library/blowfish.c \
		 ${POLARSSL}/library/md_wrap.c \
		 ${POLARSSL}/library/cipher_wrap.c \
		 ${POLARSSL}/library/sha2.c \
		 ${POLARSSL}/library/sha4.c	\
		 ${POLARSSL}/library/entropy.c \
		 ${POLARSSL}/library/ctr_drbg.c \
		 ${POLARSSL}/library/entropy_poll.c \
		 ${POLARSSL}/library/ssl_cache.c \
		 ${POLARSSL}/library/ssl_tls.c \
		 ${POLARSSL}/library/debug.c \
		 ${POLARSSL}/library/dhm.c \
		 ${POLARSSL}/library/error.c \
		 ${POLARSSL}/library/ssl_cli.c \
		 ${POLARSSL}/library/gcm.c
		 
		 
SSLINC = \
		 ${POLARSSL}/include/polarssl
