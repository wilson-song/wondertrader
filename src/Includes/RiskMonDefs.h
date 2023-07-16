/*!
 * \file RiskMonDefs.h
 * \project	WonderTrader
 *
 * \author Wesley
 * \date 2020/03/30
 * 
 * \brief 
 */
#pragma once
#include "../Includes/WTSMarcos.h"

#include <cstdio>
#include <cstdint>
#include <cstring>

/*
 *	���Ƿ��ģ����������ļ�
 *	Ŀǰ���ģ����ʱ�����Ǹ���������
 *	ֻʵ�ֻ������ʽ��ص�Ҫ��
 *	������ͨ�����߲����ʱ������ʵ��
 *	���ڷ��ģ���ﴦ��
 *	���ģ��ֻ�����Ч�ķ�ز���
 */

NS_WTP_BEGIN
class WTSVariant;
class WTSPortFundInfo;

/*
 *	���������
 */
class WtPortfolioContext
{
public:
	/*
	 *	��ȡ����ʽ�����
	 */
	virtual WTSPortFundInfo*	getFundInfo() = 0;

	/*
	 *	������������
	 *	@scale	��������, һ��С�ڵ���1, ���ڿ��������λ����
	 */
	virtual void	setVolScale(double scale) = 0;

	/*
	 *	����Ƿ��ڽ���״̬
	 *
	 *	return	���ڽ���״̬Ϊtrue
	 */
	virtual bool	isInTrading() = 0;

	/*
	 *	д��־
	 */
	virtual void	writeRiskLog(const char* message) = 0;

	/*
	 *	��ȡ��ǰ����
	 */
	virtual uint32_t	getCurDate() = 0;

	/*
	 *	��ȡ��ǰʱ��
	 */
	virtual uint32_t	getCurTime() = 0;

	/*
	 *	��ȡ��ǰ������
	 */
	virtual uint32_t	getTradingDate() = 0;

	/*
	 *	��ʱ��ת��Ϊ������(������Ч)
	 */
	virtual uint32_t	transTimeToMin(uint32_t uTime) = 0;
};

/*
 *	��Ϸ��ģ��
 */
class WtRiskMonitor
{
public:
	WtRiskMonitor():_ctx(nullptr){}
	virtual ~WtRiskMonitor()= default;

public:
	/*
	*	ִ�е�Ԫ����
	*/
	virtual const char* getName() = 0;

	/*
	*	����ִ������������
	*/
	virtual const char* getFactName() = 0;

	/*
	*	��ʼ��ִ�е�Ԫ
	*	ctx		ִ�е�Ԫ���л���
	*	code	����ĺ�Լ����
	*/
	virtual void init(WtPortfolioContext* ctx, WTSVariant* cfg){ _ctx = ctx; }

	/*
	 *	�������ģ��
	 */
	virtual void run(){}

	/*
	 *	ֹͣ���ģ��
	 */
	virtual void stop(){}

protected:
	WtPortfolioContext*	_ctx;
};


//////////////////////////////////////////////////////////////////////////
//���ģ�鹤���ӿ�
typedef void(*FuncEnumRiskMonCallback)(const char* factName, const char* unitName, bool isLast);

class IRiskMonitorFactory
{
public:
	IRiskMonitorFactory()= default;
	virtual ~IRiskMonitorFactory()= default;

public:
	/*
	*	��ȡ������
	*/
	virtual const char* getName() = 0;

	/*
	*	ö�ٽ���ͨ�����ģ��
	*/
	virtual void enumRiskMonitors(FuncEnumRiskMonCallback cb) = 0;

	/*
	*	�������ƴ�������ͨ�����ģ��
	*/
	virtual WtRiskMonitor* createRiskMonitor(const char* name) = 0;

	/*
	*	ɾ������ͨ�����ģ��
	*/
	virtual bool deleteRiskMonitor(WtRiskMonitor* unit) = 0;
};

//����ִ�й���
typedef IRiskMonitorFactory* (*FuncCreateRiskMonFact)();
//ɾ��ִ�й���
typedef void(*FuncDeleteRiskMonFact)(IRiskMonitorFactory* &fact);

NS_WTP_END
