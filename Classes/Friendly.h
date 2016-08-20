﻿#pragma once

#include "SkeletonObject.h"

namespace cocos2d
{
	enum class Country
	{
		Korea,
		China,
		Japen,
	};

	class Friendly
		: public Sprite
	{
	private:
		SkeletonObject* _object;
		Country		_country;
		Vec2		_backPosition;
		int			_touchID;
		bool		_move;
		bool		_die;
		bool		_inLine;

		bool init(Country country);
		void countryImage(Country country);

	protected:
		Friendly();
		virtual ~Friendly();
	
	public:
		static Friendly* create(Country country);

		Country getCountry() { return _country; }

		void setBackPosition(const Vec2& position);
		const Vec2& getBackPosition() { return _backPosition; }

		void setTouchID(const int id);
		int getTouchID() { return _touchID; }

		void setMove(const bool move);
		bool isMove() const { return _move; }

		void die(bool left);
		bool isDie() { return _die; }
		void setInLine(const bool inLine);
		bool isInLine() const { return _inLine; }

	};
}