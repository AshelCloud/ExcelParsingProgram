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

	/** ����ü �ҽ��ڵ�� Json ���� */
	void Generate(XLWorksheet& workSheet, std::string outputDirectory);

private:
	/** .cpp�� .h�� ���� */
	void GenerateSourceCode(std::string workSheetName, std::string outputDirectory);

	/** Json ���� */
	/** A���� �ִ� Cell�� Root�̸� �� ���� �����ͷ� �߰� */
	void GenerateJson(XLWorksheet& workSheet, std::string outputDirectory);

	/** TODO: API �̸�(���� TESTPROJECT_API) argv�� �޾ƿ��� */
	/** .h�� �� row code�� ��ȯ */
	std::string CreateHeaderCode(std::string workSheetName);

	/** .cpp�� �� row code�� ��ȯ */
	std::string CreateSourceCode(std::string workSheetName);

	/** ����ü�� �� �������� ä������ */
	void SetDataNames(XLWorksheet& workSheet);

	const std::string& ConvertCellValueTypeToString(XLWorksheet& workSheet, char asciiCode);

private:
	/** 1�࿡ �ִ� Cell �̸��� (A1��° Cell �� ����) */
	std::vector<std::pair<std::string, std::string>> cellDatas;
};