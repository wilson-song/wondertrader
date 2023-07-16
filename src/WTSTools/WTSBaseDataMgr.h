/*!
 * \file WTSBaseDataMgr.h
 * \project	WonderTrader
 *
 * \author Wesley
 * \date 2020/03/30
 * 
 * \brief 基础数据管理器实现
 */
#pragma once
#include "../Includes/IBaseDataMgr.h"
#include "../Includes/WTSCollection.hpp"
#include "../Includes/FasterDefs.h"

USING_NS_WTP;

typedef faster_hashmap<ShortKey, TradingDayTpl>	TradingDayTplMap;

typedef WTSHashMap<LongKey>		WTSContractList;
typedef WTSHashMap<ShortKey>	WTSExchgContract;
typedef WTSHashMap<LongKey>		WTSContractMap;

typedef WTSHashMap<ShortKey>		WTSSessionMap;
typedef WTSHashMap<ShortKey>		WTSCommodityMap;

typedef faster_hashmap<ShortKey, CodeSet> SessionCodeMap;


class WTSBaseDataMgr : public IBaseDataMgr
{
public:
	WTSBaseDataMgr();
	~WTSBaseDataMgr();

public:
	WTSCommodityInfo*	getCommodity(const char* stdPID) override;
	WTSCommodityInfo*	getCommodity(const char* exchange, const char* pid) override;

	WTSContractInfo*	getContract(const char* code, const char* exchange) override;
	WTSArray*			getContracts(const char* exchange) override;

	WTSSessionInfo*		getSession(const char* sid) override;
	WTSSessionInfo*		getSessionByCode(const char* code, const char* exchange) override;
	WTSArray*			getAllSessions() override;
	bool				isHoliday(const char* stdPID, uint32_t uDate, bool isTpl) override;


	uint32_t			calcTradingDate(const char* stdPID, uint32_t uDate, uint32_t uTime, bool isSession) override;
	uint64_t			getBoundaryTime(const char* stdPID, uint32_t tDate, bool isSession, bool isStart) override;
	void		release();


	bool		loadSessions(const char* filename);
	bool		loadCommodities(const char* filename);
	bool		loadContracts(const char* filename);
	bool		loadHolidays(const char* filename);

public:
	uint32_t	getTradingDate(const char* stdPID, uint32_t uOffDate = 0, uint32_t uOffMinute = 0, bool isTpl = false);
	uint32_t	getNextTDate(const char* stdPID, uint32_t uDate, int days = 1, bool isTpl = false);
	uint32_t	getPrevTDate(const char* stdPID, uint32_t uDate, int days = 1, bool isTpl = false);
	bool		isTradingDate(const char* stdPID, uint32_t uDate, bool isTpl = false);
	void		setTradingDate(const char* stdPID, uint32_t uDate, bool isTpl = false);

	CodeSet*	getSessionComms(const char* sid);

private:
	const char* getTplIDByPID(const char* stdPID);

private:
	TradingDayTplMap	m_mapTradingDay;

	SessionCodeMap		m_mapSessionCode;

	WTSExchgContract*	m_mapExchgContract;
	WTSSessionMap*		m_mapSessions;
	WTSCommodityMap*	m_mapCommodities;
	WTSContractMap*		m_mapContracts;
};

