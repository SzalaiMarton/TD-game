#pragma once

namespace Stats {
	struct SizeStat {
		float xSize{};
		float ySize{};
	};

	// CAT STATS ------------------------------------------------
	// CAT STATS ------------------------------------------------
	// CAT STATS ------------------------------------------------

	struct CatStat {
		uint16_t firerate{};
		uint16_t range{};
		uint16_t meleeDmg{};
		uint16_t meleePen{};
		uint16_t cost{};
		SizeStat size{};

		CatStat() {}
	};

	struct BlackGreyCatStats : public CatStat{
		BlackGreyCatStats() {
			this->firerate = 100;
			this->range = 150;
			this->meleeDmg = 0;
			this->meleePen = 0;
			this->size.xSize = 60;
			this->size.ySize = 40;
			this->cost = 400;
		}
	};

	struct OrangeCatStats : public CatStat {
		OrangeCatStats() {
			this->firerate = 80;
			this->range = 80;
			this->meleeDmg = 800;
			this->meleePen = 1;
			this->size.xSize = 60;
			this->size.ySize = 40;
			this->cost = 500;
		}
	};

	// TARGET STATS ------------------------------------------------
	// TARGET STATS ------------------------------------------------
	// TARGET STATS ------------------------------------------------

	struct TargetStats {
		uint8_t speed{};
		uint32_t health{};
		sf::Vector2f size{};
		TargetType childType{ TargetType::NONE };

		TargetStats() {}
	};

	struct BeatenBoxStats : public TargetStats {
		BeatenBoxStats() {
			this->speed = 2;
			this->health = 10;
			this->size = {40, 40};
			this->childType = TargetType::NONE;
		}
	};

	// TARGET STATS GROUP ------------------------------------------------
	// TARGET STATS GROUP ------------------------------------------------
	// TARGET STATS GROUP ------------------------------------------------

	struct TargetGroupStats {
		uint8_t targetAmount{};
		uint16_t spawnDelay{};
		uint16_t cost{};
		TargetType type{ TargetType::BEATEN };

		TargetGroupStats() {}
	};

	struct Beaten_8 : public TargetGroupStats {
		Beaten_8() {
			this->targetAmount = 8;
			this->spawnDelay = 100;
			this->type = TargetType::BEATEN;
			this->cost = 100;
		}
	};

	// BULLET STATS ------------------------------------------------
	// BULLET STATS ------------------------------------------------
	// BULLET STATS ------------------------------------------------

	struct BulletStat {
		uint8_t speed{};
		uint8_t penetration{};
		uint16_t dmg{};
		sf::Vector2f directionVector{};
		SizeStat size{};

		BulletStat() {}
	};

	struct BasicBulletStat : public BulletStat {
		BasicBulletStat() {
			this->speed = 1;
			this->penetration = 1;
			this->dmg = 10;
			this->directionVector = {0, 0};
			this->size = {3, 15};
		}
	};

	CatStat* getStatByType(CatType type);
	TargetStats* getStatByType(TargetType type);
	TargetGroupStats* getStatByType(TargetGroupType type);
	BulletStat* getStatByType(BulletType type);
}
