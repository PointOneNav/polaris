ADD_LIBRARY(septentrio
sbfsvid.h
ssntypes.h
measepochconfig.h
sbfdef.h
sbfread.h
crc.h
sbfread_meas.c
sbfread.c
ReadMe.txt
measepoch.h
crc.c
sbfsvid.c
sviddef.h
sbfsigtypes.h )

#TARGET_INCLUDE_DIRECTORIES(septentrio PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
TARGET_COMPILE_DEFINITIONS(septentrio PRIVATE NO_DECRYPTION=1)