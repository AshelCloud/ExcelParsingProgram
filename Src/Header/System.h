#pragma once

#include<vector>
#include<string>
#include <OpenXLSX.hpp>

using namespace OpenXLSX;

class System
{
public:
	System();
	~System();

public:
	/** filePath�� �ִ� ��� ���ϰ�� ��ȯ */
	std::vector<std::string> GetFilesName(std::string filePath) const;

	/** ValueType�� Error Ȥ�� Empty���� �˻� */
	/** Error || Empty �Ͻ� ture */
	bool InvalidValue(XLValueType type);

	/** Sheet�� �ִ� ù��° Column�� �������� ����ü�� ���� */
	void GenerateStruct(XLWorksheet& workSheet, std::string workSheetName, std::string outputDirectory);

	/** Json ���� */
	/** A���� �ִ� Cell�� Root�̸� �� ���� �����ͷ� �߰� */
	void GenerateJson();
	
private:
	/** .cpp�� .h�� ���� */
	void GenerateSourceCode(std::vector<std::string> cellNames, std::string workSheetName, std::string outputDirectory);

	/** .h�� �� row code�� ��ȯ */
	std::string CreateHeaderCode(std::vector<std::string> cellNames, std::string workSheetName);

	/** .cpp�� �� row code�� ��ȯ */
	std::string CreateSourceCode(std::string workSheetName);
};