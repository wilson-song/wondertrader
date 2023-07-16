/*!
 * \file WtCtaEngine.h
 * \project	WonderTrader
 *
 * \author Wesley
 * \date 2020/03/30
 * 
 * \brief 
 */
#pragma once
#include "../Includes/ICtaStraCtx.h"
#include "WtExecMgr.h"
#include "WtEngine.h"

NS_WTP_BEGIN
class WTSVariant;
typedef std::shared_ptr<ICtaStraCtx> CtaContextPtr;

class WtCtaRtTicker;

class WtCtaEngine : public WtEngine, public IExecutorStub
{
public:
	WtCtaEngine();
	virtual ~WtCtaEngine();

public:
	//////////////////////////////////////////////////////////////////////////
	//WtEngine接口
	void handle_push_quote(WTSTickData* newTick, uint32_t hotFlag) override;

	void on_tick(const char* stdCode, WTSTickData* curTick) override;

	void on_bar(const char* stdCode, const char* period, uint32_t times, WTSBarStruct* newBar) override;

	void on_init() override;
	void on_session_begin() override;
	void on_session_end() override;

	void run(bool executor) override;

	void init(WTSVariant* cfg, IBaseDataMgr* bdMgr, WtDtMgr* dataMgr, IHotMgr* hotMgr, EventNotifier* notifier) override;

	bool isInTrading() override;
	uint32_t transTimeToMin(uint32_t uTime) override;

	///////////////////////////////////////////////////////////////////////////
	//IExecutorStub 接口
	uint64_t get_real_time() override;
	WTSCommodityInfo* get_comm_info(const char* stdCode) override;
	WTSSessionInfo* get_sess_info(const char* stdCode) override;
	IHotMgr* get_hot_mon() override { return _hot_mgr; }
	virtual uint32_t get_trading_day() { return _cur_tdate; }


public:
	void on_schedule(uint32_t curDate, uint32_t curTime);	

	void handle_pos_change(const char* straName, const char* stdCode, double diffPos);

	void addContext(const CtaContextPtr& ctx);
	
	CtaContextPtr	getContext(uint32_t id);

	inline void addExecutor(const ExecCmdPtr& executor)
	{
		_exec_mgr.add_executer(executor);
        executor->setStub(this);
	}

	inline bool loadRouterRules(WTSVariant* cfg)
	{
		return _exec_mgr.load_router_rules(cfg);
	}

public:
	void notify_chart_marker(uint64_t time, const char* straId, double price, const char* icon, const char* tag);
	void notify_chart_index(uint64_t time, const char* straId, const char* idxName, const char* lineName, double val);
	void notify_trade(const char* straId, const char* stdCode, bool isLong, bool isOpen, uint64_t curTime, double price, const char* userTag);

private:
	typedef faster_hashmap<uint32_t, CtaContextPtr> ContextMap;
	ContextMap		_ctx_map;

	WtCtaRtTicker*	_tm_ticker;

	WtExecuterMgr	_exec_mgr;

	WTSVariant*		_cfg;
};

NS_WTP_END

