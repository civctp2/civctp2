









#include "c3.h"
#include "Unit.h"
#include "Gold.h"
#include "Advances.h"
#include "AgreementData.h"
#include "Agreement.h"
#include "AgreementPool.h"
#include "player.h"
#include "pollution.h"
#include "AICause.h"
#include "civarchive.h"
#include "AgreementDynArr.h"
#include "Globals.h"
#include "gamefile.h"
	
	extern	Player	**g_player ;

	extern Pollution *g_thePollution ;









AgreementPool::AgreementPool() : ObjPool(k_BIT_GAME_OBJ_TYPE_AGREEMENT)
	{
	}









AgreementPool::AgreementPool(CivArchive &archive) : ObjPool(k_BIT_GAME_OBJ_TYPE_AGREEMENT)
	{
	Serialize(archive) ;
	}












void AgreementPool::Serialize(CivArchive &archive)
	{
	AgreementData	*newData ;

	sint32	i,
			count = 0 ;

    CHECKSERIALIZE

#define AGREEMENTPOOL_MAGIC 0x28400399
	if (archive.IsStoring())
		{
		}
	else
		{
			if(g_saveFileVersion < 55) {
				archive.TestMagic(AGREEMENTPOOL_MAGIC) ;
				archive>>count;
				for (i=0; i<count; i++)
				{
					newData = new AgreementData(archive) ;
					Insert(newData) ;
				}
			}

		}

	}
	
	







Agreement AgreementPool::Create(PLAYER_INDEX owner, PLAYER_INDEX recipient, AGREEMENT_TYPE request)
	{
	AgreementData* newData;
	Agreement newAgreement(NewKey(k_BIT_GAME_OBJ_TYPE_AGREEMENT));
	
	newData = new AgreementData(newAgreement, owner, recipient, request) ;
	Insert(newData) ;

	if(g_player[owner])
		g_player[owner]->AddAgreement(newAgreement) ;					

	if(g_player[recipient])
		g_player[recipient]->AddAgreement(newAgreement) ;				

	return (newAgreement) ;
	}












void AgreementPool::EndRound(void)
	{
	sint32	i ;

	AgreementDynamicArray	expired ;

	for(i = 0; i < k_OBJ_POOL_TABLE_SIZE; i++)
		{
		if(m_table[i])
			{
			AgreementData	*agreeData = (AgreementData *)(m_table[i]) ;				
#if 0
			
			
			
			
			if (agreeData->GetAgreement() == AGREEMENT_TYPE_PACT_END_POLLUTION)	
				{
				
				if (g_thePollution->GetTrend() == k_TREND_DOWNWARD)
					{
					
					
					expired.Insert(m_table[i]->m_id) ;
					}

				}
#endif

			if (agreeData->IsExpired())								
				{



				
				expired.Insert(m_table[i]->m_id) ;					
				}

			agreeData->DecrementTurns() ;
			}

		}

    expired.KillList() ;											
	}

AgreementData *AgreementPool::AccessAgreement(const Agreement id)
{
	return ((AgreementData*)Access((const ID)id)) ;
}

AgreementData *AgreementPool::GetAgreement(const Agreement id) const
{
	return ((AgreementData*)Get((const ID)id)) ;
}
