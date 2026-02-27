#pragma once

namespace Stats {
	struct SizeStat {
		float xSize{};
		float ySize{};
	};

	struct CatStat {
		uint16_t firerate{};
		uint16_t cost{};
		uint16_t range{};
		uint16_t meleeDmg{};
		uint16_t meleePen{};
		SizeStat size{};

		CatStat() {};
	};

	struct BlackGreyCatStats : public CatStat{
		BlackGreyCatStats() {
			this->firerate = 100;
			this->cost = 800;
			this->range = 150;
			this->meleeDmg = 0;
			this->meleePen = 0;
			this->size.xSize = 60;
			this->size.ySize = 40;
		}
	};

	struct OrangeCatStats : public CatStat {
		OrangeCatStats() {
			this->firerate = 80;
			this->cost = 1000;
			this->range = 80;
			this->meleeDmg = 800;
			this->meleePen = 1;
			this->size.xSize = 60;
			this->size.ySize = 40;
		}
	};

	CatStat* getStatByType(CatType type);

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
			this->size = {60, 60};
			this->childType = TargetType::NONE;
		}
	};

	TargetStats* getStatByType(TargetType type);

	struct TargetGroupStats {
		uint8_t targetAmount{};
		uint16_t spawnDelay{};
		TargetType type{ TargetType::BEATEN };

		TargetGroupStats() {}
	};

	struct Beaten_8 : public TargetGroupStats {
		Beaten_8() {

		}
	};

	TargetGroupStats* getStatByType(TargetGroupType type);
}
