/*!
 * \file CtaMocker.h
 * \project	WonderTrader
 *
 * \author Wesley
 * \date 2020/03/30
 * 
 * \brief 
 */
#pragma once
#include <sstream>
#include <atomic>
#include <unordered_map>
#include "HisDataReplayer.h"

#include "../Includes/FasterDefs.h"
#include "../Includes/ICtaStraCtx.h"
#include "../Includes/CtaStrategyDefs.h"
#include "../Includes/WTSDataDef.hpp"
#include "../Includes/WTSCollection.hpp"

#include "../Share/DLLHelper.hpp"
#include "../Share/StdUtils.hpp"
#include "../Share/fmtlib.h"

NS_WTP_BEGIN
class EventNotifier;
NS_WTP_END

USING_NS_WTP;

class HisDataReplayer;
class CtaStrategy;

const char COND_ACTION_OL = 0;	//开多
const char COND_ACTION_CL = 1;	//平多
const char COND_ACTION_OS = 2;	//开空
const char COND_ACTION_CS = 3;	//平空
const char COND_ACTION_SP = 4;	//直接设置仓位

typedef struct CondEntrust
{
	WTSCompareField _field;
	WTSCompareType	_alg;
	double			_target{};

	double			_qty{};

	char			_action{};	//0-开多,1-平多,2-开空,3-平空

	char			_code[MAX_INSTRUMENT_LENGTH]{};
	char			_usertag[32]{};


	CondEntrust()
	{
		memset(this, 0, sizeof(CondEntrust));
	}

} CondEntrust;

typedef std::vector<CondEntrust>	CondList;
typedef faster_hashmap<std::string, CondList>	CondEntrustMap;


class CtaMocker : public ICtaStraCtx, public IDataSink
{
public:
	CtaMocker(HisDataReplayer* replayer, const char* name, int32_t slippage = 0, bool persistData = true, EventNotifier* notifier = nullptr);
	~CtaMocker() override;

private:
	void	dump_outputs();
	void	dump_stradata();
	void	dump_chartdata();
	inline void log_signal(const char* stdCode, double target, double price, uint64_t gentime, const char* usertag = "");
	inline void	log_trade(const char* stdCode, bool isLong, bool isOpen, uint64_t curTime, double price, double qty, const char* userTag = "", double fee = 0.0, uint32_t barNo = 0);
	inline void	log_close(const char* stdCode, bool isLong, uint64_t openTime, double openpx, uint64_t closeTime, double closepx, double qty,
		double profit, double maxprofit, double maxloss, double totalprofit = 0, const char* enterTag = "", const char* exitTag = "", uint32_t openBarNo = 0, uint32_t closeBarNo = 0);

	void	update_dyn_profit(const char* stdCode, double price);

	void	do_set_position(const char* stdCode, double qty, double price = 0.0, const char* userTag = "");
	void	append_signal(const char* stdCode, double qty, const char* userTag, double price, uint32_t sigType);

	inline CondList& get_cond_entrusts(const char* stdCode);

	void	proc_tick(const char* stdCode, double last_px, double cur_px);

public:
	bool	init_cta_factory(WTSVariant* cfg);
	void	load_incremental_data(const char* lastBacktestName);
	void	install_hook();
	void	enable_hook(bool bEnabled = true);
	bool	step_calc();

public:
	//////////////////////////////////////////////////////////////////////////
	//IDataSink
	void	handle_tick(const char* stdCode, WTSTickData* curTick, uint32_t pxType = 0) override;
	void	handle_bar_close(const char* stdCode, const char* period, uint32_t times, WTSBarStruct* newBar) override;
	void	handle_schedule(uint32_t uDate, uint32_t uTime) override;

	void	handle_init() override;
	void	handle_session_begin(uint32_t curTDate) override;
	void	handle_session_end(uint32_t curTDate) override;

	void	handle_section_end(uint32_t curTDate, uint32_t curTime) override;

	void	handle_replay_done() override;

	//////////////////////////////////////////////////////////////////////////
	//ICtaStraCtx
	uint32_t id() override { return _context_id; }

	//回调函数
	void on_init() override;
	void on_session_begin(uint32_t curTDate) override;
	void on_session_end(uint32_t curTDate) override;
	void on_tick(const char* stdCode, WTSTickData* newTick, bool bEmitStrategy = true) override;
	void on_bar(const char* stdCode, const char* period, uint32_t times, WTSBarStruct* newBar) override;
	bool on_schedule(uint32_t curDate, uint32_t curTime) override;
	void enum_position(FuncEnumCtaPosCallBack cb, bool bForExecute) override;

	void on_tick_updated(const char* stdCode, WTSTickData* newTick) override;
	void on_bar_close(const char* stdCode, const char* period, WTSBarStruct* newBar) override;
	void on_calculate(uint32_t curDate, uint32_t curTime) override;


	//////////////////////////////////////////////////////////////////////////
	//策略接口
	void stra_enter_long(const char* stdCode, double qty, const char* userTag = "", double limitprice = 0.0, double stopprice = 0.0) override;
	void stra_enter_short(const char* stdCode, double qty, const char* userTag = "", double limitprice = 0.0, double stopprice = 0.0) override;
	void stra_exit_long(const char* stdCode, double qty, const char* userTag = "", double limitprice = 0.0, double stopprice = 0.0) override;
	void stra_exit_short(const char* stdCode, double qty, const char* userTag = "", double limitprice = 0.0, double stopprice = 0.0) override;

	double stra_get_position(const char* stdCode, bool bOnlyValid = false, const char* userTag = "") override;
	void stra_set_position(const char* stdCode, double qty, const char* userTag = "", double limitprice = 0.0, double stopprice = 0.0) override;
	double stra_get_price(const char* stdCode) override;

	/*
	 *	读取当日价格
	 */
	double stra_get_day_price(const char* stdCode, int flag = 0) override;

	uint32_t stra_get_tdate() override;
	uint32_t stra_get_date() override;
	uint32_t stra_get_time() override;

	double stra_get_fund_data(int flag = 0) override;

	uint64_t stra_get_first_entertime(const char* stdCode) override;
	uint64_t stra_get_last_entertime(const char* stdCode) override;
	uint64_t stra_get_last_exittime(const char* stdCode) override;
	double stra_get_last_enterprice(const char* stdCode) override;
	double stra_get_position_avgpx(const char* stdCode) override;
	double stra_get_position_profit(const char* stdCode) override;

	uint64_t stra_get_detail_entertime(const char* stdCode, const char* userTag) override;
	double stra_get_detail_cost(const char* stdCode, const char* userTag) override;
	double stra_get_detail_profit(const char* stdCode, const char* userTag, int flag = 0) override;

	WTSCommodityInfo* stra_get_comminfo(const char* stdCode) override;
	WTSKlineSlice*	stra_get_bars(const char* stdCode, const char* period, uint32_t count, bool isMain = false) override;
	WTSTickSlice*	stra_get_ticks(const char* stdCode, uint32_t count) override;
	WTSTickData*	stra_get_last_tick(const char* stdCode) override;

	void stra_sub_ticks(const char* stdCode) override;

	/*
	 *	获取分月合约代码
	 */
	std::string		stra_get_rawcode(const char* stdCode) override;

	void stra_log_info(const char* message) override;
	void stra_log_debug(const char* message) override;
	void stra_log_warn(const char* message) override;
	void stra_log_error(const char* message) override;

	void stra_save_user_data(const char* key, const char* val) override;

	const char* stra_load_user_data(const char* key, const char* defVal = "") override;

	const char* stra_get_last_entertag(const char* stdCode) override;

	/*
	 *	设置图表K线
	 */
	void set_chart_kline(const char* stdCode, const char* period) override;

	/*
	 *	添加信号
	 */
	void add_chart_mark(double price, const char* icon, const char* tag) override;

	/*
	 *	添加指标
	 */
	void register_index(const char* idxName, uint32_t indexType) override;

	/*
	 *	添加指标线
	 */
	bool register_index_line(const char* idxName, const char* lineName, uint32_t lineType) override;

	/*
	 *	添加基准线
	 *	@idxName	指标名称
	 *	@lineName	线条名称
	 *	@val		数值
	 */
	bool add_index_baseline(const char* idxName, const char* lineName, double val) override;

	/*
	 *	设置指标值
	 */
	bool set_index_value(const char* idxName, const char* lineName, double val) override;

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

protected:
	uint32_t			_context_id;
	HisDataReplayer*	_replayer;

	uint64_t		_total_calc_time;	//总计算时间
	uint32_t		_emit_times;		//总计算次数

	int32_t			_slippage;			//成交滑点

	uint32_t		_schedule_times;	//调度次数

	std::string		_main_key;

	std::string		_main_code;
	std::string		_main_period;

	typedef struct KlineTag
	{
		bool			_closed;

		KlineTag() :_closed(false){}

	} KlineTag;
	typedef faster_hashmap<std::string, KlineTag> KlineTags;
	KlineTags	_kline_tags;

	typedef faster_hashmap<std::string, double> PriceMap;
	PriceMap		_price_map;

	typedef struct DetailInfo
	{
		bool		_long{};
		double		_price{};
		double		_volume{};
		uint64_t	_opentime{};
		uint32_t	_opentdate{};
		double		_max_profit{};
		double		_max_loss{};
		double		_max_price{};
		double		_min_price{};
		double		_profit{};
		char		_opentag[32]{};
		uint32_t	_open_barno{};

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
		uint64_t	_last_entertime;
		uint64_t	_last_exittime;
		double		_frozen;

		std::vector<DetailInfo> _details;

		PosInfo()
		{
			_volume = 0;
			_closeprofit = 0;
			_dynprofit = 0;
			_frozen = 0;
			_last_entertime = 0;
			_last_exittime = 0;
		}

		inline double valid() const { return _volume - _frozen; }
	} PosInfo;
	typedef faster_hashmap<std::string, PosInfo> PositionMap;
	PositionMap		_pos_map;
	double	_total_closeprofit;

	typedef struct SigInfo
	{
		double		_volume;
		std::string	_usertag;
		double		_sigprice;
		double		_desprice;
		uint32_t	_sigtype;
		uint64_t	_gentime;

		SigInfo()
		{
			_volume = 0;
			_sigprice = 0;
			_desprice = 0;
			_sigtype = 0;
			_gentime = 0;
		}
	}SigInfo;
	typedef faster_hashmap<std::string, SigInfo>	SignalMap;
	SignalMap		_sig_map;

	std::stringstream	_trade_logs;
	std::stringstream	_close_logs;
	std::stringstream	_fund_logs;
	std::stringstream	_sig_logs;
	std::stringstream	_pos_logs;
	std::stringstream	_index_logs;
	std::stringstream	_mark_logs;

	CondEntrustMap		_condtions;

	//是否处于调度中的标记
	bool			_is_in_schedule;	//是否在自动调度中

	//用户数据
	typedef faster_hashmap<std::string, std::string> StringHashMap;
	StringHashMap	_user_datas;
	bool			_ud_modified;

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

	typedef struct StraFactInfo
	{
		std::string		_module_path;
		DllHandle		_module_inst;
		ICtaStrategyFactory*	_fact;
		FuncCreateStrategyFact	_creator{};
		FuncDeleteStrategyFact	_remover{};

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

	CtaStrategy*	_strategy;
	EventNotifier*	_notifier;

	StdUniqueMutex	_mtx_calc;
	StdCondVariable	_cond_calc;
	bool			_has_hook;		//这是人为控制是否启用钩子
	bool			_hook_valid;	//这是根据是否是异步回测模式而确定钩子是否可用
	std::atomic<uint32_t>		_cur_step;	//临时变量，用于控制状态

	bool			_in_backtest;
	bool			_wait_calc;

	//是否对回测结果持久化
	bool			_persist_data;

	uint32_t		_cur_tdate{};
	uint32_t		_cur_bartime{};
	uint64_t		_last_cond_min{};

	//tick订阅列表
	faster_hashset<std::string> _tick_subs;

	std::string		_chart_code;
	std::string		_chart_period;

	typedef struct ChartLine
	{
		std::string	_name;
		uint32_t	_lineType;
	} ChartLine;

	typedef struct ChartIndex
	{
		std::string	_name;
		uint32_t	_indexType;
		std::unordered_map<std::string, ChartLine> _lines;
		std::unordered_map<std::string, double> _base_lines;
	} ChartIndex;

	std::unordered_map<std::string, ChartIndex>	_chart_indice;

	typedef faster_hashmap<std::string, WTSTickStruct>	TickCache;
	TickCache	_ticks;
};