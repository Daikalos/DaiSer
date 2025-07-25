#include <DaiSer/Serialization/Serializer.h>

using namespace DaiSer;

Serializer::Serializer(SerializerState aState)
	: myState(aState)
	, myBuffer()
	, myOffset(0)
{

}

WriteSerializer::WriteSerializer()
	: Serializer(SerializerState::Write)
{

}

ReadSerializer::ReadSerializer(std::vector<std::byte>&& aBuffer)
	: Serializer(SerializerState::Read)
{
	myBuffer = std::move(aBuffer);
}

ReadSerializer::ReadSerializer(std::span<const std::byte> aBuffer)
	: Serializer(SerializerState::Read)
{
	myBuffer = { aBuffer.begin(), aBuffer.end() };
}

void WriteSerializer::ReserveBytesToFit(std::size_t aNumBytesToFit)
{
	myBuffer.resize(myBuffer.size() + aNumBytesToFit);
}

void WriteSerializer::FitBufferToOffset()
{
	myBuffer.resize(myOffset);
	myBuffer.shrink_to_fit();
}

void WriteSerializer::Clear()
{
	myBuffer.clear();
	myOffset = 0;
}

std::size_t SerializeImpl<std::string>::Write(const std::string& aInData, std::vector<std::byte>& aOutBytes, std::size_t aOffset)
{
	aOutBytes.resize(aOffset + aInData.length() + 1);
	memcpy_s(aOutBytes.data() + aOffset, aInData.length() + 1, aInData.c_str(), aInData.length() + 1);
	assert(strlen(reinterpret_cast<const char*>(aOutBytes.data() + aOffset)) == aInData.length()); // make sure it went well

	return aInData.length() + 1;
}
std::size_t SerializeImpl<std::string>::Read(std::string& aOutData, const std::vector<std::byte>& aInBytes, std::size_t aOffset)
{
	aOutData = reinterpret_cast<const char*>(aInBytes.data() + aOffset);
	return aOutData.length() + 1;
}

std::size_t SerializeImpl<std::wstring>::Write(const std::wstring& aInData, std::vector<std::byte>& aOutBytes, std::size_t aOffset)
{
	static constexpr std::size_t WCHAR_SIZE = sizeof(wchar_t);

	aOutBytes.resize(aOffset + (aInData.length() + 1) * WCHAR_SIZE);
	memcpy_s(aOutBytes.data() + aOffset, (aInData.length() + 1) * WCHAR_SIZE, aInData.c_str(), (aInData.length() + 1) * WCHAR_SIZE);
	assert(wcslen(reinterpret_cast<const wchar_t*>(aOutBytes.data() + aOffset)) == aInData.length()); // make sure it went well

	return (aInData.length() + 1) * WCHAR_SIZE;
}
std::size_t SerializeImpl<std::wstring>::Read(std::wstring& aOutData, const std::vector<std::byte>& aInBytes, std::size_t aOffset)
{
	static constexpr std::size_t WCHAR_SIZE = sizeof(wchar_t);

	aOutData = reinterpret_cast<const wchar_t*>(aOutData.data() + aOffset);
	return (aOutData.length() + 1) * WCHAR_SIZE;
}
