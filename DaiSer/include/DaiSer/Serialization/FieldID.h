#pragma once

#include <cstdint>

namespace DaiSer
{
	using FieldIDType = std::uint64_t;

	class FieldID
	{
	public:
		enum
		{

		};

		FieldIDType GetID() const noexcept { return myID; }

	private:
		FieldIDType myID = 0;
	};
}