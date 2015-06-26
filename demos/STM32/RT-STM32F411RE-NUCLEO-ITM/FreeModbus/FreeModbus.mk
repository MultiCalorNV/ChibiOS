# List of all the FreeModbus files.
FREEMBSRC = FreeModbus/modbus/ascii/mbascii.c \
			FreeModbus/modbus/functions/mbfunccoils_m.c \
			FreeModbus/modbus/functions/mbfuncdiag.c \
			FreeModbus/modbus/functions/mbfuncdisc_m.c \
			FreeModbus/modbus/functions/mbfuncholding_m.c \
			FreeModbus/modbus/functions/mbfuncinput_m.c \
			FreeModbus/modbus/functions/mbfuncother.c \
			FreeModbus/modbus/functions/mbutils.c \
			FreeModbus/modbus/rtu/mbcrc.c \
			FreeModbus/modbus/rtu/mbrtu_m.c \
			FreeModbus/modbus/mb_m.c \
			FreeModbus/port/user_mb_app_m.c \
			FreeModbus/port/rtt/port.c \
			FreeModbus/port/rtt/portevent_m.c \
			FreeModbus/port/rtt/portserial_m.c \
			FreeModbus/port/rtt/porttimer_m.c \


# Required include directories
FREEMBINC = FreeModbus/modbus/ascii \
			FreeModbus/modbus/include \
			FreeModbus/modbus/rtu \
			FreeModbus/port
