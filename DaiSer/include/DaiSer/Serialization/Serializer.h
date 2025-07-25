#pragma once

#include <cassert>
#include <vector>
#include <span>
#include <string>

#include <DaiSer/Config.h>

#include "FieldID.h"

/// This header just contains pure serialization, where it applies template specialization 
/// to handle different types.

namespace DaiSer
{
	enum class SerializerState
	{
		Write,	// Copes bytes from type onto buffer
		Read,	// Copies bytes from buffer onto type
	};

	template<typename T>
	struct SerializeImpl
	{
		NODISC std::size_t Write(const T& aInData, std::vector<std::byte>& aOutBytes, std::size_t aOffset)
			requires (std::is_trivially_copyable_v<T>); // trivially copyable is required to prevent UB

		NODISC std::size_t Read(T& aOutData, const std::vector<std::byte>& aInBytes, std::size_t aOffset)
			requires (std::is_trivially_copyable_v<T>); // trivially copyable is required to prevent UB
	};

	template<>
	struct DAISER_API SerializeImpl<std::string>
	{
		NODISC std::size_t Write(const std::string& aInData, std::vector<std::byte>& aOutBytes, std::size_t aOffset);

		NODISC std::size_t Read(std::string& aOutData, const std::vector<std::byte>& aInBytes, std::size_t aOffset);
	};

	template<>
	struct DAISER_API SerializeImpl<std::wstring>
	{
		NODISC std::size_t Write(const std::wstring& aInData, std::vector<std::byte>& aOutBytes, std::size_t aOffset);

		NODISC std::size_t Read(std::wstring& aOutData, const std::vector<std::byte>& aInBytes, std::size_t aOffset);
	};

	template<typename T>
	struct SerializeImpl<std::vector<T>>
	{
		NODISC std::size_t Write(const std::vector<T>& aInData, std::vector<std::byte>& aOutBytes, std::size_t aOffset)
			requires (std::is_trivially_copyable_v<T>);

		NODISC std::size_t Read(std::vector<T>& aOutData, const std::vector<std::byte>& aInBytes, std::size_t aOffset)
			requires (std::is_trivially_copyable_v<T>);

		NODISC std::size_t Write(const std::vector<T>& aInData, std::vector<std::byte>& aOutBytes, std::size_t aOffset)
			requires (!std::is_trivially_copyable_v<T>); // user must provide their own custom specialization for this type to work

		NODISC std::size_t Read(std::vector<T>& aOutData, const std::vector<std::byte>& aInBytes, std::size_t aOffset)
			requires (!std::is_trivially_copyable_v<T>);
	};

	template<typename T, typename U> requires (!std::is_trivially_copyable_v<T> || !std::is_trivially_copyable_v<U>)
	struct SerializeImpl<std::pair<T, U>>
	{
		NODISC std::size_t Write(const std::pair<T, U>& aInData, std::vector<std::byte>& aOutBytes, std::size_t aOffset);

		NODISC std::size_t Read(std::pair<T, U>& aOutData, const std::vector<std::byte>& aInBytes, std::size_t aOffset);
	};

	template<typename... Ts> requires (!std::is_trivially_copyable_v<Ts> || ...)
	struct SerializeImpl<std::tuple<Ts...>>
	{
		NODISC std::size_t Write(const std::tuple<Ts...>& aInData, std::vector<std::byte>& aOutBytes, std::size_t aOffset);

		NODISC std::size_t Read(std::tuple<Ts...>& aOutData, const std::vector<std::byte>& aInBytes, std::size_t aOffset);

	private:
		template<std::size_t I = 0>
		std::size_t WriteTuple(const std::tuple<Ts...>& aInData, std::vector<std::byte>& aOutBytes, std::size_t aOffset);

		template<std::size_t I = 0>
		std::size_t ReadTuple(std::tuple<Ts...>& aOutData, const std::vector<std::byte>& aInBytes, std::size_t aOffset);
	};

	class Serializer
	{
	public:
		Serializer() = delete;
		virtual ~Serializer() = default;

		NODISC SerializerState GetState() const noexcept { return myState; }
		NODISC std::size_t GetOffset() const noexcept { return myOffset; }

	protected:
		DAISER_API Serializer(SerializerState aState);

		SerializerState			myState;
		std::vector<std::byte>	myBuffer;
		std::size_t				myOffset;
	};

	/// Serialize variables to buffer
	/// 
	class WriteSerializer : protected Serializer
	{
	public:
		DAISER_API WriteSerializer();

		template<typename T>
		void Serialize(const T& aInData);

		NODISC std::vector<std::byte>&& MoveBuffer() { return std::move(myBuffer); }

		NODISC std::span<const std::byte> GetBuffer() const noexcept { return myBuffer; }
		NODISC const std::byte* GetBufferData() const noexcept { return myBuffer.data(); }

		DAISER_API void ReserveBytesToFit(std::size_t aNumBytesToFit);

		DAISER_API void FitBufferToOffset();

		DAISER_API void Clear();
	};

	/// Deserialize buffer to variables
	/// 
	class ReadSerializer : protected Serializer
	{
	public:
		DAISER_API ReadSerializer(std::vector<std::byte>&& aBuffer);
		DAISER_API ReadSerializer(std::span<const std::byte> aBuffer);

		template<typename T>
		void Deserialize(T& aOutData) const;
	};

	template<typename T>
	inline void WriteSerializer::Serialize(const T& aInData)
	{
		myOffset += SerializeImpl<std::decay_t<T>>{}.Write(aInData, myBuffer, myOffset);
	}

	template<typename T>
	inline void ReadSerializer::Deserialize(T& aOutData) const
	{
		myOffset += SerializeImpl<std::decay_t<T>>{}.Read(aOutData, myBuffer, myOffset);
	}

	template<typename T>
	inline std::size_t SerializeImpl<T>::Write(const T& aInData, std::vector<std::byte>& aOutBytes, std::size_t aOffset)
		requires (std::is_trivially_copyable_v<T>)
	{
		static constexpr std::size_t numBytes = sizeof(T);

		aOutBytes.resize(aOffset + numBytes);
		memcpy_s(aOutBytes.data() + aOffset, numBytes, &aInData, numBytes);

		return numBytes;
	}

	template<typename T>
	inline std::size_t SerializeImpl<T>::Read(T& aOutData, const std::vector<std::byte>& aInBytes, std::size_t aOffset)
		requires (std::is_trivially_copyable_v<T>)
	{
		static constexpr std::size_t numBytes = sizeof(T);

		assert((aOffset + numBytes) <= aInBytes.size() && "Not enough memory to read from!");
		memcpy_s(&aOutData, numBytes, aInBytes.data() + aOffset, numBytes);

		return numBytes;
	}

	template<typename T>
	inline std::size_t SerializeImpl<std::vector<T>>::Write(const std::vector<T>& aInData, std::vector<std::byte>& aOutBytes, std::size_t aOffset)
		requires (std::is_trivially_copyable_v<T>)
	{
		static constexpr std::size_t TYPE_SIZE = sizeof(T);

		std::size_t numElements = aInData.size();
		std::size_t numBytes	= TYPE_SIZE * numElements;

		aOutBytes.resize(aOffset + numBytes + sizeof(std::size_t));

		memcpy_s(aOutBytes.data() + aOffset, sizeof(std::size_t), &numElements, sizeof(std::size_t));

		aOffset += sizeof(std::size_t);

		memcpy_s(aOutBytes.data() + aOffset, numBytes, aInData.data(), numBytes);

		return numBytes + sizeof(std::size_t);
	}
	template<typename T>
	inline std::size_t SerializeImpl<std::vector<T>>::Read(std::vector<T>& aOutData, const std::vector<std::byte>& aInBytes, std::size_t aOffset)
		requires (std::is_trivially_copyable_v<T>)
	{
		static constexpr std::size_t TYPE_SIZE = sizeof(T);

		std::size_t numElements = 0;
		memcpy_s(&numElements, sizeof(std::size_t), aOutData.data() + aOffset, sizeof(std::size_t));

		std::size_t numBytes = TYPE_SIZE * numElements;

		assert((aOffset + numBytes + sizeof(std::size_t)) <= aInBytes.size() && "Not enough memory to read from!");
			
		aOutData.resize(numElements);

		aOffset += sizeof(std::size_t);

		memcpy_s(aOutData.data(), numBytes, aInBytes.data() + aOffset, numBytes);

		return numBytes + sizeof(std::size_t);
	}

	template<typename T>
	inline std::size_t SerializeImpl<std::vector<T>>::Write(const std::vector<T>& aInData, std::vector<std::byte>& aOutBytes, std::size_t aOffset)
		requires (!std::is_trivially_copyable_v<T>)
	{
		aOutBytes.resize(aOffset + sizeof(std::size_t));

		std::size_t numElements = aInOutData.size();
		memcpy_s(aInOutBytes.data() + aOffset, sizeof(std::size_t), &numElements, sizeof(std::size_t));

		std::size_t numBytes = sizeof(std::size_t);

		for (std::size_t i = 0; i < numElements; ++i)
		{
			numBytes += SerializeImpl<T>{}.Write(aInOutData[i], aOutBytes, aOffset + numBytes);
		}

		return numBytes;
	}
	template<typename T>
	inline std::size_t SerializeImpl<std::vector<T>>::Read(std::vector<T>& aOutData, const std::vector<std::byte>& aInBytes, std::size_t aOffset)
		requires (!std::is_trivially_copyable_v<T>)
	{
		std::size_t numElements = 0;
		memcpy_s(&numElements, sizeof(std::size_t), aInBytes.data() + aOffset, sizeof(std::size_t));

		aOutData.resize(numElements);

		std::size_t numBytes = sizeof(std::size_t);

		for (std::size_t i = 0; i < numElements; ++i)
		{
			numBytes += SerializeImpl<T>{}.Read(aOutData[i], aInBytes, aOffset + numBytes);
		}

		return numBytes;
	}

	template<typename T, typename U> requires (!std::is_trivially_copyable_v<T> || !std::is_trivially_copyable_v<U>)
	inline std::size_t SerializeImpl<std::pair<T, U>>::Write(const std::pair<T, U>& aInData, std::vector<std::byte>& aOutBytes, std::size_t aOffset)
	{
		const std::size_t prevOffset = aOffset;

		aOffset += SerializeImpl<T>{}.Write(aInData.first, aOutBytes, aOffset);
		aOffset += SerializeImpl<U>{}.Write(aInData.second, aOutBytes, aOffset);

		const std::size_t numBytes = aOffset - prevOffset;

		return numBytes;
	}

	template<typename T, typename U> requires (!std::is_trivially_copyable_v<T> || !std::is_trivially_copyable_v<U>)
	inline std::size_t SerializeImpl<std::pair<T, U>>::Read(std::pair<T, U>& aOutData, const std::vector<std::byte>& aInBytes, std::size_t aOffset)
	{
		const std::size_t prevOffset = aOffset;

		aOffset += SerializeImpl<T>{}.Read(aOutData.first, aInBytes, aOffset);
		aOffset += SerializeImpl<U>{}.Read(aOutData.second, aInBytes, aOffset);

		const std::size_t numBytes = aOffset - prevOffset;

		return numBytes;
	}

	template<typename... Ts> requires (!std::is_trivially_copyable_v<Ts> || ...)
	inline std::size_t SerializeImpl<std::tuple<Ts...>>::Write(const std::tuple<Ts...>& aInData, std::vector<std::byte>& aOutBytes, std::size_t aOffset)
	{
		const std::size_t prevOffset = aOffset;

		aOffset = WriteTuple<>(aInData, aOutBytes, aOffset);

		const std::size_t numBytes = aOffset - prevOffset;

		return numBytes;
	}

	template<typename... Ts> requires (!std::is_trivially_copyable_v<Ts> || ...)
	inline std::size_t SerializeImpl<std::tuple<Ts...>>::Read(std::tuple<Ts...>& aOutData, const std::vector<std::byte>& aInBytes, std::size_t aOffset)
	{
		const std::size_t prevOffset = aOffset;

		aOffset = ReadTuple<>(aOutData, aInBytes, aOffset);

		const std::size_t numBytes = aOffset - prevOffset;

		return numBytes;
	}

	template<typename... Ts> requires (!std::is_trivially_copyable_v<Ts> || ...)
	template<std::size_t I>
	inline std::size_t SerializeImpl<std::tuple<Ts...>>::WriteTuple(const std::tuple<Ts...>& aInData, std::vector<std::byte>& aOutBytes, std::size_t aOffset)
	{
		if constexpr (I != sizeof...(Ts))
		{
			aOffset += SerializeImpl<std::tuple_element_t<I, std::tuple<Ts...>>>{}.Write(std::get<I>(aInData), aOutBytes, aOffset);
			return WriteTuple<I + 1>(aInData, aOutBytes, aOffset);
		}
		else
		{
			return aOffset;
		}
	}

	template<typename... Ts> requires (!std::is_trivially_copyable_v<Ts> || ...)
	template<std::size_t I>
	inline std::size_t SerializeImpl<std::tuple<Ts...>>::ReadTuple(std::tuple<Ts...>& aOutData, const std::vector<std::byte>& aInBytes, std::size_t aOffset)
	{
		if constexpr (I != sizeof...(Ts))
		{
			aOffset += SerializeImpl<std::tuple_element_t<I, std::tuple<Ts...>>>{}.Read(std::get<I>(aOutData), aInBytes, aOffset);
			return WriteTuple<I + 1>(aOutData, aInBytes, aOffset);
		}
		else
		{
			return aOffset;
		}
	}

	template<typename T>
	inline WriteSerializer& operator<<(WriteSerializer& aWriteSerializer, const T& aInData)
	{
		aWriteSerializer.Serialize(aInData);
		return aWriteSerializer;
	}
	template<typename T>
	inline ReadSerializer& operator>>(ReadSerializer& aReadSerializer, T& aOutData)
	{
		aReadSerializer.Serialize(aOutData);
		return aReadSerializer;
	}
}