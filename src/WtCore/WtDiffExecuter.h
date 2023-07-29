/*!
 * \file WtExecuter.h
 * \project	WonderTrader
 *
 * \author Wesley
 * \date 2020/03/30
 * 
 * \brief 
 */
#pragma once

#include "ITrdNotifySink.h"
#include "IExecCommand.h"
#include "WtExecuterFactory.h"
#include "../Includes/ExecuteDefs.h"
#include "../Share/threadpool.hpp"
#include "../Share/SpinMutex.hpp"

NS_WTP_BEGIN
class WTSVariant;
class IDataManager;
class IBaseDataMgr;
class TraderAdapter;
class IHotMgr;

//本地执行器
class WtDiffExecuter : public ExecuteContext,
		public ITrdNotifySink, public IExecCommand
{
public:
	WtDiffExecuter(WtExecuterFactory* factory, const char* name, IDataManager* dataMgr, IBaseDataMgr* bdMgr);
	~WtDiffExecuter() override;

public:
	/*
	 *	初始化执行器
	 *	传入初始化参数
	 */
	bool init(WTSVariant* params);

	void setTrader(TraderAdapter* adapter);

private:
	ExecuteUnitPtr	getUnit(const char* code, bool bAutoCreate = true);

	void	save_data();
	void	load_data();

public:
	//////////////////////////////////////////////////////////////////////////
	//ExecuteContext
	WTSTickSlice* getTicks(const char* code, uint32_t count, uint64_t etime /*= 0*/) override;

	WTSTickData*	grabLastTick(const char* code) override;

	double		getPosition(const char* stdCode, bool validOnly, int32_t flag) override;
	OrderMap*	getOrders(const char* code) override;
	double		getUndoneQty(const char* code) override;

	OrderIDs	buy(const char* code, double price, double qty, bool bForceClose /*= false*/) override;
	OrderIDs	sell(const char* code, double price, double qty, bool bForceClose /*= false*/) override;
	bool		cancel(uint32_t localid) override;
	OrderIDs	cancel(const char* code, bool isBuy, double qty) override;
	void		writeLog(const char* message) override;

	WTSCommodityInfo*	getCommodityInfo(const char* stdCode) override;
	WTSSessionInfo*		getSessionInfo(const char* stdCode) override;

	uint64_t	getCurTime() override;

public:
	/*
	 *	设置目标仓位
	 */
	void set_position(const faster_hashmap<LongKey, double>& targets) override;


	/*
	 *	合约仓位变动
	 */
	void on_position_changed(const char* stdCode, double diffPos) override;

	/*
	 *	实时行情回调
	 */
	void on_tick(const char* stdCode, WTSTickData* newTick) override;

	/*
	 *	成交回报
	 */
	void on_trade(uint32_t localid, const char* stdCode, bool isBuy, double vol, double price) override;

	/*
	 *	订单回报
	 */
	void on_order(uint32_t localid, const char* stdCode, bool isBuy, double totalQty, double leftQty, double price, bool isCanceled /*= false*/) override;

	/*
	 *	
	 */
	void on_position(const char* stdCode, bool isLong, double prevol, double preavail, double newvol, double newavail, uint32_t tradingday) override;

	/*
	 *	
	 */
	void on_entrust(uint32_t localid, const char* stdCode, bool bSuccess, const char* message) override;

	/*
	 *	交易通道就绪
	 */
	void on_channel_ready() override;

	/*
	 *	交易通道丢失
	 */
	void on_channel_lost() override;

	/*
	 *	资金回报
	 */
	void on_account(const char* currency, double prebalance, double balance, double dynbalance,
		double avaliable, double closeprofit, double dynprofit, double margin, double fee, double deposit, double withdraw) override;


private:
	ExecuteUnitMap		_unit_map;
	TraderAdapter*		_trader;
	WtExecuterFactory*	_factory;
	IDataManager*		_data_mgr;
	IBaseDataMgr*		_bd_mgr;
	WTSVariant*			_config{};

	double				_scale;				//放大倍数
	bool				_channel_ready;

	SpinMutex			_mtx_units;

	faster_hashmap<LongKey, double> _target_pos;
	faster_hashmap<LongKey, double> _diff_pos;

	typedef std::shared_ptr<boost::threadpool::pool> ThreadPoolPtr;
	ThreadPoolPtr		_pool;
};
NS_WTP_END
