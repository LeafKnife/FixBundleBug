#include "mod/MyMod.h"

#include <ll/api/memory/Hook.h>
#include <mc/world/actor/Hopper.h>
#include <mc/world/item/ItemStack.h>

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
    short id = item.getId();
    if (id <= 273 && id >= 257) {
        return false;
    }
    return origin(region, container, item, slot, face, itemCount);
}
void enable() { HopperAddItemHook::hook(); }
} // namespace my_mod::hook