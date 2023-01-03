#pragma once

namespace RHI{
	enum RBufferUsage
	{
		RBufferUsage_None=1,
		RBufferUsage_Dynamic=2,
	};

	enum RFormat {
        FORMAT_UNDEFINED = 0,
        FORMAT_R4G4_UNORM_PACK8 = 1,
        FORMAT_R4G4B4A4_UNORM_PACK16 = 2,
        FORMAT_B4G4R4A4_UNORM_PACK16 = 3,
        FORMAT_R5G6B5_UNORM_PACK16 = 4,
        FORMAT_B5G6R5_UNORM_PACK16 = 5,
        FORMAT_R5G5B5A1_UNORM_PACK16 = 6,
        FORMAT_B5G5R5A1_UNORM_PACK16 = 7,
        FORMAT_A1R5G5B5_UNORM_PACK16 = 8,
        FORMAT_R8_UNORM = 9,
        FORMAT_R8_SNORM = 10,
        FORMAT_R8_USCALED = 11,
        FORMAT_R8_SSCALED = 12,
        FORMAT_R8_UINT = 13,
        FORMAT_R8_SINT = 14,
        FORMAT_R8_SRGB = 15,
        FORMAT_R8G8_UNORM = 16,
        FORMAT_R8G8_SNORM = 17,
        FORMAT_R8G8_USCALED = 18,
        FORMAT_R8G8_SSCALED = 19,
        FORMAT_R8G8_UINT = 20,
        FORMAT_R8G8_SINT = 21,
        FORMAT_R8G8_SRGB = 22,
        FORMAT_R8G8B8_UNORM = 23,
        FORMAT_R8G8B8_SNORM = 24,
        FORMAT_R8G8B8_USCALED = 25,
        FORMAT_R8G8B8_SSCALED = 26,
        FORMAT_R8G8B8_UINT = 27,
        FORMAT_R8G8B8_SINT = 28,
        FORMAT_R8G8B8_SRGB = 29,
        FORMAT_B8G8R8_UNORM = 30,
        FORMAT_B8G8R8_SNORM = 31,
        FORMAT_B8G8R8_USCALED = 32,
        FORMAT_B8G8R8_SSCALED = 33,
        FORMAT_B8G8R8_UINT = 34,
        FORMAT_B8G8R8_SINT = 35,
        FORMAT_B8G8R8_SRGB = 36,
        FORMAT_R8G8B8A8_UNORM = 37,
        FORMAT_R8G8B8A8_SNORM = 38,
        FORMAT_R8G8B8A8_USCALED = 39,
        FORMAT_R8G8B8A8_SSCALED = 40,
        FORMAT_R8G8B8A8_UINT = 41,
        FORMAT_R8G8B8A8_SINT = 42,
        FORMAT_R8G8B8A8_SRGB = 43,
        FORMAT_B8G8R8A8_UNORM = 44,
        FORMAT_B8G8R8A8_SNORM = 45,
        FORMAT_B8G8R8A8_USCALED = 46,
        FORMAT_B8G8R8A8_SSCALED = 47,
        FORMAT_B8G8R8A8_UINT = 48,
        FORMAT_B8G8R8A8_SINT = 49,
        FORMAT_B8G8R8A8_SRGB = 50,
        FORMAT_A8B8G8R8_UNORM_PACK32 = 51,
        FORMAT_A8B8G8R8_SNORM_PACK32 = 52,
        FORMAT_A8B8G8R8_USCALED_PACK32 = 53,
        FORMAT_A8B8G8R8_SSCALED_PACK32 = 54,
        FORMAT_A8B8G8R8_UINT_PACK32 = 55,
        FORMAT_A8B8G8R8_SINT_PACK32 = 56,
        FORMAT_A8B8G8R8_SRGB_PACK32 = 57,
        FORMAT_A2R10G10B10_UNORM_PACK32 = 58,
        FORMAT_A2R10G10B10_SNORM_PACK32 = 59,
        FORMAT_A2R10G10B10_USCALED_PACK32 = 60,
        FORMAT_A2R10G10B10_SSCALED_PACK32 = 61,
        FORMAT_A2R10G10B10_UINT_PACK32 = 62,
        FORMAT_A2R10G10B10_SINT_PACK32 = 63,
        FORMAT_A2B10G10R10_UNORM_PACK32 = 64,
        FORMAT_A2B10G10R10_SNORM_PACK32 = 65,
        FORMAT_A2B10G10R10_USCALED_PACK32 = 66,
        FORMAT_A2B10G10R10_SSCALED_PACK32 = 67,
        FORMAT_A2B10G10R10_UINT_PACK32 = 68,
        FORMAT_A2B10G10R10_SINT_PACK32 = 69,
        FORMAT_R16_UNORM = 70,
        FORMAT_R16_SNORM = 71,
        FORMAT_R16_USCALED = 72,
        FORMAT_R16_SSCALED = 73,
        FORMAT_R16_UINT = 74,
        FORMAT_R16_SINT = 75,
        FORMAT_R16_SFLOAT = 76,
        FORMAT_R16G16_UNORM = 77,
        FORMAT_R16G16_SNORM = 78,
        FORMAT_R16G16_USCALED = 79,
        FORMAT_R16G16_SSCALED = 80,
        FORMAT_R16G16_UINT = 81,
        FORMAT_R16G16_SINT = 82,
        FORMAT_R16G16_SFLOAT = 83,
        FORMAT_R16G16B16_UNORM = 84,
        FORMAT_R16G16B16_SNORM = 85,
        FORMAT_R16G16B16_USCALED = 86,
        FORMAT_R16G16B16_SSCALED = 87,
        FORMAT_R16G16B16_UINT = 88,
        FORMAT_R16G16B16_SINT = 89,
        FORMAT_R16G16B16_SFLOAT = 90,
        FORMAT_R16G16B16A16_UNORM = 91,
        FORMAT_R16G16B16A16_SNORM = 92,
        FORMAT_R16G16B16A16_USCALED = 93,
        FORMAT_R16G16B16A16_SSCALED = 94,
        FORMAT_R16G16B16A16_UINT = 95,
        FORMAT_R16G16B16A16_SINT = 96,
        FORMAT_R16G16B16A16_SFLOAT = 97,
        FORMAT_R32_UINT = 98,
        FORMAT_R32_SINT = 99,
        FORMAT_R32_SFLOAT = 100,
        FORMAT_R32G32_UINT = 101,
        FORMAT_R32G32_SINT = 102,
        FORMAT_R32G32_SFLOAT = 103,
        FORMAT_R32G32B32_UINT = 104,
        FORMAT_R32G32B32_SINT = 105,
        FORMAT_R32G32B32_SFLOAT = 106,
        FORMAT_R32G32B32A32_UINT = 107,
        FORMAT_R32G32B32A32_SINT = 108,
        FORMAT_R32G32B32A32_SFLOAT = 109,
        FORMAT_R64_UINT = 110,
        FORMAT_R64_SINT = 111,
        FORMAT_R64_SFLOAT = 112,
        FORMAT_R64G64_UINT = 113,
        FORMAT_R64G64_SINT = 114,
        FORMAT_R64G64_SFLOAT = 115,
        FORMAT_R64G64B64_UINT = 116,
        FORMAT_R64G64B64_SINT = 117,
        FORMAT_R64G64B64_SFLOAT = 118,
        FORMAT_R64G64B64A64_UINT = 119,
        FORMAT_R64G64B64A64_SINT = 120,
        FORMAT_R64G64B64A64_SFLOAT = 121,
        FORMAT_B10G11R11_UFLOAT_PACK32 = 122,
        FORMAT_E5B9G9R9_UFLOAT_PACK32 = 123,
        FORMAT_D16_UNORM = 124,
        FORMAT_X8_D24_UNORM_PACK32 = 125,
        FORMAT_D32_SFLOAT = 126,
        FORMAT_S8_UINT = 127,
        FORMAT_D16_UNORM_S8_UINT = 128,
        FORMAT_D24_UNORM_S8_UINT = 129,
        FORMAT_D32_SFLOAT_S8_UINT = 130,
        FORMAT_BC1_RGB_UNORM_BLOCK = 131,
        FORMAT_BC1_RGB_SRGB_BLOCK = 132,
        FORMAT_BC1_RGBA_UNORM_BLOCK = 133,
        FORMAT_BC1_RGBA_SRGB_BLOCK = 134,
        FORMAT_BC2_UNORM_BLOCK = 135,
        FORMAT_BC2_SRGB_BLOCK = 136,
        FORMAT_BC3_UNORM_BLOCK = 137,
        FORMAT_BC3_SRGB_BLOCK = 138,
        FORMAT_BC4_UNORM_BLOCK = 139,
        FORMAT_BC4_SNORM_BLOCK = 140,
        FORMAT_BC5_UNORM_BLOCK = 141,
        FORMAT_BC5_SNORM_BLOCK = 142,
        FORMAT_BC6H_UFLOAT_BLOCK = 143,
        FORMAT_BC6H_SFLOAT_BLOCK = 144,
        FORMAT_BC7_UNORM_BLOCK = 145,
        FORMAT_BC7_SRGB_BLOCK = 146,
        FORMAT_ETC2_R8G8B8_UNORM_BLOCK = 147,
        FORMAT_ETC2_R8G8B8_SRGB_BLOCK = 148,
        FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK = 149,
        FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK = 150,
        FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK = 151,
        FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK = 152,
        FORMAT_EAC_R11_UNORM_BLOCK = 153,
        FORMAT_EAC_R11_SNORM_BLOCK = 154,
        FORMAT_EAC_R11G11_UNORM_BLOCK = 155,
        FORMAT_EAC_R11G11_SNORM_BLOCK = 156,
        FORMAT_ASTC_4x4_UNORM_BLOCK = 157,
        FORMAT_ASTC_4x4_SRGB_BLOCK = 158,
        FORMAT_ASTC_5x4_UNORM_BLOCK = 159,
        FORMAT_ASTC_5x4_SRGB_BLOCK = 160,
        FORMAT_ASTC_5x5_UNORM_BLOCK = 161,
        FORMAT_ASTC_5x5_SRGB_BLOCK = 162,
        FORMAT_ASTC_6x5_UNORM_BLOCK = 163,
        FORMAT_ASTC_6x5_SRGB_BLOCK = 164,
        FORMAT_ASTC_6x6_UNORM_BLOCK = 165,
        FORMAT_ASTC_6x6_SRGB_BLOCK = 166,
        FORMAT_ASTC_8x5_UNORM_BLOCK = 167,
        FORMAT_ASTC_8x5_SRGB_BLOCK = 168,
        FORMAT_ASTC_8x6_UNORM_BLOCK = 169,
        FORMAT_ASTC_8x6_SRGB_BLOCK = 170,
        FORMAT_ASTC_8x8_UNORM_BLOCK = 171,
        FORMAT_ASTC_8x8_SRGB_BLOCK = 172,
        FORMAT_ASTC_10x5_UNORM_BLOCK = 173,
        FORMAT_ASTC_10x5_SRGB_BLOCK = 174,
        FORMAT_ASTC_10x6_UNORM_BLOCK = 175,
        FORMAT_ASTC_10x6_SRGB_BLOCK = 176,
        FORMAT_ASTC_10x8_UNORM_BLOCK = 177,
        FORMAT_ASTC_10x8_SRGB_BLOCK = 178,
        FORMAT_ASTC_10x10_UNORM_BLOCK = 179,
        FORMAT_ASTC_10x10_SRGB_BLOCK = 180,
        FORMAT_ASTC_12x10_UNORM_BLOCK = 181,
        FORMAT_ASTC_12x10_SRGB_BLOCK = 182,
        FORMAT_ASTC_12x12_UNORM_BLOCK = 183,
        FORMAT_ASTC_12x12_SRGB_BLOCK = 184,
        FORMAT_G8B8G8R8_422_UNORM = 1000156000,
        FORMAT_B8G8R8G8_422_UNORM = 1000156001,
        FORMAT_G8_B8_R8_3PLANE_420_UNORM = 1000156002,
        FORMAT_G8_B8R8_2PLANE_420_UNORM = 1000156003,
        FORMAT_G8_B8_R8_3PLANE_422_UNORM = 1000156004,
        FORMAT_G8_B8R8_2PLANE_422_UNORM = 1000156005,
        FORMAT_G8_B8_R8_3PLANE_444_UNORM = 1000156006,
        FORMAT_R10X6_UNORM_PACK16 = 1000156007,
        FORMAT_R10X6G10X6_UNORM_2PACK16 = 1000156008,
        FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16 = 1000156009,
        FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16 = 1000156010,
        FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16 = 1000156011,
        FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16 = 1000156012,
        FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16 = 1000156013,
        FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16 = 1000156014,
        FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16 = 1000156015,
        FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16 = 1000156016,
        FORMAT_R12X4_UNORM_PACK16 = 1000156017,
        FORMAT_R12X4G12X4_UNORM_2PACK16 = 1000156018,
        FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16 = 1000156019,
        FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16 = 1000156020,
        FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16 = 1000156021,
        FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16 = 1000156022,
        FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16 = 1000156023,
        FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16 = 1000156024,
        FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16 = 1000156025,
        FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16 = 1000156026,
        FORMAT_G16B16G16R16_422_UNORM = 1000156027,
        FORMAT_B16G16R16G16_422_UNORM = 1000156028,
        FORMAT_G16_B16_R16_3PLANE_420_UNORM = 1000156029,
        FORMAT_G16_B16R16_2PLANE_420_UNORM = 1000156030,
        FORMAT_G16_B16_R16_3PLANE_422_UNORM = 1000156031,
        FORMAT_G16_B16R16_2PLANE_422_UNORM = 1000156032,
        FORMAT_G16_B16_R16_3PLANE_444_UNORM = 1000156033,
        FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG = 1000054000,
        FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG = 1000054001,
        FORMAT_PVRTC2_2BPP_UNORM_BLOCK_IMG = 1000054002,
        FORMAT_PVRTC2_4BPP_UNORM_BLOCK_IMG = 1000054003,
        FORMAT_PVRTC1_2BPP_SRGB_BLOCK_IMG = 1000054004,
        FORMAT_PVRTC1_4BPP_SRGB_BLOCK_IMG = 1000054005,
        FORMAT_PVRTC2_2BPP_SRGB_BLOCK_IMG = 1000054006,
        FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG = 1000054007,
        FORMAT_ASTC_4x4_SFLOAT_BLOCK_EXT = 1000066000,
        FORMAT_ASTC_5x4_SFLOAT_BLOCK_EXT = 1000066001,
        FORMAT_ASTC_5x5_SFLOAT_BLOCK_EXT = 1000066002,
        FORMAT_ASTC_6x5_SFLOAT_BLOCK_EXT = 1000066003,
        FORMAT_ASTC_6x6_SFLOAT_BLOCK_EXT = 1000066004,
        FORMAT_ASTC_8x5_SFLOAT_BLOCK_EXT = 1000066005,
        FORMAT_ASTC_8x6_SFLOAT_BLOCK_EXT = 1000066006,
        FORMAT_ASTC_8x8_SFLOAT_BLOCK_EXT = 1000066007,
        FORMAT_ASTC_10x5_SFLOAT_BLOCK_EXT = 1000066008,
        FORMAT_ASTC_10x6_SFLOAT_BLOCK_EXT = 1000066009,
        FORMAT_ASTC_10x8_SFLOAT_BLOCK_EXT = 1000066010,
        FORMAT_ASTC_10x10_SFLOAT_BLOCK_EXT = 1000066011,
        FORMAT_ASTC_12x10_SFLOAT_BLOCK_EXT = 1000066012,
        FORMAT_ASTC_12x12_SFLOAT_BLOCK_EXT = 1000066013,
        FORMAT_G8_B8R8_2PLANE_444_UNORM_EXT = 1000330000,
        FORMAT_G10X6_B10X6R10X6_2PLANE_444_UNORM_3PACK16_EXT = 1000330001,
        FORMAT_G12X4_B12X4R12X4_2PLANE_444_UNORM_3PACK16_EXT = 1000330002,
        FORMAT_G16_B16R16_2PLANE_444_UNORM_EXT = 1000330003,
        FORMAT_A4R4G4B4_UNORM_PACK16_EXT = 1000340000,
        FORMAT_A4B4G4R4_UNORM_PACK16_EXT = 1000340001,
        FORMAT_G8B8G8R8_422_UNORM_KHR = FORMAT_G8B8G8R8_422_UNORM,
        FORMAT_B8G8R8G8_422_UNORM_KHR = FORMAT_B8G8R8G8_422_UNORM,
        FORMAT_G8_B8_R8_3PLANE_420_UNORM_KHR = FORMAT_G8_B8_R8_3PLANE_420_UNORM,
        FORMAT_G8_B8R8_2PLANE_420_UNORM_KHR = FORMAT_G8_B8R8_2PLANE_420_UNORM,
        FORMAT_G8_B8_R8_3PLANE_422_UNORM_KHR = FORMAT_G8_B8_R8_3PLANE_422_UNORM,
        FORMAT_G8_B8R8_2PLANE_422_UNORM_KHR = FORMAT_G8_B8R8_2PLANE_422_UNORM,
        FORMAT_G8_B8_R8_3PLANE_444_UNORM_KHR = FORMAT_G8_B8_R8_3PLANE_444_UNORM,
        FORMAT_R10X6_UNORM_PACK16_KHR = FORMAT_R10X6_UNORM_PACK16,
        FORMAT_R10X6G10X6_UNORM_2PACK16_KHR = FORMAT_R10X6G10X6_UNORM_2PACK16,
        FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16_KHR = FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16,
        FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16_KHR = FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16,
        FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16_KHR = FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16,
        FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16_KHR = FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16,
        FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16_KHR = FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16,
        FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16_KHR = FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16,
        FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16_KHR = FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16,
        FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16_KHR = FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16,
        FORMAT_R12X4_UNORM_PACK16_KHR = FORMAT_R12X4_UNORM_PACK16,
        FORMAT_R12X4G12X4_UNORM_2PACK16_KHR = FORMAT_R12X4G12X4_UNORM_2PACK16,
        FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16_KHR = FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16,
        FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16_KHR = FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16,
        FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16_KHR = FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16,
        FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16_KHR = FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16,
        FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16_KHR = FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16,
        FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16_KHR = FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16,
        FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16_KHR = FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16,
        FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16_KHR = FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16,
        FORMAT_G16B16G16R16_422_UNORM_KHR = FORMAT_G16B16G16R16_422_UNORM,
        FORMAT_B16G16R16G16_422_UNORM_KHR = FORMAT_B16G16R16G16_422_UNORM,
        FORMAT_G16_B16_R16_3PLANE_420_UNORM_KHR = FORMAT_G16_B16_R16_3PLANE_420_UNORM,
        FORMAT_G16_B16R16_2PLANE_420_UNORM_KHR = FORMAT_G16_B16R16_2PLANE_420_UNORM,
        FORMAT_G16_B16_R16_3PLANE_422_UNORM_KHR = FORMAT_G16_B16_R16_3PLANE_422_UNORM,
        FORMAT_G16_B16R16_2PLANE_422_UNORM_KHR = FORMAT_G16_B16R16_2PLANE_422_UNORM,
        FORMAT_G16_B16_R16_3PLANE_444_UNORM_KHR = FORMAT_G16_B16_R16_3PLANE_444_UNORM,
        FORMAT_MAX_ENUM = 0x7FFFFFFF
	};

    enum RImageLayout
    {
        IMAGE_LAYOUT_UNDEFINED = 0,
        IMAGE_LAYOUT_GENERAL = 1,
        IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL = 2,
        IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL = 3,
        IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL = 4,
        IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL = 5,
        IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL = 6,
        IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL = 7,
        IMAGE_LAYOUT_PREINITIALIZED = 8,
        IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL = 1000117000,
        IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL = 1000117001,
        IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL = 1000241000,
        IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL = 1000241001,
        IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL = 1000241002,
        IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL = 1000241003,
        IMAGE_LAYOUT_PRESENT_SRC_KHR = 1000001002,
        IMAGE_LAYOUT_FRAGMENT_DENSITY_MAP_OPTIMAL_EXT = 1000218000,
        IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR = 1000164003,
        IMAGE_LAYOUT_ATTACHMENT_OPTIMAL_KHR = 1000314001,
        IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL_KHR = IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL,
        IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL_KHR = IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL,
        IMAGE_LAYOUT_SHADING_RATE_OPTIMAL_NV = IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR,
        IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL_KHR = IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
        IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL_KHR = IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL,
        IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL_KHR = IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL,
        IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL_KHR = IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL,
        IMAGE_LAYOUT_MAX_ENUM = 0x7FFFFFFF
    };

    enum RAttachmentType {
	    ATTACHMENT_COLOR,
        ATTACHMENT_DEPTH
    };

    enum RCommandBufferLevel {
       COMMAND_BUFFER_LEVEL_PRIMARY = 0,
       COMMAND_BUFFER_LEVEL_SECONDARY = 1,
       COMMAND_BUFFER_LEVEL_MAX_ENUM = 0x7FFFFFFF
    };

    enum RCommandBufferUsageFlagsBit {
        COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT = 0x00000001,
        COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT = 0x00000002,
        COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT = 0x00000004,
        COMMAND_BUFFER_USAGE_FLAG_BITS_MAX_ENUM = 0x7FFFFFFF
    };
    typedef unsigned int RCommandBufferUsageFlags;

    enum RPipelineStageFlagBits {
        PIPELINE_STAGE_TOP_OF_PIPE_BIT = 0x00000001,
        PIPELINE_STAGE_DRAW_INDIRECT_BIT = 0x00000002,
        PIPELINE_STAGE_VERTEX_INPUT_BIT = 0x00000004,
        PIPELINE_STAGE_VERTEX_SHADER_BIT = 0x00000008,
        PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT = 0x00000010,
        PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT = 0x00000020,
        PIPELINE_STAGE_GEOMETRY_SHADER_BIT = 0x00000040,
        PIPELINE_STAGE_FRAGMENT_SHADER_BIT = 0x00000080,
        PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT = 0x00000100,
        PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT = 0x00000200,
        PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT = 0x00000400,
        PIPELINE_STAGE_COMPUTE_SHADER_BIT = 0x00000800,
        PIPELINE_STAGE_TRANSFER_BIT = 0x00001000,
        PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT = 0x00002000,
        PIPELINE_STAGE_HOST_BIT = 0x00004000,
        PIPELINE_STAGE_ALL_GRAPHICS_BIT = 0x00008000,
        PIPELINE_STAGE_ALL_COMMANDS_BIT = 0x00010000,
        PIPELINE_STAGE_TRANSFORM_FEEDBACK_BIT_EXT = 0x01000000,
        PIPELINE_STAGE_CONDITIONAL_RENDERING_BIT_EXT = 0x00040000,
        PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR = 0x02000000,
        PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR = 0x00200000,
        PIPELINE_STAGE_TASK_SHADER_BIT_NV = 0x00080000,
        PIPELINE_STAGE_MESH_SHADER_BIT_NV = 0x00100000,
        PIPELINE_STAGE_FRAGMENT_DENSITY_PROCESS_BIT_EXT = 0x00800000,
        PIPELINE_STAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR = 0x00400000,
        PIPELINE_STAGE_COMMAND_PREPROCESS_BIT_NV = 0x00020000,
        PIPELINE_STAGE_NONE_KHR = 0,
        PIPELINE_STAGE_SHADING_RATE_IMAGE_BIT_NV = PIPELINE_STAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR,
        PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_NV = PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR,
        PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_NV = PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR,
        PIPELINE_STAGE_FLAG_BITS_MAX_ENUM = 0x7FFFFFFF
    };
    typedef unsigned int RPipelineStageFlags;
}