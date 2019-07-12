//////////////////////////////////////////////////////////////////////////
//SaveBeforeSendPacketHandle.cpp

#include "stdafx.h"
#include "SaveBeforeSendPacketHandle.h"
#include "FluxControlObserver.h"

CSaveBeforeSendPacketHandle::CSaveBeforeSendPacketHandle(CFluxControlObserver * pFluxControlObserver)
{		
	m_pFluxControlObserver = pFluxControlObserver;

	return;
}

const char* CSaveBeforeSendPacketHandle::BeforeSendPacket(int card_index_array,const char * pdata, unsigned int datalen, unsigned int& outdatalen)
{
	if( m_pFluxControlObserver->m_mirrorout_mgr.IsMirroring() )
	{
		BOOL bFitCard = FALSE;

		for(int card_index = 0; card_index < 32; ++card_index)
		{
			if( card_index_array&(1<<card_index) )
			{
				bFitCard = m_pFluxControlObserver->m_mirrorout_mgr.IsMirrorFromCardIndex(card_index);

				if( bFitCard )
					break;
			}
		}

		if( bFitCard )
		{
			m_pFluxControlObserver->m_mirrorout_mgr.write_packet(PACKET_OK,pdata,datalen,-1);
		}
	}

	if( m_pFluxControlObserver->m_cap_data_mgr_ex.IsCapping() )
	{
		BOOL bFitCard = FALSE;

		for(int card_index = 0; card_index < 32; ++card_index)
		{
			if( card_index_array&(1<<card_index) )
			{
				bFitCard = m_pFluxControlObserver->m_cap_data_mgr_ex.IsCapFromCardIndex(card_index);

				if( bFitCard )
					break;
			}
		}

		if( bFitCard )
		{
			m_pFluxControlObserver->m_cap_data_mgr_ex.write_packet(PACKET_OK,pdata,datalen,-1);
		}
	}

	outdatalen = datalen;

	return pdata;
}
