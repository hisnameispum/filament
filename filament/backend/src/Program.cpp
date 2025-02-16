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

#include "backend/Program.h"

using namespace utils;

namespace filament::backend {

// We want these in the .cpp file, so they're not inlined (not worth it)
Program::Program() noexcept { // NOLINT(modernize-use-equals-default)
}

Program::Program(Program&& rhs) noexcept = default;

Program& Program::operator=(Program&& rhs) noexcept {
    mUniformBlocks.operator=(rhs.mUniformBlocks);
    mSamplerGroups.operator=(std::move(rhs.mSamplerGroups));
    mShadersSource.operator=(std::move(rhs.mShadersSource));
    mName.operator=(std::move(rhs.mName));
    mLogger.operator=(std::move(rhs.mLogger));
    return *this;
}

Program::~Program() noexcept = default;

Program& Program::diagnostics(utils::CString const& name,
        utils::Invocable<io::ostream&(utils::io::ostream&)>&& logger) {
    mName = name;
    mLogger = std::move(logger);
    return *this;
}

Program& Program::shader(ShaderType shader, void const* data, size_t size) {
    ShaderBlob blob(size);
    std::copy_n((const uint8_t *)data, size, blob.data());
    mShadersSource[size_t(shader)] = std::move(blob);
    return *this;
}

Program& Program::uniformBlockBindings(
        utils::FixedCapacityVector<std::pair<const char*, uint8_t>> const& uniformBlockBindings) noexcept {
    for (auto const& item : uniformBlockBindings) {
        assert_invariant(item.second < UNIFORM_BINDING_COUNT);
        mUniformBlocks[item.second] = item.first;
    }
    return *this;
}

Program& Program::setSamplerGroup(size_t bindingPoint, ShaderStageFlags stageFlags,
        const Program::Sampler* samplers, size_t count) noexcept {
    auto& groupData = mSamplerGroups[bindingPoint];
    groupData.stageFlags = stageFlags;
    auto& samplerList = groupData.samplers;
    samplerList.reserve(count);
    samplerList.resize(count);
    std::copy_n(samplers, count, samplerList.data());
    return *this;
}

io::ostream& operator<<(io::ostream& out, const Program& builder) {
    out << "Program{";
    builder.mLogger(out);
    out << "}";
    return out;
}

} // namespace filament::backend
