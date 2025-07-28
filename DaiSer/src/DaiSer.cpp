#include <DaiSer/DaiSer.h>

using namespace DaiSer;

DSScope DSStream::Start()
{
	return DSScope(*this);
}

DSStream::operator DSScope()
{
	return Start();
}

DSScope::DSScope(DSStream& aSerializer)
	: mySerializer(&aSerializer)
{

}

void DSScope::LoadNextID()
{

}
void DSScope::SkipField()
{

}

DSStream::DSStream(DSState aState)
	: myState(aState)
{

}

ODSStream::ODSStream()
	: DSStream(DSState::Out)
{

}

IDSStream::IDSStream(std::vector<std::byte>&& aBuffer)
	: DSStream(DSState::In)
	, myReadSerializer(std::move(aBuffer))
{

}

IDSStream::IDSStream(std::span<const std::byte> aBuffer)
	: DSStream(DSState::In)
	, myReadSerializer(aBuffer)
{

}
