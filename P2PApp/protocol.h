#pragma once

#define	PN_HEADER	0x4142

enum PEN_Type
{
	PT_LOGIN,
	PT_LOGIN_RES,
	PT_HEART,
	PT_QUERY_ID,
	PT_QUERY_ID_RES,
	PT_CONN_ID,
	PT_CONN_ID_RES,
	PT_CONN_ID_REQ,
	PT_CONN_ID_REQRES,
	PT_DATA,
	PT_P2PMSG,
	PT_P2PRESCONNFAIL,
	PT_LOGOUT,

	PT_MAX_ID
};
enum E_MSG_P2P
{
	EMP_CONN_JUST_OK,
	EMP_CONN_REQ_JUST_OK,
	EMP_REQ_P2P_CONN,
	EMP_REQ_LISTEN_OK
};

#pragma pack(1)
#pragma warning(disable:4200)
typedef struct {
	unsigned int	flag;
	unsigned int	len;
	int				type;
	
} PNS_Header;

#define MAKE_PNSHeader(size,t) (PNS_Header{.flag=PN_HEADER,.len=(size),.type=(t)})
#define CHECKFLAG(h) (  (h->flag) == PN_HEADER)

#define PNSHEADER	PNS_Header header;

typedef struct {
	PNSHEADER;

	int	msg;
	unsigned long P1, P2;
}PNS_P2PMsg;

#define MAKE_PNSMsg(m) PNS_P2PMsg{.header=MAKE_PNSHeader(sizeof(PNS_P2PMsg),PT_P2PMSG), .msg=m}
#define MAKE_PNSMsg2(m,p1,p2) PNS_P2PMsg{.header=MAKE_PNSHeader(sizeof(PNS_P2PMsg),PT_P2PMSG), .msg=m,.P1=p1,.P2=p2}

typedef struct {
	PNSHEADER;

	unsigned long	cltAddr;
	unsigned short	cltPort;
}PNS_Login;

#define MAKE_PNSLogin(addr,port) PNS_Login{.header=MAKE_PNSHeader(sizeof(PNS_Login),PT_LOGIN), .cltAddr=(addr),.cltPort=(port)}


typedef struct {
	PNSHEADER;

	unsigned long	selfId;
	unsigned long	selfAddr;
	unsigned short	selfPort;
}PNS_Login_Res;

#define MAKE_PNSLoginRes(id,addr,port) PNS_Login_Res{.header=MAKE_PNSHeader(sizeof(PNS_Login_Res),PT_LOGIN_RES),.selfId=(id), .selfAddr=(addr),.selfPort=(port)}

typedef struct {
	PNSHEADER;

	unsigned long	selfId;
	unsigned long	targetId;
}PNS_P2PResConnFailed;

#define MAKE_PNSP2PResConnFailed(sid,tid) PNS_P2PResConnFailed{.header=MAKE_PNSHeader(sizeof(PNS_P2PResConnFailed),PT_P2PRESCONNFAIL),.selfId=(sid), .targetId=(tid)}

typedef struct {
	PNSHEADER;

	unsigned long	selfId;
}PNS_Heart;

#define MAKE_PNSHeart(id) PNS_Heart{.header=MAKE_PNSHeader(sizeof(PNS_Heart),PT_HEART),.selfId=(id)}



typedef struct {
	PNSHEADER;

	unsigned long	selfId;
}PNS_QueryId;

#define MAKE_PNS_QueryId(id) PNS_QueryId{.header=MAKE_PNSHeader(sizeof(PNS_QueryId),PT_QUERY_ID),.selfId=(id)}


typedef struct {
	PNSHEADER;

	unsigned long	size;
	unsigned long	val[0];
}PNS_QueryIdRes;

#define MAKE_PNS_QueryIdRes(s) PNS_QueryIdRes{.header=MAKE_PNSHeader(sizeof(PNS_QueryIdRes),PT_QUERY_ID_RES),.size=(s)}

typedef struct {
	PNSHEADER;

	unsigned long	selfId;
	unsigned long	targetId;
}PNS_ConnId;

#define MAKE_PNS_ConnId(id,tid) PNS_ConnId{.header=MAKE_PNSHeader(sizeof(PNS_ConnId),PT_CONN_ID),.selfId=(id),.targetId=(tid)}


typedef struct {
	PNSHEADER;
	unsigned long	targetId;
	unsigned long	selfId;
	unsigned long	addr;
	unsigned short	port;
	bool			ok;
}PNS_ConnIdRes;

#define MAKE_PNS_ConnIdRes(a,p,tid,sid) PNS_ConnIdRes{.header=MAKE_PNSHeader(sizeof(PNS_ConnIdRes),PT_CONN_ID_RES),.targetId=(tid),.selfId=(sid),.addr=(a),.port=(p),.ok=true}

typedef struct {
	PNSHEADER;
	unsigned long	sid;
	unsigned long	addr;
	unsigned short	port;
}PNS_ConnIdReq;

#define MAKE_PNS_ConnIdReq(a,p,id) PNS_ConnIdReq{.header=MAKE_PNSHeader(sizeof(PNS_ConnIdReq),PT_CONN_ID_REQ),.sid=id,.addr=(a),.port=(p)}

typedef struct {
	PNSHEADER;
	unsigned long	selfId;
	unsigned long	tid;
}PNS_ConnIdReqRes;

#define MAKE_PNS_ConnIdReqRes(sid,targetid) PNS_ConnIdReqRes{.header=MAKE_PNSHeader(sizeof(PNS_ConnIdReqRes),PT_CONN_ID_REQRES),.selfId=sid,.tid=targetid}

typedef struct {
	PNSHEADER;

	unsigned long	len;
	unsigned char	bType;
	unsigned char	buf[0];
}PNS_Data;

#define MAKE_PNS_Data(len,type) PNS_Data{.header=MAKE_PNSHeader(sizeof(PNS_Data),PT_DATA),.len=(l),.bType = type}

#define MAKE_PNS_Header(h,l,t) {(h)->flag=PN_HEADER;(h)->len=l;(h)->type=t;}
typedef struct {
	PNSHEADER;

}PNS_Logout;

#define MAKE_PNS_Logout() PNS_Logout{.header=MAKE_PNSHeader(sizeof(PNS_Logout),PT_LOGOUT)}

#pragma warning(default:4200)
#pragma pack()