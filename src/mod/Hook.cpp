#include "mod/MyMod.h"

#include <ll/api/memory/Hook.h>
#include <mc/world/actor/Hopper.h>
#include <mc/world/item/ItemStack.h>
#include <mc/world/level/block/actor/DropperBlockActor.h>

#include <regex>

namespace my_mod::hook {

LL_TYPE_INSTANCE_HOOK(
    HopperAddItemHook,
    ll::memory::HookPriority::Normal,
    Hopper,
    &Hopper::_tryMoveInItem,
    bool,
    ::BlockSource& region,
    ::Container&   container,
    ::ItemStack&   item,
    int            slot,
    int            face,
    int            itemCount
) {
    // short id = item.getId();
    // if (id <= 280 && id >= 264) {
    //     return false;
    // }
    auto       type = item.getTypeName();
    std::regex regex("^minecraft:(.*)bundle$");
    if (std::regex_match(type, regex)) {
        return false;
    }
    return origin(region, container, item, slot, face, itemCount);
}

LL_TYPE_STATIC_HOOK(
    DropperTryMoveInItemsHook,
    ll::memory::HookPriority::Normal,
    DropperBlockActor,
    &DropperBlockActor::_tryMoveInItemsAndDepleteStack,
    bool,
    ::Container& container,
    ::ItemStack& item,
    int          stackSizeLimit,
    int          slot,
    int          face
) {
    auto       type = item.getTypeName();
    std::regex regex("^minecraft:(.*)bundle$");
    if (std::regex_match(type, regex)) {
        return false;
    }
    return origin(container, item, stackSizeLimit, slot, face);
}

void enable() {
    HopperAddItemHook::hook();
    DropperTryMoveInItemsHook::hook();
}
} // namespace my_mod::hook