// SPDX-License-Identifier: MPL-2.0
#include "KetProfile.h"

#include "KetTokens.h"

namespace ket {
namespace {

Profile& storage() {
    static Profile profile = Profile::ketPlus();
    return profile;
}

} // namespace

const Profile& Profile::current() { return storage(); }

void Profile::setCurrent(const Profile& profile) { storage() = profile; }

} // namespace ket
