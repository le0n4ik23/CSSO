#include "netvar.h"

#include <fstream>

#include <ctype.h>
#include <format>

void SetupNetvars()
{
	for (auto clientClass = client->GetAllClasses(); clientClass; clientClass = clientClass->next)
	{
		//DUMP CLASSES
		/*std::cout << clientClass->networkName << " = " << clientClass->classID << ",\n";*/
		if (clientClass->recvTable)
			Dump(clientClass->networkName, clientClass->recvTable);
	}
}

void showDir()
{
	char buff[MAX_PATH];
	int n = GetCurrentDirectoryA(MAX_PATH, buff);

	if (n)
		std::cout << "Current Directory is:\n" << buff << '\n';
	else
		std::cout << "Get Directory failed\n";
}

void Dump(const char* baseClass, RecvTable* table, std::uint32_t offset)
{
	for (auto i = 0; i < table->propsCount; ++i)
	{
		const auto prop = &table->props[i];

		if (!prop || isdigit(prop->varName[0]))
			continue;

		if (fnv::Hash(prop->varName) == fnv::HashConst("baseclass"))
			continue;
		
		if (prop->recvType == SendPropType::DATATABLE &&
			prop->dataTable &&
			prop->dataTable->tableName[0] == 'D')
			Dump(baseClass, prop->dataTable, offset + prop->offset);

		const auto netvarName = std::format("{}->{}", baseClass, prop->varName);

		netvars[fnv::Hash(netvarName.c_str())] = offset + prop->offset;

		//DUMP NETVAR OFFSETS
		/*std::cout << netvarName << " = " << std::hex << prop->offset << std::dec << "\n";*/
	}
}