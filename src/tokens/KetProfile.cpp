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

QString Profile::monoFamily() const { return Tokens::installedFamily(monoFamilies); }

const Profile& Profile::current() { return storage(); }

void Profile::setCurrent(const Profile& profile) { storage() = profile; }

} // namespace ket
