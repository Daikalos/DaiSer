#pragma once

#include <numeric>
#include <filesystem>

#include "Serialization/Serializer.h"
#include "Serialization/FieldID.h"

namespace DaiSer
{
	/// For clarity
	/// 
	///	Out = Write = Serialize -> Write to buffer from type
	/// In	= Read	= Deserialize -> Write to type from buffer

	inline constexpr FieldIDType NULL_ID = std::numeric_limits<FieldIDType>::max();

	enum class DSState
	{
		Out,
		In
	};

	class DSStream;

	class DSScope
	{
	public:
		template<typename T>
		void Serialize(FieldIDType aID, T& aInOutData);

	private:
		DAISER_API DSScope(DSStream& aSerializer);

		template<typename T>
		void Write(FieldIDType aID, const T& aInData);

		template<typename T>
		void Read(FieldIDType aID, T& aInData);

		DAISER_API void LoadNextID();
		DAISER_API void SkipField();

		DSStream*	mySerializer	= nullptr;
		FieldIDType	myPrevID		= NULL_ID;
		FieldIDType	myNextID		= NULL_ID;

		friend class DSStream;
	};

	class DSStream
	{
	public:
		DSStream() = delete;
		virtual ~DSStream() = default;

		DSState GetState() const noexcept { return myState; }

		DAISER_API DSScope Start();
		DAISER_API operator DSScope();

	protected:
		DAISER_API DSStream(DSState aState);

		DSState myState;

		friend class DSScope;
	};

	class ODSStream : public DSStream
	{
	public:
		DAISER_API ODSStream();

	private:
		template<typename T>
		void Serialize(FieldIDType aID, const T& aInData);

		WriteSerializer myWriteSerializer;

		friend class DSScope;
	};

	class IDSStream : public DSStream
	{
	public:
		DAISER_API IDSStream(std::vector<std::byte>&& aBuffer);
		DAISER_API IDSStream(std::span<const std::byte> aBuffer);

	private:
		template<typename T>
		void Serialize(FieldIDType aID, T& aOutData);

		ReadSerializer myReadSerializer;

		friend class DSScope;
	};

	template<typename T>
	inline void DSScope::Serialize(FieldIDType aID, T& aInOutData)
	{
		assert(mySerializer != nullptr && "Serializer must be defined!");

		switch (mySerializer->GetState())
		{
			case DSState::Out:
			{
				Write(aID, aInOutData);
				break;
			}
			case DSState::In:
			{
				Read(aID, aInOutData);
				break;
			}
		}
	}

	template<typename T>
	inline void DSScope::Write(FieldIDType aID, const T& aInData)
	{
		std::uint64_t deltaID = (myPrevID == NULL_ID) ? aID : (aID - myPrevID - 1);

		// construct field ID from id and delta ID

		ODSStream& out = static_cast<ODSStream&>(*mySerializer);
		out.Serialize(aID, aInData);

		myPrevID = aID;
	}

	template<typename T>
	inline void DSScope::Read(FieldIDType aID, T& aOutData)
	{
		IDSStream& in = static_cast<IDSStream&>(*mySerializer);

		in.Serialize(aID, aOutData);
	}

	template<typename T>
	inline void ODSStream::Serialize(FieldIDType aID, const T& aInData)
	{

	}

	template<typename T>
	inline void IDSStream::Serialize(FieldIDType aID, T& aOutData)
	{

	}
}