﻿#include "GameScene.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

enum Depth
{
	PlatformArea,
	BackGround,
	Platform = 4,
	Friendly,
	Enemy,
	PauseButton,
	PausePopUp,
};

Scene* GameScene::createScene()
{
    auto scene = Scene::create();

	auto layer = GameScene::create();
    scene->addChild(layer);

    return scene;
}

bool GameScene::init()
{
	if (!Layer::init())
	{
		return false;
	}

	GameManager::getInstance()->setScoreZero();
	GameManager::getInstance()->setDifficulty(Difficulty::Easy);

	std::string backGroundNames[]=
	{
		"Temp_Background.png",  // Hard
		"Temp_Car.png", // Normal
		"Temp_Background3.png", // Easy
	};

	for (int i = 0; i < 3; ++i)
	{
		backGround[i] = Sprite::create(backGroundNames[i]);
		backGround[i]->setPosition(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
		this->addChild(backGround[i], 3 - i);
	}

	Rect area[] =
	{
		Rect(0, 0, 720, 475),
		Rect(0, 0, 720, 320),
		Rect(0, 0, 720, 320),
	};

	spawner = EnemySpawner::create(this);
	spawner->setDepth(Depth::Enemy);

	float y = SCREEN_HEIGHT / 2 + 220;
	for (int i = 0; i < 3; ++i)
	{
		GameManager::getInstance()->setSelectCountrys(i, Country::Korea);

		floor[i] = Floor::create("Temp_Platform.png", y + 105);
		floor[i]->setPosition(SCREEN_WIDTH / 2, y);
		floor[i]->initArea(area[i], this);
		this->addChild(floor[i], Depth::Platform);

		friendly[i] = Friendly::create(GameManager::getInstance()->getSelectCountrys(i));
		friendly[i]->setPosition(SCREEN_WIDTH / 2, y + 105);
		friendly[i]->setBackPosition(friendly[i]->getPosition());
		friendly[i]->setOpacity(0.0f);
		this->addChild(friendly[i], Depth::Friendly);

		spawnPoint[i].position = Vec2(SCREEN_WIDTH / 2 + 475, y + 105);
		spawnPoint[i].left = false;
		spawner->addSpawnPoint(spawnPoint[i]);

		spawnPoint[i + 1].position = Vec2(SCREEN_WIDTH / 2 - 475, y + 105);
		spawnPoint[i + 1].left = true;
		spawner->addSpawnPoint(spawnPoint[i + 1]);

		y -= 320.0f;
	}

	pauseButton = Button::create("Temp_Pause.png", Color3B(125, 125, 125));
	pauseButton->setPosition(SCREEN_WIDTH / 2 + 300, SCREEN_HEIGHT / 2 + 580);
	pauseButton->click = [=]()
	{
		Director::getInstance()->pause();
		menuButton->resume();
		cancelButton->resume();
		retryButton->resume();
		popUpLayer->setVisible(true);
	};
	this->addChild(pauseButton, Depth::PauseButton);

	GameManager::getInstance()->pushEnemyImage("Temp_Car.png");

	popUpLayer = LayerColor::create(Color4B(0, 0, 0, 191), SCREEN_WIDTH, SCREEN_HEIGHT);
	popUpLayer->setVisible(false);
	popUpLayer->setPosition(0, 0);
	this->addChild(popUpLayer, Depth::PausePopUp);

	popUpFrame = Sprite::create("pause/Frame.png");
	popUpFrame->setPosition(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
	popUpLayer->addChild(popUpFrame);

	menuButton = Button::create("pause/Menu.png", Color3B(125, 125, 125));
	menuButton->setPosition(popUpFrame->getContentSize().width / 2, popUpFrame->getContentSize().height / 2);
	menuButton->click = [=]()
	{
		Director::getInstance()->resume();
		// goto titleScene
	};
	popUpFrame->addChild(menuButton);

	cancelButton = Button::create("pause/Cancel.png", Color3B(125, 125, 125));
	cancelButton->setPosition(popUpFrame->getContentSize().width / 2, popUpFrame->getContentSize().height / 2 - 200);
	cancelButton->click = [=]()
	{
		Director::getInstance()->resume();
		popUpLayer->setVisible(false);
	};
	popUpFrame->addChild(cancelButton);

	retryButton = Button::create("pause/Retry.png", Color3B(125, 125, 125));
	retryButton->setPosition(popUpFrame->getContentSize().width / 2, popUpFrame->getContentSize().height / 2 + 200);
	retryButton->click = [=]()
	{
		Director::getInstance()->resume();
		// restart game
	};
	popUpFrame->addChild(retryButton);

	auto listener = EventListenerTouchAllAtOnce::create();
	listener->onTouchesBegan = CC_CALLBACK_2(GameScene::onTouchesBegan, this);
	listener->onTouchesMoved = CC_CALLBACK_2(GameScene::onTouchesMoved, this);
	listener->onTouchesEnded = CC_CALLBACK_2(GameScene::onTouchesEnded, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

	this->schedule(schedule_selector(GameScene::update));
	this->schedule(schedule_selector(GameScene::scoreUp), 1.0f);

	return true;
}

void GameScene::scoreUp(float dt)
{
	GameManager::getInstance()->upScore();
}

void GameScene::update(float dt)
{
	int score = GameManager::getInstance()->getScore();
	if (score >= 150)
	{
		GameManager::getInstance()->setDifficulty(Difficulty::Hard);
		if (backGround[(int)Difficulty::Normal]->getOpacity() >= 255.0f && backGround[(int)Difficulty::Normal]->isVisible())
		{
			if (backGround[(int)Difficulty::Normal]->getOpacity() <= 0.0f)
			{
				backGround[(int)Difficulty::Normal]->setVisible(false);
			}
			backGround[(int)Difficulty::Normal]->setOpacity(clampf(backGround[(int)Difficulty::Normal]->getOpacity() - 250.0f * dt, 0.0f, 255.0f));
		}
	}

	else if (score >= 50)
	{
		GameManager::getInstance()->setDifficulty(Difficulty::Normal);
		if (backGround[(int)Difficulty::Easy]->getOpacity() >= 0.0f && backGround[(int)Difficulty::Easy]->isVisible())
		{
			if (backGround[(int)Difficulty::Easy]->getOpacity() <= 0.0f)
			{
				backGround[(int)Difficulty::Easy]->setVisible(false);
			}
			backGround[(int)Difficulty::Easy]->setOpacity(clampf(backGround[(int)Difficulty::Easy]->getOpacity() - 250.0f * dt, 0.0f, 255.0f));
		}
	}

	spawner->update(dt);
	CCLOG("%d\n", GameManager::getInstance()->getScore());

	for (int i = 0; i < spawner->getEnemyCount(); ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			if (friendly[j]->isVisible())
			{
				if (friendly[j]->getBoundingBox().intersectsRect(spawner->getEnemy(i)->getBoundingBox()))
				{
					friendly[j]->die(spawner->getEnemy(i)->isMoveLeft());
				}
			}
		}
	}
}

void GameScene::onTouchesBegan(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event)
{
	friendly[0]->die(true);
	for (auto& touch : touches)
	{
		auto touchPoint = touch->getLocation();
		if (!popUpLayer->isVisible())
		{
			pauseButton->begin(touchPoint);

			for (int i = 0; i < 3; ++i)
			{
				if (friendly[i]->getBoundingBox().containsPoint(touchPoint))
				{
					friendly[i]->setMove(true);
					friendly[i]->setInLine(false);
					friendly[i]->setBackPosition(friendly[i]->getPosition());
					friendly[i]->setTouchID(touch->getID());
					friendly[i]->setPosition(touchPoint);
					return;
				}
			}
		}
		else
		{
			Vec2 touchPoint = popUpLayer->convertToNodeSpace(popUpFrame->convertToNodeSpace(touch->getLocation()));
			menuButton->begin(touchPoint);
			cancelButton->begin(touchPoint);
			retryButton->begin(touchPoint);
		}
	}
}

void GameScene::onTouchesMoved(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event)
{
	if (!popUpLayer->isVisible())
	{
		for (int i = 0; i < 3; ++i)
		{
			if (friendly[i]->isMove())
			{
				int touchID = friendly[i]->getTouchID();
				friendly[i]->setPosition(touches[touchID]->getLocation());
			}
		}
	}
}

void GameScene::onTouchesEnded(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event)
{
	for (auto& touch : touches)
	{
		auto touchPoint = touch->getLocation();

		if (!popUpLayer->isVisible())
		{
			pauseButton->ended(touchPoint);

			for (int i = 0; i < 3; ++i)
			{
				for (int j = 0; j < 3; ++j)
				{
					floor[i]->dropCheck(friendly[j]);
				}
			}

			for (int i = 0; i < 3; ++i)
			{
				if (!friendly[i]->isInLine())
				{
					friendly[i]->setMove(false);
					friendly[i]->setPosition(friendly[i]->getBackPosition());
				}
			}
		}
		else
		{
			Vec2 touchPoint = popUpLayer->convertToNodeSpace(popUpFrame->convertToNodeSpace(touch->getLocation()));
			menuButton->ended(touchPoint);
			cancelButton->ended(touchPoint);
			retryButton->ended(touchPoint);
		}
	}
}