/*
 * Copyright (C) 2015 The Android Open Source Project
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

#ifndef TNT_FILAMENT_DETAILS_MATERIALINSTANCE_H
#define TNT_FILAMENT_DETAILS_MATERIALINSTANCE_H

#include "upcast.h"
#include "UniformBuffer.h"
#include "details/Engine.h"

#include "private/backend/DriverApi.h"

#include <backend/Handle.h>

#include <math/scalar.h>

#include <utils/BitmaskEnum.h>
#include <utils/compiler.h>

#include <filament/MaterialInstance.h>

namespace filament {

class FMaterial;

class FMaterialInstance : public MaterialInstance {
public:
    FMaterialInstance(FEngine& engine, FMaterialInstance const* other, const char* name);
    FMaterialInstance(const FMaterialInstance& rhs) = delete;
    FMaterialInstance& operator=(const FMaterialInstance& rhs) = delete;

    static FMaterialInstance* duplicate(FMaterialInstance const* other, const char* name) noexcept;

    ~FMaterialInstance() noexcept;

    void terminate(FEngine& engine);

    void commit(FEngine::DriverApi& driver) const {
        if (UTILS_UNLIKELY(mUniforms.isDirty() || mSamplers.isDirty())) {
            commitSlow(driver);
        }
    }

    void use(FEngine::DriverApi& driver) const {
        if (mUbHandle) {
            driver.bindUniformBuffer(+UniformBindingPoints::PER_MATERIAL_INSTANCE, mUbHandle);
        }
        if (mSbHandle) {
            driver.bindSamplers(+SamplerBindingPoints::PER_MATERIAL_INSTANCE, mSbHandle);
        }
    }

    FMaterial const* getMaterial() const noexcept { return mMaterial; }

    uint64_t getSortingKey() const noexcept { return mMaterialSortingKey; }

    UniformBuffer const& getUniformBuffer() const noexcept { return mUniforms; }
    backend::SamplerGroup const& getSamplerGroup() const noexcept { return mSamplers; }

    void setScissor(int32_t left, int32_t bottom, uint32_t width, uint32_t height) noexcept {
        mScissorRect = { left, bottom,
                std::min(width, (uint32_t)std::numeric_limits<int32_t>::max()),
                std::min(height, (uint32_t)std::numeric_limits<int32_t>::max())
        };
    }

    void unsetScissor() noexcept {
        mScissorRect = { 0, 0,
                (uint32_t)std::numeric_limits<int32_t>::max(),
                (uint32_t)std::numeric_limits<int32_t>::max()
        };
    }

    backend::Viewport const& getScissor() const noexcept { return mScissorRect; }

    backend::CullingMode getCullingMode() const noexcept { return mCulling; }

    bool getColorWrite() const noexcept { return mColorWrite; }

    bool getDepthWrite() const noexcept { return mDepthWrite; }

    backend::StencilState getStencilState() const noexcept { return mStencilState; }

    TransparencyMode getTransparencyMode() const noexcept { return mTransparencyMode; }

    backend::RasterState::DepthFunc getDepthFunc() const noexcept { return mDepthFunc; }

    void setPolygonOffset(float scale, float constant) noexcept {
        // handle reversed Z
        mPolygonOffset = { -scale, -constant };
    }

    backend::PolygonOffset getPolygonOffset() const noexcept { return mPolygonOffset; }

    void setMaskThreshold(float threshold) noexcept;

    void setSpecularAntiAliasingVariance(float variance) noexcept;

    void setSpecularAntiAliasingThreshold(float threshold) noexcept;

    void setDoubleSided(bool doubleSided) noexcept;

    void setTransparencyMode(TransparencyMode mode) noexcept;

    void setCullingMode(CullingMode culling) noexcept { mCulling = culling; }

    void setColorWrite(bool enable) noexcept { mColorWrite = enable; }

    void setDepthWrite(bool enable) noexcept { mDepthWrite = enable; }

    void setStencilWrite(bool enable) noexcept { mStencilState.stencilWrite = enable; }

    void setDepthCulling(bool enable) noexcept;

    void setStencilCompareFunction(StencilCompareFunc func, StencilFace face) noexcept {
        if (any(face & StencilFace::FRONT)) {
            mStencilState.front.stencilFunc = func;
        }
        if (any(face & StencilFace::BACK)) {
            mStencilState.back.stencilFunc = func;
        }
    }

    void setStencilOpStencilFail(StencilOperation op, StencilFace face) noexcept {
        if (any(face & StencilFace::FRONT)) {
            mStencilState.front.stencilOpStencilFail = op;
        }
        if (any(face & StencilFace::BACK)) {
            mStencilState.back.stencilOpStencilFail = op;
        }
    }

    void setStencilOpDepthFail(StencilOperation op, StencilFace face) noexcept {
        if (any(face & StencilFace::FRONT)) {
            mStencilState.front.stencilOpDepthFail = op;
        }
        if (any(face & StencilFace::BACK)) {
            mStencilState.back.stencilOpDepthFail = op;
        }
    }

    void setStencilOpDepthStencilPass(StencilOperation op, StencilFace face) noexcept {
        if (any(face & StencilFace::FRONT)) {
            mStencilState.front.stencilOpDepthStencilPass = op;
        }
        if (any(face & StencilFace::BACK)) {
            mStencilState.back.stencilOpDepthStencilPass = op;
        }
    }

    void setStencilReferenceValue(uint8_t value, StencilFace face) noexcept {
        if (any(face & StencilFace::FRONT)) {
            mStencilState.front.ref = value;
        }
        if (any(face & StencilFace::BACK)) {
            mStencilState.back.ref = value;
        }
    }

    void setStencilReadMask(uint8_t readMask, StencilFace face) noexcept {
        if (any(face & StencilFace::FRONT)) {
            mStencilState.front.readMask = readMask;
        }
        if (any(face & StencilFace::BACK)) {
            mStencilState.back.readMask = readMask;
        }
    }

    void setStencilWriteMask(uint8_t writeMask, StencilFace face) noexcept {
        if (any(face & StencilFace::FRONT)) {
            mStencilState.front.writeMask = writeMask;
        }
        if (any(face & StencilFace::BACK)) {
            mStencilState.back.writeMask = writeMask;
        }
    }

    const char* getName() const noexcept;

    void setParameter(std::string_view name,
            backend::Handle<backend::HwTexture> texture, backend::SamplerParams params) noexcept;

    using MaterialInstance::setParameter;

private:
    friend class FMaterial;
    friend class MaterialInstance;

    template<size_t Size>
    void setParameterUntypedImpl(std::string_view name, const void* value);

    template<size_t Size>
    void setParameterUntypedImpl(std::string_view name, const void* value, size_t count);

    template<typename T>
    void setParameterImpl(std::string_view name, T const& value);

    template<typename T>
    void setParameterImpl(std::string_view name, const T* value, size_t count);

    void setParameterImpl(std::string_view name,
            Texture const* texture, TextureSampler const& sampler);

    FMaterialInstance() noexcept;
    void initDefaultInstance(FEngine& engine, FMaterial const* material);

    void commitSlow(FEngine::DriverApi& driver) const;

    // keep these grouped, they're accessed together in the render-loop
    FMaterial const* mMaterial = nullptr;
    backend::Handle<backend::HwBufferObject> mUbHandle;
    backend::Handle<backend::HwSamplerGroup> mSbHandle;

    UniformBuffer mUniforms;
    backend::SamplerGroup mSamplers;
    backend::PolygonOffset mPolygonOffset;
    backend::CullingMode mCulling;
    bool mColorWrite;
    bool mDepthWrite;
    backend::StencilState mStencilState = {};
    backend::RasterState::DepthFunc mDepthFunc;
    TransparencyMode mTransparencyMode;

    uint64_t mMaterialSortingKey = 0;

    // Scissor rectangle is specified as: Left Bottom Width Height.
    backend::Viewport mScissorRect = { 0, 0,
            (uint32_t)std::numeric_limits<int32_t>::max(),
            (uint32_t)std::numeric_limits<int32_t>::max()
    };

    utils::CString mName;
};

FILAMENT_UPCAST(MaterialInstance)

} // namespace filament

#endif // TNT_FILAMENT_DETAILS_MATERIALINSTANCE_H
