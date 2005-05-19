

#include "c3.h"

#include "c3_utilitydialogbox.h"
#include "c3dialogs.h"

#include "player.h"

extern Player **g_player;





ForeignTradeBidInfo::ForeignTradeBidInfo(sint32 player, Unit &fromCity, 
											Unit &toCity, sint32 resource)
											:m_player(player),
											 m_fromCity(fromCity),
											 m_toCity(toCity),
											 m_resource(resource)
{
}

void c3dialogs_PostForeignTradeBidDialog(sint32 player, Unit &fromCity, Unit &toCity, 
								sint32 resource)
{
	ForeignTradeBidInfo			*info = new ForeignTradeBidInfo(player, fromCity, 
									toCity, resource);

	
	
	
	static c3_UtilityTextFieldPopup	*pop = NULL;
	if(!pop) {
		pop = new c3_UtilityTextFieldPopup(
			c3dialogs_ForeignTradeBidDialogCallback,
			NULL, 
			NULL, 
			NULL,
			"ForeignTradeBidPopup", 
			(void *)info);
	}
	pop->m_data = info;
	pop->DisplayWindow();
}

void c3dialogs_ForeignTradeBidDialogCallback(MBCHAR *text, sint32 accepted, void *data)
{
	ForeignTradeBidInfo	*info = (ForeignTradeBidInfo *)data;

	if (accepted) {
		sint32 price = atoi(text);
		g_player[info->m_player]->SendTradeBid(info->m_fromCity, info->m_resource, 
													info->m_toCity, price);
	}

	delete info;
}


