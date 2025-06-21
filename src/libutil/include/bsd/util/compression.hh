#pragma once
///@file

#include "bsd/util/ref.hh"
#include "bsd/util/types.hh"
#include "bsd/util/serialise.hh"

#include <string>

namespace bsd {

struct CompressionSink : BufferedSink, FinishSink
{
    using BufferedSink::operator();
    using BufferedSink::writeUnbuffered;
    using FinishSink::finish;
};

std::string decompress(const std::string & method, std::string_view in);

std::unique_ptr<FinishSink> makeDecompressionSink(const std::string & method, Sink & nextSink);

std::string compress(const std::string & method, std::string_view in, const bool parallel = false, int level = -1);

ref<CompressionSink>
makeCompressionSink(const std::string & method, Sink & nextSink, const bool parallel = false, int level = -1);

MakeError(UnknownCompressionMethod, Error);

MakeError(CompressionError, Error);

}
