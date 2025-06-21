#include "bsd/util/signature/signer.hh"
#include "bsd/util/error.hh"

#include <sodium.h>

namespace bsd {

LocalSigner::LocalSigner(SecretKey && privateKey)
    : privateKey(privateKey)
    , publicKey(privateKey.toPublicKey())
{ }

std::string LocalSigner::signDetached(std::string_view s) const
{
    return privateKey.signDetached(s);
}

const PublicKey & LocalSigner::getPublicKey()
{
    return publicKey;
}

}
