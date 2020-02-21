#include <iostream>
#include <fstream>
#include <vector>

#include <sstream>
#include <algorithm>
#include <iterator>

#include "DbEmulator.h"


DbEmulator::DbEmulator()
{
    loadData();
}

void DbEmulator::loadData()
{
    StringsVector lines;

    // Loading legal persons data
    lines = service::loadFileToVector("TextData\\LegalPersons.txt");
    for (int i = 0; i < lines.size(); ++i) {
        StringsVector lineData = service::split(lines.at(i), '_');

        Person element = { stoi(lineData.at(0)), PERSON_TYPE::LEGAL, encoding::utf8_to_cp1251(lineData.at(1)) };
        legalPersons.addRow(element.ID, element);
    }

    // Loading individuals data
    lines = service::loadFileToVector("TextData\\Individuals.txt");
    for (int i = 0; i < lines.size(); ++i) {
        StringsVector lineData = service::split(lines.at(i), '_');

        Person element = { stoi(lineData.at(0)) , PERSON_TYPE::INDIVIDUAL, encoding::utf8_to_cp1251(lineData.at(1)) };
        individuals.addRow(element.ID, element);
    }

    // Loading ownership stake data
    lines = service::loadFileToVector("TextData\\OwnershipStake.txt");
    for (int i = 0; i < lines.size(); ++i) {
        StringsVector lineData = service::split(lines.at(i), '_');

        OwnershipStake element = { stoi(lineData.at(0)),    // ID
                                   stoi(lineData.at(1)),    // CompanyID
                                   stoi(lineData.at(2)),    // CompanyType
                                   stoi(lineData.at(3)),    // OwnerID
                                   stoi(lineData.at(4)),    // OwnerType
                                   stof(lineData.at(5))     // Share
                                 };
        ownershipStake.addRow(element.ID, element);
    }

    // Loading single executive data
    lines = service::loadFileToVector("TextData\\SingleExecutive.txt");
    for (int i = 0; i < lines.size(); ++i) {
        StringsVector lineData = service::split(lines.at(i), '_');

        SingleExecutive element = { stoi(lineData.at(0)),   // ID
                                    stoi(lineData.at(1)),    // CompanyID
                                    stoi(lineData.at(2)),    // CompanyType
                                    stoi(lineData.at(3)),    // OwnerID
                                    stoi(lineData.at(4)),    // OwnerType
                                  };
        singleExecutive.addRow(element.ID, element);
    }
}


StringsVector DbEmulator::getAllPersonsName()
{
    StringsVector result;

    for (auto it = legalPersons.begin(); it != legalPersons.end(); ++it) {
        result.emplace_back(it->second.Name);
    }

    for (auto it = individuals.begin(); it != individuals.end(); ++it) {
        result.emplace_back(it->second.Name);
    }

    return result;
}

std::vector<ReportItem> DbEmulator::getLinkedCompanies(const Person& person, bool dual)
{
    std::vector<ReportItem> result;

    // ����� ����������� ���, � ������� ��������� �������� ����� ������� ������� ����� 50%
    for (auto it = ownershipStake.begin(); it != ownershipStake.end(); ++it)
    {
        ReportItem element;

        auto row = it->second;
        if (row.OwnerType == person.Type && row.OwnerID == person.ID && row.Share > 50) {
            element.�ompany = legalPersons.getRow(row.CompanyID);
        }
        else {
            continue;
        }

        element.relation = service::replace("%1  ����� ����� ������������� ����� 50% ������� %2", person.Name.c_str(), element.�ompany.Name.c_str());
        element.basis = "��.1 �.1 ��.9 �-�� �135-��";
        result.emplace_back(element);
    }

    // ����� ����������� ���, � ������� ��������� �������� �������� ��� 
    for (auto it = singleExecutive.begin(); it != singleExecutive.end(); ++it)
    {
        ReportItem element;

        auto row = it->second;
        if (row.OwnerType == person.Type && row.OwnerID == person.ID) {
            element.�ompany = row.CompanyType == PERSON_TYPE::LEGAL ? legalPersons.getRow(row.CompanyID) : individuals.getRow(row.CompanyID);
        }
        else {
            continue;
        }

        element.relation = service::replace("%1 �������� ����������� �������������� ������� %2", person.Name.c_str(), element.�ompany.Name.c_str());
        element.basis = "��.2 �.1 ��.9 �-�� �135-��";
        result.emplace_back(element);
    }

    // ����� �����������/���������� ���, ���������� ��� �������� 
    for (auto it = singleExecutive.begin(); it != singleExecutive.end(); ++it)
    {
        ReportItem element;

        auto row = it->second;
        if (row.CompanyID == person.ID && row.CompanyType == person.Type) {
            element.�ompany = row.OwnerType == PERSON_TYPE::LEGAL ? legalPersons.getRow(row.OwnerID) : individuals.getRow(row.OwnerID);
        }
        else {
            continue;
        }

        element.relation = service::replace("%1 �������� ����������� �������������� ������� %2", element.�ompany.Name.c_str(), person.Name.c_str());
        element.basis = "��.2 �.1 ��.9 �-�� �135-��";
        result.emplace_back(element);
    }

    // ����� �����������/���������� ��� �� ������� ������� � �������� (����� 50%)
    for (auto it = ownershipStake.begin(); it != ownershipStake.end(); ++it)
    {
        ReportItem element;

        auto row = it->second;
        if (row.CompanyID == person.ID && row.CompanyType == person.Type && row.Share > 50) {
            element.�ompany = person.Type == PERSON_TYPE::LEGAL ? legalPersons.getRow(row.OwnerID) : individuals.getRow(row.OwnerID);
        }
        else {
            continue;
        }

        element.relation = service::replace("%1  ����� ����� ������������� ����� 50% ������� %2", element.�ompany.Name.c_str(), person.Name.c_str());
        element.basis = "��.1 �.1 ��.9 �-�� �135-��";
        result.emplace_back(element);
    }

    return result;
}

void DbEmulator::fillReportNode(ReportTreeNode* node)
{
    std::vector<ReportItem> relations = getLinkedCompanies(node->data.�ompany, node->parent == nullptr);
    for each (auto relation in relations)
    {
        if (node->parent != nullptr && relation.�ompany.Name == node->parent->data.�ompany.Name) {
            continue;
        }

        ReportTreeNode child;
        child.parent = node;
        child.data = relation;
        fillReportNode(&child);
        node->childs.push_back(child);
    }

}

ReportTreeNode DbEmulator::getReport(const std::string& companyName)
{
    // ����� ������������ ���� �� ������������
    Person selected;
    for (auto it = legalPersons.begin(); it != legalPersons.end(); ++it) {
        if (it->second.Name == companyName) {
            selected = it->second;
            break;
        }
    }

    if (selected.Name.empty()) {
        for (auto it = individuals.begin(); it != individuals.end(); ++it) {
            if (it->second.Name == companyName) {
                selected = it->second;
                break;
            }
        }
    }

    ReportTreeNode result;
    result.parent = nullptr;
    result.data = { selected, "", ""};
    fillReportNode(&result);

    return result;
}

