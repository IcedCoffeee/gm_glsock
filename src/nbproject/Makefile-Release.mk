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
CND_PLATFORM=GNU-Linux-x86
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/GLSockAcceptor.o \
	${OBJECTDIR}/GLSock.o \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/BufferMgr.o \
	${OBJECTDIR}/BindingGLSock.o \
	${OBJECTDIR}/SockMgr.o \
	${OBJECTDIR}/GLSockBuffer.o \
	${OBJECTDIR}/GLSockTCP.o \
	${OBJECTDIR}/GLSockUDP.o \
	${OBJECTDIR}/BindingGLSockBuffer.o


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
LDLIBSOPTIONS=-static -lboost_date_time-gcc46-mt-s-1_42 -lboost_regex-gcc46-mt-s-1_42 -lboost_serialization-gcc46-s-1_42 -lboost_system-gcc46-mt-s-1_42 -lboost_thread-gcc46-mt-s-1_42

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/gm_glsock_linux.dll

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/gm_glsock_linux.dll: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -lpthread -static-libgcc -static-libstdc++ -shared -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/gm_glsock_linux.dll -fPIC ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/GLSockAcceptor.o: GLSockAcceptor.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -I../../boost_1_42_0 -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/GLSockAcceptor.o GLSockAcceptor.cpp

${OBJECTDIR}/GLSock.o: GLSock.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -I../../boost_1_42_0 -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/GLSock.o GLSock.cpp

${OBJECTDIR}/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -I../../boost_1_42_0 -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/main.o main.cpp

${OBJECTDIR}/BufferMgr.o: BufferMgr.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -I../../boost_1_42_0 -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/BufferMgr.o BufferMgr.cpp

${OBJECTDIR}/BindingGLSock.o: BindingGLSock.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -I../../boost_1_42_0 -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/BindingGLSock.o BindingGLSock.cpp

${OBJECTDIR}/SockMgr.o: SockMgr.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -I../../boost_1_42_0 -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/SockMgr.o SockMgr.cpp

${OBJECTDIR}/GLSockBuffer.o: GLSockBuffer.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -I../../boost_1_42_0 -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/GLSockBuffer.o GLSockBuffer.cpp

${OBJECTDIR}/GLSockTCP.o: GLSockTCP.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -I../../boost_1_42_0 -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/GLSockTCP.o GLSockTCP.cpp

${OBJECTDIR}/GLSockUDP.o: GLSockUDP.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -I../../boost_1_42_0 -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/GLSockUDP.o GLSockUDP.cpp

${OBJECTDIR}/BindingGLSockBuffer.o: BindingGLSockBuffer.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -I../../boost_1_42_0 -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/BindingGLSockBuffer.o BindingGLSockBuffer.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/gm_glsock_linux.dll

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
