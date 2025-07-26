#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <array>

namespace DaiSer
{ 
	/// Packs the values from left most bit to right most bit with the given sizes
	/// 
	template<std::uint64_t... BitSizes> requires ((BitSizes + ...) == sizeof(std::uint64_t) * CHAR_BIT)
	constexpr std::uint64_t PackValues64(const std::array<std::uint64_t, sizeof...(BitSizes)>& aValuesToPack)
	{
		constexpr auto ComputeDisplacements = []<std::uint64_t... BitSizes>() constexpr
		{
			constexpr std::array<std::uint64_t, sizeof...(BitSizes)> bitArr{ BitSizes... };
			std::array<std::uint64_t, sizeof...(BitSizes)> bitDisplacements{};

			for (std::uint64_t i = 1; i < sizeof...(BitSizes); ++i)
				bitDisplacements[i] += bitDisplacements[i - 1] + bitArr[i - 1];

			return bitDisplacements;
		};

		constexpr std::array<std::uint64_t, sizeof...(BitSizes)> bitArr { BitSizes... };
		constexpr std::array<std::uint64_t, sizeof...(BitSizes)> bitDisplacementArr = ComputeDisplacements.template operator()<BitSizes...>();

		std::uint64_t result{};

		for (std::uint64_t i = 0; i < sizeof...(BitSizes); ++i)
		{
			result |= aValuesToPack[i] << uint64_t(sizeof(std::uint64_t) * CHAR_BIT - (bitArr[i] + bitDisplacementArr[i]));
		}

		return result;
	}

	/// Extracts N bits from a number with the offset starting from left most bit
	/// 
	template<std::uint64_t BitSize, std::uint64_t BitOffset>
	constexpr std::uint64_t ExtractValue64(std::uint64_t aPackedValues)
	{
		constexpr std::uint64_t bitPos = sizeof(std::uint64_t) * CHAR_BIT - (BitOffset + BitSize);
		return (aPackedValues >> bitPos) & ((1ULL << BitSize) - 1);
	}

	/// Packs the values from left most bit to right most bit with the given sizes
	/// 
	template<std::uint32_t... BitSizes> requires ((BitSizes + ...) == sizeof(std::uint32_t) * CHAR_BIT)
	constexpr std::uint32_t PackValues32(const std::array<std::uint32_t, sizeof...(BitSizes)>& aValuesToPack)
	{
		constexpr auto ComputeDisplacements = []<std::uint32_t... BitSizes>() constexpr
		{
			constexpr std::array<std::uint32_t, sizeof...(BitSizes)> bitArr{ BitSizes... };
			std::array<std::uint32_t, sizeof...(BitSizes)> bitDisplacements{};

			for (std::uint32_t i = 1; i < sizeof...(BitSizes); ++i)
				bitDisplacements[i] += bitDisplacements[i - 1] + bitArr[i - 1];

			return bitDisplacements;
		};

		constexpr std::array<std::uint32_t, sizeof...(BitSizes)> bitArr { BitSizes... };
		constexpr std::array<std::uint32_t, sizeof...(BitSizes)> bitDisplacementArr = ComputeDisplacements.template operator()<BitSizes...>();

		std::uint32_t result{};

		for (std::uint32_t i = 0; i < sizeof...(BitSizes); ++i)
		{
			result |= aValuesToPack[i] << uint32_t(sizeof(std::uint32_t) * CHAR_BIT - (bitArr[i] + bitDisplacementArr[i]));
		}

		return result;
	}

	/// Extracts N bits from a number with the offset starting from left most bit
	/// 
	template<std::uint32_t BitSize, std::uint32_t BitOffset>
	constexpr std::uint32_t ExtractValue32(std::uint32_t aPackedValues)
	{
		constexpr std::uint32_t bitPos = sizeof(std::uint32_t) * CHAR_BIT - (BitOffset + BitSize);
		return (aPackedValues >> bitPos) & ((1ULL << BitSize) - 1);
	}
}