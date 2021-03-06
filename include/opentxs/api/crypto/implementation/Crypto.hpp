/************************************************************
 *
 *                 OPEN TRANSACTIONS
 *
 *       Financial Cryptography and Digital Cash
 *       Library, Protocol, API, Server, CLI, GUI
 *
 *       -- Anonymous Numbered Accounts.
 *       -- Untraceable Digital Cash.
 *       -- Triple-Signed Receipts.
 *       -- Cheques, Vouchers, Transfers, Inboxes.
 *       -- Basket Currencies, Markets, Payment Plans.
 *       -- Signed, XML, Ricardian-style Contracts.
 *       -- Scripted smart contracts.
 *
 *  EMAIL:
 *  fellowtraveler@opentransactions.org
 *
 *  WEBSITE:
 *  http://www.opentransactions.org/
 *
 *  -----------------------------------------------------
 *
 *   LICENSE:
 *   This Source Code Form is subject to the terms of the
 *   Mozilla Public License, v. 2.0. If a copy of the MPL
 *   was not distributed with this file, You can obtain one
 *   at http://mozilla.org/MPL/2.0/.
 *
 *   DISCLAIMER:
 *   This program is distributed in the hope that it will
 *   be useful, but WITHOUT ANY WARRANTY; without even the
 *   implied warranty of MERCHANTABILITY or FITNESS FOR A
 *   PARTICULAR PURPOSE.  See the Mozilla Public License
 *   for more details.
 *
 ************************************************************/

#ifndef OPENTXS_API_CRYPTO_IMPLEMENTATION_CRYPTO_HPP
#define OPENTXS_API_CRYPTO_IMPLEMENTATION_CRYPTO_HPP

#include "opentxs/Version.hpp"

#include "opentxs/api/crypto/Crypto.hpp"
#include "opentxs/api/Editor.hpp"
#include "opentxs/Proto.hpp"
#include "opentxs/Types.hpp"

#include <map>
#include <memory>
#include <mutex>

namespace opentxs
{
class Libsecp256k1;
class Libsodium;
class OpenSSL;
class TrezorCrypto;

// Choose your OpenSSL-compatible library here.
#if OT_CRYPTO_USING_OPENSSL
typedef OpenSSL SSLImplementation;
#endif

#if OT_CRYPTO_USING_LIBSECP256K1
typedef Libsecp256k1 secp256k1;
#endif

#if OT_CRYPTO_USING_TREZOR
typedef TrezorCrypto bitcoincrypto;
#endif

typedef Libsodium Curve25519;

namespace api
{
class Native;

namespace implementation
{
class Native;

class Crypto : virtual public opentxs::api::Crypto
{
public:
    EXPORT const OTCachedKey& DefaultKey() const override;
    EXPORT Editor<OTCachedKey> mutable_DefaultKey() const override;
    EXPORT const OTCachedKey& CachedKey(const Identifier& id) const override;
    EXPORT const OTCachedKey& CachedKey(
        const OTCachedKey& source) const override;
    EXPORT const OTCachedKey& LoadDefaultKey(
        const OTASCIIArmor& serialized) const override;
    EXPORT void SetTimeout(const std::chrono::seconds& timeout) const override;
    EXPORT void SetSystemKeyring(const bool useKeyring) const override;

    // Encoding function interface
    EXPORT crypto::Encode& Encode() const override;

    // Hash function interface
    EXPORT crypto::Hash& Hash() const override;

    // Utility class for misc OpenSSL-provided functions
    EXPORT crypto::Util& Util() const override;

    // Asymmetric encryption engines
    EXPORT CryptoAsymmetric& ED25519() const override;
#if OT_CRYPTO_SUPPORTED_KEY_RSA
    EXPORT CryptoAsymmetric& RSA() const override;
#endif
#if OT_CRYPTO_SUPPORTED_KEY_SECP256K1
    EXPORT CryptoAsymmetric& SECP256K1() const override;
#endif

    // Symmetric encryption engines
    EXPORT crypto::Symmetric& Symmetric() const override;

#if OT_CRYPTO_SUPPORTED_ALGO_AES
    EXPORT CryptoSymmetric& AES() const override;
#endif
#if OT_CRYPTO_WITH_BIP32
    EXPORT Bip32& BIP32() const override;
#endif
#if OT_CRYPTO_WITH_BIP39
    EXPORT Bip39& BIP39() const override;
#endif

    std::unique_ptr<SymmetricKey> GetStorageKey(
        std::string& seed) const override;

    ~Crypto();

private:
    friend class api::implementation::Native;

    api::Native& native_;
    mutable std::mutex cached_key_lock_;
    mutable std::unique_ptr<OTCachedKey> primary_key_;
    mutable std::map<Identifier, std::unique_ptr<OTCachedKey>> cached_keys_;
#if OT_CRYPTO_USING_TREZOR
    std::unique_ptr<bitcoincrypto> bitcoincrypto_;
#endif
    std::unique_ptr<Curve25519> ed25519_;
    std::unique_ptr<SSLImplementation> ssl_;
#if OT_CRYPTO_SUPPORTED_KEY_SECP256K1
    std::unique_ptr<secp256k1> secp256k1_;
#endif
    std::unique_ptr<crypto::Encode> encode_;
    std::unique_ptr<crypto::Hash> hash_;
    std::unique_ptr<crypto::Symmetric> symmetric_;

    void init_default_key(const Lock& lock) const;

    void Init();
    void Cleanup();

    Crypto(api::Native& native);
    Crypto() = delete;
    Crypto(const Crypto&) = delete;
    Crypto(Crypto&&) = delete;
    Crypto& operator=(const Crypto&) = delete;
    Crypto& operator=(Crypto&&) = delete;
};
}  // namespace implementation
}  // namespace api
}  // namespace opentxs
#endif  // OPENTXS_API_CRYPTO_IMPLEMENTATION_CRYPTO_HPP
