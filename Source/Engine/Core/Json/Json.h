#pragma once
#include "Core/Math/Math.h"
#include <document.h>
#include <String>
namespace Json {
	using Type = rapidjson::Type;
	using Value = rapidjson::Value;
	using Document = rapidjson::Document;

	bool ReadFile(const char* file, Document& doc);
	bool WriteFile(const char* file, const Document& doc, bool pretty=true);

	void AddString(Value& obj, const char* name, const std::string& str, Document::AllocatorType& a);
	void AddVector3(Value& obj, const char* name, Math::FVector3& vector, Document::AllocatorType& a);
	void LoadVector3(const Value& obj, Math::FVector3& vector);
}	