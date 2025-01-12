#if !defined(GLSOCKACCEPTOR_H)
#define GLSOCKACCEPTOR_H

#include "GLSock.h"
#include "GLSockTCP.h"

namespace GLSock {

class CGLSockAcceptor : public CGLSock
{
private:
	boost::mutex m_Mutex;
	int m_nReferences;
	AcceptorSock_t m_Sock;
	lua_State *state;

public:
	CGLSockAcceptor(IOService_t& IOService_t, lua_State* pLua);

	virtual bool Bind(CEndpoint& Endpoint, Callback_t Callback);
	virtual bool Listen(int iBacklog, Callback_t Callback);
	virtual bool Accept(Callback_t Callback);
	virtual bool Resolve(const char* cszHostname, Callback_t Callback);
	virtual bool Close(void);
	virtual bool Cancel(void)
	{
		boost::system::error_code ec;
		m_Sock.cancel(ec);
		return ec.value() == 0;
	}
	virtual int Type(void)
	{
		return eSockTypeAcceptor;
	}
	AcceptorSock_t& Socket(void)
	{
		return m_Sock;
	}
	virtual int Reference(void);
	virtual int Unreference(void);
	virtual void Destroy(void)
	{
		boost::system::error_code ec;
		try
		{
			m_Sock.cancel(ec);
			m_Sock.close(ec);
			OnDestroy();
		}
		catch (boost::exception& ex)
		{
#if defined(_DEBUG)
			LUA->Msg("GLSock(Acceptor): %s\n",  boost::diagnostic_information(ex).c_str());
#endif
			UNREFERENCED_PARAM(ex);
		}
	}

private:
	void OnAccept(Callback_t Callback, class CGLSockTCP* pSock, const boost::system::error_code& ec);
	void OnDestroy(void);
};

}

#endif // GLSOCKACCEPTOR_H
