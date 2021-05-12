#include "System.h"
#include <filesystem>
#include <iostream>
#include <fstream>
#include <json.h>
#include <cmath>
#include <Windows.h>

System::System():
	warningCount(0), errorCount(0)
{
}

System::~System()
{
}

std::vector<std::string> System::GetFilesName(std::string filePath) const
{
	std::vector<std::string> output;

	for(const auto& file : std::filesystem::directory_iterator(filePath))
	{
		output.push_back(file.path().string());
	}
	
	return output;
}

bool System::InvalidValue(XLValueType type)
{
	return (type == XLValueType::Error) || (type == XLValueType::Empty);
}

void System::Generate(XLWorksheet& workSheet, std::string sourceCodePath, std::string headerCodePath, std::string jsonPath)
{
	SetDataNames(workSheet);

	bool result = true;

	result = GenerateSourceCode(workSheet.name(), sourceCodePath, headerCodePath);
	if (!result)
	{
		std::cout << "ERROR: Failed Generate SourceCode" << std::endl;
		exit(-1);
	}
	result = GenerateJson(workSheet, jsonPath);
	if (!result)
	{
		std::cout << "ERROR: Failed Generate Json" << std::endl;
		exit(-1);
	}
}

bool System::GenerateJson(XLWorksheet& workSheet, std::string outputDirectory)
{
	auto ConvertCellToJsonValue = [](XLCellValue Cell) 
	{
		Json::Value data;
		switch (Cell.valueType())
		{
		case XLValueType::Float:
			data = Cell.get<double>();
			break;

		case XLValueType::Integer:
			data = Cell.get<int>();
			break;

		case XLValueType::String:
			data = Cell.get<std::string>();
			break;

		case XLValueType::Boolean:
			data = Cell.get<bool>();
			break;
		}

		return data;
	};

	if (cellDatas.empty())
	{
		std::cout << "ERROR: DataName Is Empty" << std::endl;
		return false;
	}

	/** TODO: Json ������ ���� */
	Json::Value root;

	Json::Value id;

	int Index = 3;
	while(true)
	{
		int asciiCode = 66;

		auto checkInvalid = workSheet.cell(XLCellReference("A" + std::to_string(Index)));
		if(InvalidValue(checkInvalid.valueType()))
		{
			break;
		}

		Json::Value data;
		for (auto name : cellDatas)
		{
			std::string cellIndex;
			cellIndex = ((char)asciiCode);
			cellIndex += std::to_string(Index);

			auto currentCell = workSheet.cell(XLCellReference(cellIndex));

			data[name.second] = ConvertCellToJsonValue(currentCell.value());

			asciiCode++;
		}

		id.append(data);
		Index++;
	}

	root["id"] = id;

	Json::StreamWriterBuilder writer;

	std::string jsonString = Json::writeString(writer, root);

	std::string path = outputDirectory + "Files\\";
	std::ofstream file;
	file.open(path + workSheet.name() + ".json");
	if(file.is_open())
	{
		file.write(jsonString.c_str(), jsonString.size());
	}
	else
	{
		file.close();
		std::cout << "ERROR: Failed open file: " << path + workSheet.name() + ".json" << std::endl;
		return false;
	}
	file.close();

	return true;
}

bool System::GenerateSourceCode(std::string workSheetName, std::string sourceCodePath, std::string headerCodePath)
{
	/**TODO: ��� ���� */
	std::string headerPath = headerCodePath + "TableStruct\\";

	if (cellDatas.empty())
	{
		std::cout << "ERROR: DataName Is Empty" << std::endl;
		return false;
	}

	std::ofstream file;

	file.open(headerPath + workSheetName + ".h");
	if (file.is_open())
	{
		std::string Header = CreateHeaderCode(workSheetName);
		file.write(Header.c_str(), Header.size());
	}
	else
	{
		file.close();
		std::cout << "ERROR: Failed open file: " << headerPath + workSheetName + ".h" << std::endl;
		return false;
	}
	file.close();

	std::string sourcePath = sourceCodePath + "TableStruct\\";
	/**TODO: ��� ���� */
	file.open(sourcePath + workSheetName + ".cpp");
	if (file.is_open())
	{
		std::string Source = CreateSourceCode(workSheetName);
		file.write(Source.c_str(), Source.size());
	}
	else
	{
		file.close();
		std::cout << "ERROR: Failed open file: " << sourcePath + workSheetName + ".cpp" << std::endl;
		return false;
	}
	file.close();

	return true;
}

std::string System::CreateHeaderCode(std::string workSheetName)
{

	std::string HeaderCode =
		"#pragma once\n"
		"\n"
		"#include \"CoreMinimal.h\"\n"
		"#include \"" + workSheetName + ".generated.h\"\n"
		"\n"
		"USTRUCT(Atomic, BlueprintType)\n"
		"struct F" + workSheetName + "\n"
		"{\n"
		"\tGENERATED_USTRUCT_BODY()\n"
		"public:\n";

	for (auto name : cellDatas)
	{
		HeaderCode +=
			"\tUPROPERTY(EditAnywhere, BlueprintReadWrite)\n"
			"\t" + name.first + " " + name.second + ";\n";
	}

	HeaderCode +=
		"};\n\n"
		"class PROJECTSINGWI_API " + workSheetName + "\n"
		"{\n"
		"public:\n"
		"\t" + workSheetName + "();\n"
		"\t~" + workSheetName + "();\n"
		"};\n";

	return HeaderCode;
}

std::string System::CreateSourceCode(std::string workSheetName)
{
	std::string SourceCode =
		"#include \"" + workSheetName + ".h\"\n"
		"" + workSheetName + "::" + workSheetName + "()\n"
		"{\n}\n"
		"" + workSheetName + "::" + "~" + workSheetName + "()\n"
		"{\n}\n";

	return SourceCode;
}

void System::SetDataNames(XLWorksheet& workSheet)
{
	if(!cellDatas.empty())
	{
		cellDatas.clear();
	}

	int asciiCode = 65; // A

	while (true)
	{
		std::string cellIndex;
		cellIndex = (char(asciiCode));
		cellIndex += "1";

		auto cell = workSheet.cell(XLCellReference(cellIndex)).value();
		if (InvalidValue(cell.valueType()))
		{
			break;
		}

		try
		{
			if (cellIndex != "A1")
			{
				std::string toString;
				toString = ((char)asciiCode);

				std::string type = ConvertCellValueTypeToString(workSheet, asciiCode);
				if(type == "")
				{
					PrintLog(Color::ERROR_RED, workSheet.name() + "�� �����ִ� " + cell.get<std::string>() + "�� Ÿ�� ������ �����߽��ϴ�.");
					asciiCode++;
					continue;
				}
				cellDatas.push_back(std::make_pair(type, cell.get<std::string>()));


				PrintLog(Color::LIGHTBLUE, cell.get<std::string>() + "��(��) " + type + "���� �����Ǿ����ϴ�.");
			}
		}
		catch (XLException e)
		{
			std::cout << e.what() << std::endl;
		}

		asciiCode++;
	}
}

const std::string System::ConvertCellValueTypeToString(XLWorksheet& workSheet, char asciiCode)
{
	std::string result = "";
	int index = 3;

	bool convertFloat = false;

	while(true)
	{
		std::string toString;
		toString = asciiCode;

		auto checkDataType = workSheet.cell(XLCellReference(toString + std::to_string(index))).value();
		if (InvalidValue(checkDataType.valueType()))
		{
			break;
		}

		switch (checkDataType.valueType())
		{
		case XLValueType::Boolean:
			result = "bool";
			break;
		case XLValueType::Float:
			result = "float";
			break;
		case XLValueType::Integer:
			if (result == "float") 
			{
				convertFloat = true;
				break;
			}
			result = "int";
			break;
		case XLValueType::String:
			result = "FString";
			break;
		}

		index++;
	}

	if(convertFloat)
	{
		std::string toString;
		toString = asciiCode;
		toString += "1";

		auto cell = workSheet.cell(XLCellReference(toString));

		PrintLog(Color::WARNING_YELLOW, workSheet.name() + "�� �����ִ� " + cell.value().get<std::string>() + "�� type�� int�� �����Ǵٰ� float���� ����Ǿ����ϴ�.");
	}

	return result;
}

void System::PrintLog(Color color, std::string log, bool pushLog)
{
	SetTextColor(static_cast<int>(color));

	if(pushLog)
	{
		switch (color)
		{
		case Color::ERROR_RED:
			errorCount++;
			errorLogs.push_back(log);
			break;
		case Color::WARNING_YELLOW:
			warningCount++;
			warningLogs.push_back(log);
			break;
		}
	}

	std::cout << log << std::endl;

	SetTextColor(static_cast<int>(Color::WHITE));
}

void System::PrintResult()
{
	Color logColor = Color::LIGHTRED;

	if(errorCount + warningCount == 0)
	{
		logColor = Color::WHITE;
	}
	

	if(errorCount + warningCount >= 1)
	{
		PrintLog(Color::WHITE, "���� ���: ");
		for (auto log : errorLogs)
		{
			PrintLog(Color::LIGHTRED, log);
		}

		PrintLog(Color::WHITE, "��� ���: ", false);
		for(auto log : warningLogs)
		{
			PrintLog(Color::WARNING_YELLOW, log, false);
		}
	}

	PrintLog(logColor, "����: " + std::to_string(errorCount) + "��, ���: " + std::to_string(warningCount) + "��");
}

void System::SetTextColor(int color)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}
