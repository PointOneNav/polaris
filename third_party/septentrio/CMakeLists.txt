add_library(septentrio
            sbfsvid.h
            ssntypes.h
            measepochconfig.h
            sbfdef.h
            sbfread.h
            crc.h
            sbfread_meas.c
            sbfread.c
            measepoch.h
            crc.c
            sbfsvid.c
            sviddef.h
            sbfsigtypes.h)

# Disable Septentrio encryption support.
target_compile_options(septentrio PUBLIC -DNO_DECRYPTION=1)
