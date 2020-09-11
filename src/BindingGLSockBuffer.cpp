#include "Common.h"
#include "BindingGLSockBuffer.h"
#include "GLSockBuffer.h"
#include "BufferMgr.h"
#include "SockMgr.h"

namespace GLSockBuffer {

CGLSockBuffer* CheckBuffer(lua_State *state, int idx)
{
	LUA->CheckType(idx, GLSOCKBUFFER_TYPE);

	GarrysMod::Lua::UserData *pUserData = (GarrysMod::Lua::UserData*)LUA->GetUserdata(idx);
	if( !pUserData )
		return NULL;

	if( pUserData->type != GLSOCKBUFFER_TYPE )
		return NULL;

	CGLSockBuffer* pBuffer = reinterpret_cast<CGLSockBuffer*>(pUserData->data);
	if( !g_pBufferMgr->ValidHandle(pBuffer) )
		return NULL;

	return pBuffer;
}

void PushBuffer(lua_State *state, CGLSockBuffer *pBuffer)
{
	GarrysMod::Lua::UserData *pUserData = static_cast<GarrysMod::Lua::UserData*>(LUA->NewUserdata(sizeof(GarrysMod::Lua::UserData)));
	int iRefUserData = LUA->ReferenceCreate();

	pUserData->type = GLSOCKBUFFER_TYPE;
	pUserData->data = static_cast<void*>(pBuffer);

	LUA->CreateMetaTableType(GLSOCKBUFFER_NAME, GLSOCKBUFFER_TYPE);
	int iRefMetatable = LUA->ReferenceCreate();

	LUA->ReferencePush(iRefUserData);
	LUA->ReferencePush(iRefMetatable);
	LUA->SetMetaTable(-2);

	LUA->ReferenceFree(iRefMetatable);
	LUA->ReferenceFree(iRefUserData);
}

static int __new(lua_State *state)
{
	CGLSockBuffer* pBuffer = g_pBufferMgr->Create();
	pBuffer->Reference();

	PushBuffer(state, pBuffer);

	return 1;
}

static int __delete(lua_State *state)
{
	CGLSockBuffer* pBuffer = CheckBuffer(state, 1);
	if( !pBuffer )
		return 0;

	int nTableRef = pBuffer->m_nTableRef;
	if( pBuffer->Unreference() <= 0 )
	{
		LUA->ReferenceFree(nTableRef);
	}

	return 0;
}

static int __index(lua_State *state)
{
	CGLSockBuffer* pBuffer = CheckBuffer(state, 1);
	if( !pBuffer )
		return 0;

	LUA->CreateMetaTableType(GLSOCKBUFFER_NAME, GLSOCKBUFFER_TYPE);
	CAutoUnRef Metatable(state);
	{
		Metatable.Push();
		LUA->PushString("__functions");
		LUA->RawGet(-2);

		LUA->Push(2);
		LUA->RawGet(-2);

		if( !LUA->IsType(-1, GarrysMod::Lua::Type::NIL) )
			return 1;
		else
			LUA->Pop();
	}

	if( pBuffer->m_nTableRef == 0 )
		return 0;

	LUA->ReferencePush(pBuffer->m_nTableRef);
	LUA->Push(2);
	LUA->RawGet(-2);

	return 1;
}

static int __newindex(lua_State *state)
{
	CGLSockBuffer* pBuffer = CheckBuffer(state, 1);
	if( !pBuffer )
		return 0;

	if( pBuffer->m_nTableRef == 0 )
	{
		LUA->CreateTable();
		pBuffer->m_nTableRef = LUA->ReferenceCreate();
	}

	LUA->ReferencePush(pBuffer->m_nTableRef);
	LUA->Push(2);
	LUA->Push(3);
	LUA->RawSet(-3);

	return 0;
}

static int __tostring(lua_State *state)
{
	CGLSockBuffer* pBuffer = CheckBuffer(state, 1);
	if( !pBuffer )
		return 0;

	std::stringstream ss;
	ss << "GLSockBuffer: " << (void*)pBuffer;

	std::string strType = ss.str();
	LUA->PushString(strType.c_str());

	return 1;
}

static int __eq(lua_State *state)
{
	CGLSockBuffer* pBuffer = CheckBuffer(state, 1);
	if( !pBuffer )
		return 0;

	CGLSockBuffer* pBufferCmp = CheckBuffer(state, 2);
	if( !pBuffer )
		return 0;

	LUA->PushBool(pBuffer == pBufferCmp);
	return 1;
}

static int Write(lua_State *state)
{
	CGLSockBuffer* pBuffer = CheckBuffer(state, 1);
	if( !pBuffer )
		return 0;

	LUA->CheckType(2, GarrysMod::Lua::Type::STRING);

	unsigned int nLen = 0;
	const char *pData = LUA->GetString(2, &nLen);

	LUA->PushNumber(pBuffer->Write(pData, nLen));
	return 1;
}

static int Read(lua_State *state)
{
	CGLSockBuffer* pBuffer = CheckBuffer(state, 1);
	if( !pBuffer )
		return 0;

	LUA->CheckType(2, GarrysMod::Lua::Type::NUMBER);

	unsigned int nReadBytes = static_cast<unsigned int>(LUA->GetNumber(2));

	char* szData = new char[nReadBytes + 1];
	unsigned int nRead = pBuffer->Read(szData, nReadBytes);
	szData[nRead] = '\0';

	if( nReadBytes > 0 )
	{
		LUA->PushString(szData, nReadBytes);
		LUA->PushNumber(static_cast<double>(nReadBytes));
	}
	else
	{
		LUA->PushNil();
		LUA->PushNumber(0);
	}

	delete[] szData;

	return 2;
}

static int WriteString(lua_State *state)
{
	CGLSockBuffer* pBuffer = CheckBuffer(state, 1);
	if( !pBuffer )
		return 0;

	LUA->CheckType(2, GarrysMod::Lua::Type::STRING);

	const char *pData = LUA->GetString(2);

	LUA->PushNumber( static_cast<double>(pBuffer->Write(pData, strlen(pData) + 1)));

	return 1;
}

static int ReadString(lua_State *state)
{
	CGLSockBuffer* pBuffer = CheckBuffer(state, 1);
	if( !pBuffer )
		return 0;

	unsigned int uStringLength = 0;
	const char* pData = pBuffer->Buffer();
	bool bValid = false;

	for(unsigned int i = pBuffer->Tell(); i < pBuffer->Size(); i++)
	{
		if( pData[i] == '\0' )
		{
			bValid = true;
			break;
		}
		uStringLength++;
	}

	if( bValid )
	{
		// Copy string.
		std::string strData( pData + pBuffer->Tell(), uStringLength );
		LUA->PushString(pData + pBuffer->Tell());
		LUA->PushNumber(static_cast<double>(uStringLength + 1));

		// Update position.
		pBuffer->Seek(uStringLength + 1, SOCKBUFFER_SEEK_CUR);
	}
	else
	{
		LUA->PushNil();
		LUA->PushNumber(0);
	}

	return 2;
}

static int WriteDouble(lua_State *state)
{
	CGLSockBuffer* pBuffer = CheckBuffer(state, 1);
	if( !pBuffer )
		return 0;

	LUA->CheckType(2, GarrysMod::Lua::Type::NUMBER);
	double nValue = LUA->GetNumber(2);

	if( LUA->Top() >= 3 )
	{
		LUA->CheckType(3, GarrysMod::Lua::Type::BOOL);

		bool bSwap = LUA->GetBool(3);
		if( bSwap )
			pBuffer->SwapEndian(nValue);
	}

	LUA->PushNumber( static_cast<double>(pBuffer->Write(nValue)));
	return 1;
}

static int ReadDouble(lua_State *state)
{
	CGLSockBuffer* pBuffer = CheckBuffer(state, 1);
	if( !pBuffer )
		return 0;

	double nValue = 0;
	unsigned int nRead = pBuffer->Read(nValue);

	if( LUA->Top() >= 2 )
	{
		LUA->CheckType(2, GarrysMod::Lua::Type::BOOL);

		bool bSwap = LUA->GetBool(2);
		if( bSwap )
			pBuffer->SwapEndian(nValue);
	}

	if( nRead == 0 )
	{
		LUA->PushNil();
		LUA->PushNumber(0);
	}
	else
	{
		LUA->PushNumber(static_cast<double>(nValue));
		LUA->PushNumber(static_cast<double>(nRead));
	}

	return 2;
}

static int WriteFloat(lua_State *state)
{
	CGLSockBuffer* pBuffer = CheckBuffer(state, 1);
	if( !pBuffer )
		return 0;

	LUA->CheckType(2, GarrysMod::Lua::Type::NUMBER);

	float nValue = static_cast<float>(LUA->GetNumber(2));

	if( LUA->Top() >= 3 )
	{
		LUA->CheckType(3, GarrysMod::Lua::Type::BOOL);

		bool bSwap = LUA->GetBool(3);
		if( bSwap )
			pBuffer->SwapEndian(nValue);
	}

	LUA->PushNumber( static_cast<double>(pBuffer->Write(nValue)));
	return 1;
}

static int ReadFloat(lua_State *state)
{
	CGLSockBuffer* pBuffer = CheckBuffer(state, 1);
	if( !pBuffer )
		return 0;

	float nValue = 0;
	unsigned int nRead = pBuffer->Read(nValue);

	if( LUA->Top() >= 2 )
	{
		LUA->CheckType(2, GarrysMod::Lua::Type::BOOL);

		bool bSwap = LUA->GetBool(2);
		if( bSwap )
			pBuffer->SwapEndian(nValue);
	}

	if( nRead == 0 )
	{
		LUA->PushNil();
		LUA->PushNumber(0);
	}
	else
	{
		LUA->PushNumber(static_cast<double>(nValue));
		LUA->PushNumber(nRead);
	}

	return 2;
}

static int _WriteInt(lua_State *state, bool sign)
{
	CGLSockBuffer* pBuffer = CheckBuffer(state, 1);
	if( !pBuffer )
		return 0;

	LUA->CheckType(2, GarrysMod::Lua::Type::NUMBER);

	signed int nValue = static_cast<signed int>(LUA->GetNumber(2));
	if (!sign)
		nValue = static_cast<unsigned int>(nValue);

	if( LUA->Top() >= 3 )
	{
		LUA->CheckType(3, GarrysMod::Lua::Type::BOOL);

		bool bSwap = LUA->GetBool(3);
		if( bSwap )
			pBuffer->SwapEndian(nValue);
	}

	LUA->PushNumber( static_cast<double>(pBuffer->Write(nValue)));
	return 1;
}

static int WriteInt(lua_State *state)
{
	return _WriteInt(state, true);
}

static int WriteUInt(lua_State *state)
{
	return _WriteInt(state, false);
}

static int _ReadInt(lua_State *state, bool sign)
{
	CGLSockBuffer* pBuffer = CheckBuffer(state, 1);
	if( !pBuffer )
		return 0;

	signed int nValue = 0;
	if (!sign)
		nValue = static_cast<unsigned int>(nValue);
	unsigned int nRead = pBuffer->Read(nValue);

	if( LUA->Top() >= 2 )
	{
		LUA->CheckType(2, GarrysMod::Lua::Type::BOOL);

		bool bSwap = LUA->GetBool(2);
		if( bSwap )
			pBuffer->SwapEndian(nValue);
	}

	if( nRead == 0 )
	{
		LUA->PushNil();
		LUA->PushNumber(0);
	}
	else
	{
		LUA->PushNumber(static_cast<double>(nValue));
		LUA->PushNumber(static_cast<double>(nRead));
	}

	return 2;
}

static int ReadInt(lua_State *state)
{
	return _ReadInt(state, true);
}

static int ReadUInt(lua_State *state)
{
	return _ReadInt(state, false);
}

static int _WriteLong(lua_State *state, bool sign)
{
	CGLSockBuffer* pBuffer = CheckBuffer(state, 1);
	if (!pBuffer)
		return 0;

	LUA->CheckType(2, GarrysMod::Lua::Type::NUMBER);

	signed long nValue = static_cast<signed long>(LUA->GetNumber(2));
	if (!sign)
		nValue = static_cast<unsigned long>(nValue);

	if (LUA->Top() >= 3)
	{
		LUA->CheckType(3, GarrysMod::Lua::Type::BOOL);

		bool bSwap = LUA->GetBool(3);
		if (bSwap)
			pBuffer->SwapEndian(nValue);
	}

	LUA->PushNumber(static_cast<double>(pBuffer->Write(nValue)));
	return 1;
}

static int WriteLong(lua_State *state)
{
	return _WriteLong(state, true);
}

static int WriteULong(lua_State *state)
{
	return _WriteLong(state, false);
}

static int _ReadLong(lua_State *state, bool sign)
{
	CGLSockBuffer* pBuffer = CheckBuffer(state, 1);
	if (!pBuffer)
		return 0;

	signed long nValue = 0;
	if (!sign)
		nValue = static_cast<unsigned long>(nValue);
	unsigned int nRead = pBuffer->Read(nValue);

	if (LUA->Top() >= 2)
	{
		LUA->CheckType(2, GarrysMod::Lua::Type::BOOL);

		bool bSwap = LUA->GetBool(2);
		if (bSwap)
			pBuffer->SwapEndian(nValue);
	}

	if (nRead == 0)
	{
		LUA->PushNil();
		LUA->PushNumber(0);
	}
	else
	{
		LUA->PushNumber(static_cast<double>(nValue));
		LUA->PushNumber(static_cast<double>(nRead));
	}

	return 2;
}

static int ReadLong(lua_State *state)
{
	return _ReadLong(state, true);
}

static int ReadULong(lua_State *state)
{
	return _ReadLong(state, false);
}

static int _WriteShort(lua_State *state, bool sign)
{
	CGLSockBuffer* pBuffer = CheckBuffer(state, 1);
	if( !pBuffer )
		return 0;

	LUA->CheckType(2, GarrysMod::Lua::Type::NUMBER);

	signed short nValue = static_cast<signed short>(LUA->GetNumber(2));
	if (!sign)
		nValue = static_cast<unsigned short>(nValue);

	if( LUA->Top() >= 3 )
	{
		LUA->CheckType(3, GarrysMod::Lua::Type::BOOL);

		bool bSwap = LUA->GetBool(3);
		if( bSwap )
			pBuffer->SwapEndian(nValue);
	}

	LUA->PushNumber( static_cast<double>(pBuffer->Write(nValue)));
	return 1;
}

static int WriteShort(lua_State *state)
{
	return _WriteShort(state, true);
}

static int WriteUShort(lua_State *state)
{
	return _WriteShort(state, false);
}

static int _ReadShort(lua_State *state, bool sign)
{
	CGLSockBuffer* pBuffer = CheckBuffer(state, 1);
	if( !pBuffer )
		return 0;

	short nValue = 0;
	if (!sign)
		nValue = static_cast<unsigned short>(nValue);
	unsigned int nRead = pBuffer->Read(nValue);

	if( LUA->Top() >= 2 )
	{
		LUA->CheckType(2, GarrysMod::Lua::Type::BOOL);

		bool bSwap = LUA->GetBool(2);
		if( bSwap )
			pBuffer->SwapEndian(nValue);
	}

	if( nRead == 0 )
	{
		LUA->PushNil();
		LUA->PushNumber(0);
	}
	else
	{
		LUA->PushNumber(static_cast<double>(nValue));
		LUA->PushNumber(static_cast<double>(nRead));
	}

	return 2;
}

static int ReadShort(lua_State *state)
{
	return _ReadShort(state, true);
}

static int ReadUShort(lua_State *state)
{
	return _ReadShort(state, false);
}

static int _WriteByte(lua_State *state, bool sign)
{
	CGLSockBuffer* pBuffer = CheckBuffer(state, 1);
	if (!pBuffer)
		return 0;

	LUA->CheckType(2, GarrysMod::Lua::Type::NUMBER);

	signed char nValue = static_cast<signed char>(LUA->GetNumber(2));
	if (!sign)
		nValue = static_cast<unsigned char>(nValue);

	LUA->PushNumber(static_cast<double>(pBuffer->Write(nValue)));
	return 1;
}

static int WriteByte(lua_State *state)
{
	return _WriteByte(state, false);
}

static int WriteSByte(lua_State *state)
{
	return _WriteByte(state, true);
}

static int _ReadByte(lua_State *state, bool sign)
{
	CGLSockBuffer* pBuffer = CheckBuffer(state, 1);
	if( !pBuffer )
		return 0;

	signed char nValue = 0;
	if (!sign)
		nValue = static_cast<unsigned char>(nValue);
	unsigned int nRead = pBuffer->Read(nValue);

	if( nRead == 0 )
	{
		LUA->PushNil();
		LUA->PushNumber(0);
	}
	else
	{
		LUA->PushNumber(static_cast<double>(nValue));
		LUA->PushNumber(static_cast<double>(nRead));
	}

	return 2;
}

static int ReadByte(lua_State *state)
{
	return _ReadByte(state, false);
}

static int ReadSByte(lua_State *state)
{
	return _ReadByte(state, true);
}

static int WriteChar(lua_State *state)
{
	CGLSockBuffer* pBuffer = CheckBuffer(state, 1);
	if (!pBuffer)
		return 0;

	LUA->CheckType(2, GarrysMod::Lua::Type::STRING);

	const char* nStr = LUA->GetString(2);
	if (strlen(nStr) != 1)
		return 0;

	unsigned char nValue = static_cast<unsigned char>(nStr[0]);

	LUA->PushNumber(static_cast<double>(pBuffer->Write(nValue)));
	return 1;
}

static int Size(lua_State *state)
{
	CGLSockBuffer* pBuffer = CheckBuffer(state, 1);
	if( !pBuffer )
		return 0;

	LUA->PushNumber(static_cast<double>(pBuffer->Size()));
	return 1;
}

static int Tell(lua_State *state)
{
	CGLSockBuffer* pBuffer = CheckBuffer(state, 1);
	if( !pBuffer )
		return 0;

	LUA->PushNumber(static_cast<double>(pBuffer->Tell()));
	return 1;
}

static int Seek(lua_State *state)
{
	CGLSockBuffer* pBuffer = CheckBuffer(state, 1);
	if( !pBuffer )
		return 0;

	LUA->CheckType(2, GarrysMod::Lua::Type::NUMBER);
	LUA->CheckType(3, GarrysMod::Lua::Type::NUMBER);

	unsigned int nPos = static_cast<unsigned int>(LUA->GetNumber(2));
	unsigned int nMethod = static_cast<unsigned int>(LUA->GetNumber(3));

	LUA->PushNumber( static_cast<double>(pBuffer->Seek(nPos, nMethod)) );
	return 1;
}

static int EOB(lua_State *state)
{
	CGLSockBuffer* pBuffer = CheckBuffer(state, 1);
	if( !pBuffer )
		return 0;

	LUA->PushBool( pBuffer->EOB() );
	return 1;
}

static int Empty(lua_State *state)
{
	CGLSockBuffer* pBuffer = CheckBuffer(state, 1);
	if( !pBuffer )
		return 0;

	LUA->PushBool( pBuffer->Empty() );
	return 1;
}

static int Clear(lua_State *state)
{
	CGLSockBuffer* pBuffer = CheckBuffer(state, 1);
	if( !pBuffer )
		return 0;

	LUA->CheckType(2, GarrysMod::Lua::Type::NUMBER);

	unsigned int nPos = 0;
	unsigned int cSize = static_cast<unsigned int>(LUA->GetNumber(2));

	if( LUA->Top() >= 3 )
	{
		LUA->CheckType(3, GarrysMod::Lua::Type::NUMBER);
		nPos = static_cast<unsigned int>(LUA->GetNumber(3));
	}

	LUA->PushBool( pBuffer->Clear(nPos, cSize) );

	return 1;
}

static int Append(lua_State *state)
{
	CGLSockBuffer* tBuffer = CheckBuffer(state, 1);
	CGLSockBuffer* sBuffer = CheckBuffer(state, 2);

	if (!tBuffer || !sBuffer)
		return 0;

	tBuffer->Append(sBuffer);

	LUA->PushBool(true);
	return 1;
}

void Startup( lua_State *state )
{
	LUA->CreateMetaTableType(GLSOCKBUFFER_NAME, GLSOCKBUFFER_TYPE);
	CAutoUnRef Metatable(state);
	{
		#define SetMember(k, v) LUA->PushString(k); LUA->PushCFunction(v); LUA->SetTable(-3);

		Metatable.Push();
		LUA->PushString("__functions");
		LUA->CreateTable();
		{
			SetMember("Write", GLSockBuffer::Write);
			SetMember("Read", GLSockBuffer::Read);

			SetMember("WriteString", GLSockBuffer::WriteString);
			SetMember("ReadString", GLSockBuffer::ReadString);
			SetMember("WriteDouble", GLSockBuffer::WriteDouble);
			SetMember("ReadDouble", GLSockBuffer::ReadDouble);
			SetMember("WriteFloat", GLSockBuffer::WriteFloat);
			SetMember("ReadFloat", GLSockBuffer::ReadFloat);
			SetMember("WriteInt", GLSockBuffer::WriteInt);
			SetMember("WriteUInt", GLSockBuffer::WriteUInt);
			SetMember("ReadInt", GLSockBuffer::ReadInt);
			SetMember("ReadUInt", GLSockBuffer::ReadUInt);
			SetMember("WriteLong", GLSockBuffer::WriteLong);
			SetMember("WriteULong", GLSockBuffer::WriteULong);
			SetMember("ReadLong", GLSockBuffer::ReadLong);
			SetMember("ReadULong", GLSockBuffer::ReadULong);
			SetMember("WriteShort", GLSockBuffer::WriteShort);
			SetMember("WriteUShort", GLSockBuffer::WriteUShort);
			SetMember("ReadShort", GLSockBuffer::ReadShort);
			SetMember("ReadUShort", GLSockBuffer::ReadUShort);
			SetMember("WriteByte", GLSockBuffer::WriteByte);
			SetMember("WriteSByte", GLSockBuffer::WriteSByte);
			SetMember("ReadByte", GLSockBuffer::ReadByte);
			SetMember("ReadSByte", GLSockBuffer::ReadSByte);
			SetMember("WriteChar", GLSockBuffer::WriteChar);

			SetMember("Size", GLSockBuffer::Size);
			SetMember("Tell", GLSockBuffer::Tell);
			SetMember("Seek", GLSockBuffer::Seek);
			SetMember("EOB", GLSockBuffer::EOB);
			SetMember("Empty", GLSockBuffer::Empty);
			SetMember("Clear", GLSockBuffer::Clear);
			SetMember("Append", GLSockBuffer::Append);
		}
		LUA->RawSet(-3);

		SetMember("__gc", GLSockBuffer::__delete);
		SetMember("__eq", GLSockBuffer::__eq);
		SetMember("__tostring", GLSockBuffer::__tostring);
		SetMember("__index", GLSockBuffer::__index);
		SetMember("__newindex", GLSockBuffer::__newindex);

		#undef SetMember
	}

	LuaSetGlobal(state, "GLSockBuffer", GLSockBuffer::__new);

	// Seek Methods
	LuaSetGlobal(state, "GLSOCKBUFFER_SEEK_SET", SOCKBUFFER_SEEK_SET);
	LuaSetGlobal(state, "GLSOCKBUFFER_SEEK_CUR", SOCKBUFFER_SEEK_CUR);
	LuaSetGlobal(state, "GLSOCKBUFFER_SEEK_END", SOCKBUFFER_SEEK_END);
}

void Cleanup( lua_State *state )
{
}

}
