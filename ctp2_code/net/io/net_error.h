#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#if 0
#ifndef _NET_ERROR_H_
#define _NET_ERROR_H_

class ErrNet {
public:
	ErrNet() : m_error(NET_ERR_OK)
	{
	};

	ErrNet(NET_ERR err) :
		m_error(err)
	{
	};
	NET_ERR m_error;
};

class ErrNetBadTransport : public ErrNet {
public:
	ErrNetBadTransport() : ErrNet(NET_ERR_BADTRANSPORT) {};
};

class ErrNetTransportError : public ErrNet {
public:
	ErrNetTransportError() : ErrNet(NET_ERR_TRANSPORTERROR) {};
};

class ErrNetAlreadyOpen : public ErrNet {
public:
	ErrNetAlreadyOpen() : ErrNet(NET_ERR_ALREADYOPEN) {};
};

class ErrNetNotStarted : public ErrNet {
public:
	ErrNetNotStarted() : ErrNet(NET_ERR_NOTSTARTED) {};
};

class ErrNetNotImplemented : public ErrNet {
public:
	ErrNetNotImplemented() : ErrNet(NET_ERR_NOTIMPLEMENTED) {};
};

class ErrNetWrite : public ErrNet {
public:
	ErrNetWrite() : ErrNet(NET_ERR_WRITEERR) {};
};

#else
class NetError;
#endif
#endif
