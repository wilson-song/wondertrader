/*!
* \file MfMocker.h
* \project	WonderTrader
*
* \author Wesley
* \date 2020/03/30
*
* \brief
*/
#pragma once
#include <sstream>
#include "HisDataReplayer.h"

#include "../Includes/FasterDefs.h"
#include "../Includes/ISelStraCtx.h"
#include "../Includes/SelStrategyDefs.h"
#include "../Includes/WTSDataDef.hpp"
#include "../Share/fmtlib.h"
#include "../Share/DLLHelper.hpp"

class SelStrategy;

USING_NS_WTP;

class HisDataReplayer;

class SelMocker : public ISelStraCtx, public IDataSink
{
public:
	SelMocker(HisDataReplayer* replayer, const char* name, int32_t slippage = 0);
	~SelMocker() override;

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

private:
	void	dump_outputs();
	void	dump_stradata();
	inline void log_signal(const char* stdCode, double target, double price, uint64_t gentime, const char* usertag = "");
	inline void	log_trade(const char* stdCode, bool isLong, bool isOpen, uint64_t curTime, double price, double qty, const char* userTag = "", double fee = 0.0);
	inline void	log_close(const char* stdCode, bool isLong, uint64_t openTime, double openpx, uint64_t closeTime, double closepx, double qty,
		double profit, double totalprofit = 0, const char* enterTag = "", const char* exitTag = "");

	void	update_dyn_profit(const char* stdCode, double price);

	void	do_set_position(const char* stdCode, double qty, double price = 0.0, const char* userTag = "", bool bTriggered = false);
	void	append_signal(const char* stdCode, double qty, const char* userTag = "", double price = 0.0);

public:
	bool	init_sel_factory(WTSVariant* cfg);

public:
	//////////////////////////////////////////////////////////////////////////
	//IDataSink
	void	handle_tick(const char* stdCode, WTSTickData* curTick, uint32_t pxType) override;
	void	handle_bar_close(const char* stdCode, const char* period, uint32_t times, WTSBarStruct* newBar) override;
	void	handle_schedule(uint32_t uDate, uint32_t uTime) override;

	void	handle_init() override;
	void	handle_session_begin(uint32_t uCurDate) override;
	void	handle_session_end(uint32_t uCurDate) override;
	void	handle_replay_done() override;

	//////////////////////////////////////////////////////////////////////////
	//ICtaStraCtx
	uint32_t id() override { return _context_id; }

	//�ص�����
	void on_init() override;
	void on_session_begin(uint32_t curTDate) override;
	void on_session_end(uint32_t curTDate) override;
	void on_tick(const char* stdCode, WTSTickData* newTick, bool bEmitStrategy = true) override;
	void on_bar(const char* stdCode, const char* period, uint32_t times, WTSBarStruct* newBar) override;
	bool on_schedule(uint32_t curDate, uint32_t curTime, uint32_t fireTime) override;
	void enum_position(FuncEnumSelPositionCallBack cb) override;

	void on_tick_updated(const char* stdCode, WTSTickData* newTick) override;
	void on_bar_close(const char* stdCode, const char* period, WTSBarStruct* newBar) override;
	void on_strategy_schedule(uint32_t curDate, uint32_t curTime) override;


	//////////////////////////////////////////////////////////////////////////
	//���Խӿ�
	double stra_get_position(const char* stdCode, bool bOnlyValid = false, const char* userTag = "") override;
	void stra_set_position(const char* stdCode, double qty, const char* userTag = "") override;
	double stra_get_price(const char* stdCode) override;

	uint32_t stra_get_date() override;
	uint32_t stra_get_time() override;

	WTSCommodityInfo* stra_get_comminfo(const char* stdCode) override;
	WTSSessionInfo* stra_get_sessinfo(const char* stdCode) override;
	WTSKlineSlice*	stra_get_bars(const char* stdCode, const char* period, uint32_t count) override;
	WTSTickSlice*	stra_get_ticks(const char* stdCode, uint32_t count) override;
	WTSTickData*	stra_get_last_tick(const char* stdCode) override;

	/*
	 *	��ȡ���º�Լ����
	 */
	std::string		stra_get_rawcode(const char* stdCode) override;

	void stra_sub_ticks(const char* stdCode) override;

	void stra_log_info(const char* message) override;
	void stra_log_debug(const char* message) override;
	void stra_log_warn(const char* message) override;
	void stra_log_error(const char* message) override;

	void stra_save_user_data(const char* key, const char* val) override;

	const char* stra_load_user_data(const char* key, const char* defVal = "") override;

protected:
	uint32_t			_context_id;
	HisDataReplayer*	_replayer;

	uint64_t		_total_calc_time;	//�ܼ���ʱ��
	uint32_t		_emit_times;		//�ܼ������
	int32_t			_slippage;	//�ɽ�����

	std::string		_main_key;

	typedef struct KlineTag
	{
		bool			_closed;

		KlineTag() :_closed(false){}

	} KlineTag;
	typedef faster_hashmap<std::string, KlineTag> KlineTags;
	KlineTags	_kline_tags;

	typedef std::pair<double, uint64_t>	PriceInfo;
	typedef faster_hashmap<std::string, PriceInfo> PriceMap;
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
		double		_profit{};
		char		_opentag[32]{};

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

	typedef struct SigInfo
	{
		double		_volume;
		std::string	_usertag;
		double		_sigprice;
		double		_desprice;
		bool		_triggered;
		uint64_t	_gentime;

		SigInfo()
		{
			_volume = 0;
			_sigprice = 0;
			_desprice = 0;
			_triggered = false;
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

	//�Ƿ��ڵ����еı��
	bool			_is_in_schedule;	//�Ƿ����Զ�������

	//�û�����
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
		ISelStrategyFact*		_fact;
		FuncCreateSelStraFact	_creator{};
		FuncDeleteSelStraFact	_remover{};

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

	SelStrategy*	_strategy;

	uint32_t		_cur_tdate{};

	//tick�����б�
	faster_hashset<std::string> _tick_subs;
};