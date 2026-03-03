#include "stdafx.hpp"

Stats::CatStat* Stats::getStatByType(CatType type) {
	switch (type) {
	case CatType::BLACKGREY:
		return new BlackGreyCatStats();
	case CatType::ORANGE:
		return new OrangeCatStats();
	}
	return new BlackGreyCatStats();
}

Stats::TargetStats* Stats::getStatByType(TargetType type) {
	switch (type) {
	case TargetType::BEATEN:
		return new BeatenBoxStats();
	}
	return new TargetStats();
}

Stats::TargetGroupStats* Stats::getStatByType(TargetGroupType type) {
	switch (type) {
	case TargetGroupType::BEATEN_8:
		return new Beaten_8();
	}
	return new TargetGroupStats();
}

Stats::BulletStat* Stats::getStatByType(BulletType type) {
	switch (type) {
		case BulletType::BASIC:
			return new BasicBulletStat();
	}
	return new BulletStat();
}
