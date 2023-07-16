/*!
 * \file WTSHotMgr.h
 * \project	WonderTrader
 *
 * \author Wesley
 * \date 2020/03/30
 * 
 * \brief 主力合约管理器实现
 */
#pragma once
#include "../Includes/IHotMgr.h"
#include "../Includes/FasterDefs.h"
#include "../Includes/WTSCollection.hpp"
#include <string>

NS_WTP_BEGIN
	class WTSSwitchItem;
NS_WTP_END

USING_NS_WTP;

//换月主力映射
typedef WTSMap<uint32_t>			WTSDateHotMap;
//品种主力映射
typedef WTSHashMap<ShortKey>		WTSProductHotMap;
//分市场主力映射
typedef WTSHashMap<ShortKey>		WTSExchgHotMap;

//自定义切换规则映射
typedef WTSHashMap<ShortKey>		WTSCustomSwitchMap;

class WTSHotMgr : public IHotMgr
{
public:
	WTSHotMgr();
	~WTSHotMgr();

public:
	bool loadHots(const char* filename);
	bool loadSeconds(const char* filename);
	void release();

	bool loadCustomRules(const char* tag, const char* filename);

	inline bool isInitialized() const {return m_bInitialized;}

public:
	const char* getRuleTag(const char* stdCode) override;

	double		getRuleFactor(const char* ruleTag, const char* fullPid, uint32_t uDate) override;

	//////////////////////////////////////////////////////////////////////////
	//主力接口
	const char* getRawCode(const char* exchg, const char* pid, uint32_t dt) override;

	virtual const char* getPrevRawCode(const char* exchg, const char* pid, uint32_t dt = 0) override;

	bool	isHot(const char* exchg, const char* rawCode, uint32_t dt = 0) override;

	virtual bool	splitHotSections(const char* exchg, const char* pid, uint32_t sDt, uint32_t eDt, HotSections& sections) override;

	//////////////////////////////////////////////////////////////////////////
	//次主力接口
	const char* getSecondRawCode(const char* exchg, const char* pid, uint32_t dt) override;

	virtual const char* getPrevSecondRawCode(const char* exchg, const char* pid, uint32_t dt = 0) override;

	bool		isSecond(const char* exchg, const char* rawCode, uint32_t dt) override;

	bool		splitSecondSections(const char* exchg, const char* pid, uint32_t sDt, uint32_t eDt, HotSections& sections) override;

	//////////////////////////////////////////////////////////////////////////
	//通用接口
	const char* getCustomRawCode(const char* tag, const char* fullPid, uint32_t dt) override;

	const char* getPrevCustomRawCode(const char* tag, const char* fullPid, uint32_t dt) override;

	bool		isCustomHot(const char* tag, const char* fullCode, uint32_t dt) override;

	bool		splitCustomSections(const char* tag, const char* fullPid, uint32_t sDt, uint32_t eDt, HotSections& sections) override;


private:
	//WTSExchgHotMap*	m_pExchgHotMap;
	//WTSExchgHotMap*	m_pExchgScndMap;
	//faster_hashset<ShortKey>	m_curHotCodes;
	//faster_hashset<ShortKey>	m_curSecCodes;
	bool			m_bInitialized;

	WTSCustomSwitchMap*	m_mapCustomRules;
	typedef faster_hashmap<ShortKey, faster_hashset<ShortKey>>	CustomSwitchCodes;
	CustomSwitchCodes	m_mapCustomCodes;
};

