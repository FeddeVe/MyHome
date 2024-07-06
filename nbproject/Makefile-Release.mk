#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=None-Linux
CND_DLIB_EXT=so
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/Area/Area.o \
	${OBJECTDIR}/Area/Area_PID.o \
	${OBJECTDIR}/Area/Area_Surround_Brightness.o \
	${OBJECTDIR}/Area/Areas.o \
	${OBJECTDIR}/Background/Background.o \
	${OBJECTDIR}/Background/Background_Scenes.o \
	${OBJECTDIR}/Background/Background_Slot.o \
	${OBJECTDIR}/Background/Background_Value.o \
	${OBJECTDIR}/Background/sunset.o \
	${OBJECTDIR}/Global.o \
	${OBJECTDIR}/Hubs/Hue_Bridge.o \
	${OBJECTDIR}/Hubs/Hue_Bridges.o \
	${OBJECTDIR}/Hubs/Hue_Device.o \
	${OBJECTDIR}/Hubs/Hue_EventStream.o \
	${OBJECTDIR}/Hubs/Hue_Send_Buffer.o \
	${OBJECTDIR}/Hubs/Hue_Service.o \
	${OBJECTDIR}/Hubs/Tree_Mesh/Tree_Message.o \
	${OBJECTDIR}/Hubs/Tree_Mesh/Tree_Node.o \
	${OBJECTDIR}/Hubs/Tree_Mesh/Tree_OTA.o \
	${OBJECTDIR}/Hubs/Tree_Mesh/Tree_Server.o \
	${OBJECTDIR}/Hubs/Tree_Mesh/Tree_Structs.o \
	${OBJECTDIR}/Inputs/Button.o \
	${OBJECTDIR}/Inputs/Button_Action.o \
	${OBJECTDIR}/Inputs/Button_Member.o \
	${OBJECTDIR}/Inputs/Buttons.o \
	${OBJECTDIR}/Inputs/Sensor.o \
	${OBJECTDIR}/Inputs/Sensors.o \
	${OBJECTDIR}/Macro/Macro.o \
	${OBJECTDIR}/Macro/Macro_Step.o \
	${OBJECTDIR}/Macro/Macros.o \
	${OBJECTDIR}/Master.o \
	${OBJECTDIR}/Outputs/Light.o \
	${OBJECTDIR}/Outputs/Lights.o \
	${OBJECTDIR}/PowerMeterSolar.o \
	${OBJECTDIR}/Scenes/Scene.o \
	${OBJECTDIR}/Scenes/Scene_Values.o \
	${OBJECTDIR}/Thread.o \
	${OBJECTDIR}/hardware.o \
	${OBJECTDIR}/hardware_nrf_msg.o \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/powermeter.o \
	${OBJECTDIR}/powermeter_values.o \
	${OBJECTDIR}/tcp.o \
	${OBJECTDIR}/tcpacceptor.o \
	${OBJECTDIR}/tcpstream.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/my_home_2.0

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/my_home_2.0: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/my_home_2.0 ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/Area/Area.o: Area/Area.cpp
	${MKDIR} -p ${OBJECTDIR}/Area
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Area/Area.o Area/Area.cpp

${OBJECTDIR}/Area/Area_PID.o: Area/Area_PID.cpp
	${MKDIR} -p ${OBJECTDIR}/Area
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Area/Area_PID.o Area/Area_PID.cpp

${OBJECTDIR}/Area/Area_Surround_Brightness.o: Area/Area_Surround_Brightness.cpp
	${MKDIR} -p ${OBJECTDIR}/Area
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Area/Area_Surround_Brightness.o Area/Area_Surround_Brightness.cpp

${OBJECTDIR}/Area/Areas.o: Area/Areas.cpp
	${MKDIR} -p ${OBJECTDIR}/Area
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Area/Areas.o Area/Areas.cpp

${OBJECTDIR}/Background/Background.o: Background/Background.cpp
	${MKDIR} -p ${OBJECTDIR}/Background
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Background/Background.o Background/Background.cpp

${OBJECTDIR}/Background/Background_Scenes.o: Background/Background_Scenes.cpp
	${MKDIR} -p ${OBJECTDIR}/Background
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Background/Background_Scenes.o Background/Background_Scenes.cpp

${OBJECTDIR}/Background/Background_Slot.o: Background/Background_Slot.cpp
	${MKDIR} -p ${OBJECTDIR}/Background
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Background/Background_Slot.o Background/Background_Slot.cpp

${OBJECTDIR}/Background/Background_Value.o: Background/Background_Value.cpp
	${MKDIR} -p ${OBJECTDIR}/Background
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Background/Background_Value.o Background/Background_Value.cpp

${OBJECTDIR}/Background/sunset.o: Background/sunset.cpp
	${MKDIR} -p ${OBJECTDIR}/Background
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Background/sunset.o Background/sunset.cpp

${OBJECTDIR}/Global.o: Global.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Global.o Global.cpp

${OBJECTDIR}/Hubs/Hue_Bridge.o: Hubs/Hue_Bridge.cpp
	${MKDIR} -p ${OBJECTDIR}/Hubs
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Hubs/Hue_Bridge.o Hubs/Hue_Bridge.cpp

${OBJECTDIR}/Hubs/Hue_Bridges.o: Hubs/Hue_Bridges.cpp
	${MKDIR} -p ${OBJECTDIR}/Hubs
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Hubs/Hue_Bridges.o Hubs/Hue_Bridges.cpp

${OBJECTDIR}/Hubs/Hue_Device.o: Hubs/Hue_Device.cpp
	${MKDIR} -p ${OBJECTDIR}/Hubs
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Hubs/Hue_Device.o Hubs/Hue_Device.cpp

${OBJECTDIR}/Hubs/Hue_EventStream.o: Hubs/Hue_EventStream.cpp
	${MKDIR} -p ${OBJECTDIR}/Hubs
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Hubs/Hue_EventStream.o Hubs/Hue_EventStream.cpp

${OBJECTDIR}/Hubs/Hue_Send_Buffer.o: Hubs/Hue_Send_Buffer.cpp
	${MKDIR} -p ${OBJECTDIR}/Hubs
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Hubs/Hue_Send_Buffer.o Hubs/Hue_Send_Buffer.cpp

${OBJECTDIR}/Hubs/Hue_Service.o: Hubs/Hue_Service.cpp
	${MKDIR} -p ${OBJECTDIR}/Hubs
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Hubs/Hue_Service.o Hubs/Hue_Service.cpp

${OBJECTDIR}/Hubs/Tree_Mesh/Tree_Message.o: Hubs/Tree_Mesh/Tree_Message.cpp
	${MKDIR} -p ${OBJECTDIR}/Hubs/Tree_Mesh
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Hubs/Tree_Mesh/Tree_Message.o Hubs/Tree_Mesh/Tree_Message.cpp

${OBJECTDIR}/Hubs/Tree_Mesh/Tree_Node.o: Hubs/Tree_Mesh/Tree_Node.cpp
	${MKDIR} -p ${OBJECTDIR}/Hubs/Tree_Mesh
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Hubs/Tree_Mesh/Tree_Node.o Hubs/Tree_Mesh/Tree_Node.cpp

${OBJECTDIR}/Hubs/Tree_Mesh/Tree_OTA.o: Hubs/Tree_Mesh/Tree_OTA.cpp
	${MKDIR} -p ${OBJECTDIR}/Hubs/Tree_Mesh
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Hubs/Tree_Mesh/Tree_OTA.o Hubs/Tree_Mesh/Tree_OTA.cpp

${OBJECTDIR}/Hubs/Tree_Mesh/Tree_Server.o: Hubs/Tree_Mesh/Tree_Server.cpp
	${MKDIR} -p ${OBJECTDIR}/Hubs/Tree_Mesh
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Hubs/Tree_Mesh/Tree_Server.o Hubs/Tree_Mesh/Tree_Server.cpp

${OBJECTDIR}/Hubs/Tree_Mesh/Tree_Structs.o: Hubs/Tree_Mesh/Tree_Structs.cpp
	${MKDIR} -p ${OBJECTDIR}/Hubs/Tree_Mesh
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Hubs/Tree_Mesh/Tree_Structs.o Hubs/Tree_Mesh/Tree_Structs.cpp

${OBJECTDIR}/Inputs/Button.o: Inputs/Button.cpp
	${MKDIR} -p ${OBJECTDIR}/Inputs
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Inputs/Button.o Inputs/Button.cpp

${OBJECTDIR}/Inputs/Button_Action.o: Inputs/Button_Action.cpp
	${MKDIR} -p ${OBJECTDIR}/Inputs
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Inputs/Button_Action.o Inputs/Button_Action.cpp

${OBJECTDIR}/Inputs/Button_Member.o: Inputs/Button_Member.cpp
	${MKDIR} -p ${OBJECTDIR}/Inputs
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Inputs/Button_Member.o Inputs/Button_Member.cpp

${OBJECTDIR}/Inputs/Buttons.o: Inputs/Buttons.cpp
	${MKDIR} -p ${OBJECTDIR}/Inputs
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Inputs/Buttons.o Inputs/Buttons.cpp

${OBJECTDIR}/Inputs/Sensor.o: Inputs/Sensor.cpp
	${MKDIR} -p ${OBJECTDIR}/Inputs
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Inputs/Sensor.o Inputs/Sensor.cpp

${OBJECTDIR}/Inputs/Sensors.o: Inputs/Sensors.cpp
	${MKDIR} -p ${OBJECTDIR}/Inputs
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Inputs/Sensors.o Inputs/Sensors.cpp

${OBJECTDIR}/Macro/Macro.o: Macro/Macro.cpp
	${MKDIR} -p ${OBJECTDIR}/Macro
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Macro/Macro.o Macro/Macro.cpp

${OBJECTDIR}/Macro/Macro_Step.o: Macro/Macro_Step.cpp
	${MKDIR} -p ${OBJECTDIR}/Macro
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Macro/Macro_Step.o Macro/Macro_Step.cpp

${OBJECTDIR}/Macro/Macros.o: Macro/Macros.cpp
	${MKDIR} -p ${OBJECTDIR}/Macro
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Macro/Macros.o Macro/Macros.cpp

${OBJECTDIR}/Master.o: Master.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Master.o Master.cpp

${OBJECTDIR}/Outputs/Light.o: Outputs/Light.cpp
	${MKDIR} -p ${OBJECTDIR}/Outputs
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Outputs/Light.o Outputs/Light.cpp

${OBJECTDIR}/Outputs/Lights.o: Outputs/Lights.cpp
	${MKDIR} -p ${OBJECTDIR}/Outputs
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Outputs/Lights.o Outputs/Lights.cpp

${OBJECTDIR}/PowerMeterSolar.o: PowerMeterSolar.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/PowerMeterSolar.o PowerMeterSolar.cpp

${OBJECTDIR}/Scenes/Scene.o: Scenes/Scene.cpp
	${MKDIR} -p ${OBJECTDIR}/Scenes
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Scenes/Scene.o Scenes/Scene.cpp

${OBJECTDIR}/Scenes/Scene_Values.o: Scenes/Scene_Values.cpp
	${MKDIR} -p ${OBJECTDIR}/Scenes
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Scenes/Scene_Values.o Scenes/Scene_Values.cpp

${OBJECTDIR}/Thread.o: Thread.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Thread.o Thread.cpp

${OBJECTDIR}/hardware.o: hardware.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/hardware.o hardware.cpp

${OBJECTDIR}/hardware_nrf_msg.o: hardware_nrf_msg.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/hardware_nrf_msg.o hardware_nrf_msg.cpp

${OBJECTDIR}/main.o: main.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.cpp

${OBJECTDIR}/powermeter.o: powermeter.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/powermeter.o powermeter.cpp

${OBJECTDIR}/powermeter_values.o: powermeter_values.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/powermeter_values.o powermeter_values.cpp

${OBJECTDIR}/tcp.o: tcp.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/tcp.o tcp.cpp

${OBJECTDIR}/tcpacceptor.o: tcpacceptor.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/tcpacceptor.o tcpacceptor.cpp

${OBJECTDIR}/tcpstream.o: tcpstream.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/tcpstream.o tcpstream.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
