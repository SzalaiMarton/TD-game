#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <typeindex>
#include <filesystem>
#include <functional>
#include <memory>
#include <set>
#include <span>
#include <variant>
#include <fstream>
#include <queue>

#define DEBUG_TREE 0

#include "Enums.hpp"
#include "Logger.hpp"
#include "Color.hpp"
#include "AssetLoader.hpp"
#include "Attribute.hpp"
#include "Shapes.hpp"
#include "Maps.hpp"
#include "Bullets.hpp"
#include "Cats.hpp"
#include "Graphics.hpp"
#include "Targets.hpp"
#include "Game.hpp"

// onUpdate returns true if something died, returns false if not
