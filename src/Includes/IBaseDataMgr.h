/*!
 * \file IBaseDataMgr.h
 * \project	WonderTrader
 *
 * \author Wesley
 * \date 2020/03/30
 * 
 * \brief 基础数据管理器接口定义
 */
#pragma once
#include <string>
#include <cstdint>

#include "WTSMarcos.h"
#include "FasterDefs.h"

NS_WTP_BEGIN
typedef CodeSet ContractSet;

class WTSContractInfo;
class WTSArray;
class WTSSessionInfo;
class WTSCommodityInfo;

typedef faster_hashset<uint32_t> HolidaySet;
typedef struct _TradingDayTpl
{
	uint32_t	_cur_tdate;
	HolidaySet	_holidays;

	_TradingDayTpl() :_cur_tdate(0){}
} TradingDayTpl;

class IBaseDataMgr
{
public:
	virtual WTSCommodityInfo*	getCommodity(const char* exchgpid)						= 0;
	virtual WTSCommodityInfo*	getCommodity(const char* exchange, const char* pid)		= 0;

	virtual WTSContractInfo*	getContract(const char* code, const char* exchange)	= 0;
	virtual WTSArray*			getContracts(const char* exchange)					= 0;

	virtual WTSSessionInfo*		getSession(const char* sid)						= 0;
	virtual WTSSessionInfo*		getSessionByCode(const char* code, const char* exchange) = 0;
	virtual WTSArray*			getAllSessions() = 0;

	virtual bool				isHoliday(const char* pid, uint32_t uDate, bool isTpl) = 0;

	virtual uint32_t			calcTradingDate(const char* stdPID, uint32_t uDate, uint32_t uTime, bool isSession) = 0;
	virtual uint64_t			getBoundaryTime(const char* stdPID, uint32_t tDate, bool isSession, bool isStart) = 0;
};
NS_WTP_END