#include "c3.h"
#include "net_util.h"
#include "net_report.h"
#include "network.h"
#include "net_info.h"

NetReport::NetReport(NET_REPORT type)
{
	m_type = type;
}

void NetReport::Packetize(uint8 *buf, uint16 &size)
{
	buf[0] = k_PACKET_REPORT_ID >> 8;
	buf[1] = k_PACKET_REPORT_ID & 0xff;

	size = 2;
	PUSHBYTE(m_type);
}

void NetReport::Unpacketize(uint16 id, uint8 *buf, uint16 size)
{
	Assert(MAKE_CIV3_ID(buf[0], buf[1]) == k_PACKET_REPORT_ID);

	sint32 pos = 2;

	PULLBYTETYPE(m_type,NET_REPORT);
	switch(m_type) {
		case NET_REPORT_READY_FOR_DATA:
		{
			BOOL wasReady = g_network.ReadyToStart();

			g_network.ProcessNewPlayer(id);




			if(!wasReady && g_network.ReadyToStart()) {
				g_network.Enqueue(new NetInfo(NET_INFO_CODE_ALL_PLAYERS_READY));
			}

			break;
		}
		case NET_REPORT_ACK_RESYNC:
		{
			DPRINTF(k_DBG_NET, ("Client %d acks resync\n", g_network.IdToIndex(id)));
			g_network.AckResync(g_network.IdToIndex(id));
			break;
		}
		default:
			Assert(FALSE);
			break;
	}
}
