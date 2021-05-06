#include <iomanip>
#include <iostream>
#include "System.h"

/** TODO: Console ��¿� �� �־ Error, Warning ����
*	TODO: �ڷ����� �������������� Error ����ϰ� exit
*/


int main(int argc, char* argv[])
{
	/*if (argc < 2)
	{
		std::cout << "ERROR: argv�� ���ǵ����ʾҽ��ϴ�." << std::endl;
		return 0;
	}*/

	System system;
	std::vector<std::string> fileNames = system.GetFilesName(argv[1]);

	/** Debug�� �ڵ� */
	//std::vector<std::string> fileNames = system.GetFilesName("C:\\Users\\PC5\\Desktop\\ExcelParsingProgram\\x64\\Release\\Excels\\");

	/** Debug�� �ڵ�*/
	//const std::string outputDirectory = "C:\\Users\\PC5\\Desktop\\ExcelParsingProgram\\x64\\Release\\Project\\Content\\";

	for (std::string fileName : fileNames)
	{
		XLDocument doc;
		doc.open(fileName);

		std::cout << std::endl;
		std::cout << "���õ� xlsx���� �̸�: " << fileName << std::endl;

		int asciiCode = 65; // A
		for(std::string worksheetName : doc.workbook().worksheetNames())
		{
			auto wks = doc.workbook().worksheet(worksheetName);

			std::cout << std::endl;
			std::cout << "���õ� sheet �̸�: " << worksheetName << std::endl;
			std::cout << std::endl;

			system.Generate(wks, argv[2], argv[3]);
		}

		doc.close();
	}

	std::cout << std::endl;

	return 0;
}