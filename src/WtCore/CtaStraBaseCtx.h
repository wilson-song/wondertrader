/*!
 * \file CtaStraBaseCtx.h
 * \project	WonderTrader
 *
 * \author Wesley
 * \date 2020/03/30
 * 
 * \brief 
 */
#pragma once
#include "../Includes/ICtaStraCtx.h"
#include "../Includes/FasterDefs.h"
#include "../Includes/WTSDataDef.hpp"

#include "../Share/BoostFile.hpp"
#include "../Share/fmtlib.h"
#include "../Share/SpinMutex.hpp"

#include <unordered_map>

class CtaStrategy;

NS_WTP_BEGIN

class WtCtaEngine;

const char COND_ACTION_OL = 0;	//开多
const char COND_ACTION_CL = 1;	//平多
const char COND_ACTION_OS = 2;	//开空
const char COND_ACTION_CS = 3;	//平空
const char COND_ACTION_SP = 4;	//直接设置仓位

typedef struct CondEntrust
{
	WTSCompareField _field{};
	WTSCompareType	_alg{};
	double			_target{};
	double			_qty{};
	char			_action{};	//0-开多, 1-平多, 2-开空, 3-平空
	char			_code[MAX_INSTRUMENT_LENGTH]{};
	char			_usertag[32]{};

	CondEntrust()
	{
		memset(this, 0, sizeof(CondEntrust));
	}

} CondEntrust;

typedef std::vector<CondEntrust>	CondList;
typedef faster_hashmap<LongKey, CondList>	CondEntrustMap;


class CtaStraBaseCtx : public ICtaStraCtx
{
public:
	CtaStraBaseCtx(WtCtaEngine* engine, const char* name, int32_t slippage);
	~CtaStraBaseCtx() override;

private:
	void	init_outputs();
	inline void log_signal(const char* stdCode, double target, double price, uint64_t gentime, const char* usertag = "");
	inline void	log_trade(const char* stdCode, bool isLong, bool isOpen, uint64_t curTime, double price, double qty, const char* userTag = "", double fee = 0.0, uint32_t barNo = 0);
	inline void	log_close(const char* stdCode, bool isLong, uint64_t openTime, double openpx, uint64_t closeTime, double closepx, double qty,
		double profit, double totalprofit = 0, const char* enterTag = "", const char* exitTag = "", uint32_t openBarNo = 0, uint32_t closeBarNo = 0);

	void	save_data(uint32_t flag = 0xFFFFFFFF);
	void	load_data(uint32_t flag = 0xFFFFFFFF);

	void	load_userdata();
	void	save_userdata();

	void	update_dyn_profit(const char* stdCode, double price);

	void	do_set_position(const char* stdCode, double qty, const char* userTag = "", bool bFireAtOnce = false);
	void	append_signal(const char* stdCode, double qty, const char* userTag = "", uint32_t sigType = 0);

	inline CondList& get_cond_entrusts(const char* stdCode);

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

	void	dump_chart_info();

public:
	uint32_t id() override { return _context_id; }

	//回调函数
	void on_init() override;
	void on_session_begin(uint32_t uTDate) override;
	void on_session_end(uint32_t uTDate) override;
	void on_tick(const char* stdCode, WTSTickData* newTick, bool bEmitStrategy /*= true*/) override;
	void on_bar(const char* stdCode, const char* period, uint32_t times, WTSBarStruct* newBar) override;
	bool on_schedule(uint32_t curDate, uint32_t curTime) override;

	void enum_position(FuncEnumCtaPosCallBack cb, bool bForExecute /*= false*/) override;


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

	double stra_get_fund_data(int flag /* = 0 */) override;

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

	/*
	 *	获取分月合约代码
	 */
	std::string		stra_get_rawcode(const char* stdCode) override;

	void stra_sub_ticks(const char* stdCode) override;

	void stra_log_info(const char* message) override;
	void stra_log_debug(const char* message) override;
	void stra_log_warn(const char* message) override;
	void stra_log_error(const char* message) override;

	void stra_save_user_data(const char* key, const char* val) override;

	const char* stra_load_user_data(const char* key, const char* defVal = "") override;

	const char* stra_get_last_entertag(const char* stdCode) override;

public:
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

protected:
	uint32_t		_context_id;
	WtCtaEngine*	_engine;

	int32_t			_slippage;

	uint64_t		_total_calc_time;	//总计算时间
	uint32_t		_emit_times;		//总计算次数

	std::string		_main_key;
	std::string		_main_code;
	std::string		_main_period;

	typedef struct KlineTag
	{
		bool			_closed;

		KlineTag() :_closed(false){}

	} KlineTag;
	typedef faster_hashmap<LongKey, KlineTag> KlineTags;
	KlineTags	_kline_tags;

	typedef faster_hashmap<LongKey, double> PriceMap;
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
		uint32_t	_frozen_date;

		std::vector<DetailInfo> _details;

		PosInfo()
		{
			_volume = 0;
			_closeprofit = 0;
			_dynprofit = 0;
			_last_entertime = 0;
			_last_exittime = 0;
			_frozen = 0;
			_frozen_date = 0;
		}
	} PosInfo;
	typedef faster_hashmap<LongKey, PosInfo> PositionMap;
	PositionMap		_pos_map;

	typedef struct SigInfo
	{
		double		_volume;
		std::string	_usertag;
		double		_sigprice;
		uint32_t	_sigtype;	// 0-onschedule信号，1-ontick信号，2-条件单信号
		uint64_t	_gentime;
		bool		_triggered;

		SigInfo()
		{
			_volume = 0;
			_sigprice = 0;
			_sigtype = 0;
			_gentime = 0;
			_triggered = false;
		}
	}SigInfo;
	typedef faster_hashmap<LongKey, SigInfo>	SignalMap;
	SignalMap		_sig_map;

	BoostFilePtr	_trade_logs;
	BoostFilePtr	_close_logs;
	BoostFilePtr	_fund_logs;
	BoostFilePtr	_sig_logs;
	BoostFilePtr	_pos_logs;
	BoostFilePtr	_idx_logs;
	BoostFilePtr	_mark_logs;

	CondEntrustMap	_condtions;
	uint64_t		_last_cond_min;	//上次设置条件单的时间
	uint32_t		_last_barno;	//上次设置的K线编号

	//是否处于调度中的标记
	bool			_is_in_schedule;	//是否在自动调度中

	//用户数据
	typedef faster_hashmap<LongKey, std::string> StringHashMap;
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

	//tick订阅列表
	faster_hashset<LongKey> _tick_subs;

	//////////////////////////////////////////////////////////////////////////
	//图表相关
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
		faster_hashmap<std::string, ChartLine> _lines;
		faster_hashmap<std::string, double> _base_lines;
	} ChartIndex;

	faster_hashmap<LongKey, ChartIndex>	_chart_indice;

private:
	SpinMutex		_mutex;
};


NS_WTP_END