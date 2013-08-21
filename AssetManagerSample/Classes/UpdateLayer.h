//
//  UpdateLayer.h
//  AssetManagerSample
//
//  Created by netcanis on 13. 8. 21..
//
//

#ifndef __AssetManagerSample__UpdateLayer__
#define __AssetManagerSample__UpdateLayer__


#include "cocos2d.h"
#include "AssetsManager/AssetsManager.h"
#include <curl/curl.h>


using namespace cocos2d;


class UpdateLayer : public CCLayer, public cocos2d::extension::AssetsManagerDelegateProtocol
{
public:
    UpdateLayer();
    virtual ~UpdateLayer();
    
    virtual void onEnter();
	virtual void onExit();
    
	// a selector callback
	virtual void menuCloseCallback(CCObject* pSender);
    
	// implement the "static node()" method manually
	CREATE_FUNC(UpdateLayer);
    
    void ccTouchesBegan(CCSet* touches, CCEvent* event);
    void ccTouchesMoved(CCSet* touches, CCEvent* event);
    void ccTouchesEnded(CCSet* touches, CCEvent* event);
    
    // Virtual Functions
    void setButtonEnable(bool enableFlag);
    void onButton(CCObject* pSender);
    
public:
    void createBackground();
    void updateProgressLabel(const char* pcName, int nProgress);
    
    cocos2d::extension::AssetsManager* getAssetsManager();
    
    
protected:

    void reset(cocos2d::CCObject *pSender);
    void update(cocos2d::CCObject *pSender);
    
    virtual void onError(cocos2d::extension::AssetsManager::ErrorCode errorCode);
    virtual void onDownloadProgress(float fPercent);
    virtual void onUpdateProgress(float fPercent);
    virtual void onSuccess();
    
private:
    
    void createDownloadedDir();
    
    cocos2d::CCMenuItemFont *pItemReset;
    cocos2d::CCMenuItemFont *pItemUpdate;
    std::string pathToSave;
    
    
    cocos2d::extension::AssetsManager* m_pAssetsManager;
    CCLabelTTF * m_pProgressLabel;
    int m_nUpdateState;
    
public:
    static UpdateLayer * sharedManager();
private:
    static UpdateLayer * ps_Layer;
};




#endif /* defined(__AssetManagerSample__UpdateLayer__) */
