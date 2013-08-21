//
//  UpdateLayer.cpp
//  AssetManagerSample
//
//  Created by netcanis on 13. 8. 21..
//
//

#include "UpdateLayer.h"
#include "SimpleAudioEngine.h"


#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#include "AssetsManager.h"
#endif

#include <iostream>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>


#include "cocos2d.h"

#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)
#include <dirent.h>
#include <sys/stat.h>
#endif


USING_NS_CC;
USING_NS_CC_EXT;
using namespace CocosDenshion;



UpdateLayer* UpdateLayer::ps_Layer = NULL;

UpdateLayer* UpdateLayer::sharedManager()
{
	return UpdateLayer::ps_Layer;
}



UpdateLayer::UpdateLayer()
: m_pProgressLabel ( NULL )
, m_pAssetsManager ( NULL )
, m_nUpdateState ( 0 )
, pItemReset ( NULL )
, pItemUpdate ( NULL )
{
    UpdateLayer::ps_Layer = this;
}

UpdateLayer::~UpdateLayer()
{
    stopAllActions();
    unscheduleAllSelectors();
    
    // 어셋메니저 초기화
    if ( NULL != m_pAssetsManager )
    {
        CC_SAFE_DELETE ( m_pAssetsManager );
    }
    
    UpdateLayer::ps_Layer = NULL;
}

void UpdateLayer::onEnter()
{
    CCLayer::onEnter();
    
    
    pItemReset = NULL;
    pItemUpdate = NULL;
    
    createDownloadedDir();
    
    getAssetsManager();
    
    
    CCSize size = CCDirector::sharedDirector()->getWinSize();
    
    pItemReset = CCMenuItemFont::create("reset", this, menu_selector(UpdateLayer::reset));
    pItemUpdate = CCMenuItemFont::create("update", this, menu_selector(UpdateLayer::update));
    
    pItemReset->setPosition(ccp(size.width/2, size.height/2));
    pItemUpdate->setPosition(ccp(size.width/2, size.height/2 - 50));
    
    CCMenu *menu = CCMenu::create(pItemUpdate, pItemReset, NULL);
    menu->setPosition(ccp(0,0));
    addChild(menu);

    
    
    m_pProgressLabel = CCLabelTTF::create ( "0%", "Thonburi", 32 );
    m_pProgressLabel->setAnchorPoint ( ccp ( 1.0, 0.0 ) );
    m_pProgressLabel->setPosition ( ccp ( 300, 100 ) );
    m_pProgressLabel->setColor ( ccc3 ( 255, 255, 255 ) );
    addChild ( m_pProgressLabel );
}

void UpdateLayer::onExit()
{
    CCLayer::onExit();

}

void UpdateLayer::menuCloseCallback(CCObject* pSender)
{
	CCDirector::sharedDirector()->end();
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif
}

#pragma mark -
#pragma mark Touch Event Handling

void UpdateLayer::ccTouchesBegan(CCSet* touches, CCEvent* event)
{
	CCSetIterator it = touches->begin();
	CCTouch* touch = (CCTouch*)(*it);
	CCPoint location = touch->getLocationInView();
	CCPoint convertedLocation = CCDirector::sharedDirector()->convertToGL( location );
    
}

void UpdateLayer::ccTouchesMoved(CCSet* touches, CCEvent* event)
{
	CCSetIterator it = touches->begin();
	CCTouch* touch = (CCTouch*)(*it);
	CCPoint location = touch->getLocationInView();
	CCPoint convertedLocation = CCDirector::sharedDirector()->convertToGL( location );
    
}

void UpdateLayer::ccTouchesEnded(CCSet* touches, CCEvent* event)
{
	CCSetIterator it = touches->begin();
	CCTouch* touch = (CCTouch*)(*it);
	CCPoint location = touch->getLocationInView();
	CCPoint convertedLocation = CCDirector::sharedDirector()->convertToGL( location );
    
}



//----------------------------------------------------------
// Virtual Functions
void UpdateLayer::setButtonEnable(bool enableFlag)
{
    
}

void UpdateLayer::onButton(CCObject* pSender)
{
    
}

void UpdateLayer::update(cocos2d::CCObject *pSender)
{
    // update resources
    getAssetsManager()->update();
    
}

void UpdateLayer::reset(cocos2d::CCObject *pSender)
{
    // Remove downloaded files
#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)
    std::string command = "rm -r ";
    // Path may include space.
    command += "\"" + pathToSave + "\"";
    system(command.c_str());
#else
    string command = "rd /s /q ";
    // Path may include space.
    command += "\"" + pathToSave + "\"";
    system(command.c_str());
#endif
    // Delete recorded version codes.
    getAssetsManager()->deleteVersion();
    
    createDownloadedDir();
    
    updateProgressLabel ( "", 0 );
}

void UpdateLayer::createDownloadedDir()
{
    pathToSave = CCFileUtils::sharedFileUtils()->getWritablePath();
    pathToSave += "myfolder";    // 루트 디렉토리로 EOC 디렉토리 생성
    
    // Create the folder if it doesn't exist
#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)
    DIR *pDir = NULL;
    
    pDir = opendir (pathToSave.c_str());
    if (! pDir)
    {
        mkdir(pathToSave.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
    }
#else
	if ((GetFileAttributesA(pathToSave.c_str())) == INVALID_FILE_ATTRIBUTES)
	{
		CreateDirectoryA(pathToSave.c_str(), 0);
	}
#endif
}

cocos2d::extension::AssetsManager* UpdateLayer::getAssetsManager()
{
    if ( NULL == m_pAssetsManager )
    {
        m_pAssetsManager = new AssetsManager ( "http://192.168.5.250/update/update.zip",
                                               "http://192.168.5.250/update/version",
                                               pathToSave.c_str() );
        m_pAssetsManager->setDelegate ( this );
        m_pAssetsManager->setConnectionTimeout ( 3 );
    }
    
    return m_pAssetsManager;
}

void UpdateLayer::onError(AssetsManager::ErrorCode errorCode)
{
    if (errorCode == AssetsManager::kNoNewVersion)
    {
        CCLOG ( "no new version" );
    }
    
    if (errorCode == AssetsManager::kNetwork)
    {
        CCLOG ( "network error" );
    }
}

void UpdateLayer::onDownloadProgress(float fPercent)
{
    updateProgressLabel ( "downloading...", (int)fPercent );
}

void UpdateLayer::onUpdateProgress(float fPercent)
{
    updateProgressLabel ( "uncompressing...", (int)fPercent );
}

// 다운로드 및 업데이트가 모두 완료되면 호출됨.
void UpdateLayer::onSuccess()
{
    updateProgressLabel ( "done", 100 );
    CCLOG ( "success" );
    
    // update resources
    getAssetsManager()->update();
    CCLOG ( "success and retry to find next version" );
}

void UpdateLayer::updateProgressLabel(const char* pcName, int nProgress)
{
    if ( NULL != m_pProgressLabel )
    {
        char szBuf[128] = {0,};
        sprintf ( szBuf, "%s %d%%", pcName, nProgress );
        m_pProgressLabel->setString ( szBuf );
    }
}


