/*
 * Copyright (C) 2020 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef GLTFIO_FILAMENTINSTANCE_H
#define GLTFIO_FILAMENTINSTANCE_H

#include <utils/compiler.h>
#include <utils/Entity.h>

namespace filament::gltfio {

class Animator;
class FilamentAsset;

/**
 * \class FilamentInstance FilamentInstance.h gltfio/FilamentInstance.h
 * \brief Provides access to a hierarchy of entities that have been instanced from a glTF asset.
 *
 * Every entity has a filament::TransformManager component, and some entities also have \c Name or
 * \c Renderable components.
 *
 * \see AssetLoader::createInstancedAsset()
 */
class UTILS_PUBLIC FilamentInstance {
public:
    /**
     * Gets the owner of this instance.
     */
    FilamentAsset* getAsset() const noexcept;

    /**
     * Gets the list of entities in this instance, one for each glTF node. All of these have a
     * Transform component. Some of the returned entities may also have a Renderable component or
     * Name component.
     */
    const utils::Entity* getEntities() const noexcept;

    /**
     * Gets the number of entities returned by getEntities().
     */
    size_t getEntityCount() const noexcept;

    /** Gets the transform root for the instance, which has no matching glTF node. */
    utils::Entity getRoot() const noexcept;

    /**
     * Applies the given material variant to all primitives in this instance.
     *
     * Ignored if variantIndex is out of bounds.
     */
    void applyMaterialVariant(size_t variantIndex) noexcept;

    /**
     * Returns the animation engine for the instance.
     *
     * Note that an animator can be obtained either from an individual instance, or from the
     * originating FilamentAsset. In the latter case, the animation frame is shared amongst all
     * instances. If individual control is desired, users must obtain the animator from the
     * individual instances.
     *
     * The animator is owned by the asset and should not be manually deleted.
     */
    Animator* getAnimator() noexcept;

    /**
     * Gets the number of skins.
     */
    size_t getSkinCount() const noexcept;

    /**
     * Gets the skin name at skin index.
     */
    const char* getSkinNameAt(size_t skinIndex) const noexcept;

    /**
     * Gets the number of joints at skin index.
     */
    size_t getJointCountAt(size_t skinIndex) const noexcept;

    /**
     * Gets joints at skin index.
     */
    const utils::Entity* getJointsAt(size_t skinIndex) const noexcept;

    /**
     * Attaches the given skin to the given node, which must have an associated mesh with
     * BONE_INDICES and BONE_WEIGHTS attributes.
     *
     * This is a no-op if the given skin index or target is invalid.
     */
    void attachSkin(size_t skinIndex, utils::Entity target) noexcept;

    /**
     * Detaches the given skin from the given node.
     *
     * This is a no-op if the given skin index or target is invalid.
     */
    void detachSkin(size_t skinIndex, utils::Entity target) noexcept;
};

} // namespace filament::gltfio

#endif // GLTFIO_FILAMENTINSTANCE_H
