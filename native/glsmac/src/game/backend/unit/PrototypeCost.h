#pragma once
#include "UnitCatalog.h"

namespace game::backend::unit {
// Returns mineral rows, before difficulty/industry multipliers and prototype surcharge.
int PrototypeCost(const UnitCatalog& rules, const UnitBlueprint& design);
}
