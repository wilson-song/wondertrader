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
class TraderAdapter;
class IHotMgr;

//����ִ����
class WtArbiExecuter : public ExecuteContext,
	public ITrdNotifySink, public IExecCommand
{
public:
	WtArbiExecuter(WtExecuterFactory* factory, const char* name, IDataManager* dataMgr);
	virtual ~WtArbiExecuter();

public:
	/*
	 *	��ʼ��ִ����
	 *	�����ʼ������
	 */
	bool init(WTSVariant* params);

	void setTrader(TraderAdapter* adapter);

private:
	ExecuteUnitPtr	getUnit(const char* code, bool bAutoCreate = true);

public:
	//////////////////////////////////////////////////////////////////////////
	//ExecuteContext
	virtual WTSTickSlice*	getTicks(const char* code, uint32_t count, uint64_t etime = 0) override;

	virtual WTSTickData*	grabLastTick(const char* code) override;

	virtual double		getPosition(const char* stdCode, bool validOnly = true, int32_t flag = 3) override;
	virtual OrderMap*	getOrders(const char* code) override;
	virtual double		getUndoneQty(const char* code) override;

	virtual OrderIDs	buy(const char* code, double price, double qty, bool bForceClose = false) override;
	virtual OrderIDs	sell(const char* code, double price, double qty, bool bForceClose = false) override;
	virtual bool		cancel(uint32_t localid) override;
	virtual OrderIDs	cancel(const char* code, bool isBuy, double qty) override;
	virtual void		writeLog(const char* message) override;

	virtual WTSCommodityInfo*	getCommodityInfo(const char* stdCode) override;
	virtual WTSSessionInfo*		getSessionInfo(const char* stdCode) override;

	virtual uint64_t	getCurTime() override;

public:
	/*
	 *	����Ŀ���λ
	 */
	virtual void set_position(const fastest_hashmap<std::string, double>& targets) override;


	/*
	 *	��Լ��λ�䶯
	 */
	virtual void on_position_changed(const char* stdCode, double diffPos) override;

	/*
	 *	ʵʱ����ص�
	 */
	virtual void on_tick(const char* stdCode, WTSTickData* newTick) override;

	/*
	 *	�ɽ��ر�
	 */
	virtual void on_trade(uint32_t localid, const char* stdCode, bool isBuy, double vol, double price) override;

	/*
	 *	�����ر�
	 */
	virtual void on_order(uint32_t localid, const char* stdCode, bool isBuy, double totalQty, double leftQty, double price, bool isCanceled = false) override;

	/*
	 *
	 */
	virtual void on_position(const char* stdCode, bool isLong, double prevol, double preavail, double newvol, double newavail, uint32_t tradingday) override;

	/*
	 *
	 */
	virtual void on_entrust(uint32_t localid, const char* stdCode, bool bSuccess, const char* message) override;

	/*
	 *	����ͨ������
	 */
	virtual void on_channel_ready() override;

	/*
	 *	����ͨ����ʧ
	 */
	virtual void on_channel_lost() override;

	/*
	 *	�ʽ�ر�
	 */
	virtual void on_account(const char* currency, double prebalance, double balance, double dynbalance, 
		double avaliable, double closeprofit, double dynprofit, double margin, double fee, double deposit, double withdraw) override;

private:
	ExecuteUnitMap		_unit_map;
	TraderAdapter*		_trader;
	WtExecuterFactory*	_factory;
	IDataManager*		_data_mgr;
	WTSVariant*			_config;

	double				_scale;				//�Ŵ���
	bool				_auto_clear;		//�Ƿ��Զ�������һ�ڵ�������Լͷ��
	bool				_strict_sync;		//�Ƿ��ϸ�ͬ��Ŀ���λ
	bool				_channel_ready;

	SpinMutex			_mtx_units;

	typedef struct _CodeGroup
	{
		char	_name[32] = { 0 };
		fastest_hashmap<std::string, double>	_items;
	} CodeGroup;
	typedef std::shared_ptr<CodeGroup> CodeGroupPtr;
	typedef fastest_hashmap<std::string, CodeGroupPtr>	CodeGroups;
	CodeGroups				_groups;			//��Լ��ϣ�������Ƶ���ϵ�ӳ�䣩
	CodeGroups				_code_to_groups;	//��Լ���뵽��ϵ�ӳ��

	fastest_hashset<std::string>	_clear_includes;	//�Զ���������Ʒ��
	fastest_hashset<std::string>	_clear_excludes;	//�Զ������ų�Ʒ��

	fastest_hashset<std::string> _channel_holds;		//ͨ���ֲ�

	fastest_hashmap<std::string, double> _target_pos;

	typedef std::shared_ptr<boost::threadpool::pool> ThreadPoolPtr;
	ThreadPoolPtr		_pool;
};

typedef std::shared_ptr<IExecCommand> ExecCmdPtr;

NS_WTP_END