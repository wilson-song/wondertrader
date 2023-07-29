/*!
 * \file HftStraBaseCtx.h
 * \project	WonderTrader
 *
 * \author Wesley
 * \date 2020/03/30
 *
 * \brief
 */
#pragma once

#include "../Includes/FasterDefs.h"
#include "../Includes/IHftStraCtx.h"
#include "../Share/BoostFile.hpp"
#include "../Share/fmtlib.h"

#include <boost/circular_buffer.hpp>

#include "ITrdNotifySink.h"

NS_WTP_BEGIN

class WtHftEngine;
class TraderAdapter;

class HftStraBaseCtx : public IHftStraCtx, public ITrdNotifySink
{
public:
	HftStraBaseCtx(WtHftEngine* engine, const char* name, bool bAgent, int32_t slippage);
	~HftStraBaseCtx() override;

	void setTrader(TraderAdapter* trader);

public:
	//////////////////////////////////////////////////////////////////////////
	//IHftStraCtx 接口
	uint32_t id() override;

	void on_init() override;

	void on_tick(const char* stdCode, WTSTickData* newTick) override;

	void on_order_queue(const char* stdCode, WTSOrdQueData* newOrdQue) override;

	void on_order_detail(const char* stdCode, WTSOrdDtlData* newOrdDtl) override;

	void on_transaction(const char* stdCode, WTSTransData* newTrans) override;

	void on_bar(const char* stdCode, const char* period, uint32_t times, WTSBarStruct* newBar) override;

	void on_session_begin(uint32_t uTDate) override;

	void on_session_end(uint32_t uTDate) override;

	bool stra_cancel(uint32_t localid) override;

	OrderIDs stra_cancel(const char* stdCode, bool isBuy, double qty) override;

	/*
	 *	下单接口: 买入
	 *
	 *	@stdCode	合约代码
	 *	@price		下单价格，0则是市价单
	 *	@qty		下单数量
	 *	@flag		下单标志: 0-normal，1-fak，2-fok，默认0
	 */
	OrderIDs stra_buy(const char* stdCode, double price, double qty, const char* userTag, int flag = 0, bool bForceClose = false) override;

	/*
	 *	下单接口: 卖出
	 *
	 *	@stdCode	合约代码
	 *	@price		下单价格，0则是市价单
	 *	@qty		下单数量
	 *	@flag		下单标志: 0-normal，1-fak，2-fok，默认0
	 */
	OrderIDs stra_sell(const char* stdCode, double price, double qty, const char* userTag, int flag = 0, bool bForceClose = false) override;

	/*
	 *	下单接口: 开多
	 *
	 *	@stdCode	合约代码
	 *	@price		下单价格，0则是市价单
	 *	@qty		下单数量
	 *	@flag		下单标志: 0-normal，1-fak，2-fok
	 */
	uint32_t	stra_enter_long(const char* stdCode, double price, double qty, const char* userTag, int flag = 0) override;

	/*
	 *	下单接口: 开空
	 *
	 *	@stdCode	合约代码
	 *	@price		下单价格，0则是市价单
	 *	@qty		下单数量
	 *	@flag		下单标志: 0-normal，1-fak，2-fok
	 */
	uint32_t	stra_enter_short(const char* stdCode, double price, double qty, const char* userTag, int flag = 0) override;

	/*
	 *	下单接口: 平多
	 *
	 *	@stdCode	合约代码
	 *	@price		下单价格，0则是市价单
	 *	@qty		下单数量
	 *	@isToday	是否今仓，默认false
	 *	@flag		下单标志: 0-normal，1-fak，2-fok，默认0
	 */
	uint32_t	stra_exit_long(const char* stdCode, double price, double qty, const char* userTag, bool isToday = false, int flag = 0) override;

	/*
	 *	下单接口: 平空
	 *
	 *	@stdCode	合约代码
	 *	@price		下单价格，0则是市价单
	 *	@qty		下单数量
	 *	@isToday	是否今仓，默认false
	 *	@flag		下单标志: 0-normal，1-fak，2-fok，默认0
	 */
	uint32_t	stra_exit_short(const char* stdCode, double price, double qty, const char* userTag, bool isToday = false, int flag = 0) override;

	WTSCommodityInfo* stra_get_comminfo(const char* stdCode) override;

	WTSKlineSlice* stra_get_bars(const char* stdCode, const char* period, uint32_t count) override;

	WTSTickSlice* stra_get_ticks(const char* stdCode, uint32_t count) override;

	WTSOrdDtlSlice*	stra_get_order_detail(const char* stdCode, uint32_t count) override;

	WTSOrdQueSlice*	stra_get_order_queue(const char* stdCode, uint32_t count) override;

	WTSTransSlice*	stra_get_transaction(const char* stdCode, uint32_t count) override;

	WTSTickData* stra_get_last_tick(const char* stdCode) override;

	/*
	 *	获取分月合约代码
	 */
	std::string		stra_get_rawcode(const char* stdCode) override;

	void stra_log_info(const char* message) override;
	void stra_log_debug(const char* message) override;
	void stra_log_warn(const char* message) override;
	void stra_log_error(const char* message) override;

	double stra_get_position(const char* stdCode, bool bOnlyValid = false, int flag = 3) override;
	double stra_get_position_avgpx(const char* stdCode) override;
	double stra_get_position_profit(const char* stdCode) override;
	double stra_get_price(const char* stdCode) override;
	double stra_get_undone(const char* stdCode) override;

	uint32_t stra_get_date() override;
	uint32_t stra_get_time() override;
	uint32_t stra_get_secs() override;

	void stra_sub_ticks(const char* stdCode) override;
	void stra_sub_order_details(const char* stdCode) override;
	void stra_sub_order_queues(const char* stdCode) override;
	void stra_sub_transactions(const char* stdCode) override;

	void stra_save_user_data(const char* key, const char* val) override;

	const char* stra_load_user_data(const char* key, const char* defVal = "") override;

	//////////////////////////////////////////////////////////////////////////
	void on_trade(uint32_t localid, const char* stdCode, bool isBuy, double vol, double price) override;

	void on_order(uint32_t localid, const char* stdCode, bool isBuy, double totalQty, double leftQty, double price, bool isCanceled) override;

	void on_channel_ready() override;

	void on_channel_lost() override;

	void on_entrust(uint32_t localid, const char* stdCode, bool bSuccess, const char* message) override;

	void on_position(const char* stdCode, bool isLong, double prevol, double preavail, double newvol, double newavail, uint32_t tradingday) override;

protected:
	template<typename... Args>
	void log_debug(const char* format, const Args& ...args)
	{
		const char* buffer = fmtutil::format(format, args...);
		stra_log_debug(buffer);
	}

	template<typename... Args>
	void log_info(const char* format, const Args& ...args)
	{
		const char* buffer = fmtutil::format(format, args...);
		stra_log_info(buffer);
	}

	template<typename... Args>
	void log_error(const char* format, const Args& ...args)
	{
		const char* buffer = fmtutil::format(format, args...);
		stra_log_error(buffer);
	}

protected:
	const char* get_inner_code(const char* stdCode);

	void	load_userdata();
	void	save_userdata();

	void	init_outputs();

	void	do_set_position(const char* stdCode, double qty, double price = 0.0, const char* userTag = "");
	void	update_dyn_profit(const char* stdCode, WTSTickData* newTick);

	inline void	log_trade(const char* stdCode, bool isLong, bool isOpen, uint64_t curTime, double price, double qty, double fee, const char* userTag);
	inline void	log_close(const char* stdCode, bool isLong, uint64_t openTime, double openpx, uint64_t closeTime, double closepx, double qty,
		double profit, double maxprofit, double maxloss, double totalprofit, const char* enterTag, const char* exitTag);

	inline const char* getOrderTag(uint32_t localid)
	{
		thread_local static OrderTag oTag;
		oTag._localid = localid;
		auto it = std::lower_bound(_orders.begin(), _orders.end(), oTag, [](const OrderTag& a, const OrderTag& b) {
			return a._localid < b._localid;
		});

		if (it == _orders.end())
			return "";

		return (*it)._usertag;
	}


	inline void setUserTag(uint32_t localid, const char* usertag)
	{
		_orders.push_back({ localid, usertag });
	}

	inline void eraseOrderTag(uint32_t localid)
	{
		thread_local static OrderTag oTag;
		oTag._localid = localid;
		auto it = std::lower_bound(_orders.begin(), _orders.end(), oTag, [](const OrderTag& a, const OrderTag& b) {
			return a._localid < b._localid;
		});

		if (it == _orders.end())
			return;

		_orders.erase(it);
	}

protected:
	uint32_t		_context_id;
	WtHftEngine*	_engine;
	TraderAdapter*	_trader{};
	int32_t			_slippage;

	faster_hashmap<LongKey, std::string> _code_map;

	BoostFilePtr	_sig_logs;
	BoostFilePtr	_close_logs;
	BoostFilePtr	_trade_logs;
	BoostFilePtr	_fund_logs;

	//用户数据
	typedef faster_hashmap<LongKey, std::string> StringHashMap;
	StringHashMap	_user_datas;
	bool			_ud_modified{};

	bool			_data_agent;	//数据托管

	//tick订阅列表
	faster_hashset<LongKey> _tick_subs;

private:
	typedef struct DetailInfo
	{
		bool		_long{};
		double		_price{};
		double		_volume{};
		uint64_t	_opentime{};
		uint32_t	_opentdate{};
		double		_max_profit{};
		double		_max_loss{};
		double		_profit{};
		char		_usertag[32]{};

		DetailInfo()
		{
			memset(this, 0, sizeof(DetailInfo));
		}
	} DetailInfo;

	typedef struct PosInfo
	{
		double		_volume;
		double		_closeprofit;
		double		_dynprofit;

		std::vector<DetailInfo> _details;

		PosInfo()
		{
			_volume = 0;
			_closeprofit = 0;
			_dynprofit = 0;
		}
	} PosInfo;
	typedef faster_hashmap<LongKey, PosInfo> PositionMap;
	PositionMap		_pos_map;

	typedef struct OrderTag
	{
		uint32_t	_localid{};
		char		_usertag[64] = { 0 };

		OrderTag()= default;
		OrderTag(uint32_t localid, const char* usertag)
		{
			_localid = localid;
			wt_strcpy(_usertag, usertag);
		}
	} OrderTag;
	//typedef faster_hashmap<uint32_t, LongKey> OrderMap;
	//OrderMap		_orders;
	boost::circular_buffer<OrderTag> _orders;

	typedef struct StraFundInfo
	{
		double	_total_profit{};
		double	_total_dynprofit{};
		double	_total_fees{};

		StraFundInfo()
		{
			memset(this, 0, sizeof(StraFundInfo));
		}
	} StraFundInfo;

	StraFundInfo		_fund_info;

	typedef faster_hashmap<LongKey, double> PriceMap;
	PriceMap		_price_map;
};

NS_WTP_END
