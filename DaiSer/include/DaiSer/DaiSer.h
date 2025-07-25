#pragma once

#include "Serialization/Serializer.h"

namespace DaiSer
{
	class DSOut;
	class DSIn;

	//void DWrite(DSOut& aOut);

	//void DRead(DSIn& aIn);

	namespace details
	{
		class DaiSer
		{
		public:
			DaiSer() = delete;
			virtual ~DaiSer() = default;

		private:

		};
	}

	class DSOut : public details::DaiSer
	{
	public:

	private:

	};

	class DSIn : public details::DaiSer
	{
	public:

	private:

	};
}