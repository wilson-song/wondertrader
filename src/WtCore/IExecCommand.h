#pragma once
#include "../Includes/FasterDefs.h"
#include <cstdint>

NS_WTP_BEGIN
class WTSCommodityInfo;
class WTSSessionInfo;
class IHotMgr;
class WTSTickData;

class IExecutorStub
{
public:
	virtual uint64_t get_real_time() = 0;
	virtual WTSCommodityInfo* get_comm_info(const char* stdCode) = 0;
	virtual WTSSessionInfo* get_sess_info(const char* stdCode) = 0;
	virtual IHotMgr* get_hot_mon() = 0;
	virtual uint32_t get_trading_day() = 0;
};

class IExecCommand
{
public:
	explicit IExecCommand(const char* name) :_stub(nullptr), _name(name){}
	/*
	 *	设置目标仓位
	 */
	virtual void set_position(const faster_hashmap<LongKey, double>& targets) {}

	/*
	 *	合约仓位变动
	 */
	virtual void on_position_changed(const char* stdCode, double diffPos) {}

	/*
	 *	实时行情回调
	 */
	virtual void on_tick(const char* stdCode, WTSTickData* newTick) {}


	inline void setStub(IExecutorStub* stub) { _stub = stub; }

	inline const char* name() const { return _name.c_str(); }

	inline void setName(const char* name) { _name = name; }

protected:
	IExecutorStub*	_stub;
	std::string		_name;
};
NS_WTP_END