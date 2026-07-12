#include "mc/world/level/ChunkPos.h"
#include "mod/MyMod.h"

#include <ll/api/memory/Hook.h>
#include <mc/world/actor/Hopper.h>
#include <mc/world/containers/models/LevelContainerModel.h>
#include <mc/world/item/ItemStack.h>
#include <mc/world/level/BlockSource.h>
#include <mc/world/level/block/actor/ChestBlockActor.h>
#include <mc/world/level/block/actor/DropperBlockActor.h>
#include <mc/world/level/chunk/predicate.h>


#include <regex>

namespace my_mod::hook {

auto& logger = MyMod::getInstance().getSelf().getLogger();

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

LL_TYPE_INSTANCE_HOOK(
    startOpenChestBlockActorHook,
    ll::memory::HookPriority::Normal,
    ChestBlockActor,
    &ChestBlockActor::canOpen,
    bool,
    ::BlockSource& region
) {
    if (mLargeChestPaired) {
        ChunkPos chunkPos1{mPosition};
        ChunkPos chunkPos2{mLargeChestPairedPosition};
        if (region.hasUntickedNeighborChunk(chunkPos1, 0) || region.hasUntickedNeighborChunk(chunkPos2, 0))
            return false;
    } else {
        ChunkPos chunkPos{mPosition};
        if (region.hasUntickedNeighborChunk(chunkPos, 0)) return false;
    }
    return origin(region);
}


LL_TYPE_INSTANCE_HOOK(
    HopperTryPullInItemsFromAboveContainerHook,
    ll::memory::HookPriority::Normal,
    Hopper,
    &Hopper::_tryPullInItemsFromAboveContainer,
    bool,
    ::BlockSource& region,
    ::Container&   toContainer,
    ::Vec3 const&  pos
) {
    ChunkPos chunkPos{pos};
    if (region.hasUntickedNeighborChunk(chunkPos, 1)) return false;
    auto r = origin(region, toContainer, pos);
    return r;
}

void enable() {
    HopperAddItemHook::hook();
    DropperTryMoveInItemsHook::hook();
    // HopperTryTakeInItemFromSlotHook::hook();
    HopperTryPullInItemsFromAboveContainerHook::hook();
    startOpenChestBlockActorHook::hook();
}
} // namespace my_mod::hook