/*!
 * \file WtSimpRiskMon.h
 *
 * \author Wesley
 * \date 2020/03/30
 *
 * 
 */
#pragma once
#include <thread>
#include <memory>

#include "../Includes/RiskMonDefs.h"

USING_NS_WTP;

class WtSimpleRiskMon : public WtRiskMonitor
{
public:
	WtSimpleRiskMon() :_stopped(false), _limited(false){}

public:
	const char* getName() override;

	const char* getFactName() override;

	void init(WtPortfolioContext* ctx, WTSVariant* cfg) override;

	void run() override;

	void stop() override;

private:
	typedef std::shared_ptr<std::thread> ThreadPtr;
	ThreadPtr		_thread;
	bool			_stopped;
	bool			_limited;

	uint64_t		_last_time{};

	uint32_t		_calc_span{};			//����ʱ����,��λs
	uint32_t		_risk_span{};			//�س��Ƚ�ʱ��
	double			_basic_ratio{};		//����ӯ����
	double			_risk_scale{};		//���տ���ϵ��
	double			_inner_day_fd{};		//���ڸߵ�س��߽�
	bool			_inner_day_active{};	//���ڷ������
	double			_multi_day_fd{};		//���ոߵ�س��߽�
	bool			_multi_day_active{};	//���շ������
	double			_base_amount{};		//�����ʽ��ģ
};