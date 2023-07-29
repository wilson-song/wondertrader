/*!
 * \file HftMocker.h
 * \project	WonderTrader
 *
 * \author Wesley
 * \date 2020/03/30
 * 
 * \brief 
 */
#pragma once
#include <queue>
#include <sstream>

#include "HisDataReplayer.h"

#include "../Includes/FasterDefs.h"
#include "../Includes/IHftStraCtx.h"
#include "../Includes/HftStrategyDefs.h"

#include "../Share/StdUtils.hpp"
#include "../Share/DLLHelper.hpp"
#include "../Share/fmtlib.h"

class HisDataReplayer;

class HftMocker : public IDataSink, public IHftStraCtx
{
public:
	HftMocker(HisDataReplayer* replayer, const char* name);
	~HftMocker() override;

private:
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

public:
	//////////////////////////////////////////////////////////////////////////
	//IDataSink
	void	handle_tick(const char* stdCode, WTSTickData* curTick, uint32_t pxType) override;
	void	handle_order_queue(const char* stdCode, WTSOrdQueData* curOrdQue) override;
	void	handle_order_detail(const char* stdCode, WTSOrdDtlData* curOrdDtl) override;
	void	handle_transaction(const char* stdCode, WTSTransData* curTrans) override;

	void	handle_bar_close(const char* stdCode, const char* period, uint32_t times, WTSBarStruct* newBar) override;
	void	handle_schedule(uint32_t uDate, uint32_t uTime) override;

	void	handle_init() override;
	void	handle_session_begin(uint32_t curTDate) override;
	void	handle_session_end(uint32_t curTDate) override;

	void	handle_replay_done() override;

	void	on_tick_updated(const char* stdCode, WTSTickData* newTick) override;
	void	on_ordque_updated(const char* stdCode, WTSOrdQueData* newOrdQue) override;
	void	on_orddtl_updated(const char* stdCode, WTSOrdDtlData* newOrdDtl) override;
	void	on_trans_updated(const char* stdCode, WTSTransData* newTrans) override;

	//////////////////////////////////////////////////////////////////////////
	//IHftStraCtx
	void on_tick(const char* stdCode, WTSTickData* newTick) override;

	void on_order_queue(const char* stdCode, WTSOrdQueData* newOrdQue) override;

	void on_order_detail(const char* stdCode, WTSOrdDtlData* newOrdDtl) override;

	void on_transaction(const char* stdCode, WTSTransData* newTrans) override;

	uint32_t id() override;

	void on_init() override;

	void on_bar(const char* stdCode, const char* period, uint32_t times, WTSBarStruct* newBar) override;

	void on_session_begin(uint32_t curTDate) override;

	void on_session_end(uint32_t curTDate) override;

	bool stra_cancel(uint32_t localid) override;

	OrderIDs stra_cancel(const char* stdCode, bool isBuy, double qty = 0) override;

	OrderIDs stra_buy(const char* stdCode, double price, double qty, const char* userTag, int flag = 0, bool bForceClose = false) override;

	OrderIDs stra_sell(const char* stdCode, double price, double qty, const char* userTag, int flag = 0, bool bForceClose = false) override;

	WTSCommodityInfo* stra_get_comminfo(const char* stdCode) override;

	WTSKlineSlice* stra_get_bars(const char* stdCode, const char* period, uint32_t count) override;

	WTSTickSlice* stra_get_ticks(const char* stdCode, uint32_t count) override;

	WTSOrdDtlSlice*	stra_get_order_detail(const char* stdCode, uint32_t count) override;

	WTSOrdQueSlice*	stra_get_order_queue(const char* stdCode, uint32_t count) override;

	WTSTransSlice*	stra_get_transaction(const char* stdCode, uint32_t count) override;

	WTSTickData* stra_get_last_tick(const char* stdCode) override;

	/*
	 *	��ȡ���º�Լ����
	 */
	std::string		stra_get_rawcode(const char* stdCode) override;

	double stra_get_position(const char* stdCode, bool bOnlyValid = false, int flag = 3) override;

	double stra_get_position_avgpx(const char* stdCode) override;

	double stra_get_position_profit(const char* stdCode) override;

	double stra_get_undone(const char* stdCode) override;

	double stra_get_price(const char* stdCode) override;

	uint32_t stra_get_date() override;

	uint32_t stra_get_time() override;

	uint32_t stra_get_secs() override;

	void stra_sub_ticks(const char* stdCode) override;

	void stra_sub_order_queues(const char* stdCode) override;

	void stra_sub_order_details(const char* stdCode) override;

	void stra_sub_transactions(const char* stdCode) override;

	void stra_log_info(const char* message) override;
	void stra_log_debug(const char* message) override;
	void stra_log_warn(const char* message) override;
	void stra_log_error(const char* message) override;

	void stra_save_user_data(const char* key, const char* val) override;

	const char* stra_load_user_data(const char* key, const char* defVal = "") override;

	//////////////////////////////////////////////////////////////////////////
	virtual void on_trade(uint32_t localid, const char* stdCode, bool isBuy, double vol, double price, const char* userTag);

	virtual void on_order(uint32_t localid, const char* stdCode, bool isBuy, double totalQty, double leftQty, double price, bool isCanceled, const char* userTag);

	virtual void on_channel_ready();

	virtual void on_entrust(uint32_t localid, const char* stdCode, bool bSuccess, const char* message, const char* userTag);

public:
	bool	init_hft_factory(WTSVariant* cfg);
	void	install_hook();
	void	enable_hook(bool bEnabled = true);
	void	step_tick();

private:
	typedef std::function<void()> Task;
	void	postTask(const Task& task);
	void	procTask();

	bool	procOrder(uint32_t localid);

	void	do_set_position(const char* stdCode, double qty, double price = 0.0, const char* userTag = "");
	void	update_dyn_profit(const char* stdCode, WTSTickData* newTick);

	void	dump_outputs();
	inline void	log_trade(const char* stdCode, bool isLong, bool isOpen, uint64_t curTime, double price, double qty, double fee, const char* userTag);
	inline void	log_close(const char* stdCode, bool isLong, uint64_t openTime, double openpx, uint64_t closeTime, double closepx, double qty,
		double profit, double maxprofit, double maxloss, double totalprofit, const char* enterTag, const char* exitTag);

private:
	HisDataReplayer*	_replayer;

	bool			_use_newpx;
	uint32_t		_error_rate;
	bool			_match_this_tick;	//�Ƿ��ڵ�ǰtick���

	typedef faster_hashmap<std::string, double> PriceMap;
	PriceMap		_price_map;


	typedef struct StraFactInfo
	{
		std::string		_module_path;
		DllHandle		_module_inst;
		IHftStrategyFact*	_fact;
		FuncCreateHftStraFact	_creator{};
		FuncDeleteHftStraFact	_remover{};

		StraFactInfo()
		{
			_module_inst = nullptr;
			_fact = nullptr;
		}

		~StraFactInfo()
		{
			if (_fact)
				_remover(_fact);
		}
	} StraFactInfo;
	StraFactInfo	_factory;

	HftStrategy*	_strategy;

	StdUniqueMutex		_mtx;
	std::queue<Task>	_tasks;

	StdRecurMutex		_mtx_control;

	typedef struct OrderInfo
	{
		bool	_isBuy{};
		char	_code[32]{};
		double	_price{};
		double	_total{};
		double	_left{};
		char	_usertag[32]{};
		
		uint32_t	_localid{};

		bool	_proced_after_placed{};	//�µ����Ƿ����

		OrderInfo()
		{
			memset(this, 0, sizeof(OrderInfo));
		}

		OrderInfo(const struct OrderInfo& rhs)
		{
			memcpy(this, &rhs, sizeof(OrderInfo));
		}

		OrderInfo& operator =(const struct OrderInfo& rhs)
		{
			memcpy(this, &rhs, sizeof(OrderInfo));
			return *this;
		}

	} OrderInfo;
	typedef std::shared_ptr<OrderInfo> OrderInfoPtr;
	typedef faster_hashmap<uint32_t, OrderInfoPtr> Orders;
	StdRecurMutex	_mtx_ords;
	Orders			_orders;

	typedef WTSHashMap<std::string> CommodityMap;
	CommodityMap*	_commodities;

	//�û�����
	typedef faster_hashmap<std::string, std::string> StringHashMap;
	StringHashMap	_user_datas;
	bool			_ud_modified{};

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
		double		_frozen;

		std::vector<DetailInfo> _details;

		PosInfo()
		{
			_volume = 0;
			_closeprofit = 0;
			_dynprofit = 0;
			_frozen = 0;
		}

		inline double valid() const { return _volume - _frozen; }
	} PosInfo;
	typedef faster_hashmap<std::string, PosInfo> PositionMap;
	PositionMap		_pos_map;

	std::stringstream	_trade_logs;
	std::stringstream	_close_logs;
	std::stringstream	_fund_logs;
	std::stringstream	_sig_logs;
	std::stringstream	_pos_logs;

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

protected:
	uint32_t		_context_id;

	StdUniqueMutex	_mtx_calc;
	StdCondVariable	_cond_calc;
	bool			_has_hook;		//������Ϊ�����Ƿ����ù���
	bool			_hook_valid;	//���Ǹ����Ƿ����첽�ز�ģʽ��ȷ�������Ƿ����
	std::atomic<bool>	_resumed;	//��ʱ���������ڿ���״̬

	//tick�����б�
	faster_hashset<std::string> _tick_subs;

	typedef WTSHashMap<std::string>	TickCache;
	TickCache*	_ticks;
};

