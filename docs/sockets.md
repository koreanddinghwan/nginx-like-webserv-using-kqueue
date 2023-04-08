# socket

## socket

- socket – create an endpoint for communication

```cpp

#include <sys/socket.h>

int
socket(int domain, int type, int protocol);
```

### domain

- protocol family 정보
- IPv4, IPv6와 더불어 소켓의 프로토콜 체계를 정의한다.
- IP suite에서 인터넷계층을 의미.
- 운영체제별로 다르게 정의되어있다.
- IPC socket을 위해 PF_UNIX와 같은 unix domain 소켓을 정의해서 사용할수도 있다.

```
PF_LOCAL        Host-internal protocols, formerly called PF_UNIX,
PF_UNIX         Host-internal protocols, deprecated, use PF_LOCAL,
PF_INET         Internet version 4 protocols,
PF_ROUTE        Internal Routing protocol,
PF_KEY          Internal key-management function,
PF_INET6        Internet version 6 protocols,
PF_SYSTEM       System domain,
PF_NDRV         Raw access to network device,
PF_VSOCK        VM Sockets protocols
```

<br>

### type

- 소켓의 데이터 전송방식을 정의한다.
- 연결지향형(tcp), 비연결지향형(udp)로 전송계층 프로토콜을 정의한다.

```
The socket has the indicated type, which specifies the semantics of communication.
Currently defined types are:

SOCK_STREAM : tcp
SOCK_DGRAM : udp
SOCK_RAW : super user에게만 허용되며, 내부 네트워크 프로토콜, 인터페이스 접근
```

<br>

### protocol

- socket에 사용될 특정 프로토콜을 명시한다.
- 이에 대한 정보는 mac에서 /etc/protocols 파일에 명시되어있다.

<details markdown="1">
<summary>mac의 protocols 파일</summary>
```
#
# Internet protocols
#
# $FreeBSD$
#	from: @(#)protocols	5.1 (Berkeley) 4/17/89
#
# See also http://www.iana.org/assignments/protocol-numbers
#
ip	0	IP		# internet protocol, pseudo protocol number
#hopopt	0	HOPOPT		# hop-by-hop options for ipv6
icmp	1	ICMP		# internet control message protocol
igmp	2	IGMP		# internet group management protocol
ggp	3	GGP		# gateway-gateway protocol
ipencap	4	IP-ENCAP	# IP encapsulated in IP (officially ``IP'')
st2	5	ST2		# ST2 datagram mode (RFC 1819) (officially ``ST'')
tcp	6	TCP		# transmission control protocol
cbt	7	CBT		# CBT, Tony Ballardie <A.Ballardie@cs.ucl.ac.uk>
egp	8	EGP		# exterior gateway protocol
igp	9	IGP		# any private interior gateway (Cisco: for IGRP)
bbn-rcc	10	BBN-RCC-MON	# BBN RCC Monitoring
nvp	11	NVP-II		# Network Voice Protocol
pup	12	PUP		# PARC universal packet protocol
argus	13	ARGUS		# ARGUS
emcon	14	EMCON		# EMCON
xnet	15	XNET		# Cross Net Debugger
chaos	16	CHAOS		# Chaos
udp	17	UDP		# user datagram protocol
mux	18	MUX		# Multiplexing protocol
dcn	19	DCN-MEAS	# DCN Measurement Subsystems
hmp	20	HMP		# host monitoring protocol
prm	21	PRM		# packet radio measurement protocol
xns-idp	22	XNS-IDP		# Xerox NS IDP
trunk-1	23	TRUNK-1		# Trunk-1
trunk-2	24	TRUNK-2		# Trunk-2
leaf-1	25	LEAF-1		# Leaf-1
leaf-2	26	LEAF-2		# Leaf-2
rdp	27	RDP		# "reliable datagram" protocol
irtp	28	IRTP		# Internet Reliable Transaction Protocol
iso-tp4	29	ISO-TP4		# ISO Transport Protocol Class 4
netblt	30	NETBLT		# Bulk Data Transfer Protocol
mfe-nsp	31	MFE-NSP		# MFE Network Services Protocol
merit-inp	32	MERIT-INP	# MERIT Internodal Protocol
dccp	33	DCCP		# Datagram Congestion Control Protocol
3pc	34	3PC		# Third Party Connect Protocol
idpr	35	IDPR		# Inter-Domain Policy Routing Protocol
xtp	36	XTP		# Xpress Tranfer Protocol
ddp	37	DDP		# Datagram Delivery Protocol
idpr-cmtp	38	IDPR-CMTP	# IDPR Control Message Transport Proto
tp++	39	TP++		# TP++ Transport Protocol
il	40	IL		# IL Transport Protocol
ipv6	41	IPV6		# ipv6
sdrp	42	SDRP		# Source Demand Routing Protocol
ipv6-route	43	IPV6-ROUTE	# routing header for ipv6
ipv6-frag	44	IPV6-FRAG	# fragment header for ipv6
idrp	45	IDRP		# Inter-Domain Routing Protocol
rsvp	46	RSVP		# Resource ReSerVation Protocol
gre	47	GRE		# Generic Routing Encapsulation
dsr	48	DSR		# Dynamic Source Routing Protocol
bna	49	BNA		# BNA
esp	50	ESP		# encapsulating security payload
ah	51	AH		# authentication header
i-nlsp	52	I-NLSP		# Integrated Net Layer Security TUBA
swipe	53	SWIPE		# IP with Encryption
narp	54	NARP		# NBMA Address Resolution Protocol
mobile	55	MOBILE		# IP Mobility
tlsp	56	TLSP		# Transport Layer Security Protocol
skip	57	SKIP		# SKIP
ipv6-icmp	58	IPV6-ICMP	icmp6	# ICMP for IPv6
ipv6-nonxt	59	IPV6-NONXT	# no next header for ipv6
ipv6-opts	60	IPV6-OPTS	# destination options for ipv6
#	61			# any host internal protocol
cftp	62	CFTP		# CFTP
#	63			# any local network
sat-expak	64	SAT-EXPAK	# SATNET and Backroom EXPAK
kryptolan	65	KRYPTOLAN	# Kryptolan
rvd	66	RVD		# MIT Remote Virtual Disk Protocol
ippc	67	IPPC		# Internet Pluribus Packet Core
#	68			# any distributed filesystem
sat-mon	69	SAT-MON		# SATNET Monitoring
visa	70	VISA		# VISA Protocol
ipcv	71	IPCV		# Internet Packet Core Utility
cpnx	72	CPNX		# Computer Protocol Network Executive
cphb	73	CPHB		# Computer Protocol Heart Beat
wsn	74	WSN		# Wang Span Network
pvp	75	PVP		# Packet Video Protocol
br-sat-mon	76	BR-SAT-MON	# Backroom SATNET Monitoring
sun-nd	77	SUN-ND		# SUN ND PROTOCOL-Temporary
wb-mon	78	WB-MON		# WIDEBAND Monitoring
wb-expak	79	WB-EXPAK	# WIDEBAND EXPAK
iso-ip	80	ISO-IP		# ISO Internet Protocol
vmtp	81	VMTP		# Versatile Message Transport
secure-vmtp	82	SECURE-VMTP	# SECURE-VMTP
vines	83	VINES		# VINES
ttp	84	TTP		# TTP
#iptm	84	IPTM		# Protocol Internet Protocol Traffic
nsfnet-igp	85	NSFNET-IGP	# NSFNET-IGP
dgp	86	DGP		# Dissimilar Gateway Protocol
tcf	87	TCF		# TCF
eigrp	88	EIGRP		# Enhanced Interior Routing Protocol (Cisco)
ospf	89	OSPFIGP		# Open Shortest Path First IGP
sprite-rpc	90	Sprite-RPC	# Sprite RPC Protocol
larp	91	LARP		# Locus Address Resolution Protocol
mtp	92	MTP		# Multicast Transport Protocol
ax.25	93	AX.25		# AX.25 Frames
ipip	94	IPIP		# Yet Another IP encapsulation
micp	95	MICP		# Mobile Internetworking Control Pro.
scc-sp	96	SCC-SP		# Semaphore Communications Sec. Pro.
etherip	97	ETHERIP		# Ethernet-within-IP Encapsulation
encap	98	ENCAP		# Yet Another IP encapsulation
#	99			# any private encryption scheme
gmtp	100	GMTP		# GMTP
ifmp	101	IFMP		# Ipsilon Flow Management Protocol
pnni	102	PNNI		# PNNI over IP
pim	103	PIM		# Protocol Independent Multicast
aris	104	ARIS		# ARIS
scps	105	SCPS		# SCPS
qnx	106	QNX		# QNX
a/n	107	A/N		# Active Networks
ipcomp	108	IPComp		# IP Payload Compression Protocol
snp	109	SNP		# Sitara Networks Protocol
compaq-peer	110	Compaq-Peer	# Compaq Peer Protocol
ipx-in-ip	111	IPX-in-IP	# IPX in IP
carp	112	CARP	vrrp		# Common Address Redundancy Protocol
pgm	113	PGM		# PGM Reliable Transport Protocol
#	114			# any 0-hop protocol
l2tp	115	L2TP		# Layer Two Tunneling Protocol
ddx	116	DDX		# D-II Data Exchange
iatp	117	IATP		# Interactive Agent Transfer Protocol
stp	118	STP		# Schedule Transfer Protocol
srp	119	SRP		# SpectraLink Radio Protocol
uti	120	UTI		# UTI
smp	121	SMP		# Simple Message Protocol
sm	122	SM		# SM
ptp	123	PTP		# Performance Transparency Protocol
isis	124	ISIS		# ISIS over IPv4
fire	125	FIRE
crtp	126	CRTP		# Combat Radio Transport Protocol
crudp	127	CRUDP		# Combat Radio User Datagram
sscopmce	128	SSCOPMCE
iplt	129	IPLT
sps	130	SPS		# Secure Packet Shield
pipe	131	PIPE		# Private IP Encapsulation within IP
sctp	132	SCTP		# Stream Control Transmission Protocol
fc	133	FC		# Fibre Channel
rsvp-e2e-ignore	134	RSVP-E2E-IGNORE	# Aggregation of RSVP for IP reservations
mobility-header	135	Mobility-Header	# Mobility Support in IPv6
udplite	136	UDPLite		# The UDP-Lite Protocol
mpls-in-ip	137	MPLS-IN-IP	# Encapsulating MPLS in IP
manet	138	MANET		# MANET Protocols (RFC5498)
hip	139	HIP		# Host Identity Protocol (RFC5201)
shim6	140	SHIM6		# Shim6 Protocol (RFC5533)
wesp	141	WESP		# Wrapped Encapsulating Security Payload (RFC5840)
rohc	142	ROHC		# Robust Header Compression (RFC5858)
#	138-254			# Unassigned
pfsync	240	PFSYNC		# PF Synchronization
#	253-254			# Use for experimentation and testing (RFC3692)
#	255			# Reserved
```
</details>

<br><br>

# sockaddr_in 구조체

- socket에 naming하는 `bind`함수에서 사용된다.
- 사용될 포트번호, ip주소를 부여한다.

```c
struct sockaddr_in {
	sa_family_t		sin_family; //주소체계, address family
	uint16_t		sin_port; //16bit tcp/udp port번호
	struct in_addr	sin_addr; //32bit ip주소
	char			sin_zero[8]; //사용 x
};

struct in_addr {
	in_addr_t	s_addr; //32bit IPv4 인터넷 주소
};
```

<br>

## sin_family

- 주소체계를 정의한다.
- socket에서 사용된 PF_INET과 여기서 사용할 수 있는 AF_INET은 같은 상수값을 지니고있다.

```
AF_INET //IPv4 인터넷 프로토콜 주소체계
AF_INET6 //IPv6 인터넷 프로토콜 주소체계
AF_LOCAL //로컬통신위한 유닉스 프로토콜 주소체계
```

<br>

## sin_port

- 네트워크 바이트 순서로 16bit PORT번호를 저장한다.

- 네트워크 바이트?

<details markdown="1">
<summary>네트워크 바이트</summary>

- CPU마다 빅엔디안인지, 리틀엔디안인지 데이터 저장 방식이 다르다.
- 따라서, 데이터를 전송할때도 순서가 달라지게된다.
- 이 문제를 해결하기위해, `네트워크 바이트 순서`가 따로 규약으로 존재하고,
- 네트워크 상으로 데이터를 전달하는 모든 컴퓨터는 `빅엔디안`방식으로 데이터를 전송하기로 규약했다.
</details>

<br>

## sin_addr

- 32bit IP주소정보 저장, 네트워크 바이트 순서대로 저장해야한다.

<details markdown="1"> 
<summary>문자열로된 주소를 네트워크 바이트순서 정수로 변환 </summary>

```c
#include <arpa/inet.h>

in_addr_t inet_addr(const char *string);
```

- 파싱에 매우 유용한 함수.
- 문자열로 존재하는 dot notation 된 주소를 32bit 네트워크바이트 주소로 변환한다.
- "127.0.0.1" -> ((unsigned int)0 - 1) -> `4294967295`
- localhost는 `htonl(INADDR_ANY)`로 대체할 수 있다.

```cpp
const char *addr = "127.0.0.1";
if (!inet_aton(addr, &address.sin_addr))
{
    std::cerr << "Failed to set address"<<addr<<std::endl;
	return 1;
}
```

</details>

<br><br>

# setsockopt

- socket 옵션 값을 설정하는 시스템콜.

```cpp

#include <sys/types.h>
#include <sys/socket.h>

int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);
```

<br>

- sockfd: 옵션 값을 설정할 소켓의 파일 디스크립터입니다.
- level: 옵션 값을 설정할 프로토콜 레벨(level)을 지정합니다. 예를 들어, SOL_SOCKET은 일반 소켓 옵션을 지정하며, IPPROTO_TCP는 TCP 소켓 옵션을 지정합니다.
- optname: 설정할 옵션의 이름입니다.
- optval: 설정할 옵션의 값을 가리키는 포인터입니다.
- optlen: optval이 가리키는 값의 길이입니다.

<br>

### SOL_SOCKET vs IPPROTO_TCP

- 이 소켓 옵션이 어느 레벨에서 적용되는지를 의미한다.
- SOL_SOCKET: Internet layer의 socket option 설정 시 사용.
- IPPROTO_TCP: Transport layer의 socket option 설정 시 사용.

<br>

- optname이름, optval의 값으로 표현되는 option이 어느 internet layer에서 적용될 지 결정.
- 각 level에따라 적용될 수 있는 option이 다르다.

<br>

### options

- SOL_SOCKET 옵션
  - SO_ACCEPTCONN: 소켓이 수신 대기 상태인지 여부를 설정합니다.
  - SO_BROADCAST: 소켓이 브로드캐스트 메시지를 전송할 수 있는지 여부를 설정합니다.
  - SO_DEBUG: 소켓 디버깅을 사용할지 여부를 설정합니다.
  - SO_DONTROUTE: 소켓이 경로 탐색을 사용하지 않고 직접 목적지로 전송할지 여부를 설정합니다.
  - SO_ERROR: 소켓 오류를 검색할 때 사용합니다.
  - SO_KEEPALIVE: TCP 연결이 유지되는 동안 주기적으로 keepalive 패킷을 보낼지 여부를 설정합니다.
  - SO_LINGER: 소켓이 닫힐 때 보류 중인 데이터를 처리하는 방법을 설정합니다.
  - SO_RCVBUF: 소켓의 수신 버퍼 크기를 설정합니다.
  - SO_RCVLOWAT: read() 함수가 반환하기 위해 수신 대기 중인 데이터의 최소 크기를 설정합니다.
  - SO_RCVTIMEO: 소켓 수신 작업의 시간 초과 값을 설정합니다.
  - SO_REUSEADDR: 서버 소켓이 종료된 후에도 동일한 포트 번호를 재사용할 수 있도록 합니다.
  - SO_SNDBUF: 소켓의 송신 버퍼 크기를 설정합니다.
  - SO_SNDLOWAT: write() 함수가 반환하기 위해 송신 대기 중인 데이터의 최소 크기를 설정합니다.
  - SO_SNDTIMEO: 소켓 송신 작업의 시간 초과 값을 설정합니다.

<br>

- IPPROTO_TCP 옵션

  - TCP_NODELAY: Nagle 알고리즘을 비활성화하여 작은 데이터 패킷을 즉시 전송할 수 있도록 합니다.
  - TCP_MAXSEG: TCP 세그먼트 크기를 설정합니다.

- `이 option들 별로 사용되는 optval의 의미가 다르다.`
  - SO_REUSEPORT의 경우 optval이 1이면 활성화, 0이면 비활성화
  - SO_SNDBUF의 경우, 소켓 송신 버퍼 크기 지정의 역할.

<br><br>

# bind

- 소켓(sockfd)에 주소정보(sockaddr)을 부여한다.

```c
#include <sys/socket.h>

int bind(int sockfd, struct sockaddr *myaddr, socklen_t addrlen);
```

# listen

- 연결 대기상태, 클라이언트가 연결요청가능한 상태가된다.

```c
#include <sys/socket.h>

int listen(int sock, int backlog);
```

- pending connection queue의 길이를 의미한다.
- 꽉차게되면, 이후에 요청한 client는 `ECONNREFUSED`를 받는다.
- `worker-connections`를 의미한다.
